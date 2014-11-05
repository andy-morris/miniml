#include "ast.hxx"
#include "parser.hxx"

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
  if (argc <= 1) return 1;
  auto str = read_file(argv[1]);
  auto expr = Parser().parse(str);
  if (expr) {
    cout << *expr->ppr() << endl;
  } else {
    cout << "parser error" << endl;
  }
}
