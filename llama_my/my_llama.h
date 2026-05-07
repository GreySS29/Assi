#pragma once
#include "llama.h"
#include <string>
#include <iostream>
#include <vector>

using std::string;

void error(const string& s);


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