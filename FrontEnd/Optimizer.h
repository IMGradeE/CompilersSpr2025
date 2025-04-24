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
    int loopNumber = 0;
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
                nsg.write("\tmov eax, "+a+"\n"
                                "\tsub eax, "+ to_string(b)+"\n"); // eax = a-b
                break;
            case MULTIPLY:
                nsg.write("\tmov eax, "+a+"\n"
                                "\tmov edi, "+ to_string(b)+"\n"
                                "\timul edi\n"); // eax = a*b
                break;
            case DIVIDE:
                if(b == 0){
                    nsg.write("\tmov esi, div_by_0\n"
                           "\tmov edi, fmtstr\n"
                           "\tprint esi, edi\n");
                }else{
                    nsg.write("mov edx,0\n"
                              "\tmov eax, "+a+"\n"
                              "\tmov esi, "+to_string(b)+"\n"
                              "\tcdq\n"
                              "\tidiv esi\n"); // eax = a/b ; quotient in eax, remainder in edx
                }
                break;
            case PLUS:
                nsg.write("\tmov eax, "+a+"\n"
                                "\tadd eax, "+ to_string(b)+"\n"); // eax = a+b
                break;
            case ASSIGN:
                nsg.write("\tmov "+a+", "+ to_string(b)+"\n");
                return "";
            case POWER:
                if(b > 1) {
                    ++this->loopNumber;
                    nsg.write(
                        "\tmov esi, " + a + ";loop preamble for exponentiation\n"
                        "\tmov eax, " + a + "\n"
                        "\tmov r8d, " + to_string(b) + "\n"
                        "exploop" +to_string(this->loopNumber) + ":\n"
                        "\timul esi\n"
                        "\tsub r8d, 1\n"
                        "\tcmp r8d, 1\n"
                        "\tjne exploop" +to_string(this->loopNumber) + "\n"
                    );
                }else{
                    nsg.write("\tmov eax, "+a+"\n");
                }
                break;
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
                nsg.write("\tmov r8d, "+ to_string(a)+"\n"
                                        "\tsub r8d,"+b+"\n"); // eax = a-b
                return "r8d";
            case MULTIPLY:
                nsg.write("\tmov eax, "+ to_string(a)+"\n"
                                        "\tmov edi, "+b+"\n"
                                         "\timul edi\n"
                                         "\tmov r9d, eax\n"); // eax = a*b
                return "r9d";
            case DIVIDE:
                nsg.write("\tmov edx, 0\n"
                                "\tmov eax, "+to_string(a)+"\n"
                                "\tcdq\n"
                                "\tidiv "+b+"\n"
                               "\tmov r10d, eax\n"); // eax = a/b ; quotient in eax, remainder in edx Todo might need register
                return "r10d";
            case PLUS:
                if(b != "r8d") {
                    nsg.write("\tmov r8d, " + to_string(a) + "\n"
                                                            "\tadd r8d, " + b + "\n"); // eax = a+b
                    return "r8d";
                }else{
                    nsg.write("\tmov r11d, " + to_string(a) + "\n"
                                                             "\tadd r11d, " + b + "\n");
                    return "r11d";
                }
            case POWER:
                ++this->loopNumber;
                nsg.write(
                    "\tcmp dword "+b+", 0\n" // if  0 set eax to 1 and return it. else if exponent is 1, return a in eax, else loop
                           "\tjle zeroExpCond"+to_string(this->loopNumber)+"\n" //we're not handling negative exponents
                           "\tcmp dword "+b+", 1\n" // if  0 set eax to 1 and return it. else if exponent is 1, return a in eax, else loop
                           "\tje zeroExpCondElse"+to_string(this->loopNumber)+"\n"
                          "\tjmp preloop"+to_string(this->loopNumber)+"\n"
                        "zeroExpCond"+to_string(this->loopNumber)+":\n" // should do this via a single label that jumps to a dynamic label but this also works.
                            "\tmov eax, 1\n"
                            "\tjmp afterloop"+to_string(this->loopNumber)+"\n"
                        "zeroExpCondElse"+to_string(this->loopNumber)+":\n"
                          "\tmov eax, " + to_string(a) + ";loop preamble for exponentiation\n" // this should really have a conditional jump after but idc this works
                          "\tjmp afterloop"+to_string(this->loopNumber)+"\n"
                        "\tpreloop"+to_string(this->loopNumber)+":\n"
                        "\tmov eax, " + to_string(a) + ";loop preamble for exponentiation\n"
                        "\tmov esi, " + to_string(a) + "\n"
                        "\tmov r8d, "+ b + "\n"
                        "exploop" +to_string(this->loopNumber) + ":\n"
                        "\timul esi\n"
                        "\tsub r8d, 1\n"
                        "\tcmp r8d, 1\n" // todo maybe needs to be one
                        "\tjne exploop" +to_string(this->loopNumber) + "\n"
                        "afterloop"+to_string(this->loopNumber)+":\n"
                );


                break;
        }
        return "eax";
    }

    std::string calculate_value(const std::string a, std::string b, const char c){ // name operand
        // find value in symbol table (must exist by this point)
        // todo fix this it's definitely broken
        switch ((Symbol) c) {
            case MINUS:
                nsg.write("\tmov eax, "+a+"\n"
                                 "\tsub eax, "+b+"\n"
                                 "\tmov r8d, eax\n"); // eax = a-b
                return "r8d";
            case MULTIPLY:
                nsg.write("\tmov eax, "+a+"\n"
                                "\tmov edi, "+b+"\n"
                                 "\timul edi\n"
                                 "\tmov r9d, eax\n"); // eax = a*b
                return "r9d";
            case DIVIDE:
                    if (b[0] == '['){
                        string x = "\tdword " + b;
                        b = x;
                    }
                    nsg.write("\tmov edx,0\n"
                              "\tmov eax, "+a+"\n"
                                "\tcdq\n"
                                "\tidiv "+b+"\n"
                                           "\tmov r10d, eax\n"); // eax = a/b ; quotient in eax, remainder in edx
                return "r10d";
            case PLUS:
                nsg.write("\tmov r11d, "+a+"\n"
                                    "\tmov edi, "+b+"\n"
                                    "\tadd r11d, edi\n"); // eax = a+b
                return "r11d";
            case ASSIGN:
                if (b[0] == '['){
                    nsg.write("\tmov eax, "+b+"\n"
                                    "\tmov "+a+", eax\n");
                }else{
                    nsg.write("\tmov "+a+", "+b+"\n");
                }
                return "";

            case POWER:
                ++this->loopNumber;
                ++this->loopNumber;
                nsg.write(
                    "\tcmp dword "+b+", 0\n" // if  0 set eax to 1 and return it. else if exponent is 1, return a in eax, else loop
                    "\tjle zeroExpCond"+to_string(this->loopNumber)+"\n" //we're not handling negative exponents
                    "\tcmp dword "+b+", 1\n" // if  0 set eax to 1 and return it. else if exponent is 1, return a in eax, else loop
                    "\tje zeroExpCondElse"+to_string(this->loopNumber)+"\n"
                    "\tjmp preloop"+to_string(this->loopNumber)+"\n"
                    "zeroExpCond"+to_string(this->loopNumber)+":\n" // should do this via a single label that jumps to a dynamic label but this also works.
                    "\tmov eax, 1\n"
                    "\tjmp afterloop"+to_string(this->loopNumber)+"\n"
                    "zeroExpCondElse"+to_string(this->loopNumber)+":\n"
                    "\tmov eax, " + a + ";loop preamble for exponentiation\n" // this should really have a conditional jump after but idc this works
                    "\tjmp afterloop"+to_string(this->loopNumber)+"\n"
                    "\tpreloop"+to_string(this->loopNumber)+":\n"
                    "\tmov eax, " + a + ";loop preamble for exponentiation\n"
                    "\tmov esi, " + a + "\n"
                    "\tmov r8d, "+ b + "\n"
                    "exploop" +to_string(this->loopNumber) + ":\n"
                    "\timul esi\n"
                    "\tsub r8d, 1\n"
                    "\tcmp r8d, 1\n" // todo maybe needs to be one
                    "\tjne exploop" +to_string(this->loopNumber) + "\n"
                    "afterloop"+to_string(this->loopNumber)+":\n"
                );

                break;
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
        string str = ";";
        for (auto x: postfix) {
            str += x.second;
        }
        nsg.write(str);
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
                            nsg.write("\tmov r11d, "+postfix[back_one].second+"\n"
                                      "\tNEG r11d\n");
                            postfix[back_one].second = "r11d";
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
