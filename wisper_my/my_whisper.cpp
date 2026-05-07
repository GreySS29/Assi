#include "my_whisper.h" 


    VoiceOperate::VoiceOperate() {
        cparams = whisper_context_default_params();
        whisper_P_ctx=whisper_init_from_file_with_params("../../whisper.cpp/models/ggml-small.en.bin", cparams);

        if (!whisper_P_ctx) {
        throw std::runtime_error("whisper init failed");
        }

        int rc = snd_pcm_open(
        &capture_handle,
        "plughw:3,0",
        SND_PCM_STREAM_CAPTURE,
        0
        );

        if (rc < 0) {
            throw std::runtime_error(std::string("snd_pcm_open failed: ") + snd_strerror(rc));
        }

        rc = snd_pcm_set_params(capture_handle,SND_PCM_FORMAT_FLOAT_LE,
            SND_PCM_ACCESS_RW_INTERLEAVED,1,44100,1,500000);

        if (rc < 0) {
            snd_pcm_close(capture_handle);
            capture_handle = nullptr;
            throw std::runtime_error(std::string("snd_pcm_set_params failed: ") + snd_strerror(rc));
            }
        
    }

    VoiceOperate::~VoiceOperate(){
        whisper_free(whisper_P_ctx);
        snd_pcm_close(capture_handle);
    }

//Эта функция изолирует всю работу с ALSA: 
//читает до конца буфера и при ошибках пытается восстановиться через snd_pcm_recover()
std::vector<float> VoiceOperate::capture_audio(int duration_sec, int sample_rate)
{
    std::cout << "Listening.. " << duration_sec << " s...\n";

    const int total_frames = duration_sec * sample_rate;
    std::vector<float> audio(total_frames, 0.0f);

    snd_pcm_prepare(capture_handle);
    snd_pcm_start(capture_handle);

    int total_read = 0;

    while (total_read < total_frames) {
        int ret = snd_pcm_readi(
            capture_handle,
            audio.data() + total_read,
            total_frames - total_read
        );

        if (ret == -EAGAIN) {
            continue;
        }

        if (ret < 0) {
            ret = snd_pcm_recover(capture_handle, ret, 1);
            if (ret < 0) {
                std::cerr << "snd_pcm_readi failed: " << snd_strerror(ret) << "\n";
                snd_pcm_drop(capture_handle);
                return {};
            }
            continue;
        }

        total_read += ret;
    }

    snd_pcm_drop(capture_handle);
    audio.resize(total_read);

    return audio; 
}   

//линейный ресемплер
std::vector<float> VoiceOperate::resample_audio(
    const std::vector<float>& input,
    int input_rate,
    int output_rate
)
{
    if (input.empty() || input_rate <= 0 || output_rate <= 0) {
        return {};
    }

    if (input_rate == output_rate) {
        return input;
    }

    const int output_size =
        static_cast<int>((1.0 * input.size() * output_rate) / input_rate);

    std::vector<float> output(output_size, 0.0f);

    for (int i = 0; i < output_size; ++i) {
        float src_pos = (static_cast<float>(i) * input_rate) / output_rate;

        int idx0 = static_cast<int>(src_pos);
        int idx1 = idx0 + 1;

        if (idx0 >= static_cast<int>(input.size())) {
            output[i] = 0.0f;
            continue;
        }

        if (idx1 >= static_cast<int>(input.size())) {
            output[i] = input[idx0];
            continue;
        }

        float frac = src_pos - idx0;

        output[i] =
            input[idx0] * (1.0f - frac) +
            input[idx1] * frac;
    }

    return output;
}


// Такой вариант соответствует API whisper.cpp: сначала whisper_full(...),
// потом количество сегментов через whisper_full_n_segments(...), 
//потом текст каждого сегмента через whisper_full_get_segment_text(...)

std::string VoiceOperate::transcribe_audio(const std::vector<float>& audio_16khz)
{
    if (audio_16khz.empty()) {
        return "";
    }

    whisper_full_params params =
        whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

    params.language = "en";

    int rc = whisper_full(
        whisper_P_ctx,
        params,
        audio_16khz.data(),
        audio_16khz.size()
    );

    if (rc != 0) {
        std::cerr << "whisper_full failed\n";
        return "";
    }

    int n_segments = whisper_full_n_segments(whisper_P_ctx);
    if (n_segments <= 0) {
        return "";
    }

    std::string result;
    for (int i = 0; i < n_segments; ++i) {
        const char* text = whisper_full_get_segment_text(whisper_P_ctx, i);
        if (text) {
            result += text;
        }
    }

    return result;
}

  std::string VoiceOperate::wish_rec(int duration_sec)
{
    const int capture_rate = 44100;
    const int whisper_rate = 16000;

    std::vector<float> raw_audio = capture_audio(duration_sec, capture_rate);
    if (raw_audio.empty()) {
        std::cout << "wasn't captured" << '\n';
        return "";
    }

    std::vector<float> whisper_audio =
        resample_audio(raw_audio, capture_rate, whisper_rate);

    if (whisper_audio.empty()) {
        std::cout << "wasn't resampled" << '\n';
        return "";
    }

    return transcribe_audio(whisper_audio);
}




