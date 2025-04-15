cmake_minimum_required(VERSION 3.22)
project(assignment1)

set(CMAKE_CXX_STANDARD 20)

add_executable(assignment1
        FrontEnd/LL1ArithmeticParser.cpp
        FrontEnd/Optimizer.h
        FrontEnd/ShuntingYard.h
        Util/DFASerializer.cpp
        Util/DFASerializer.h
        Util/Split.h
        FrontEnd/DFAState.h
        FrontEnd/Tokenizer.cpp
        FrontEnd/NASMGenerator.cpp
        BackEnd/NASMGenerator.h
        FrontEnd/SymbolTable.cpp
        FrontEnd/SymbolTable.h
)

