#include "repl.hxx"
#include "parser.hxx"
#include "tc.hxx"
#include "eval.hxx"
#include "ppr.hxx"
#include <iostream>
#include <cassert>

namespace miniml
{

Repl::Repl(OStream &out, IStream &in):
  m_prompt("> "), m_out(out), m_in(in)
{
  m_env = ptr<Env<EnvEntry>>();
  in.exceptions(in.badbit | in.failbit);
}


void Repl::prompt_line(String &into)
{
  out() << prompt();
  std::flush(out());
  try {
    std::getline(in(), into);
  } catch (std::ios_base::failure) {
    if (in().eof()) {
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
      semi_line = line.find(';');
      semi += semi_line == String::npos? line.size() : semi_line;
    } while (semi_line == String::npos);

  }

  assert(semi <= input.length());
  assert(input[semi] == ';');

  return std::make_pair(input.substr(0, semi), input.substr(semi+1));
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
    out() << *msg << std::endl;
  }
}

[[noreturn]] void Repl::run()
{
  String input, rest;
  Parser p;

  while (true) {
    auto inp = get_next(rest);
    input = inp.first;
    rest = inp.second;

    Ptr<Decl> decl;
    try {
      decl = p.parse(input);
      process(decl);
    } catch (Lexer::LexicalError &e) {
      out() << e.what() << std::endl;
      input = "";
    } catch (Parser::ParseFail &e) {
      out() << e.what() << std::endl;
      input = "";
    } catch (TCException &e) {
      out() << e.what() << std::endl;
      input = "";
    }
  }
}

}
