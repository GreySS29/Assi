#include "Alsa.h"

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

     err = snd_pcm_hw_params(pcm, params);
    if (err < 0) {
        std::cerr << "ALSA hw_params error: " << snd_strerror(err) << std::endl;
        
    }

    //std::cerr << "ALSA ready, rate: " << rate << std::endl;

}