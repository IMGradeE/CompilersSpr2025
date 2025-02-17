#include <iostream>
#include <cstdlib>
#include <stack>
#include <string>
#include <queue>
#include <map>
#include <list>
#include <algorithm>
#include <fstream>
#include <set>

using namespace std;
enum spec{ // OR, CONCAT, and STAR integer values can be compared to establish precedence between op symbols
    RPAREN = -6,
    LPAREN,
    OR,
    CONCAT,
    STAR,
    LAMBDA
};
enum tokenTypes{
    ENDL,
    KEYWORD,
    OPERATOR,
    DQUOTE,
    QUOTE,
    BSLASH,
    IDENTIFIER, // this could be name instead.
    INT,
    FLOAT,
    OPENPAREN, // (
    CLOSEPAREN, // )
    OPENBRACE, // {
    CLOSEBRACE, // }
    OPENBRACKET, // [
    CLOSEBRACKET, // ]
    COMMA,
    INVALID
}; // TODO Expand keywords into multiple individual categories.
void accumulate(string& accumulator, queue<char>& output){
    for(char c: accumulator){
        output.push(c);
    }
    accumulator.clear();
}
queue<char> shuntingYard(string workingString){
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
                    if (operators.empty()) throw EACCES;
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
        if (operators.top() == LPAREN || operators.top() == RPAREN) throw EACCES;
        accumulator += operators.top();
        accumulate(accumulator, output);
        operators.pop();
    }
    return output;
}
class state{
    public:
        int id;
        state() {
            id = statID;
            ++statID;

            isAcceptingState = false;
            type = INVALID;
            states.push_back(this);
        }
        bool  isAcceptingState;
        bool operator==(const state &rhs) const{
            return (this->id == rhs.id);
        }
        bool operator!=(const state &rhs) const{
            return !(*this == rhs);
        }
        static int statID;
        static vector<state*> states;
        int type;
        map<char, vector<state*>> transitions =  map<char, vector<state*>>();
        map<char, state*> DFATransitions =  map<char, state*>();
};
int state::statID = 0;
vector<state*> state::states=vector<state*>();

