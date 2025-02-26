//
// Created by wilke on 2/19/2025.
//

/*REFERENCE (Gemini)
#include <iostream>
#include <vector>
#include <map>

enum class Symbol {
    PLUS, NUMBER, EXPRESSION
};

struct Production {
    Symbol lhs;
    std::vector<Symbol> rhs;
};

int main() {
    std::vector<Production> grammar = {
        {Symbol::EXPRESSION, {Symbol::EXPRESSION, Symbol::PLUS, Symbol::NUMBER}},
        {Symbol::EXPRESSION, {Symbol::NUMBER}}
    };

    std::map<Symbol, std::vector<std::vector<Symbol>>> grammarMap;
    for (const auto& prod : grammar) {
        grammarMap[prod.lhs].push_back(prod.rhs);
    }

    // Print the grammar
    for (const auto& [lhs, rhs_list] : grammarMap) {
        std::cout << "Production for: ";
        if (lhs == Symbol::EXPRESSION) {
            std::cout << "Expression" << std::endl;
        }
        for (const auto& rhs : rhs_list) {
            std::cout << "  ::= ";
            for (const auto& symbol : rhs) {
                if (symbol == Symbol::PLUS) {
                    std::cout << "+ ";
                } else if (symbol == Symbol::NUMBER) {
                    std::cout << "Number ";
                } else if (symbol == Symbol::EXPRESSION) {
                    std::cout << "Expression ";
                }
            }
            std::cout << std::endl;
        }
    }

    return 0;
}
 * */

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include "GlobalEnums.h"

using namespace std;

typedef struct Production{
    Symbol lhs;
    vector<vector<Symbol>> rhs;
}Production;

class Parser{
public:
    int MIN_NONTERMINAL = EPSILON;
    int last_generated_non_terminal = ANONYMOUS_START;
    /*virtual void ParserGenerator();
    virtual void ConstructTable();
    virtual void SkeletonParser();*/

    void leftRecursionToRightRecursion(){
        for (int i = 0; i < grammar.size(); ++i){
            for (int j = 0; j < grammar[i].rhs.size(); ++j) {
                leftRecursionToRightRecursion(i,j);
            }
        }
    }

    void leftRecursionToRightRecursion(int i, int k){
        if(MIN_NONTERMINAL > grammar[i].lhs && grammar[i].rhs[k][0] == grammar[i].lhs){
            // rhs of any such production rhs[index>0] is inserted as the prefix in the production from the generated non-terminal to the generated non-terminal.
            // generated non-terminal also receives an epsilon production
            auto p = Production();
            p.lhs = (Symbol) last_generated_non_terminal;
            p.rhs = vector<vector<Symbol>>();

            auto epsilonProd = vector<Symbol>(1, EPSILON);
            auto selfProd = vector<Symbol>();
            selfProd.insert(selfProd.begin(), next(grammar[i].rhs[k].begin(), 1),grammar[i].rhs[k].end());
            selfProd.push_back(p.lhs);
            p.rhs.push_back(selfProd);
            p.rhs.push_back(epsilonProd);
            grammar.push_back(p);

            auto currentProd = grammar[i].rhs.begin();
            currentProd = next(currentProd, k+1);
            auto replacementProduction = vector<vector<Symbol>>();

            while (currentProd != grammar[i].rhs.end()){ // construct the replacement productions from all productions that start with a terminal
                if(grammar[i].lhs != (*currentProd)[0]){
                    auto newRightHandSide = vector<Symbol>();
                    newRightHandSide.insert(newRightHandSide.begin(), currentProd[0].begin(),currentProd[0].end());
                    newRightHandSide.push_back((Symbol) last_generated_non_terminal);
                    replacementProduction.push_back(newRightHandSide);
                }
                currentProd = next(currentProd, 1);
            }
            grammar[i].rhs = replacementProduction;
            ++last_generated_non_terminal;
        }
    }

