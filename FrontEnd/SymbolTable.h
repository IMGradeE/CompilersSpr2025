//
// Created by wilke on 4/8/2025.
//
#ifndef ASSIGNMENT1_SYMBOLTABLE_H
#define ASSIGNMENT1_SYMBOLTABLE_H
#include <string>
#include <tuple>
#include <map>


class SymbolTableEntry{
public:
    int stackOffset;
    int declaredAt;
    int lastAccessed;
    int varType;
    std::string _register = "";
    SymbolTableEntry(int off, int at, int last, int type_):stackOffset(off), declaredAt(at), lastAccessed(last), varType(type_){}
};

class SymbolTable{
    // name, type, and offset?; scope is implicit (if it was implemented it would just be represented by linked lists of symbol tables.)
    std::map<std::string/*name*/, SymbolTableEntry> table;
    int offset = -4; // todo may need to be static for scoping.
    int ref_dec_count = 0;
public:
    SymbolTable();
    void addSymbol(const std::string& key, const int& type);
    SymbolTableEntry getSymbol(std::string key);
};


#endif //ASSIGNMENT1_SYMBOLTABLE_H
