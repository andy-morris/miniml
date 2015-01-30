#include "repl.hxx"
#include "parser.hxx"
#include "tc.hxx"
#include "eval.hxx"
#include "ppr.hxx"
#include <iostream>
#include <fstream>
#include <cassert>

namespace miniml
{

namespace { using namespace std; }

Repl::Repl():
  m_prompt("miniml> ")
{
  m_env = init_val_env();
  read_file("prelude.mml");
  m_env->insert("use"_i,
                builtin(arr(string_, unit),
                        [&](Ptr<Expr> file) {
                          read_file(STRING(file).data());
                        }));
#ifndef NDEBUG
  m_env->debug();
#endif
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

void Repl::process(Ptr<Input> inp, bool output)
{
  switch (inp->type()) {
  case InputType::DECL:
    process(dyn_cast<DeclInput>(inp)->decl, output);
    break;
  case InputType::EXPR:
    process(dyn_cast<ExprInput>(inp)->expr, output);
    break;
  case InputType::MODULE:
    for (auto decl: *dyn_cast<ModuleInput>(inp)->decls) {
      process(decl, output);
    }
  }
}


void Repl::process(Ptr<Expr> expr, bool output)
{
  using namespace ppr;

  auto ty = type_of(expr, type_env());
  auto nf = eval(expr, value_env());

  if (output) {
    cout << *vcat({nf->ppr(), hcat({": "_p, ty->ppr()}) >> 1}) << endl;
  }
}


void Repl::process(Ptr<Decl> decl, bool output)
{
  switch (decl->type()) {
  case DeclType::VAL:
    process_val(dyn_cast<ValDecl>(decl), output);
  }
}

void Repl::process_val(Ptr<ValDecl> val, bool output)
{
  auto local_env = type_env();

  if (val->rec()) {
    local_env = ptr<Env<Type>>(local_env);
    if (!val->ty()) {
      throw UntypedRec(val);
    }
    local_env->insert(val->name(), val->ty());
  }

  auto ty = val->type_of(local_env);
  auto def = eval(val->def(), value_env());

  env()->insert(val->name(), ptr<EnvEntry>(ty, def));

  if (output) {
    auto msg = ppr::vcat({ppr::hcat({"val"_p, +val->name().ppr(),
                                     ':'_p, +ty->ppr(), +'='_p}),
                          def->ppr() >> 1});
    cout << *msg << endl;
  }
}

bool Repl::try_parse_process(const String &input, bool output)
{
  Ptr<Input> inp;
  Parser p;
  try {
    inp = p.parse(input);
    process(inp, output);
  } catch (LexerError &e) {
    cout << e.what() << endl;
    return false;
  } catch (Parser::ParseFail &e) {
    cout << e.what() << endl;
    return false;
  } catch (TCException &e) {
    cout << e.what() << endl;
    return false;
  }
  return true;
}


void Repl::read_file(const char *filename, bool output)
{
#ifndef NDEBUG
  clog << "reading " << filename << " ..." << endl;
#endif
  ifstream in(filename);
  if (in.fail()) {
    cerr << "file " << filename << " doesn't exist" << endl;
    return;
  }
  string contents;
  while (in.good()) {
    string line;
    getline(in, line);
    contents += line + "\n";
  }
  try_parse_process(contents, output);
}


[[noreturn]] void Repl::run()
{
  String input, rest;

  while (true) {
    auto input_pair = get_next(rest);
    input = input_pair.first;
    rest = input_pair.second;
    if (!try_parse_process(input)) {
      input = "";
    }
  }
}

}
