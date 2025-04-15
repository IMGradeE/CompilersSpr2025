//
// Created by wilke on 4/1/2025.
//

#ifndef ASSIGNMENT1_DFASTATE_H
#define ASSIGNMENT1_DFASTATE_H
#include "../Util/GlobalEnums.h"

using namespace std;
class DFAState{
    static int statID;
public:
    static vector<DFAState*> states;
    int type;
    bool  isAcceptingState;
    map<char, DFAState*> transitions =  map<char, DFAState*>();
    int id = 0;

    DFAState(int id_, bool accept, int type_){
        id = id_;
        isAcceptingState = accept;
        type = type_;
        states.push_back(this);
    }

    DFAState() {
        id = statID;
        ++statID;
        isAcceptingState = false;
        type = INVALID;
        states.push_back(this);
    }

    bool operator==(const DFAState &rhs) const{
        return (this->id == rhs.id);
    }
    bool operator!=(const DFAState &rhs) const{
        return !(*this == rhs);
    }
};
int DFAState::statID = 0;
vector<DFAState*> DFAState::states=vector<DFAState*>();

#endif //ASSIGNMENT1_DFASTATE_H
