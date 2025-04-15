//
// Created by wilke on 3/25/2025.
//

#ifndef ASSIGNMENT1_OPTIMIZER_H
#define ASSIGNMENT1_OPTIMIZER_H
#include <string>
#include <vector>
#include <stdexcept>
#include <list>
#include "../Util/GlobalEnums.h"
#include "NASMGenerator.cpp"


class ArithmeticOptimizer{
    std::string calculate_value(int a, const int b, const char c){
        switch ((Symbol) c) {
            case MINUS:
                return  std::to_string(a - b);
            case MULTIPLY:
                return  std::to_string(a * b);
            case DIVIDE:
                if(b == 0){
                    return "div_by_0ERR";
                }
                return  std::to_string(a / b);
            case PLUS:
                return  std::to_string(a + b);
            case POWER:
                for (int i = 1, r = a; i < b; ++i) {
                    a = a*r;
                }
                return std::to_string(a);
        }
        return "";
    }
    std::string calculate_value(std::string a, const int b, const char c){ // name operand
        // find value in symbol table (must exist by this point)
        // todo return the name of the memory location or register where the result lives
        if (a[0] == '['){
            string x = "dword " + a;
            a = x;
        }
        switch ((Symbol) c) {
            case MINUS:
                nsg.write("mov eax, "+a+"\n"
                                "sub eax,"+ to_string(b)+"\n"); // eax = a-b
                break;
            case MULTIPLY:
                nsg.write("mov eax, "+a+"\n"
                                "mov edi,"+ to_string(b)+"\n"
                                "mul edi\n"); // eax = a*b
                break;
            case DIVIDE:
                if(b == 0){
                    nsg.write("mov esi, div_by_0\n"
                           "mov edi, fmtstr\n"
                           "print esi, edi\n");
                }else{
                    nsg.write("mov edx,0\n"
                              "mov eax, "+a+"\n"
                              "mov esi,"+to_string(b)+"\n"
                              "div esi\n"); // eax = a/b ; quotient in eax, remainder in edx
                }
                break;
            case PLUS:
                nsg.write("mov eax, "+a+"\n"
                                "add eax,"+ to_string(b)+"\n"); // eax = a+b
                break;
            case ASSIGN:
                nsg.write("mov "+a+", "+ to_string(b)+"\n");
                return "";
        }
        return "eax";
    }

    std::string calculate_value(const int a, std::string b, const char c){ // name operand
        // find value in symbol table (must exist by this point)
        // todo return the name of the memory location or register where the result lives
        if (b[0] == '['){
            string x = "dword " + b;
            b = x;
        }
        switch ((Symbol) c) {
            case MINUS:
                nsg.write("mov eax, "+ to_string(a)+"\n"
                                        "sub eax,"+b+"\n"); // eax = a-b
                break;
            case MULTIPLY:
                nsg.write("mov eax, "+ to_string(a)+"\n"
                                        "mov edi,"+b+"\n"
                                         "mul edi\n"); // eax = a*b
                break;
            case DIVIDE:
                nsg.write("mov edx,0\n"
                                "mov eax, "+to_string(a)+"\n"
                                "div "+b+"\n"); // eax = a/b ; quotient in eax, remainder in edx Todo might need register
                break;
            case PLUS:
                nsg.write("mov eax, "+to_string(a)+"\n"
                                "add eax,"+b+"\n"); // eax = a+b
                break;
        }
        return "eax";
    }

