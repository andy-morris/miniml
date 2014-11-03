#include "ppr.hxx"
#include "lexer.hxx"

#include <memory>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace miniml;

auto read_file(const char *file)
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
  if (argc <= 1) return 1;

  auto str = read_file(argv[1]);
  Lexer l(str);

  for (auto tok: l.tokens()) {
    cout << *tok << endl;
  }
}
