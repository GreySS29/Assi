
#include "my_whisper.h"
#include "my_llama.h"
#include "my_piper.h"
#include <memory>
#include <string>

const char start {'s'};
const char end {'q'};
const char* inputAlsa  {"plughw:3,0"};
const char* mic_devices[] = {"plughw:0,0", "plughw:1,0", "plughw:2,0", "default"};


int main () 
{
    char but {};
    string input {};
    std::unique_ptr<VoiceOperate> voice;
    Llama llama;


    bool voice_created = false;
    for (const char* device : mic_devices) {
        try {
            voice = std::make_unique<VoiceOperate>(device);
            voice_created = true;
            std::cout << "Микрофон успешно создан: " << device << '\n';
            break;  
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка с " << device << ": " << e.what() << '\n';
            break; 
        }
    }
    
    if (!voice_created) {
        std::cerr << "Не удалось создать VoiceOperate ни с одним устройством\n";
        return 1;
    }

    
    try
    {  
        while (true) 
        {
            std::cin >> but;
            if (but == end) break;
            if (but == start)
            {
                input = voice->wish_rec(3);
                if (input.empty())
                {
                    std::cout<< "empty";
                    return 1;
                } 
                std::cout << input << '\n';
                string answer = llama.llm_generate(input);
                std::cout << answer<<'\n';
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
