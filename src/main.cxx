#include "repl.hxx"
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
  String read_file(const char *file)
  {
    ifstream in(file);
    in >> noskipws;
    stringstream ss;
    while (in) {
      string l;
      getline(in, l);
      ss << l;
    }
    return ss.str();
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

    auto decl = Parser().parse(str);
#ifndef NDEBUG
    cerr << endl;
#endif

    auto env = ptr<Env<Type>>();
    if (decl) {
#ifndef NDEBUG
      cerr << "Pretty printed AST:" << endl << *decl->ppr(true) << endl;
      cerr << "AST without location crud:" << endl << *decl->ppr() << endl;
#endif
      switch (decl->type()) {
      case DeclType::VAL:
        auto val = dyn_cast<ValDecl>(decl);
        cerr << "Type:" << endl << *type_of(val->def(), env)->ppr() << endl;
        cerr << *eval(ptr<Env<Expr>>(), val->def())->ppr() << endl;
        break;
      }
    } else {
      cerr << "parser error" << endl;
    }
  } catch (Exception &e) {
    cerr << e << endl;
    exit(1);
  }
}
