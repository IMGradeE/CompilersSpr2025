//
// Created by wilke on 2/25/2025.
//

#ifndef ASSIGNMENT1_GLOBALENUMS_H
#define ASSIGNMENT1_GLOBALENUMS_H
#include <string>

enum tokenTypes{
    OPERATOR,
    INT,
    FLOAT,
    NAME,
    OPENPAREN,
    CLOSEPAREN,
    ENDL,
    WHITESPACE,
    INVALID = -2,
    END = -1
};
std::string tokenIdstrings[] = {
        "OPERATOR",
        "INT",
        "FLOAT",
        "NAME",
        "OPENPAREN",
        "CLOSEPAREN",
        "ENDL",
        "WHITESPACE",
};


enum Symbol{
    ANONYMOUS_START =  INT16_MIN,
    // anonymous non-terminals reside here
    GOAL = -255, // non-terminals
    LINEFULL,
    EXPR,
    LTERM_ADD_SUB,
    LTERM_MULT_DIV,
    RTERM_ADD_SUB,
    RTERM_MULT_DIV,
    ADD_SUB,
    MULT_DIV,
    MULT_DIV_AND_RIGHT_OP,
    POWER_NT,
    POWER_AND_RIGHT_OP,
    LTERM_POWER,
    RTERM_POWER,
    GTERM_SIGN,
    GTERM,
    PARENS,

    // terminals
    NUM = -6,
    NAME_,
    ERROR,
    EOF_ = EOF,
    EPSILON = 0,
    PLUS = 43,
    MINUS = 45,
    MULTIPLY = 42,
    DIVIDE = 47,
    LEFT_PAREN = 40,
    RIGHT_PAREN = 41,
    POWER = 94,
};

class MatchSymbolToToken{
    // focus is a Symbol, word is a TokenType, given a specific Non-Terminal Symbol,
    // and a TokenType, determine if the two match.
    static std::multimap<Symbol, tokenTypes> Map;
    static std::map<tokenTypes, Symbol> InvertedMap;
public:
    static bool match(const Symbol& focus, const tokenTypes& word){
        auto iterators = Map.equal_range(focus);
        for(auto it = iterators.first; it != iterators.second; it = std::next(it,1)){
            if(it->second == word){
                return true;
            }
        }
        return false;
    }
    static Symbol reverse(const std::tuple<int,int,std::string>& token){
        auto it = InvertedMap.find((tokenTypes)std::get<1>(token));
        if (it != InvertedMap.end()){ // not an operator
            return it->second;
        }else{
            const std::string s = std::get<2>(token);
            if (s == "+") {
                return PLUS;
            }else if(s == "-"){
                return MINUS;
            }else if(s == "*"){
                return MULTIPLY;
            }else if(s == "^"){
                return POWER;
            }else if("/"){
                return DIVIDE;
            }
        }
    }
};
std::multimap<Symbol, tokenTypes> MatchSymbolToToken::Map = std::multimap<Symbol, tokenTypes>{
        {NUM, INT},
        {NUM, FLOAT},
        {NAME_, NAME},
        {ERROR, INVALID},
        {EOF_, ENDL},
        {EPSILON, ENDL},
        {PLUS, OPERATOR},
        {MINUS, OPERATOR},
        {MULTIPLY, OPERATOR},
        {DIVIDE, OPERATOR},
        {POWER, OPERATOR},
        {LEFT_PAREN, OPENPAREN},
        {RIGHT_PAREN, CLOSEPAREN},

};
std::map<tokenTypes, Symbol> MatchSymbolToToken::InvertedMap = std::map<tokenTypes, Symbol>{
        {INT, NUM},
        {FLOAT, NUM},
        {NAME, NAME_},
        {INVALID,ERROR},
        {ENDL,EOF_},
        {OPENPAREN,LEFT_PAREN},
        {CLOSEPAREN,RIGHT_PAREN},
};


class SymbolToString {
public:
    static std::map<Symbol, std::string> SymbolMap;

    static std::string getString(int x){
      if(SymbolMap.find({static_cast<const Symbol>(x)}) == SymbolMap.end()){
          std::string s = "ANONYMOUS ";
          s += std::to_string(x);
          return s;
      }else{
          return SymbolMap.at({static_cast<const Symbol>(x)});
      }
    };
};


std::map<Symbol, std::string> SymbolToString::SymbolMap = std::map<Symbol, std::string>{{ ANONYMOUS_START,       "anon-nonterminal"},
                                                                                        {LINEFULL, "LINEFULL"},
                                                                                         {GOAL,                  "GOAL"},
                                                                                         {EXPR,                  "EXPR"},
                                                                                         {LTERM_ADD_SUB,         "LTERM_ADD_SUB" },
                                                                                         {LTERM_MULT_DIV,        "LTERM_MULT_DIV" },
                                                                                         {RTERM_ADD_SUB,         "RTERM_ADD_SUB" },
                                                                                         {RTERM_MULT_DIV,        "RTERM_MULT_DIV" },
                                                                                         {ADD_SUB,               "ADD_SUB" },
                                                                                         {MULT_DIV,              "MULT_DIV" },
                                                                                         {MULT_DIV_AND_RIGHT_OP, "MULT_DIV_AND_RIGHT_OP" },
                                                                                         {POWER_NT,              "POWER_NT" },
                                                                                         {POWER_AND_RIGHT_OP,    "POWER_AND_RIGHT_OP" },
                                                                                         {LTERM_POWER,           "LTERM_POWER" },
                                                                                         {RTERM_POWER,           "RTERM_POWER" },
                                                                                         {GTERM_SIGN,            "GTERM_SIGN" },
                                                                                         {GTERM,                 "GTERM" },
                                                                                         {PARENS,                "PARENS" },
                                                                                         {NUM,                   "num"},
                                                                                         {NAME_,                 "name"},
                                                                                         {ERROR,                 "ERROR"},
                                                                                         {EOF_,                  "eof"},
                                                                                         {EPSILON,               "EPSILON"},
                                                                                         {POWER,              "^"},
                                                                                         {PLUS,                  "+"},
                                                                                         {MINUS,                 "-"},
                                                                                         {MULTIPLY,              "*"},
                                                                                         {DIVIDE,                "/"},
                                                                                         {LEFT_PAREN,            "("},
                                                                                         {RIGHT_PAREN,           ")"},};
#endif //ASSIGNMENT1_GLOBALENUMS_H