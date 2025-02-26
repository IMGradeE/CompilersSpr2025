//
// Created by wilke on 2/25/2025.
//

#ifndef ASSIGNMENT1_GLOBALENUMS_H
#define ASSIGNMENT1_GLOBALENUMS_H
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
    //EOF
};

enum Symbol{
    ANONYMOUS_START =  INT16_MIN,
    // anonymous non-terminals reside here
    GOAL = -255, // non-terminals
    EXPR,
    EXPR_PRIME,
    TERM,
    TERM_PRIME,
    FACTOR,
    // terminals
    NUM = -4,
    ERROR,
    NAME_,
    EOF_ = EOF,
    EPSILON = 0,
    PLUS = 43,
    MINUS = 45,
    MULTIPLY = 42,
    DIVIDE = 47,
    LEFT_PAREN = 40,
    RIGHT_PAREN = 41,
};

#endif //ASSIGNMENT1_GLOBALENUMS_H
