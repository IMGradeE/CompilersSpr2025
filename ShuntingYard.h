//
// Created by wilke on 3/20/2025.
//

#ifndef ASSIGNMENT1_SHUNTINGYARD_H
#define ASSIGNMENT1_SHUNTINGYARD_H

#include <queue>
#include <vector>
#include <stack>
#include <string>
using namespace std;
class ShuntingYard{

    static void accumulate(string& accumulator, queue<char>& output){
        for(char c: accumulator){
            output.push(c);
        }
        accumulator.clear();
    }
public:
    enum spec{ // OR, CONCAT, and STAR integer values can be compared to establish precedence between op symbols
        RPAREN = -6,
        LPAREN,
        OR,
        CONCAT,
        STAR,
        LAMBDA,
        PLUS = 43,
        MINUS = 45,
        LEFT_PAREN = 40,
        RIGHT_PAREN = 41,
        MULTIPLY = 42,
        DIVIDE = 47,
        POWER = 94,
    };

    static map<spec, pair<int, char>> Map;

    static queue<char> regexShunt(string workingString){
        queue output = queue<char>();
        stack operators = stack<char>();
        char rawToken;
        string accumulator;
        while(!workingString.empty()){ // while there are still characters to process

            rawToken = workingString.front(); // get first character
            workingString = workingString.substr(1); // delete first character from the working string

            if(rawToken >= 0) { // if the token character is not an operator
                accumulator += rawToken;
            }else {
                if(!accumulator.empty()) {
                    accumulate(accumulator, output);
                }
                if (rawToken == STAR || rawToken == OR || rawToken == CONCAT) {
                    while (!operators.empty() && operators.top() != LPAREN && (operators.top() > rawToken || rawToken == operators.top())) {
                        // all operators in our expressions are left associative, and only have the same precedence when they are the same character.
                        accumulator += operators.top();
                        operators.pop();
                        accumulate(accumulator, output);
                    }
                    operators.push(rawToken);
                } else if (rawToken == LPAREN) {
                    operators.push(rawToken);
                } else if (rawToken == RPAREN) {
                    while (operators.top() != LPAREN) {
                        if (operators.empty())
                            throw exception();
                        accumulator += operators.top();
                        accumulate(accumulator, output);
                        operators.pop();
                    }
                    operators.pop(); // discard right paren )
                }
            }
            if(!accumulator.empty() && workingString.empty()){
                accumulate(accumulator, output);
            }
        }
        while(!operators.empty()){
            if (operators.top() == LPAREN || operators.top() == RPAREN)
                throw EACCES;
            accumulator += operators.top();
            accumulate(accumulator, output);
            operators.pop();
        }
        return output;
    }

    static list<string> arithmeticShunt(string workingString){
        list output = list<string>();
        stack operators = stack<pair<spec, pair<int, char>>>();
        char rawToken;
        string accumulator;
        while(!workingString.empty()){ // while there are still characters to process
            rawToken = workingString.front(); // get first character
            if(iscntrl(rawToken)){
                workingString = workingString.substr(2); // delete first character from the working string
                continue;
            }
            workingString = workingString.substr(1); // delete first character from the working string
            auto it = Map.find((spec) rawToken);
            if(it == Map.end() ) { // if the token character is not an operator
                accumulator += rawToken;
            }else {
                if(!accumulator.empty()) {
                    output.push_back(accumulator);
                    accumulator.clear();
                }
                while (!operators.empty() &&  operators.top().first != LEFT_PAREN &&
                        ( operators.top().second.first > it->second.first || (it->second.first ==  operators.top().second.first && it->second.second =='l'))) {
                    accumulator += ((char)  operators.top().first);
                    operators.pop();
                    output.push_back(accumulator);
                    accumulator.clear();
                }
                operators.push(*it);
                if (it->first == LEFT_PAREN) {
                    operators.push(*it);
                } else if (it->first == RIGHT_PAREN) {
                    while (!operators.empty() && operators.top().first != LEFT_PAREN) {
                        if (operators.empty())
                            throw EACCES;
                        accumulator += (char)operators.top().first;
                        output.push_back(accumulator);
                        accumulator.clear();
                        operators.pop();
                    }
                    operators.pop(); // discard right paren )
                }
            }
            if(!accumulator.empty() && workingString.empty()){
                output.push_back(accumulator);
                accumulator.clear();
            }
        }
        while(!operators.empty()){
            if (operators.top().first == LEFT_PAREN || operators.top().first == RIGHT_PAREN)
                throw exception();
            accumulator += (char) operators.top().first;
            output.push_back(accumulator);
            operators.pop();
            accumulator.clear();
        }
        return output;
    }
};

map<ShuntingYard::spec, pair<int, char>> ShuntingYard::Map = map<ShuntingYard::spec, pair<int, char>>{
        {PLUS,{0,'l'}},
        {MINUS,{0,'l'}},
        {MULTIPLY,{1,'l'}},
        {DIVIDE,{1,'l'}},
        {POWER,{2,'r'}},
        {LEFT_PAREN, {3, 'l'} },
        {RIGHT_PAREN, {3, 'l'} },
};

#endif //ASSIGNMENT1_SHUNTINGYARD_H
