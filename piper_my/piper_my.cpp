#include <fstream>
#include <piper.h>
#include <iostream>

int main() {

    piper_synthesizer *synth = piper_create("../../piper1-gpl/voices/en_US-amy-medium.onnx",
                                            "../../piper1-gpl/voices/en_US-amy-medium.onnx.json",
                                            "../../piper1-gpl/libpiper/build/espeak_ng-install/share/espeak-ng-data");

    if (!synth) {
       fprintf(stderr, "Failed to create synthesizer\n");
       return 1;
   }
    // aplay -r 22050 -c 1 -f FLOAT_LE -t raw output.raw
    std::ofstream audio_stream("output.raw", std::ios::binary);

    piper_synthesize_options options = piper_default_synthesize_options(synth);
    // Change options here:
    // options.length_scale = 2;
    // options.speaker_id = 5;

    piper_synthesize_start(synth, "Hi Sergei, I glad to see you!",
                           &options /* NULL for defaults */);

    piper_audio_chunk chunk;
    while (piper_synthesize_next(synth, &chunk) != PIPER_DONE) {
        audio_stream.write(reinterpret_cast<const char *>(chunk.samples),
                           chunk.num_samples * sizeof(float));
    }

    piper_free(synth);

    return 0;
}