#include "llama.h" // берет с папки wishper
#include <string>
#include <iostream>
#include <vector>
#include<iostream>



using std::string;

void error(const string& s)
{
    throw std::runtime_error(s);
};

class Llama {
    public:
    Llama();
    ~Llama();
    string llm_generate(const string& prompt);


    private:
    llama_model* model_;
    llama_context* ctx_;
    const llama_vocab* vocab_;
    llama_sampler* sampler_;

};

Llama::Llama(){
    llama_backend_init();
    
    llama_model_params model_param = llama_model_default_params();
    model_ = llama_model_load_from_file("../../llama.cpp/Llama-3.2-3B-Instruct-Q4_K_M.gguf",model_param);
    if (!model_) error("The model wasn't loaded");

    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = 2048;
    ctx_params.n_batch = 512;
    ctx_ = llama_init_from_model(model_, ctx_params);
    if(!ctx_) {
        llama_model_free(model_);
        error("The context wasn't created");
    }

    vocab_ = llama_model_get_vocab(model_);

    llama_sampler_chain_params sparams = llama_sampler_chain_default_params();
    sampler_ = llama_sampler_chain_init(sparams);
    llama_sampler_chain_add(sampler_, llama_sampler_init_greedy());
}

Llama::~Llama()
{
    llama_sampler_free(sampler_);
    llama_free(ctx_);
    llama_model_free(model_);
    llama_backend_free();
}


string Llama::llm_generate(const string& prompt)
{
    string system = "You are usefull assistent. Answer in 1–2 short sentences";
    string full_prompt = system + "\nMe: " + prompt + "\nAssi: ";
    

    int n_prompt = -llama_tokenize(vocab_,full_prompt.c_str(),
        full_prompt.size(), nullptr, 0 , true, true);
    //Если вы передаете nullptr в качестве буфера (аргумент tokens), функция не записывает токены, а просто возвращает количество токенов, которое было бы записано, если бы вы выделили достаточно места.
    //Отрицательное значение: Если количество токенов получается больше, чем размер переданного буфера (в вашем случае 0), функция возвращает отрицательное число, модуль которого равен требуемому количеству. Например, если нужно 15 токенов, функция вернет -15.

    if(n_prompt <=0) error("Tokenizition error");

    std::vector<llama_token> tokens(n_prompt); // n_prompt дает количество 
    int written = llama_tokenize(vocab_, full_prompt.c_str(), 
        full_prompt.size(), tokens.data(),tokens.size(), true,true);

    if(written<0) error("Tokenizition error");

    llama_batch batch = llama_batch_get_one(tokens.data(), written);
    
    if (llama_decode(ctx_, batch) !=0) error ("llama_decode error");

    string response;
    for(int i=0; i <64; ++i)
    {
        llama_token tok = llama_sampler_sample(sampler_,ctx_, -1);

        if (llama_vocab_is_eog(vocab_, tok)) break;
         if (response.find("\nMe:") != std::string::npos) break;

        char buf[64];
        int n = llama_token_to_piece(vocab_, tok, buf,sizeof(buf),0,true);

        if (n<0) break;

        response.append(buf,n);

        batch = llama_batch_get_one(&tok,1);
        if(llama_decode(ctx_, batch) !=0) break;
    }

    return response;


};


int main ()
{
    try
    {
    Llama llama;
    
    std::string input;

    while (true)
    {
       
        if(!std::getline(std::cin, input)) break;
        if(input == "q") break;
        if(input.empty()) continue;

        std::string answer = llama.llm_generate(input);
        std::cout << answer << std::endl;
    }
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    
    
}