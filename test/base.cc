#include "../lib/LR1Table.h"
#include "../lib/AST.h"
#include "../lib/LR1Parser.h"
#include "token.h"
#include "unit.h"
#include "assert.h"
#include "vector"
#include <iostream>

using namespace std;

// start = S
// S -> id, S -> ε
fst::ContextFreeGrammer egGrammer1() {
  auto start = fst::Symbol::NonTerminal("S");

  fst::Production p1(start, vector<fst::Symbol>{
    fst::Symbol::Epsilon()
  });
  fst::Production p2(start, vector<fst::Symbol>{
    fst::Symbol::NonTerminal("S"),
    fst::Symbol::Terminal("id")
  });
  fst::Production p3(start, vector<fst::Symbol>{
    fst::Symbol::Terminal("id")
  });

  return fst::ContextFreeGrammer(start, vector<fst::Production>{p1, p2, p3});
}

// start = S
// S -> id, S -> S + id
fst::ContextFreeGrammer egGrammer2() {
  auto start = fst::Symbol::NonTerminal("S");

  fst::Production p1(start, vector<fst::Symbol>{
    fst::Symbol::Terminal("id")
  });
  fst::Production p2(start, vector<fst::Symbol>{
    start,
    fst::Symbol::Terminal("+"),
    fst::Symbol::Terminal("id")
  });

  return fst::ContextFreeGrammer(start, vector<fst::Production>{p1, p2});
}

// start = S
// S -> CC , C ->cC | d
fst::ContextFreeGrammer egGrammer3() {
  auto start = fst::Symbol::NonTerminal("S");

  // S -> CC
  fst::Production p1(start, vector<fst::Symbol>{
    fst::Symbol::NonTerminal("C"),
    fst::Symbol::NonTerminal("C")
  });

  // C -> cC
  fst::Production p2(fst::Symbol::NonTerminal("C"), vector<fst::Symbol>{
    fst::Symbol::Terminal("c"),
    fst::Symbol::NonTerminal("C")
  });

  // C -> d
  fst::Production p3(fst::Symbol::NonTerminal("C"), vector<fst::Symbol>{
    fst::Symbol::Terminal("d"),
  });

  return fst::ContextFreeGrammer(start, vector<fst::Production>{p1, p2, p3});
}


int main() {
    unit_test::group("first set", vector<unit_test::UnitCase> {
      unit_test::test("first set", [&]() {
        auto g1 = egGrammer1();
        auto m = g1.getFirstSetMap();
        unordered_map<string, unordered_set<string>> r1 = {{"S", {"id", fst::EPSILON}}, {"id", {"id"}}, {fst::EPSILON, {fst::EPSILON}}, {fst::END_SYMBOL_TEXT, {fst::END_SYMBOL_TEXT}}};
        assert(g1.getFirstSetMap() == r1);
      }),
    }).run();

    unit_test::group("lr1", vector<unit_test::UnitCase> {
      unit_test::test("get item", [&]() {
        // E -> F id
        auto production = fst::Production(
          fst::Symbol::NonTerminal("E"),

          vector<fst::Symbol>{
            fst::Symbol::NonTerminal("F"),
            fst::Symbol::Terminal("id")
          }
        );

        auto item1 = fst::LR1Item(production, 0, "a");
        auto item2 = fst::LR1Item(production, 1, "a");
        auto item3 = fst::LR1Item(production, 2, "a");

        assert(item1.getItemType() == fst::LR1_WAIT_REDUCE_ITEM);
        assert(item2.getItemType() == fst::LR1_SHIFT_ITEM);
        assert(item3.getItemType() == fst::LR1_REDUCE_ITEM);
      }),

      unit_test::test("closure", [&]() {
        auto g1 = egGrammer1();
        cout << fst::LR1ItemSet(g1, vector<fst::LR1Item>{fst::LR1Item(g1.productions[1], 0, fst::END_SYMBOL_TEXT)}).toString() << endl;
        cout << fst::LR1ItemSet(g1, vector<fst::LR1Item>{fst::LR1Item(g1.productions[0], 0, fst::END_SYMBOL_TEXT)}).toString() << endl;
        cout << fst::LR1ItemSet(g1, vector<fst::LR1Item>{fst::LR1Item(g1.productions[2], 0, fst::END_SYMBOL_TEXT)}).toString() << endl;
      }),

      unit_test::test("closure2", [&]() {
        auto g = egGrammer3();
        cout << fst::LR1ItemSet(g, vector<fst::LR1Item>{
          fst::LR1Item(
           fst::Production(
             fst::Symbol(fst::EXPAND_START_SYMBOL_TEXT),
             vector<fst::Symbol>{g.start}
           ),
           0,
           fst::END_SYMBOL_TEXT)
        }).toString() << endl;
      }),

      unit_test::test("lr1 table", [&]() {
        auto g1 = egGrammer1();
        auto t = fst::LR1Table(g1);
        cout << t.C.toString() << endl;
      }),

      unit_test::test("lr1 table2", [&]() {
        auto g = egGrammer3();
        auto t = fst::LR1Table(g);
        cout << t.C.toString() << endl;
      }),
    }).run();

    // ast tree
    unit_test::group("ast tree", vector<unit_test::UnitCase> {
      unit_test::test("base", [&]() {
         // S -> E
         // E -> num + num
         // token list: 1 + 2
         auto ast = fst::AstNode::initAst(fst::Symbol(fst::NON_TERMINAL_SYMBOL_TYPE, "S"));
         fst::AstNode::appendToken(ast, ftp::Token("num", "1"));
         fst::AstNode::appendToken(ast, ftp::Token("+", "+"));
         fst::AstNode::appendToken(ast, ftp::Token("num", "2"));

         // reduce
         fst::AstNode::reduceAst(ast, 0, 2, fst::Symbol(fst::NON_TERMINAL_SYMBOL_TYPE, "E"));
         assert(ast.children.size() == 1);
         assert(ast.children[0].symbol.text == "E");
      }),
    }).run();

    // LR1 parser
    unit_test::group("lr1 parser", vector<unit_test::UnitCase> {
      unit_test::test("base", [&]() {
        auto cfg = egGrammer2();
        auto lrParser = fst::LR1Parser::getLRParser(cfg);
        // input string: 1 + 2
        lrParser.acceptToken(ftp::Token("id", "1"));
        lrParser.acceptToken(ftp::Token("+", "+"));
        lrParser.acceptToken(ftp::Token("id", "2"));
        lrParser.endToken();
      }),
    }).run();

    return 0;
}
