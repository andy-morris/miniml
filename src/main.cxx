#include "ast.hxx"
#include "lexer.hxx"
#include "parser.hxx"
#include "tc.hxx"

#include <memory>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace miniml;

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

int main(int argc, char **argv)
{
  try {
    if (argc <= 1) return 1;
    auto str = read_file(argv[1]);

    auto toks = Lexer(str).tokens();
    for (auto tok: toks) {
      cerr << *tok << endl;
    }
    cerr << endl;

    auto expr = Parser().parse(str);
    cerr << endl;

    auto env = ptr<Env<Type>>();
    if (expr) {
      cerr << *expr->ppr() << endl;
      cerr << *type_of(expr, env)->ppr() << endl;
    } else {
      cerr << "parser error" << endl;
    }
  } catch (Exception &e) {
    cerr << e << endl;
    exit(1);
  }
}
