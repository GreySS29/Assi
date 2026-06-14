#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <alsa/asoundlib.h>
#include <piper.h>
#include "Alsa.h"
#include <string>

class My_piper {
    public:

    My_piper(const char*, const char*);
    ~My_piper();

    private:
    piper_synthesizer *synth;
    Alsa* alsa = nullptr;
    piper_audio_chunk chunk;
    std::vector<int16_t> convert_float_int16();
};