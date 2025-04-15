//
// Created by wilke on 4/1/2025.
//

#include "DFASerializer.h"

// id(int)
// type(int)
// isAcceptingState(bool)
// transitions(map<char, ptr>)

const char replaceCarriageReturn = -10;
const char replaceNewline = -11;
const char replaceWhitespace = -12;

void serializeDFA(const std::vector<DFAState*>& states){
    std::fstream f{DFA_FILE_NAME, std::ios::out | std::ios::trunc};
    if(f.is_open()){
        for (auto state : states) {
            // rather than a pointer, store the ID of the state the origin state relates to, and use it to reconstruct the DFA relations later
            std::string outstring = std::to_string(state->id) + " " + std::to_string(state->type) + " " + std::to_string(state->isAcceptingState) + " ";
            for(auto transition : state->transitions){
                if(transition.first == '\r'){
                   outstring += replaceCarriageReturn;
                }else if(transition.first == '\n') {
                    outstring += replaceNewline;
                }else if(transition.first == ' '){
                    outstring += replaceWhitespace;
                }else{
                    outstring += transition.first;
                }
                outstring += " ";
                outstring += std::to_string(transition.second->id);
                outstring += " ";
            }
            outstring += "\n";
            f.write(outstring.c_str(), outstring.size());
        }
        f.close();
    }else{
        // TODO error
    }
}

/*Constructed inplace.*/
void deserializeDFA(std::fstream& f){
    // make each node, and then reconstruct the relationships
    if(f.is_open()){
        auto size = f.tellg();
        std::string str(size, '\0');
        f.seekg(0);
        f.read(&str[0],size);
        auto vecMap = std::vector<std::map<char, int>>();
        for (auto& stringState: split(str, '\n')) {
            int i = 0;
            auto state = new DFAState();
            auto id_transitions = std::map<char, int>();
            char last;
            for (auto entry: split(stringState, ' ')) {
                if(i == 0){ // id
                    state->id = stoi(entry);
                }else if(i == 1){ //type
                    state->type = stoi(entry);
                }else if(i == 2){ // accepting?
                    state->isAcceptingState = (bool) stoi(entry);
                }else {
                    if (i % 2 == 1) { // char (key)
                        if(entry[0] == replaceCarriageReturn){
                            last = '\r';
                        }else if(entry[0] == replaceNewline){
                            last = '\n';
                        }else if(entry[0] == replaceWhitespace){
                            last = ' ';
                        }else{
                            last = entry[0];
                        }
                    } else { // int value (id of the state in the relationship from char -> state)
                        id_transitions.emplace(last, stoi(entry));
                    }
                }
                ++i;
            }
            vecMap.push_back(id_transitions);
        }

        for (int i = 0; i < vecMap.size(); ++i) { // each state has a map
            for (auto pair: vecMap[i]) { // each pair of maps in states
                DFAState::states[i]->transitions.emplace(pair.first, DFAState::states[pair.second]); // insert a char->DFAState* relationship for each transition from this state
            }
        }

        f.close();
    }else{
        // TODO error
    }

}