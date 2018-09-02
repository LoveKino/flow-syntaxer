#pragma once

#include "./contextFreeGrammer.h"
#include "unordered_set"

/**
 *
 * LR(1) parser, cannonical LR parser
 * Any LR(k) grammer with k > 1 can be reansformed into an LR(1) grammer.
 *
 * LR(k) can handle all deterministic context-free langugaes.
 *
 */
namespace fst {
class LR1 {
};

const unsigned int LR1_REDUCE_ITEM = 0;
const unsigned int LR1_SHIFT_ITEM = 1;
const unsigned int LR1_WAIT_REDUCE_ITEM = 2;

class LR1Item {
public: 
  fst::Production  production;
  unsigned int position;
  Symbol follow; // terminal symbol

  LR1Item() {}

  /**
   * position in [0, produciton.body.length]
   */
  LR1Item(fst::Production pro, unsigned int pos, string fol): production(pro), position(pos), follow(fol) {}

  /**
   * reduce item: [A -> α., b]
   * shift item: [A -> α.aβ, b]
   * wait to reduce item: [A -> α.Bβ, b]
   */
  unsigned int getItemType();

  // A -> α.Bβ => B
  Symbol getNextSymbol();

  // A -> α.Bβ => β
  vector<Symbol> getNextRestSymbols();

  string getId();

  /**
   * viable prefix
   * LR(1) item [A -> α.β, a] is valid for viable prefix ρ=δα, if:
   *   (1) exists S *=> δAw => δαβw
   *   (2) a if the first letter of w. if w = ε, a = #
   */

  string toString();
};

class LR1ItemSet {
private:
  LR1Item core; // core items. S'-> S or other position is not 0 items
  unordered_map<string, LR1Item> items;
public:
  LR1ItemSet(ContextFreeGrammer& cfg, LR1Item core);

  string getId();
  string toString();
};
}; // namespace fst