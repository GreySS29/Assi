#include "my_piper.h"

My_piper::My_piper(const char* input)
{
    alsa = new Alsa(input);
    if (!alsa->get_pcm()) {
        std::cerr << "Failed to create Alsa" << std::endl;
        return;
    }

    synth = piper_create(
       "../../piper1-gpl/voices/en_US-amy-medium.onnx",
       "../../piper1-gpl/voices/en_US-amy-medium.onnx.json",
       "../../piper1-gpl/libpiper/build/espeak_ng-install/share/espeak-ng-data");

    if (!synth) std::cerr << "Failed to create synthesizer" << std::endl;

    
}
    

// void My_piper::start(const char* respond)
// {
//     piper_synthesize_options options = piper_default_synthesize_options(synth);
//     piper_synthesize_start(synth,respond, &options);

//     std::vector<int16_t> stereo = convert_float_int16();

//     snd_pcm_sframes_t frames = snd_pcm_writei(alsa->get_pcm(), stereo.data(), stereo.size() / 2);
//         if (frames < 0) {
//             frames = snd_pcm_recover(alsa->get_pcm(), frames, 0);
//             if (frames < 0) std::cerr << "ALSA write error: " << snd_strerror(frames) << std::endl;
//         }
//     }


void My_piper::start(const char* respond)
{
    piper_synthesize_options options = piper_default_synthesize_options(synth);
    piper_synthesize_start(synth, respond, &options);

    std::vector<int16_t> stereo = convert_float_int16();

    snd_pcm_t* pcm = alsa->get_pcm();

    snd_pcm_prepare(pcm);
    const size_t channels = 2;
    const size_t total_frames = stereo.size() / channels;
    const size_t chunk_frames = 4096;

    size_t offset_frames = 0;
    while (offset_frames < total_frames) {
        size_t cur_frames = std::min(chunk_frames, total_frames - offset_frames);
        snd_pcm_sframes_t rc = snd_pcm_writei(
            pcm,
            stereo.data() + offset_frames * channels,
            cur_frames
        );

        if (rc < 0) {
            std::cerr << "Underrun/error detected, recovering...\n";
            rc = snd_pcm_recover(pcm, rc, 0);
            if (rc < 0) {
                std::cerr << "ALSA write error: " << snd_strerror(rc) << std::endl;
                break;
            }
            continue;
        }

        offset_frames += static_cast<size_t>(rc);
    }
}

std::vector<int16_t> My_piper::convert_float_int16()
{
    std::vector<int16_t> all_stereo;

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
        all_stereo.insert(all_stereo.end(), stereo.begin(), stereo.end());

    }
    return all_stereo;  
}

My_piper::~My_piper()
{
    snd_pcm_drain(alsa->get_pcm());
    snd_pcm_close(alsa->get_pcm());
    if(synth!=nullptr) piper_free(synth);
}