    void removeIndirectLeftRecursion(){
        for(int i = 0; i < grammar.size(); i++){
            for (int j = 0; j < i; ++j) {
                for (int k = 0; k < grammar[i].rhs.size(); ++k) {
                    // check if sequence starts with the nonterminal in the lhs at index j
                    if(MIN_NONTERMINAL > grammar[j].lhs && grammar[i].rhs[k][0] == grammar[j].lhs){ // A_s is the first symbol in a production whose lhs is A_i
                        // replace production A_i -> A_s{} by expanding A_s
                        grammar[i].rhs[k].erase(grammar[i].rhs[k].begin());

                        auto postNonTermSequence = vector<Symbol>(grammar[i].rhs[k].size());

                        std::copy(grammar[i].rhs[k].begin(), grammar[i].rhs[k].end(), postNonTermSequence.begin());

                        int x = k;
                        for (auto& prod: grammar[j].rhs) {
                            if(x+1 > grammar[i].rhs.size()) grammar[i].rhs.push_back(postNonTermSequence);
                            grammar[i].rhs[x].insert(grammar[i].rhs[x].begin(),prod.begin(), prod.end()); // expand every production in A_s at the beginning of A_i's
                            ++x;
                        }
                    }
                    leftRecursionToRightRecursion(i,k);
                }
            }
        }
    };

    string grammarToString(){
        string ret;
        for (auto& production: grammar) {
            int count =0;
            ret += '\n';
            ret += to_string(production.lhs);
            ret += " ->";
            for(auto& yields : production.rhs){
                if(count != 0){ ret += "\t  | ";}
                else{++count;}
                for (const auto& symbol: yields) {
                    ret+= to_string(symbol);
                    ret += ' ';
                }
                ret += '\n';
            }
        }
        return ret;
    }

    vector<Production> grammar = vector<Production>();
};

class ArithmeticParser:public Parser{
private:
    ArithmeticParser(): Parser(){}
    static ArithmeticParser* singleton;
public:
    set<Symbol> First(const Symbol& product){
        if(product < NUM){ // product is nonterminal
            auto ret = set<Symbol>();
            for(auto& p : grammar){
                if(p.lhs == product){
                    for(auto& can_yield : p.rhs){
                        if(can_yield[0] >= NUM){
                            ret.insert(can_yield[0]);
                        }else{
                            ret.merge(First(can_yield[0]));
                        }
                    }
                    break;
                }
            }
            return ret;
        }else{
            return {product};
        }
    }

    map<Symbol, set<Symbol>> getFirstSet(){
        const Symbol arr[] = {NUM,NAME_,EOF_,EPSILON,PLUS,MINUS,MULTIPLY,DIVIDE,LEFT_PAREN,RIGHT_PAREN,};
        auto first = map<Symbol, set<Symbol>>();
        for (const auto& symbol:arr) {// num is the lowest valued non-terminal
            first.insert({symbol, {symbol}}); // first(a)->a
        }

        while(true) {
            auto oldFirst = first;
            for (auto &production: grammar) { // each lhs->rhs
                if (first.find(production.lhs) == first.end()) {
                    first.insert({production.lhs, {}}); // first(A)-> <EmptySet>
                }
                // B_1 is either a terminal who's first set is empty, so we remove epsilon and insert the empty set, or
                //  we insert the first set for this non-terminal
                for (auto& yields: production.rhs){
                    auto rhs = set<Symbol>();
                    auto temp = First(yields[0]);
                    temp.erase(EPSILON);
                    rhs.merge(temp);
                    bool trailing = true;
                    for (int i = 0; i < (yields.size() - 1); ++i) {
                        if(i<(yields.size() - 2)){trailing = false; break;}// each production option in rhs a_1A_1B_1 | b_1A_2 | ...
                        temp = First(yields[i]);
                        if (temp.find(EPSILON) != temp.end()) { // contains on c++20
                            temp = First(yields[i + 1]);
                            temp.erase(EPSILON);
                            rhs.merge(temp);
                        } else {
                            trailing = false;
                            break;
                        }
                    }
                    temp = First(yields[yields.size() - 1]);
                    if (trailing && temp.find(EPSILON) != temp.end()) {
                        rhs.merge(set{EPSILON});
                    }
                    first.at(production.lhs).merge(rhs);
                }
            }
            if(oldFirst == first){
                break;
            }
        }
        return first;
    }

