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

/*
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
*/

int main(int, char**)
{
  miniml::Repl().run();
}
