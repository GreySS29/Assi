#pragma once
#include <alsa/asoundlib.h>
#include <iostream>



class Alsa{
    public:
    Alsa(const char*);
    snd_pcm_t *pcm;
    snd_pcm_hw_params_t *params;
    int err;

    
    snd_pcm_t* get_pcm() const { return pcm; }

};