    map<Symbol, set<Symbol>> getFollowSet( const map<Symbol, set<Symbol>>& first){
        auto follow = map<Symbol, set<Symbol>>();
        for(const auto& lhs : first ){
            if(lhs.first < NUM){
                follow.insert({lhs.first, {}});
            }else{
                break;
            }
        }
        follow.at(GOAL) = {EOF_};

        while(true) {
            auto oldFollow = follow;
            for (auto &production: grammar) { // each lhs->rhs
                // B_1 is either a terminal who's follow set is empty, so we remove epsilon and insert the empty set, or
                //  we insert the follow set for this non-terminal
                for (auto& yields: production.rhs){
                    auto trailer = follow.at(production.lhs);
                    for (int i = yields.size() - 1; i >= 0; --i) {
                        auto B_i = yields[i];
                        if(B_i< NUM){ // nonterminal
                            auto temp = trailer;
                            follow.at(B_i).merge(temp);
                            auto t = First(B_i);
                            if(t.find(EPSILON) != t.end()){
                                t.erase(EPSILON);
                                trailer.merge(t);
                            }else{
                                trailer = t;
                            }
                        }else{
                            trailer = {B_i};
                        }
                    }
                }
            }
            if(oldFollow == follow){
                break;
            }
        }
        return follow;
    }

    map<pair<Symbol, Symbol>, set<Symbol>> getStartSet( const map<Symbol, set<Symbol>>& first, const map<Symbol, set<Symbol>>& follow){
        auto start = map<pair<Symbol, Symbol>, set<Symbol>>();
        for (const auto& p: grammar) {
            for (const auto& yields : p.rhs) {
                auto b = yields[0];
                auto atbeta = first.at(b);
                if(atbeta.find(EPSILON) == atbeta.end()){
                    start.insert({{p.lhs, b},{atbeta}});
                }else{
                    atbeta.erase(EPSILON);
                    auto t = follow.at(p.lhs);
                    atbeta.merge(t);
                    start.insert({{p.lhs, b},{atbeta}});
                }
            }
        }
        return start;
    }

    vector<vector<int>> tableGenerator(const map<Symbol, set<Symbol>>& first, map<pair<Symbol, Symbol>, set<Symbol>>& start){
        auto nonterminals = set<Symbol>();
        auto terminals = set<Symbol>();
        auto ret = vector<vector<int>>();
        for(const auto& lhs : first ){
            if(lhs.first < NUM){
                nonterminals.insert({lhs.first});
            }else{
                break;
            }
        }
        for(const auto& lhs : first ){
            if(lhs.first >= NUM){
                terminals.insert({lhs.first});
            }
        }
        int k = 0;
        for (auto& nt : nonterminals) {
            ret.push_back(vector<int>(terminals.size(), ERROR));

            for (int i = 0, p = 0; i < grammar.size(); ++i, ++p) {
                if(grammar[i].lhs == nt){
                    for (auto opts:grammar[i].rhs) {
                        for (auto x = 0; x < start.at({nt, opts[0]}).size(); ++x) {
                            ret[i][x] = p;
                            ++p;
                        }
                        if(start.at({nt, opts[0]}).find(EOF_) != start.at({nt, opts[0]}).end()){
                            ret[i][0] = p;
                        }
                        ++p;
                    }
                    break;
                }
            }

        }
        return ret;
    }

    void ParserGenerator(){

    }

    static ArithmeticParser* getInstance(){
        if(ArithmeticParser::singleton == nullptr){
            return new ArithmeticParser();
        }
        return ArithmeticParser::singleton;
    }

};
ArithmeticParser* ArithmeticParser::singleton = nullptr;



int main(){
    auto Goal = Production{GOAL, {{EXPR}}};
    auto Expr = Production{EXPR, {{TERM, EXPR_PRIME}}};
    auto ExprPrime = Production{
        EXPR_PRIME,
        {
            {PLUS, TERM, EXPR_PRIME},
            {MINUS, TERM, EXPR_PRIME},
            {EPSILON}
        }};
    auto Term = Production {
        TERM,
        {
            {FACTOR, TERM_PRIME}
        }};
    auto TermPrime = Production {
        TERM_PRIME,
        {
                {MULTIPLY, FACTOR, TERM_PRIME},
                {DIVIDE, FACTOR, TERM_PRIME},
                {EPSILON}
        }};
    auto Factor = Production {
        FACTOR,
        {
                {LEFT_PAREN, EXPR, RIGHT_PAREN},
                {NUM},
                {NAME_}
        }};
    auto p = ArithmeticParser::getInstance();
    p->grammar = {Goal, Expr, ExprPrime, Term, TermPrime, Factor};
    cout << p->grammarToString();
    p->removeIndirectLeftRecursion();
    cout << p->grammarToString();
    auto FIRST = p->getFirstSet();
    auto FOLLOW = p->getFollowSet(FIRST);
    auto START = p->getStartSet(FIRST, FOLLOW);
    auto TABLE = p->tableGenerator(FIRST, START);
    delete p;
}
