#pragma once

#include "./cfg.h"
#include "unordered_set"

namespace fst {
const unsigned int ACTION_REDUCE = 0;
const unsigned int ACTION_SHIFT = 1;
const unsigned int ACTION_ACCEPT = 2;

class LRAction {
public:
  int actionType;
  string target;
  Production reduceProduction;

  LRAction(): actionType(-1), target("") {}
  LRAction(int a): actionType(a), target("") {}
  LRAction(int a, string t):actionType(a), target(t) {}
  LRAction(int a, Production prod): actionType(a), reduceProduction(prod) {}
};

// (itemSetId, symbolText) -> action
// {itemSetId: {symbolText: action}} 
typedef unordered_map<string, LRAction> ActionMap;
typedef unordered_map<string, ActionMap> ActionTable;
typedef unordered_map<string, unordered_map<string, string>> GOTOTable;
}; // namespace fst
