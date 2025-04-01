//
// Created by wilke on 4/1/2025.
//

#ifndef ASSIGNMENT1_DFASERIALIZER_H
#define ASSIGNMENT1_DFASERIALIZER_H
#include <cstdlib>
#include <vector>
#include <map>
#include <fstream>
#include "DFAState.h"
#include "Split.h"

const std::string DFA_FILE_NAME = "DFA.txt";

void serializeDFA(const std::vector<DFAState*>& states);

void deserializeDFA(std::fstream& f);
#endif //ASSIGNMENT1_DFASERIALIZER_H
