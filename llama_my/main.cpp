#include "my_llama.h"

int main ()
{
    try
    {
    
    Llama llama;
    string input;

    while (true)
    {
       
        if(!std::getline(std::cin, input)) break;
        if(input == "q") break;
        if(input.empty()) continue;

        string answer = llama.llm_generate(input);
        std::cout << answer << std::endl;
    }
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    
    
}