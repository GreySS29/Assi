#include "whisper.h"
#include "llama.h"
#include <curl/curl.h>
#include <alsa/asoundlib.h>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <iostream>

struct AudioBuffer { // // Принимает сырые байты от curl и конвертирует в float[]
    std::vector<float> samples;
    static size_t write_cb(void* data, size_t size, size_t nmemb, void* userdata) {
        auto* buf = static_cast<AudioBuffer*>(userdata);
        size_t len = size * nmemb;
        buf->samples.insert(buf->samples.end(), (float*)data, (float*)data + len/4);
        return len;
    } 
};

class VoicePipeline {
private:
    whisper_context* whisper_ctx;
    llama_context* llama_ctx;
    CURL* curl;
    snd_pcm_t* capture_handle;
    snd_pcm_t* playback_handle;
    
public:
    VoicePipeline() {
        // Whisper init
        whisper_ctx = whisper_init_from_file_with_params("../whisper.cpp/models/ggml-small.en.bin");
        
        // Llama 3.2 3B init
        llama_context_params params = llama_context_default_params();
        params.n_ctx = 2048;
        llama_ctx = llama_new_context_with_model
        (llama_load_model_from_file("../llama.cpp/llama-3.2-3b.gguf",
             params));
        
        // Curl init
        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:59125/api/tts");
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, AudioBuffer::write_cb);
        
        // ALSA capture (микрофон)
        snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
        snd_pcm_set_params(capture_handle, SND_PCM_FORMAT_FLOAT_LE,SND_PCM_ACCESS_MMAP_INTERLEAVED, 16000, 1, 100000);
        
        // ALSA playback (HDMI)
        snd_pcm_open(&playback_handle, "hw:0,3", SND_PCM_STREAM_PLAYBACK, 0);
        snd_pcm_set_params(playback_handle, SND_PCM_FORMAT_FLOAT_LE, 2, 22050, 1, 100000);
    }
    
    ~VoicePipeline() {
        whisper_free(whisper_ctx);
        llama_free(llama_ctx);
        curl_easy_cleanup(curl);
        snd_pcm_close(capture_handle);
        snd_pcm_close(playback_handle);
    }
    
    std::string stt_record(int duration_sec = 5) {
        std::cout << "Слушаю " << duration_sec << " сек...\n";
        
        // Запись с микрофона
        snd_pcm_start(capture_handle);
        std::vector<float> audio(duration_sec * 16000);
        snd_pcm_readi(capture_handle, audio.data(), audio.size());
        snd_pcm_drop(capture_handle);
        
        // Whisper STT
        whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
        params.language = "ru";
        
        whisper_full(whisper_ctx, params, audio.data(), audio.size());
        return whisper_full_get_segment_text(whisper_ctx, 0);
    }
    
    std::string llm_generate(const std::string& prompt) {
        std::string system = "Ты полезный ассистент. Отвечай кратко и по делу.";
        std::string full_prompt = system + "\nПользователь: " + prompt + "\nАссистент:";
        
        llama_token* tokens = llama_tokenize(llama_ctx, full_prompt.c_str(), full_prompt.size(), true);
        
        llama_decode(llama_ctx, llama_batch_get_one(tokens, llama_n_tokens(llama_ctx), 0, 0));
        
        std::string response;
        int n_tokens = 0;
        while (n_tokens < 256) {
            llama_token next = llama_sample_token_greedy(llama_ctx);
            if (llama_token_to_str(llama_ctx, next) == nullptr) break;
            response += llama_token_to_str(llama_ctx, next);
            n_tokens++;
        }
        return response;
    }
    
    void tts_play(const std::string& text) {
        AudioBuffer buffer;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ("{\"text\":\"" + text + "\",\"voice\":\"ru_RU/silero_low\"}").c_str());
        
        curl_easy_perform(curl);
        
        // Воспроизведение
        snd_pcm_writei(playback_handle, buffer.samples.data(), buffer.samples.size()/2);
        snd_pcm_drain(playback_handle);
    }
    
    void run_loop() {
        std::cout << "Голосовой ассистент готов. Ctrl+C для выхода.\n";
        while (true) {
            try {
                std::string speech = stt_record(5);
                if (speech.empty()) continue;
                
                std::cout << "Распознало: " << speech << std::endl;
                
                std::string response = llm_generate(speech);
                std::cout << "Ответ: " << response << std::endl;
                
                tts_play(response);
            } catch (...) {
                std::cout << "Ошибка, повтор...\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
};

int main() {
    VoicePipeline pipeline;
    pipeline.run_loop();
    return 0;
}