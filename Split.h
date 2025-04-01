// TODO
//
// Created by wilke on 3/22/2025.
//

#ifndef ASSIGNMENT4_SPLIT_H
#define ASSIGNMENT4_SPLIT_H
#include <string>
#include <vector>


//TODO
std::vector<std::string> split(const std::string& in, const char& delim){
    auto out = std::vector<std::string>();
    std::string accumulator;
    int i = 0;
    for (; i < in.size(); ++i) {
        if(in[i] == delim){
            accumulator += '\0';
            out.push_back(accumulator);
            accumulator.clear();
            while(iscntrl(in[i+1]) && i+1 < in.size()){
                ++i;
            }
        }else{
            accumulator += in[i];
        }
    }
    if(!accumulator.empty()){
        out.push_back(accumulator);
    }
    return out;
}
#endif //ASSIGNMENT4_SPLIT_H
