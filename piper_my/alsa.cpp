#include "Alsa.h"

// Alsa::Alsa(const char* input)
// {
//     err = snd_pcm_open(&pcm, input, SND_PCM_STREAM_PLAYBACK, 0);
//     std::cout << input << '\n';
//     if (err < 0) {
//         std::cerr << "ALSA open error: " << snd_strerror(err) << std::endl;
//         return;
//     }

//     snd_pcm_hw_params_alloca(&params);
//     snd_pcm_hw_params_any(pcm, params);

//     snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
//     snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);
//     snd_pcm_hw_params_set_channels(pcm, params, 2);
//     unsigned int rate = 22050;
//     snd_pcm_hw_params_set_rate_near(pcm, params, &rate, 0);

//      err = snd_pcm_hw_params(pcm, params);
//     if (err < 0) {
//         std::cerr << "ALSA hw_params error: " << snd_strerror(err) << std::endl;
        
//     }

//     //std::cerr << "ALSA ready, rate: " << rate << std::endl;

// }


Alsa::Alsa(const char* input)
{
    err = snd_pcm_open(&pcm, input, SND_PCM_STREAM_PLAYBACK, 0);
    std::cout << input << '\n';
    if (err < 0) {
        std::cerr << "ALSA open error: " << snd_strerror(err) << std::endl;
        return;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm, params);

    snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm, params, 2);

    unsigned int rate = 22050;
    snd_pcm_hw_params_set_rate_near(pcm, params, &rate, 0);

    // --- увеличение буфера ---
    // Вариант 1: через время буфера (в микросекундах), например 500 мс
    unsigned int buffer_time = 500000; // 0.5 секунды
    snd_pcm_hw_params_set_buffer_time_near(pcm, params, &buffer_time, 0);

    // Можно также задать время периода (буфер делится на периоды)
    unsigned int period_time = 100000; // 100 мс
    snd_pcm_hw_params_set_period_time_near(pcm, params, &period_time, 0);

    // Вариант 2 (альтернатива): через количество фреймов напрямую
    // snd_pcm_uframes_t buffer_size = 44100; // например, 2 сек при 22050 Hz
    // snd_pcm_hw_params_set_buffer_size_near(pcm, params, &buffer_size);

    err = snd_pcm_hw_params(pcm, params);
    if (err < 0) {
        std::cerr << "ALSA hw_params error: " << snd_strerror(err) << std::endl;
    }

    // Проверка реально установленных значений
    snd_pcm_uframes_t actual_buffer_size;
    snd_pcm_hw_params_get_buffer_size(params, &actual_buffer_size);
    std::cerr << "ALSA buffer size (frames): " << actual_buffer_size << std::endl;
}