//
// Created by wilke on 2/19/2025.
//

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include "Optimizer.h"
#include "Tokenizer.cpp"
#include "SymbolTable.cpp"

using namespace std;

typedef struct Production{
    Symbol lhs;
    vector<vector<Symbol>> rhs;
}Production;

class Parser{
public:
    int MIN_TERMINAL = PRINT;
    int last_generated_non_terminal = ANONYMOUS_START;

    void leftRecursionToRightRecursion(){
        for (int i = 0; i < grammar.size(); ++i){
            for (int j = 0; j < grammar[i].rhs.size(); ++j) {
                leftRecursionToRightRecursion(i,j);
            }
        }
    }

    void leftRecursionToRightRecursion(int i, int k){
        if(MIN_TERMINAL > grammar[i].lhs && grammar[i].rhs[k][0] == grammar[i].lhs){
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
                    if(MIN_TERMINAL > grammar[j].lhs && grammar[i].rhs[k][0] == grammar[j].lhs){ // A_s is the first symbol in a production whose lhs is A_i
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
    vector<Production> grammar = vector<Production>();
};

class ArithmeticParser:public Parser{
private:
    explicit ArithmeticParser(vector<Production>& _grammar): Parser(){
        grammar = _grammar;
        for (const auto &g : grammar) {
            for (const auto & v: g.rhs) {
                productions.push_back(v);
            }
        }
        getFirstSet();
        getFollowSet();
        getStartSet();
        tableGenerator();
    }
    static ArithmeticParser* singleton;
    map<Symbol, set<Symbol>> first;
    map<Symbol, set<Symbol>> follow;
    map<pair<Symbol, Symbol>, set<Symbol>> start;
    map<pair<Symbol, Symbol>, int> table;
    vector<vector<Symbol>> productions = vector<vector<Symbol>>();
    set<Symbol> terminals;
    set<Symbol> nonterminals;

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

    void getFirstSet(){
        vector<Symbol> arr;
        for (const auto& p :SymbolToString::SymbolMap) {
            if (p.first >= MIN_TERMINAL){
                arr.push_back(p.first);
            }
        }
        first = map<Symbol, set<Symbol>>();
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
    }

    void getFollowSet(){
        follow = map<Symbol, set<Symbol>>();
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

    }

    void getStartSet(){
        start = map<pair<Symbol, Symbol>, set<Symbol>>();
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
    }

    void tableGenerator(){
        nonterminals = set<Symbol>();
        terminals = set<Symbol>();
        table = map<pair<Symbol, Symbol>, int>();
        for(const auto& lhs : first ){
            if(lhs.first < MIN_TERMINAL){
                nonterminals.insert({lhs.first});
            }else{
                break;
            }
        }
        for(const auto& lhs : first ){
            if(lhs.first >= MIN_TERMINAL){
                terminals.insert({lhs.first});
            }
        }
        int p = 0;
        for (int i = 0; i < grammar.size(); ++i) {
            auto A = grammar[i].lhs;
            if(table.find({A, EOF_}) == table.end()) {
                table.insert({{A, EOF_}, ERROR});
            }
            for (auto t: terminals) {
                table.insert({{A, t}, ERROR});
            }

            for (int j = 0; j < grammar[i].rhs.size(); ++j, ++p) {

                auto B = grammar[i].rhs[j][0];
                auto START_A_B = start.at({A,B});
                for (auto w: START_A_B) {
                    table.at({A, w}) = p;
                }
                if (START_A_B.find(EOF_) != START_A_B.end()){
                    table.at({A, EOF_}) = p;
                }
            }
        }
    }

public:
    string grammarToString(){
        string ret;
        for (auto& production: grammar) {
            ret += '\n';
            ret += SymbolToString::getString(production.lhs);
            ret += " -> first set {";
            for (auto i : first.at(production.lhs)) {
                ret+= SymbolToString::getString(i );
                ret += ", ";
            }
            ret += "} -> follow set {";
            for (auto i : follow.at(production.lhs)) {
                ret+= SymbolToString::getString(i );
                ret += ", ";
            }
            ret += "}\n";
            for(auto& yields : production.rhs){
                ret += "\t  | ";
                for (const auto& symbol: yields) {
                    ret+= SymbolToString::getString(symbol);
                    ret += ' ';
                }
                ret += " -> start set {";
                for (auto i : start.at({production.lhs, yields[0]})) {
                    ret+= SymbolToString::getString(i );
                    ret += ", ";
                }
                ret += "}";
                ret += '\n';
            }
        }
        return ret;
    }

    void printTableHeader(bool nt = true){
        // first pass print the column headers
        auto terms = (nt)? terminals: nonterminals;
        printf("%10.10s", "");// empty cell at the top left of the table
        // first row has the first.second of every pair starting with GOAL, which is the terminal
        // subsequent rows have the label, and then the values respective to the column
        for (auto t : terms) {
            printf("%10.10s", SymbolToString::getString(t).c_str());
        }
        cout << '\n';
    }
    void showTable(){
        printTableHeader();
        for(auto nt : nonterminals){
            printf("%10.10s", SymbolToString::getString(nt).c_str());
            for(auto t: terminals){
                auto x = table.at({nt, t});
                if(x != ERROR){
                    printf("%10.10s", to_string(x).c_str());
                }else{
                    printf("%10.10s", " ");
                }
            }
            cout << '\n';
        }
    }

    void showFirstAndFollow(){
        printTableHeader(true);
        for(auto firstSet: first){

        }
    };

    void ParserGenerator(){

    }

    static ArithmeticParser* getInstance(vector<Production>& p){
        if(ArithmeticParser::singleton == nullptr){
            return new ArithmeticParser(p);
        }
        return ArithmeticParser::singleton;
    }

    bool endCheck(int x){
        if(x == INT16_MAX) return true;
        else return false;
    }

    tuple<int, vector<tokenTypes>, string, bool> skeletonParser(Tokenizer* t, SymbolTable& symTable){ // refactor to include a vector that contains the token for each element in the string
        //TODO discard whitespace
        auto s = stack<Symbol>();
        auto token = t->nextToken();
        string ret;
        int declType;
        bool isDecl = false;
        auto vec = vector<tokenTypes>();

        // TODO scope support

        if(endCheck(get<0>(token))){
            return {0, vec, ret, isDecl};
        }
        pair<tokenTypes, basic_string<char>>word = {(tokenTypes) get<1>(token), get<2>(token)};
        s.push(EOF_);
        s.push(grammar[0].lhs);
        Symbol focus;
        while(true){
            /*TODO*/
            focus = s.top();
            if((int) focus == (int) EOF_ && MatchSymbolToToken::reverse(token) == (int) EOF_){
                // report success and return or break
                ret += get<2>(token);
                return {1, vec, ret, isDecl};
            }
            else if (terminals.find(focus) != terminals.end()){ // EOF is a terminal so || focus == eof is implicit
                if(MatchSymbolToToken::match(focus, word.first)){
                    s.pop();
                    char last = ret.back();
                    if(focus == TYPE_){
                        isDecl = true;
                        declType = (get<2>(token)=="int32")?0:1;
                    }
                    if(focus == NAME_ && isDecl == false){ // this is a reference
                        // check if the name is undeclared.
                        symTable.getSymbol(get<2>(token));
                        // if the name is undeclared, throw an error.
                        // otherwise continue.
                    }else if(focus == NAME_ && isDecl){ // this is a declaration
                         symTable.addSymbol(get<2>(token), declType);
                    }
                    if(word.second == "-" && (last == '\0' || last == '(' || last  == '*' || last == '-' || last == '+' || last == '^' || last == '/' || last == '=')){
                        ret += (char) -7;
                    }else if(word.second == "int32" || word.second == "f32"){

                    }else if(focus == PRINT){
                        ret += (char) PRINT;
                    }else{
                        ret += get<2>(token);
                    }
                    if((tokenTypes) get<1>(token) == UNARY_MINUS || (tokenTypes) get<1>(token) == INT || (tokenTypes) get<1>(token) == FLOAT || (tokenTypes) get<1>(token) == NAME){
                        vec.push_back((tokenTypes) get<1>(token));
                    }
                    token = t->nextToken();
                    if(endCheck(get<0>(token))){
                        return {0, vec, ret, isDecl};
                    }
                    word = {(tokenTypes) get<1>(token), get<2>(token)};
                }
                else{
                    //error when looking for symbol in focus
                    while ((tokenTypes)get<1>(t->nextToken()) != ENDL && get<1>(t->nextToken()) != INT16_MAX){}
                    return {2, vec, "<Expected " + SymbolToString::getString(focus) + ", got token " + ((get<2>(token) == "\r\n")?"\\r\\n":get<2>(token)) + " with token type " + tokenIdstrings[get<1>(token)], isDecl};
                }
            }
            else{ // focus is nonterminal
                Symbol wordFromToken = MatchSymbolToToken::reverse(token);

                auto it = table.find({focus,wordFromToken});
                if( it != table.end() && it->second != ERROR){ // found
                    s.pop();
                    auto prod = productions[it->second];
                    for (int i = prod.size()-1; i >= 0; --i) {
                        if(prod.at(i) != EOF_ && prod.at(i) != EPSILON){
                            s.push(prod.at(i));
                        }
                    }
                }else{
                    //error expanding focus.
                    return {2, vec, "<Could not expand focus at " + SymbolToString::getString(focus) + ", with token " + ((get<2>(token) == "\r\n")?"\\r\\n":get<2>(token)) + " of type " + tokenIdstrings[get<1>(token)], isDecl};
                }
            }
        }
    }
};

void optimize_and_print(int count, vector<vector<pair<tokenTypes, string>>> &validLines, tuple<int, vector<tokenTypes>, string, bool> &parsedToken, ArithmeticOptimizer& opti, ShuntingYard& shunt);

ArithmeticParser* ArithmeticParser::singleton = nullptr;

int main(){
    auto grammar = vector<Production>({
        Production {GOAL, {{TYPE_, NAMEDECLREM},
                {NAME_, AFTERNAME},
                {PRINT, GTERM_SIGN},}},
        Production {AFTERNAME, {{LINEVARNAMEREM},
                {EXPR}}},
        Production {NAMEDECLREM,{{NAME_, LINEVARNAMEREM},}},
        Production {LINEVARNAMEREM, {{ASSIGN, EXPR}}},
        Production {EXPR, {{LTERM_ADD_SUB, ADD_SUB},}},
        Production {RTERM_MULT_DIV, {{GTERM_SIGN, POWER_NT},}},
        Production {LTERM_ADD_SUB, {{RTERM_MULT_DIV, MULT_DIV},}},
        Production {ADD_SUB, {{PLUS, EXPR},
                              {MINUS, EXPR},
                              {EPSILON},}},
        Production {MULT_DIV, {{MULT_DIV_AND_RIGHT_OP},
                               {EPSILON},}},
        Production {MULT_DIV_AND_RIGHT_OP, {{MULTIPLY, LTERM_ADD_SUB},
                                            {DIVIDE, LTERM_ADD_SUB},}},
        Production {POWER_NT, {{POWER_AND_RIGHT_OP},
                               {EPSILON},}},
        Production {POWER_AND_RIGHT_OP, {{POWER, RTERM_MULT_DIV},}},
        Production {GTERM_SIGN, {{GTERM},
                                 {MINUS, GTERM},}},
        Production {GTERM, {{LEFT_PAREN, EXPR, RIGHT_PAREN},
                            {NAME_},
                            {NUM},}},
    });

    auto p = ArithmeticParser::getInstance(grammar);
    cout << p->grammarToString();
    p->showTable();

    int count = 1;
    auto validLines = vector<vector<pair<tokenTypes, string>>>();

    Tokenizer t("accept-7.txt");
    SymbolTable symTable;
    NASMGenerator gen;
    ArithmeticOptimizer opti(&symTable, gen);
    ShuntingYard shuntingYard(&symTable);
    while (true){
        // TODO scope support.
        auto parsedToken = p->skeletonParser(&t, symTable);
        if(get<0>(parsedToken) == 1){
            // optimization and IR
            try {
                optimize_and_print(t.line, validLines, parsedToken, opti, shuntingYard);
            }catch(exception &e) {
                cout << e.what();
                cout<< "imbalanced parenthesis in expression " << get<2>(parsedToken) << '\n';
            }

        }else if(get<0>(parsedToken) == 2){
            cout << "\nFailed line " << t.line << " "<< get<2>(parsedToken) << "\n";
        }else{
            optimize_and_print(t.line, validLines, parsedToken, opti, shuntingYard);
            break;
        }
        ++count;
    }
    gen.end();
    delete p;
    return 0;
}

void optimize_and_print(int count, vector<vector<pair<tokenTypes, string>>> &validLines, tuple<int, vector<tokenTypes>, string, bool> &parsedToken, ArithmeticOptimizer& opti, ShuntingYard& shunt) {
    string s;
    if(iscntrl((int)get<2>(parsedToken).back())){
        s = get<2>(parsedToken).substr(0, get<2>(parsedToken).size() - 2); // remove CRLF newline Suffix
    }else{
        s = get<2>(parsedToken);
    }
    validLines.push_back(shunt.arithmeticShunt(get<1>(parsedToken), s));
    opti.optimize(validLines.back());
}

/*"Classic" Arithmetic Grammar from the book:
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
                {NAME_},
                {NEG_NUM},
                {NEG_NAME_},
        }};
 * */