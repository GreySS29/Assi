
#include "my_whisper.h"
#include "my_llama.h"

const char start {'s'};
const char end {'q'};


int main () 
{
    char but {};
    string input {};

    try
    {
        VoiceOperate voice;
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
                std::cout << answer << std::endl;
            
            }
        }
        return 0;
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
