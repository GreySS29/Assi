
#include "include/Record.h"
#include "include/Llama.h"
#include "include/Voice.h"
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
    std::unique_ptr<Record> voice;
    std::unique_ptr<Voice> piper;
    std::unique_ptr<Llama> llama; 

    bool mic_created = false;
    for (const char* device : mic_devices) {
        try {
            voice = std::make_unique<Record>(device);
            mic_created = true;
            std::cout << "Mic was created: " << device << '\n';
            break;  
        }
        catch (const std::exception& e) {
            std::cerr << "Error with " << device << ": " << e.what() << '\n';
            continue;
            
        }
    }

    try { 
        llama = std::make_unique<Llama>();
        std::cout << "Llama was created" << '\n';
    }
    catch (const std::exception& e) {
        std::cerr << "Error during creating the llama model" << e.what() << '\n';
    }

    bool piper_created = false;
    for (const char* device : output_devices){
        try {
            piper = std::make_unique<Voice>(device);
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
        std::cerr << "Record with any device wasn't created\n";
        return 1;
    }

     if (!piper_created) {
        std::cerr << "Piper with any device wasn't created\n";
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
                string answer = llama->llm_generate(input,system_prompt);
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
