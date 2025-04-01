//
// Created by wilke on 3/25/2025.
//

#ifndef ASSIGNMENT1_OPTIMIZER_H
#define ASSIGNMENT1_OPTIMIZER_H
#include <string>
#include <vector>
#include <stdexcept>
#include <list>
#include "GlobalEnums.h"

class ArithmeticOptimizer{
    static std::string calculate_value(int a, int b, char c){
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
    static std::string calculate_value(double a, int b, char c){
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
    static std::string calculate_value(int a, double b, char c){
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
    static std::string calculate_value(double a, double b, char c){
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
public:
    static void erase_optimized(std::vector<std::pair<tokenTypes, std::string>> &postfix, int back_two, int back_one) {
        auto a_it = postfix.begin();
        auto b_it = postfix.begin();
        a_it = std::next(a_it, back_one);
        b_it = std::next(b_it, back_two);
        postfix.erase(a_it);
        postfix.erase(b_it);
    }

    static void optimize(std::vector<std::pair<tokenTypes, std::string>>& postfix){
        // Unary Minus == -7

        /* Takes in a vector of strings as argument.
         * Procedure:
         *      While there is an operator element in the list:
         *          Find the first Binary operator in the list scanning LTR;
         *          If the previous two items are numbers, convert them to numbers and perform the operation indicated by the operator.
         *              Remove the two number-convertible strings, and replace the operator string with the result of the operation.
         *          Else break.
         * */

        if(postfix.size() == 2){ // negative number.
            // make the number negative, return early if it was just a name.
            if(postfix[1].second[0] == UNARY_MINUS) {
                // get the last element if it is a number and negate it, continue. Else, set sentinel false and return.

                tokenTypes back_one_type = postfix[0].first;
                if (back_one_type != NAME) {
                    postfix[0].second = "-" + postfix[0].second;
                    auto it = postfix.begin();
                    std::next(it, 1);
                    postfix.erase(it);
                }
            }
            return;
        }
        else if (postfix.size() == 1 || postfix.size() == 0){
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
                            break;
                        }

                        postfix[back_one].second = "-" + postfix[back_one].second;
                        auto it = postfix.begin();
                        it = std::next(it, i);
                        postfix.erase(it);
                        goto LOOP_HEADER;

                }
                else if(postfix[i].first == OPERATOR){
                    // get the last two elements, if they're both numbers, optimize. Else, set sentinel false and return.
                    tokenTypes back_one_type = postfix[back_one].first;
                    tokenTypes back_two_type = postfix[back_two].first;
                    if(back_one_type == NAME || back_two_type == NAME){
                        break;
                    }
                    char op = postfix[i].second[0];

                    if(back_one_type == INT && back_two_type == INT){
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
