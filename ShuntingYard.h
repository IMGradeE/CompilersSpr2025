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
    static string &getString( vector<pair<tokenTypes, string>> &output, string & accumulator, tokenTypes type ) {
        output.emplace_back(type, accumulator);
        accumulator.clear();
        return accumulator;
    }

    static string &getString(vector<tokenTypes> &vec, vector<pair<tokenTypes, string>> &output, string & accumulator) {
        output.emplace_back(vec.front(), accumulator);
        if(!vec.empty()){
            vec.erase(vec.begin());
        }
        accumulator.clear();
        return accumulator;
    }

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
        UMINUS = -7,
        LEFT_PAREN = 40,
        RIGHT_PAREN = 41,
        MULTIPLY = 42,
        DIVIDE = 47,
        POWER = 94,
    };

    static map<spec, tuple<int, tokenTypes, char>> Map;

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

    static vector<pair<tokenTypes, string>> arithmeticShunt( vector<tokenTypes>& vec, string workingString){
        auto output = vector<pair<tokenTypes, string>>();
        stack operators = stack<pair<spec, tuple<int, tokenTypes, char>>>();
        tokenTypes tokenType;
        char rawToken;
        string accumulator;
        int count = 0;

        while(!workingString.empty()){ // while there are still characters to process
            rawToken = workingString.front(); // get first character
            if(iscntrl(rawToken)){
                workingString = workingString.substr(2); // delete first character from the working string
                vec.erase(vec.begin()); // endl is one token
                if(!accumulator.empty() && workingString.empty()){
                    output.emplace_back(vec.front(), accumulator);
                    accumulator.clear();
                    vec.erase(vec.begin());
                }
                continue;
            }
            workingString = workingString.substr(1); // delete first character from the working string
            auto it = Map.find((spec) rawToken);
            if(it == Map.end() ) { // if the token character is not an operator
                accumulator += rawToken;
            }else { // token is an operator
                if(!accumulator.empty()) {
                    accumulator = getString(vec, output, accumulator);
                }
                if(it->first != LEFT_PAREN && it->first != RIGHT_PAREN){
                    while (!operators.empty() && operators.top().first != LEFT_PAREN &&
                           (get<0>(operators.top().second)> get<0>(it->second) ||
                            (get<0>(it->second) == get<0>(operators.top().second) && get<2>(it->second) == 'l'))) {
                        accumulator += ((char) operators.top().first);
                        accumulator = getString(output, accumulator,
                                                static_cast<tokenTypes>(get<1>(operators.top().second)));
                        operators.pop();
                    }
                    operators.emplace(*it);
                }
                if (it->first == LEFT_PAREN) {
                    operators.emplace(*it);
                } else if (it->first == RIGHT_PAREN) {
                    while (!operators.empty() && operators.top().first != LEFT_PAREN) {
                        if (operators.empty())
                            throw EACCES;
                        accumulator += (char)operators.top().first;
                        accumulator = getString(output, accumulator,
                                                static_cast<tokenTypes>(get<1>(operators.top().second)));
                        operators.pop();
                    }
                    operators.pop(); // discard right paren )
                }
                ++count;
            }
            if(!accumulator.empty() && workingString.empty()){
                accumulator = getString(vec, output, accumulator);
            }
        }
        while(!operators.empty()){
            if (operators.top().first == LEFT_PAREN || operators.top().first == RIGHT_PAREN)
                throw exception();
            accumulator += (char) operators.top().first;
            accumulator = getString(output, accumulator, static_cast<tokenTypes>(get<1>(operators.top().second)));
            operators.pop();
        }
        return output;
    }
};

map<ShuntingYard::spec, tuple<int, tokenTypes, char>> ShuntingYard::Map = map<ShuntingYard::spec, tuple<int, tokenTypes, char>>{
        {PLUS,{0, OPERATOR,'l'}},
        {UMINUS,{2, UNARY_MINUS,'r'}},
        {MINUS,{0, OPERATOR,'l'}},
        {MULTIPLY,{1, OPERATOR,'l'}},
        {DIVIDE,{1, OPERATOR,'l'}},
        {POWER,{2, OPERATOR,'r'}},
        {LEFT_PAREN, {3, OPENPAREN ,'l'} },
        {RIGHT_PAREN, {3,  CLOSEPAREN,'l'} },
};

#endif //ASSIGNMENT1_SHUNTINGYARD_H
