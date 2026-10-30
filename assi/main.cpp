
#include "my_whisper.h"
#include "include/Llama.h"
#include "my_piper.h"
#include <memory>
#include <string>

using   str = std::string;

const char start {'s'};
const char end {'q'};
const char* mic_devices[] = {"plughw:0,0", "plughw:1,0", "plughw:2,0","default"};
const char* output_devices[] = {"plughw:3,0","plughw:3,2","default"};
const str system_prompt = "You are English teacher. Answer very shortly, 1 or 2 sentences and stop";

int main () 
{
    char but {};
    string input {};
    std::unique_ptr<VoiceOperate> voice;
    std::unique_ptr<My_piper> piper;
    Llama llama;

    bool mic_created = false;
    for (const char* device : mic_devices) {
        try {
            voice = std::make_unique<VoiceOperate>(device);
            mic_created = true;
            std::cout << "Микрофон успешно создан: " << device << '\n';
            break;  
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка с " << device << ": " << e.what() << '\n';
            continue;
            
        }
    }

    bool piper_created = false;
    for (const char* device : output_devices){
        try {
            piper = std::make_unique<My_piper>(device);
            piper_created = true;
            std::cout << "Piper created: " << device << '\n';
            break;
        }
        catch (const std:: exception& e){
            std::cerr << "Error: " << device << ": " << e.what() << '\n';
            continue;
           
        }
    }

    if (!mic_created) {
        std::cerr << "Не удалось создать VoiceOperate ни с одним устройством\n";
        return 1;
    }

     if (!piper_created) {
        std::cerr << "Не удалось создать Piper ни с одним устройством\n";
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
                string answer = llama.llm_generate(input,system_prompt);
                std::cout << answer<<'\n';
                piper->start(answer.data());
                
                
            }
        }
        return 0;
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