pair<state*, state*> getNewNFA(){
    auto start = new state();
    auto end = new state();
    return {start, end};
}
pair<state*, state*> getNFA(char c){
    auto [start, end] = getNewNFA();
    auto v = vector<state*>();
    v.push_back(end);
    start->transitions.insert({c, v});
    return {start, end};
}
void setInnerStates(pair<state*, state*> &NFA){

    NFA.second->isAcceptingState = false;
}
pair <state*, state*> unionNFAs(pair<state*, state*> NFA1, pair<state*, state*> NFA2){
    // make a new NFA with lambda transitions to the argument NFAs (lamba = -1)
    auto [start, end] = getNewNFA();
    auto sv = vector<state*>(); // isInitialState transition function vector
    sv.push_back(NFA1.first); // push the isInitialState state of NFA1 into starts' lambda transition function vector
    sv.push_back(NFA2.first); // push the isInitialState state of NFA2 into starts' lambda transition function vector
    start->transitions.insert({LAMBDA, sv}); // insert with key LAMBDA

    NFA1.second->transitions.insert({LAMBDA, vector<state*>(1, end)}); // add a lambda transition from NFA1's isAcceptingState state to the new common isAcceptingState state
    NFA2.second->transitions.insert({LAMBDA, vector<state*>(1, end)}); // add a lambda transition from NFA2's isAcceptingState state to the new common isAcceptingState state
    // clean up NFA 1&2
    setInnerStates(NFA1);
    setInnerStates(NFA2);
    return {start, end};
}
pair <state*, state*> concatNFAs(pair<state*, state*>& NFA1, const pair<state*, state*> &NFA2){
    // make NFA1's isAcceptingState state the same as NFA2's isInitialState state.
    NFA1.second->isAcceptingState = false; // isAcceptingState state no longer marked as isAcceptingState state

    // naive implementation is just to lambda transition from the old isAcceptingState of NFA1 to the old isInitialState of NFA2 so that's what I'm doing

    NFA1.second->transitions.insert({LAMBDA, vector<state*>(1, NFA2.first)});

    return {NFA1.first, NFA2.second};
}
pair <state*, state*> closeNFA(pair<state*, state*> NFA) {
    auto [start, end] = getNewNFA();
    NFA.second->isAcceptingState = false;

    // insert a transition from NFAs' old isAcceptingState state to its isInitialState state via lambda
    NFA.second->transitions.insert({LAMBDA, vector<state*>(1, NFA.first)});
     // add a lambda transition from the old isAcceptingState state to the new isAcceptingState state
    NFA.second->transitions.at(LAMBDA).push_back(end);

    start->transitions.insert({LAMBDA, vector<state*>(1, end)});
    start->transitions.at(LAMBDA).push_back(NFA.first);
    return {start, end};
}
pair<state*,state*> toNFA(queue<char> input){
    state* start;

    auto s = stack<pair<state*, state*>>();
    char c;
    while(!input.empty()){
        c = input.front();
        input.pop();
        if(c == ' '){
            continue;
        }
        if (c >= 0){ // symbol
            // generate NFA for this symbol
            // push NFA onto stack<pair<state_1,state_n>>
            s.push(getNFA(c));
        }else {// operator
            // pop the argument NFA(s) off the stack, do op, push result onto stack
            if(c == OR){
                auto s2 = s.top(); // newer
                s.pop();
                auto s1 = s.top(); // older
                s.pop();
                s.push(unionNFAs(s1, s2));
            }else if(c == STAR){
                auto s1 = s.top();
                s.pop();
                s.push(closeNFA(s1));
            }else{
                auto s2 = s.top(); // newer
                s.pop();
                auto s1 = s.top(); // older
                s.pop();
                s.push(concatNFAs(s1, s2));
            }
        }
    }
    //return the start and end state of the completed nfa for this pattern
    return {s.top().first, s.top().second};
}
set<state*> followEpsilonHelper(set<state*> parentTF){
    if(parentTF.empty()){
        return parentTF;
    }else{
        for (auto t : parentTF) {
            set<state*> temp;
            try {
                for (auto item : t->transitions.at(LAMBDA)) {
                    temp.insert(item);
                }
            }catch (exception e){}
            parentTF.merge(followEpsilonHelper(temp));
        }
        return parentTF;
    }
}
list<state*> followEpsilon(const list<state*>& stateList) {
    list<state*> epsilon = list<state*>(stateList);
        for (auto curr: stateList) {
            try {
                auto tvec = curr->transitions.at(LAMBDA);
                set<state*> temp;
                for (auto t : tvec) {
                    temp.insert(t);
                }
                temp = followEpsilonHelper(temp);
                for (state* s : temp) {
                    auto end = epsilon.end();
                    if(find(epsilon.begin(), epsilon.end(), s) == end){
                        epsilon.push_back(s);
                    }
                }
            }catch (exception &e) {
            }
        }
    return epsilon;
}
list<state*> followEpsilon(const vector<state*>& stateList) {
    list<state*> epsilon ;
    for (auto x: stateList) {
        epsilon.push_back(x);
    }
    for (auto curr: stateList) {
        try {
            auto tvec = curr->transitions.at(LAMBDA);
            set<state*> temp;
            for (auto t : tvec) {
                temp.insert(t);
            }
            temp = followEpsilonHelper(temp);
            for (state* s : temp) {
                auto end = epsilon.end();
                if(find(epsilon.begin(), epsilon.end(), s) == end){
                    epsilon.push_back(s);
                }
            }
        }catch (exception &e) {
        }
    }
    return epsilon;
}
list<state*> Delta(const list<state*>& nState, char c) {
    list<state*> ret;
    for (auto curr : nState) {
        try {
            auto vec = curr->transitions.at(c);
            for (state* s : vec) {
                auto end = ret.end();
                if(find(ret.begin(), ret.end(), s) == end)
                ret.push_back(s);
            }
        }catch (exception &e) {
        }
    }
    return ret;
}
bool compareQ(list<state*> lhs, list<state*> rhs) {
    if (lhs.size() != rhs.size()) return false;
    bool ret = true;
    for (auto i = lhs.begin(), k = rhs.begin(); i != lhs.end(); i = next(i,1), k = next(k,1)) {
        ret &= (*i == *k);
    }
    return ret;
}
int Qcontains(const set<list<state*>>& Q, const list<state*>& Qitem) {
    int count = 0;
    for (auto &q: Q) {
        if (compareQ(q, Qitem)){ return count;}
        ++count;
    }
    return -1;
}
vector<state*> ScannerGenerator() {
    // a(a+b)*b -> aab+*.b. => "a.(a+b)*.b"
    // Does not properly parse statements like ab* or (a+b)a+b where an implicit subexpr is operated on unless concatenation is specified explicitly.
    // TODO seek to endl for single line comments or to closing */ for multiline when encountered.
    auto NFAs = vector<pair<state*,state*>>();
    // Todo preprocessor for concatenation symbol insertion
    string s[1] = {
                   "a.(b+c)*+aa"};
    /*string s[17] = {
                        "\n + \r + \v + \f",
                    R"(\.n + \.r + \.v + \.f)", // ENDL,
                    "r.e.t.u.r.n + p.r.o.c.e.d.u.r.e + i.s.h + n.u.m", // KEYWORD,
                    "\\+ + / + ^ + \\* + - + =", // OPERATOR,
                    "\"", // DQUOTE,
                    "\'", // QUOTE,
                    "\\", // BSLASH, // \ //
                    "(a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x+y+z+A+B+C+D+E+F+G+H+I+J+K+L+M+N+O+P+Q+R+S+T+U+V+W+X+Y+Z).(_*+(0+1+2+3+4+5+6+7+8+9)*+(a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x+y+z+A+B+C+D+E+F+G+H+I+J+K+L+M+N+O+P+Q+R+S+T+U+V+W+X+Y+Z)*)*", // IDENTIFIER, // this could be name instead.
                    "(0+1+2+3+4+5+6+7+8+9).(0+1+2+3+4+5+6+7+8+9)*", // INT,
                    "(0+1+2+3+4+5+6+7+8+9)*.\\..(0+1+2+3+4+5+6+7+8+9)*", // FLOAT,
                    "\\(", // OPENPAREN, // (
                    "\\)", // CLOSEPAREN, // )
                    "{", // OPENBRACE, // {
                    "}", // CLOSEBRACE, // }
                    "[", // OPENBRACKET, // [
                    "]", // CLOSEBRACKET // ]
                    ",", //COMMA
    };*/


    static int i = 0, k = 0; // slightly easier than changing this to an indexed loop
    for(string lstr : s){
        char c;
        string string1;
        for (int j = 0; j < lstr.length(); ++j) {
            c = lstr[j];
            if (c == '\\') { // allow specification of literal symbols TODO check
                ++j; // skip the slash
                string1 += lstr[j]; // add character following the slash to the expression.
                continue;
            }
            if (c == '.') {
                string1 += (char) CONCAT;
            } else if (c == '+') {
                string1 += (char) OR;
            } else if (c == '*') {
                string1 += (char) STAR;
            } else if (c == '(') {
                string1 += (char) LPAREN;
            } else if (c == ')') {
                string1 += (char) RPAREN;
            } else {
                string1 += lstr[j];
            }
        }

        queue<char> postfixPattern = shuntingYard(string1);
        // uncomment to look at shunting yard output if you want I guess
        /*
        auto tokens = postfixPattern; // copy so we don't empty the queue we're building our NFA from
        cout << "The postfix representation for " << lstr << " is: ";
        while (!tokens.empty()) {
            char str = tokens.front();
            tokens.pop();
            if (tokens.empty()) cout << str << '\n';
            else cout << str;
        }*/
        auto NFA = toNFA(postfixPattern);
        NFA.second->type = i;
        NFA.second->isAcceptingState = true;
        if(k > 2){ ++i; }
        ++k;
        NFAs.push_back(NFA);
    }
    auto startStates = vector<state*>();
    for (int j = 0; j < NFAs.size(); ++j) {
        startStates.push_back(NFAs[j].first);
    }

    // convert to dfa, preserve end states
    auto Q = set<list<state*>>();
    auto workList = queue<list<state*>>();
    auto q_0 = followEpsilon(startStates);
    vector<state*> DFA_states;

    Q.insert(q_0);
    workList.push(q_0);
    int Q_index = 0;
    auto DFAStartState = new state();

    DFA_states.push_back(DFAStartState);

    state* tempState;
    while(!workList.empty()) {
        auto q= workList.front();
        workList.pop();
        for (int k = 0; k < 127; ++k) { // every character in our alphabet
            auto temp = followEpsilon(Delta(q,  k));
            if (temp.empty())
                continue; // error state
            int index = Qcontains(Q, temp);
            if (index == -1) { // this is a new DFA state
                tempState = new state();
                Q.insert(temp);
                workList.push(temp);
                for (const state *currState: temp) {
                    if (currState->isAcceptingState) {
                        tempState->type = currState->type;
                        tempState->isAcceptingState = true;
                        break;
                    }
                }
                DFA_states.push_back(tempState);
            } else {
                tempState = DFA_states[index];
            }
            DFA_states[Q_index]->DFATransitions.insert({k, tempState});
        }
        ++Q_index;
    }
    return DFA_states;
}
int getIndexOf(const vector<state*>& states, const state* arg) {
        for (int i = 0; i < states.size(); ++i) {
            if (states[i] == arg) {
                return i;
            }
        }
    return -1;
}
void truncate(string& lexeme) {
    lexeme = lexeme.substr(0, lexeme.length() - 1); // TODO verify
}
tuple<int, int, string> scanner(const string& input) {
    // non-minimal DFA represented by an array of states DFA_states TODO minimize
    static auto scannerTable = ScannerGenerator();
    static state* bad = new state();
    static state* error = new state();
    static int count = 0;
    if (count == 0){
        scannerTable.push_back(bad);
        scannerTable.push_back(error);
        ++count;
    }

    int streamPos = 0;
    auto Stack = stack<pair<state*, int>>();
    vector<vector<bool>> Failed = vector<vector<bool>>(input.length(), vector<bool>(scannerTable.size(), false));
    bad->id = -1;

    string lexeme;
    auto currentState = scannerTable[0];
    char c;
    Stack.push({bad, -1});

    while (currentState != error && currentState != bad) {
        if (Failed[getIndexOf(scannerTable, currentState)][streamPos]) {
            currentState = Stack.top().first;
            streamPos = Stack.top().second;
            Stack.pop();
            truncate(lexeme);
            break; //TODO
        }
        c = input[streamPos]; // get character from the inputstream at index streamPos
        lexeme += c; // concatenate the character to lexeme
        if (currentState->isAcceptingState) {
            Stack = stack<pair<state*, int>>();
            Stack.push({bad, -1});
        }
        Stack.push({currentState, streamPos});
        try {
            currentState = currentState->DFATransitions.at(c);
        }catch (exception &e) {
            currentState = error;
        }
        ++streamPos;
    }
    while (!currentState->isAcceptingState && currentState != bad) {
        if (currentState != error) {
            Failed[getIndexOf(scannerTable, currentState)][streamPos] = true;
        }
        currentState = Stack.top().first;
        streamPos = Stack.top().second;
        Stack.pop();
        if (currentState != bad) {
            truncate(lexeme);
        }
    }

    if (currentState->isAcceptingState) {
        return {streamPos, currentState->type, lexeme};
    }else{
        return {lexeme.length(), INVALID, lexeme};
    }
}
int main() {
    ifstream f;

    string tokenIdstrings[] = { "ENDL",
    "KEYWORD",
    "OPERATOR",
    "DQUOTE",
    "QUOTE",
    "BSLASH",
    "IDENTIFIER", // this could be name instead.
    "INT",
    "FLOAT",
    "OPENPAREN", // (
    "CLOSEPAREN", // )
    "OPENBRACE", // {
    "CLOSEBRACE", // }
    "OPENBRACKET", // [
    "CLOSEBRACKET", // ]
    "COMMA",
    "INVALID"};

    /*f.open("parser_text_hw2.txt");*/
    f.open("a_b_or_c_test.txt");
    if(!f.is_open()) {
        cout<< "The file referenced in the CLI arguments did not exist in the current directory. Resolve this issue and try again.";
        return -1;
    }
    /*int size = 3400;*/
    int size = 35;
    string input = string(size, '\0');
    f.seekg(ios::beg);
    f.read(&input[0], size);
    f.close();

    while (size > 0){
        if(isspace(input[0])) {
            input = input.substr(1, input.length());
            --size;
            continue;
        }
        auto token = scanner(input);
        cout << '<'<< tokenIdstrings[get<1>(token)] << ", " << get<2>(token) << '>' << '\n';
        if(get<0>(token) <= 0) get<0>(token) = 1;
            input = input.substr(get<0>(token), input.length());
            size -= get<0>(token);
    }
    return 0;
}
