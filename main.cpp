#include <iostream>
#include <cstdlib>
#include <stack>
#include <string>
#include <queue>
#include <map>
#include <list>
#include <algorithm>
#include <fstream>
#include <unordered_map>
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
        static int statID;
        state() {
            id = statID;
            ++statID;
            isInitialState = false;
            isAcceptingState = false;
            type = INVALID;
        }
        bool isInitialState, isAcceptingState;
        bool operator==(const state& rhs) const{
            return (this->id == rhs.id);
        }
        bool operator!=(const state& rhs) const{
            return !(*this == rhs);
        }
        int id;
        int type;
        map<char, vector<state>> transitions =  map<char, vector<state>>();
        map<char, state> DFATransitions =  map<char, state>();
};
int state::statID = 0;
pair<state, state> getNewNFA(){
    auto start = state();
    auto end = state();
    start.isInitialState = true;
    start.isAcceptingState = false;
    end.isInitialState = false;
    end.isAcceptingState = true;
    return {start, end};
}
pair<state, state> getNFA(char c){
    auto [start, end] = getNewNFA();
    auto v = vector<state>();
    v.push_back(end);
    start.transitions.insert({c, v});
    return {start, end};
}
void setInnerStates(pair<state, state> &NFA){
    NFA.first.isInitialState = false;
    NFA.second.isAcceptingState = false;
}
pair <state, state> unionNFAs(pair<state, state> NFA1, pair<state, state> NFA2){
    // make a new NFA with lambda transitions to the argument NFAs (lamba = -1)
    auto [start, end] = getNewNFA();
    auto sv = vector<state>(); // isInitialState transition function vector
    auto ev = vector<state>(); // isAcceptingState transition function vector TODO MAYBE UNUSED
    sv.push_back(NFA1.first); // push the isInitialState state of NFA1 into starts' lambda transition function vector
    sv.push_back(NFA2.first); // push the isInitialState state of NFA2 into starts' lambda transition function vector
    start.transitions.insert({LAMBDA, sv}); // insert with key LAMBDA

    NFA1.second.transitions.insert({LAMBDA, vector<state>(1, end)}); // add a lambda transition from NFA1's isAcceptingState state to the new common isAcceptingState state
    NFA2.second.transitions.insert({LAMBDA, vector<state>(1, end)}); // add a lambda transition from NFA2's isAcceptingState state to the new common isAcceptingState state
    // clean up NFA 1&2
    setInnerStates(NFA1);
    setInnerStates(NFA2);
    return {start, end};
}
pair <state, state> concatNFAs(pair<state, state> NFA1, pair<state, state> NFA2){
    // make NFA1's isAcceptingState state the same as NFA2's isInitialState state.
    NFA1.second.isAcceptingState = false; // isAcceptingState state no longer marked as isAcceptingState state
    NFA2.first.isInitialState = false; // isInitialState state no longer marked as isInitialState state
    // naive implementation is just to lambda transition from the old isAcceptingState of NFA1 to the old isInitialState of NFA2 so that's what I'm doing
    auto sv = vector<state>();
    sv.push_back(NFA2.first); // push the isInitialState state of NFA2 into the transition function vector for NFA1's old isAcceptingState state

    NFA1.second.transitions.insert({LAMBDA, sv});
    // new <isInitialState, isAcceptingState> pair of states for this machine is NFA1.first and NFA2.second
    return {NFA1.first, NFA2.second};
}
pair <state, state> closeNFA(pair<state, state> NFA) {
    auto [start, end] = getNewNFA();
    NFA.second.isAcceptingState = false;
    NFA.first.isInitialState = false;
    NFA.second.transitions.insert({LAMBDA, vector<state>(1,
                                                         NFA.first)}); // insert a transition from NFAs' old isAcceptingState state to its isInitialState state via lambda
    NFA.second.transitions.at(LAMBDA).push_back(
            end); // add a lambda transition from the old isAcceptingState state to the new isAcceptingState state

    start.transitions.insert({LAMBDA, vector<state>(1, end)});
    start.transitions.at(LAMBDA).push_back(NFA.first);
    return {start, end};
}
pair<state,state> toNFA(queue<char> input){
    state start;
    start.isInitialState = true;


    auto s = stack<pair<state, state>>();
    char c;
    while(!input.empty()){
        c = input.front();
        input.pop();
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
list<state> followEpsilon(const state &initialState) {
    try {
        auto tvec = initialState.transitions.at(LAMBDA);
        list<state> epsilon;
        for (state& s : tvec) {
            epsilon.push_back(s);
        }
        return epsilon;
    }catch (exception &e) {
        return {};
    }
}
list<state> Delta(const state &nState, char c) {
    list<state> ret;
    try {
        auto vec = nState.transitions.at(c);
        // follow epsilon from each element accessible via this character
        for (state& s : vec) {
           ret.push_back(s);
        }
        for (int i = 0; i < vec.size(); i++) {
            auto it = ret.begin();
            auto t = followEpsilon(*it);
            ret.insert(ret.end(), t.begin(), t.end());
            ++it;
        }
    }catch (exception &e) {

    }
    return ret;
}
bool compareQ(list<state> lhs, list<state> rhs) {
    if (lhs.size() != rhs.size()) return false;
    bool ret = true;
    for (auto i = lhs.begin(), k = rhs.begin(); i != lhs.end() && ret; next(i,1), next(k,1)) {
        ret &= (*i == *k);
    }
    return ret;
}
bool Qcontains(const list<list<state>>& Q, const list<state>& Qitem) {
    bool ret = false;

    for (auto q: Q) {
        ret |= compareQ(q, Qitem);
        if (ret){ return true;}
    }
    return ret;
}
vector<state> ScannerGenerator() {
    // a(a+b)*b -> aab+*.b. => "a.(a+b)*.b"
    // Does not properly parse statements like ab* or (a+b)a+b where an implicit subexpr is operated on unless concatenation is specified explicitly.
    // TODO seek to endl for single line comments or to closing */ for multiline when encountered.
    auto NFAs = vector<pair<state,state>>();
    // Todo preprocessor for concatenation symbol insertion


    string s[16] = {R"(\\.n + \\.r + \\.v + \\.f)", // ENDL,
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
    };



    static int i = 0; // slightly easier than changing this to an indexed loop
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
        NFA.second.type = (tokenTypes) i; // why am I casting this? I don't remember the reason but I remember this being helpful in some way... lol.
        ++i;
        NFAs.push_back(NFA);
    }
    auto startStates = vector<state>(NFAs.size());
    for (int j = 0; j < NFAs.size(); ++j) {
        startStates.push_back(NFAs[j].first);
    }
    auto newStartState = state();

    newStartState.transitions.insert({LAMBDA, startStates});
    // convert to dfa, preserve end states
    auto Q = list<list<state>>();
    auto workList = queue<list<state>>();
    auto q_0 = list<state>();
    vector<state> DFA_states;

    q_0 = followEpsilon(newStartState);
    Q.push_back(q_0);
    workList.push(q_0);
    int Q_index = 0;
    auto DFAStartState = state();
    DFAStartState.isInitialState = true;
    DFA_states.push_back(DFAStartState);

    while(!workList.empty()) {
        auto q = workList.front();
        workList.pop();

        auto tempState = state();
        for (int k = 32; k < 127; ++k) { // every character in our alphabet
            for (state curr : q) {
                auto temp = Delta(curr,((char) k));
                if (temp.empty()) continue; // error state
                if (!Qcontains(Q, temp)) { // this is a new DFA state
                    Q.push_back(temp);
                    workList.push(temp);
                    for (state currState : temp) {
                        if (currState.isAcceptingState) {
                            tempState.type = currState.type;
                            tempState.isAcceptingState = true;
                            break;
                        }
                    }
                    DFA_states.push_back(tempState);
                }

                DFA_states[Q_index].DFATransitions.insert({k, tempState});
            }
        }
        ++Q_index;
    }
    return DFA_states;
}
int getIndexOf(const vector<state>& states, const state& arg) {
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
    static int streamPos = 0;
    static state error = state();
    auto Stack = stack<pair<state, int>>();
    static vector<vector<bool>> Failed = vector<vector<bool>>(input.length(), vector<bool>(scannerTable.size(), false));
    static state bad = state();
    bad.id = -1;

    string lexeme;
    auto currentState = scannerTable[0];
    char c;
    Stack.push({bad, -1});

    while (currentState != bad) {
        if (Failed[getIndexOf(scannerTable, currentState)][streamPos]) {
            currentState = Stack.top().first;
            streamPos = Stack.top().second;
            Stack.pop();
            truncate(lexeme);
            break; //TODO
        }
        c = input[streamPos]; // get character from the inputstream at index streamPos
        lexeme += c; // concatenate the character to lexeme
        if (currentState.isAcceptingState) {
            Stack = stack<pair<state, int>>();
            Stack.push({bad, -1});
        }
        Stack.push({currentState, streamPos});
        try {
            currentState = currentState.DFATransitions.at(c);
        }catch (exception &e) {
            currentState = error;
        }
        ++streamPos;
    }
    while (!currentState.isAcceptingState && currentState != bad) {
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
    if (currentState.isAcceptingState) {
        return {streamPos, currentState.type, lexeme};
    }else{
        return {streamPos, INVALID, lexeme};
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

    f.open("parser_text_hw2.txt");
    if(!f.is_open()) {
        cout<< "The file referenced in the CLI arguments did not exist in the current directory. Resolve this issue and try again.";
        return -1;
    }
    int size = 3400;
    string input = string(size, '\0');
    f.seekg(ios::beg);
    f.read(&input[0], size);
    f.close();

    while (size >= 0) {
        auto token = scanner(input);
        cout << '<'<< tokenIdstrings[get<1>(token)] << ", " << get<2>(token) << '>' << '\n';
        size -= get<0>(token);
    }
    return 0;
}
