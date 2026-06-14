
#include "my_whisper.h"
#include "my_llama.h"
#include "my_piper.h"
#include <string>

const char start {'s'};
const char end {'q'};
const char* inputAlsa  {"plughw:3,0"};
const char* inputMic  {"plughw:0,0"};


int main () 
{
    char but {};
    string input {};
    
    try
    {
        VoiceOperate voice(inputMic);
        Llama llama;
        while (true) 
        {
            std::cin >> but;
            if (but == end) break;
            if (but == start)
            {
                input = voice.wish_rec(3);
                if (input.empty())
                {
                    std::cout<< "empty";
                    return 1;
                } 
                std::cout << input << '\n';
                string answer = llama.llm_generate(input);
                My_piper piper(answer.data(),inputAlsa);
                std::cout << answer << '\n';
                
            }
        }
        return 0;
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