    std::string calculate_value(const std::string a, std::string b, const char c){ // name operand
        // find value in symbol table (must exist by this point)
        // todo fix this it's definitely broken
        switch ((Symbol) c) {
            case MINUS:
                nsg.write("mov eax, "+a+"\n"
                                 "sub eax, "+b+"\n"); // eax = a-b
                break;
            case MULTIPLY:
                nsg.write("mov eax, "+a+"\n"
                                "mov edi,"+b+"\n"
                                 "mul edi\n"); // eax = a*b
                break;
            case DIVIDE:
                    if (b[0] == '['){
                        string x = "dword " + b;
                        b = x;
                    }
                    nsg.write("mov edx,0\n"
                              "mov eax, "+a+"\n"
                                "div "+b+"\n"); // eax = a/b ; quotient in eax, remainder in edx
                break;
            case PLUS:
                nsg.write("mov "+a+",eax\n"
                                    "add eax,"+b+"\n"); // eax = a+b
                break;
            case ASSIGN:
                if (b[0] == '['){
                    nsg.write("mov eax, "+b+"\n"
                                    "mov "+a+", eax\n");
                }else{
                    nsg.write("mov "+a+", "+b+"\n");
                }
                return "";
        }
        return "eax";
    }


    // TODO handle PRINT (unary)
    std::string calculate_value(double a, int b, char c){
        switch ((Symbol) c) {
            case MINUS:
                return  std::to_string(a - b);
            case MULTIPLY:
                return  std::to_string(a * b);
            case DIVIDE:
                if(b == 0){
                    return "div_by_0ERR";
                }
                return  std::to_string(a / b);
            case PLUS:
                return  std::to_string(a + b);
            case POWER:
                double r = a;
                for (int i = 1, r = a; i < b; ++i) {
                    a = a*r;
                }
                return std::to_string(a);
        }
        return "";

    }
    std::string calculate_value(int a, double b, char c){
        switch ((Symbol) c) {
            case MINUS:
                return  std::to_string(a - b);
            case MULTIPLY:
                return  std::to_string(a * b);
            case DIVIDE: // TODO integer division
                if(b == 0){
                    return "div_by_0ERR";
                }
                return  std::to_string(a / b);
            case PLUS:
                return  std::to_string(a + b);
            case POWER: // TODO Fractional Exponentiation
                for (int i = 1, r = a; i < b; ++i) {
                    a = a*r;
                }
                return std::to_string(a);
        }
        return "";

    }
    std::string calculate_value(double a, double b, char c){
        switch ((Symbol) c) {
            case MINUS:
                return  std::to_string(a - b);
            case MULTIPLY:
                return  std::to_string(a * b);
            case DIVIDE:
                if(b == 0){
                    return "div_by_0ERR";
                }
                return  std::to_string(a / b);
            case PLUS:
                return  std::to_string(a + b);
            case POWER:
                double r = a;
                for (int i = 1; i < b; ++i) {
                    a = a*r;
                }
                return std::to_string(a);
        }
        return "";
    }
    SymbolTable* symTable;
    NASMGenerator& nsg;
public:
    ArithmeticOptimizer(SymbolTable* symbolTable,NASMGenerator& nsg_):symTable(symbolTable), nsg(nsg_){}
    void erase_optimized(std::vector<std::pair<tokenTypes, std::string>> &postfix, int back_two, int back_one) {
        auto a_it = postfix.begin();
        auto b_it = postfix.begin();
        a_it = std::next(a_it, back_one);
        b_it = std::next(b_it, back_two);
        postfix.erase(a_it);
        postfix.erase(b_it);
    }

