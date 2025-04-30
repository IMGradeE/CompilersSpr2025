//
// Created by wilke on 4/8/2025.
//
#include <iostream>
#include "SymbolTable.h"
SymbolTable::SymbolTable(){
    table = std::map<std::string/*name*/, SymbolTableEntry>();
}

void SymbolTable::addSymbol(const std::string& key, const int& type){
    ++ref_dec_count;
    auto x = SymbolTable::table.insert({key, {offset, ref_dec_count, ref_dec_count, type}});
    if(x.second) {
         offset -= 4;
    }else{ // referenced
        x.first->second.lastAccessed = ref_dec_count;
    }
}

SymbolTableEntry SymbolTable::getSymbol(std::string key){
    SymbolTableEntry ret = SymbolTableEntry(-1, -1, -1, -1);
    try{
        ret = table.at(key);
        ++ref_dec_count;
    } catch (std::exception &exception) {
        std::cout << "Symbol not found: " << key << '\n';
    }
    return ret;
}

