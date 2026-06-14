#include <fstream>
#include <iostream>
#include <vector>
#include <alsa/asoundlib.h>
#include <piper.h>

int main() {
    // --- Piper ---
    piper_synthesizer *synth = piper_create(
       "../../piper1-gpl/voices/en_US-amy-medium.onnx",
       "../../piper1-gpl/voices/en_US-amy-medium.onnx.json",
       "../../piper1-gpl/libpiper/build/espeak_ng-install/share/espeak-ng-data");

    if (!synth) {
        std::cerr << "Failed to create synthesizer" << std::endl;
        return 1;
    }

    // --- ALSA ---
    // --- ALSA ---
    snd_pcm_t *pcm;
    snd_pcm_hw_params_t *params;
    int err;

    err = snd_pcm_open(&pcm, "plughw:3,0", SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        std::cerr << "ALSA open error: " << snd_strerror(err) << std::endl;
        return 1;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm, params);

    snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm, params, 2);

    unsigned int rate = 22050;
    snd_pcm_hw_params_set_rate_near(pcm, params, &rate, 0);

    err = snd_pcm_hw_params(pcm, params);
    if (err < 0) {
        std::cerr << "ALSA hw_params error: " << snd_strerror(err) << std::endl;
        return 1;
    }

    std::cerr << "ALSA ready, rate: " << rate << std::endl;

    // --- Синтез и воспроизведение ---
    piper_synthesize_options options = piper_default_synthesize_options(synth);
    piper_synthesize_start(synth, "Welcome to the world of speech synthesis!", &options);

    piper_audio_chunk chunk;
    while (piper_synthesize_next(synth, &chunk) != PIPER_DONE) {
        // Конвертируем float → int16, моно → стерео
        std::vector<int16_t> stereo(chunk.num_samples * 2);
        for (int i = 0; i < chunk.num_samples; i++) {
            float sample = chunk.samples[i];
            if (sample >  1.0f) sample =  1.0f;
            if (sample < -1.0f) sample = -1.0f;
            int16_t s16 = static_cast<int16_t>(sample * 32767.0f);
            stereo[i * 2]     = s16;  // левый канал
            stereo[i * 2 + 1] = s16;  // правый канал
        }

        snd_pcm_sframes_t frames = snd_pcm_writei(pcm, stereo.data(), chunk.num_samples);
        if (frames < 0) {
            frames = snd_pcm_recover(pcm, frames, 0);
            if (frames < 0) {
                std::cerr << "ALSA write error: " << snd_strerror(frames) << std::endl;
                break;
            }
        }
    }

    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    piper_free(synth);

    return 0;
}