    void optimize(std::vector<std::pair<tokenTypes, std::string>>& postfix){
        // Unary Minus == -7

        /* Takes in a vector of strings as argument.
         * Procedure:
         *      While there is an operator element in the list:
         *          Find the first Binary operator in the list scanning LTR;
         *          If the previous two items are numbers, convert them to numbers and perform the operation indicated by the operator.
         *              Remove the two number-convertible strings, and replace the operator string with the result of the operation.
         *          Else break.
         * */


        if (postfix.size() == 1 || postfix.size() == 0){
            // do nothing
            return;
        }
        bool sentinel = true;

        LOOP_HEADER:
        while(sentinel){
            for (int back_two = -1, back_one = 0, i = 1; i < postfix.size();  ++back_two, ++back_one, ++i) { // iterator to elements is next(begin(), i + offset)
                if(postfix[i].second[0] == UNARY_MINUS){
                    // get the last element if it is a number and negate it, continue. Else, set sentinel false and return.

                        tokenTypes back_one_type = postfix[back_one].first;
                        if(back_one_type == NAME){
                            if (postfix[back_one].second[0] == '['){
                                string x = "dword " + postfix[back_one].second;
                                nsg.write("NEG "+x+"\n");
                            }else{
                                nsg.write("NEG "+postfix[back_one].second+"\n");
                            }
                        }else{
                            postfix[back_one].second = "-" + postfix[back_one].second;
                        }
                        auto it = postfix.begin();
                        it = std::next(it, i);
                        postfix.erase(it);
                        goto LOOP_HEADER;
                }
                else if(postfix[i].first == PRINT_){
                    nsg.write("\tmov esi, "+postfix[back_one].second+"\n"
                              "\tmov edi, fmtint\n"
                           "\tmov eax, 0 ; I don't know if this is necessary\n"
                           "\tcall printf\n");
                    auto printed = postfix.begin();
                    auto call = postfix.begin();
                    postfix.erase(std::next(call, i));
                    postfix.erase(std::next(printed, back_one));
                }
                else if(postfix[i].first == OPERATOR){
                    // get the last two elements, if they're both numbers, optimize. Else, set sentinel false and return.
                    tokenTypes back_one_type = postfix[back_one].first;
                    tokenTypes back_two_type = postfix[back_two].first;
                    char op = postfix[i].second[0];
                    if(back_one_type == NAME && back_two_type == NAME){
                        string b = postfix[back_one].second;
                        string a = postfix[back_two].second;
                        postfix[i].second = calculate_value(a, b, op);
                        postfix[i].first = NAME;
                        erase_optimized(postfix, back_two, back_one);
                    }else if(back_one_type == NAME && back_two_type != NAME){
                        string b = postfix[back_one].second;
                        int a = std::stoi(postfix[back_two].second);
                        postfix[i].second = calculate_value(a, b, op);
                        postfix[i].first = NAME;
                        erase_optimized(postfix, back_two, back_one);
                    }else if(back_one_type != NAME && back_two_type == NAME){
                        int b = std::stoi(postfix[back_one].second);
                        string a = postfix[back_two].second;
                        postfix[i].second = calculate_value(a, b, op);
                        postfix[i].first = NAME;
                        erase_optimized(postfix, back_two, back_one);
                    }else if(back_one_type == INT && back_two_type == INT){
                        int b = std::stoi(postfix[back_one].second);
                        int a = std::stoi(postfix[back_two].second);
                        postfix[i].second = calculate_value(a, b, op);
                        postfix[i].first = /*TODO*/ INT;
                        erase_optimized(postfix, back_two, back_one);

                    }else if(back_one_type == FLOAT && back_two_type == FLOAT){
                        double b = std::stod(postfix[back_one].second);
                        double a = std::stod(postfix[back_two].second);
                        postfix[i].second = calculate_value(a, b, op);
                        postfix[i].first = FLOAT;
                        erase_optimized(postfix, back_two, back_one);

                    }else if(back_one_type == FLOAT && back_two_type == INT){
                        double b = std::stod(postfix[back_one].second);
                        int a = std::stoi(postfix[back_two].second);
                        postfix[i].second = calculate_value(a, b, op);
                        postfix[i].first = FLOAT;
                        erase_optimized(postfix, back_two, back_one);

                    }else if(back_one_type == INT && back_two_type == FLOAT){
                        int b = std::stoi(postfix[back_one].second);
                        double a = std::stod(postfix[back_two].second);
                        postfix[i].second = calculate_value(a, b, op);
                        postfix[i].first = FLOAT;
                        erase_optimized(postfix, back_two, back_one);

                    }
                    goto LOOP_HEADER;
                }
            }
            break;
        }
    }
};
#endif //ASSIGNMENT1_OPTIMIZER_H
