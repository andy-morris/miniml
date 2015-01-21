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
    getline(cin, into);
  } catch (ios_base::failure) {
    if (cin.eof()) {
      quit();
    } else {
      throw;
    }
  }
}


pair<String, String>
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

  return make_pair(input.substr(0, semi), input.substr(semi+2));
}

void Repl::process(Ptr<Input> inp)
{
  switch (inp->type()) {
  case InputType::DECL:
    process(dyn_cast<DeclInput>(inp)->decl);
    break;
  case InputType::EXPR:
    process(dyn_cast<ExprInput>(inp)->expr);
    break;
  }
}


void Repl::process(Ptr<Expr> expr)
{
  using namespace ppr;

  auto ty = type_of(expr, type_env());
  auto nf = eval(expr, value_env());

  cout << *vcat({nf->ppr(), hcat({':'_p, ty->ppr() >> 1})}) << endl;
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
    auto msg = vcat({hcat({"val"_p, +val->name().ppr(),
                           ':'_p, +ty->ppr(), +'='_p}),
                     def->ppr() >> 1});
    cout << *msg << endl;
  }
}


[[noreturn]] void Repl::run()
{
  String input, rest;

  while (true) {
    Parser p;
    auto input_pair = get_next(rest);
    input = input_pair.first;
    rest = input_pair.second;

    Ptr<Input> inp;
    try {
      inp = p.parse(input);
      process(inp);
    } catch (Lexer::LexicalError &e) {
      cout << e.what() << endl;
      input = "";
    } catch (Parser::ParseFail &e) {
      cout << e.what() << endl;
      input = "";
    } catch (TCException &e) {
      cout << e.what() << endl;
      input = "";
    }
  }
}

}
