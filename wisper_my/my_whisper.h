#pragma once
#include "whisper.h" 
#include <alsa/asoundlib.h>
#include <iostream>
#include <string>
#include <vector>

class VoiceOperate {

    public:
    VoiceOperate(const char*);
    VoiceOperate();
    ~VoiceOperate();
    std::vector<float> capture_audio(int duration_sec, int sample_rate);
    std::vector<float> resample_audio(const std::vector<float>& input,int input_rate,int output_rate);
    std::string transcribe_audio(const std::vector<float>& audio_16khz);
    std::string wish_rec(int duration_sec = 5);

    private:
    whisper_context* whisper_P_ctx;
    snd_pcm_t* capture_handle;
    struct whisper_context_params cparams;
};