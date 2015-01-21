#include "repl.hxx"
#include "parser.hxx"
#include "tc.hxx"
#include "eval.hxx"
#include "ppr.hxx"
#include <iostream>
#include <cassert>

namespace miniml
{

namespace { using namespace std; }

Repl::Repl():
  m_prompt("> ")
{
  m_env = init_val_env();
  cin.exceptions(cin.badbit | cin.failbit);
}


void Repl::prompt_line(String &into)
{
  cout << prompt();
  flush(cout);
  try {
    std::getline(cin, into);
  } catch (std::ios_base::failure) {
    if (cin.eof()) {
      quit();
    } else {
      throw;
    }
  }
}


std::pair<String, String>
Repl::get_next(String rest)
{
  String input(move(rest));
  size_t semi = input.find(';');

  if (semi == String::npos) {
    semi = 0;
    String line;
    size_t semi_line = 0;

    do {
      prompt_line(line);
      input += line;
      semi_line = line.find(";;");
      semi += semi_line == String::npos? line.size() : semi_line;
    } while (semi_line == String::npos);

  }

  assert(semi <= input.length());
  assert(input[semi]   == ';' &&
         input[semi+1] == ';');

  return std::make_pair(input.substr(0, semi), input.substr(semi+2));
}

void Repl::process(Ptr<Decl> decl)
{
  using namespace ppr;

  switch (decl->type()) {
  case DeclType::VAL:
    auto val = dyn_cast<ValDecl>(decl);
    auto ty = type_of(val->def(), type_env());
    auto def = eval(val->def(), value_env());
    env()->insert(val->name(), ptr<EnvEntry>(ty, def));
    auto msg = hcat({"val"_p, +val->name().ppr(),
                     ':'_p, +ty->ppr(),
                     +'='_p, +def->ppr()});
    cout << *msg << std::endl;
  }
}

[[noreturn]] void Repl::run()
{
  String input, rest;

  while (true) {
    Parser p;
    auto inp = get_next(rest);
    input = inp.first;
    rest = inp.second;

    Ptr<Decl> decl;
    try {
      decl = p.parse(input);
      process(decl);
    } catch (Lexer::LexicalError &e) {
      cout << e.what() << std::endl;
      input = "";
    } catch (Parser::ParseFail &e) {
      cout << e.what() << std::endl;
      input = "";
    } catch (TCException &e) {
      cout << e.what() << std::endl;
      input = "";
    }
  }
}

}
