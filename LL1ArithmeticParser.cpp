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

using namespace std;

enum Symbol{
    ANONYMOUS_START =  INT16_MIN,
    // anonymous non-terminals reside here
    GOAL = -255, // non-terminals
    EXPR,
    EXPR_PRIME,
    TERM,
    TERM_PRIME,
    FACTOR,
    goal, // todo
    // terminals
    EPSILON = -10,
    NUM,
    NAME,
    EOF_,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    LEFT_PAREN,
    RIGHT_PAREN,
    a,
    b
};

typedef struct Production{
    Symbol lhs;
    vector<vector<Symbol>> rhs;
}Production;

class Parser{
public:
    int MIN_NONTERMINAL = EPSILON;
    vector<Production> grammar = vector<Production>(); // INITIALIZE OUT OF LINE
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
        int count =0;
        for (auto& production: grammar) {
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
};

class ArithmeticParser:Parser{
private:
    static ArithmeticParser* singleton;
    ArithmeticParser(){

    }
public:

    void ParserGenerator(){

    }

    static ArithmeticParser* getInstance(){
        if(singleton == nullptr){
            return new ArithmeticParser();
        }
        return singleton;
    }

};


int main(){
    auto Goal = Production();
    Goal.lhs = goal;

    auto A = Production();
    A.lhs = EXPR;

    auto B = Production();
    B.lhs = EXPR_PRIME;

    Goal.rhs = {{EXPR}};
    A.rhs = {{EXPR_PRIME, a}, {a}};
    B.rhs = {{EXPR, b}};
    auto p = Parser();
    p.grammar = {Goal, A, B};
    cout << p.grammarToString();
    p.removeIndirectLeftRecursion();
    cout << p.grammarToString();
}
