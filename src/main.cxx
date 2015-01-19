#include "ast.hxx"
#include "lexer.hxx"
#include "parser.hxx"
#include "tc.hxx"
#include "eval.hxx"

#include <memory>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace miniml;

namespace
{
  Ptr<String> read_file(const char *file)
  {
    ifstream in(file);
    in >> noskipws;
    stringstream ss;
    while (in) {
      string l;
      getline(in, l);
      ss << l;
    }
    return ptr<String>(ss.str());
  }
}

int main(int argc, char **argv)
{
  try {
    if (argc <= 1) return 1;
    auto str = read_file(argv[1]);

    auto toks = Lexer(str).tokens();
#ifndef NDEBUG
    for (auto tok: toks) {
      cerr << *tok << endl;
    }
    cerr << endl;
#endif

    auto expr = Parser().parse(str);
#ifndef NDEBUG
    cerr << endl;
#endif

    auto env = ptr<Env<Type>>();
    if (expr) {
#ifndef NDEBUG
      cerr << "Pretty printed AST:" << endl << *expr->ppr(true) << endl;
      cerr << "Type:" << endl << *type_of(expr, env)->ppr() << endl;
#endif
      cerr << *eval(ptr<Env<Expr>>(), expr)->ppr() << endl;
    } else {
      cerr << "parser error" << endl;
    }
  } catch (Exception &e) {
    cerr << e << endl;
    exit(1);
  }
}
