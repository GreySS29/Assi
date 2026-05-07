#include "my_whisper.h"


int main () 
{
    VoiceOperate voice;
    std::string result = voice.wish_rec(5);
    if (result.empty()){
        std::cout<< "empty";
        return 1;
    } 
    std::cout << result;
    return 0;
}
