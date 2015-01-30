#ifndef REPL_HXX_ECGNQUAW
#define REPL_HXX_ECGNQUAW

#include "string.hxx"
#include "ptr.hxx"
#include "parser.hxx"
#include "ast.hxx"
#include "env.hxx"
#include "init_env.hxx"

namespace miniml
{

class Repl final
{
public:
  Repl();

  [[noreturn]] void run();

  inline String prompt() const { return m_prompt; }
  inline void set_prompt(String prompt) { m_prompt = prompt; }

  inline Ptr<Env<EnvEntry>> env() const { return m_env; }
  inline Ptr<Env<Type>> type_env() const;
  inline Ptr<Env<Expr>> value_env() const;

private:
  void prompt_line(String&);
  std::pair<String, String> get_next(String);
  bool try_parse_process(const String &input, bool output = true);
  void process(Ptr<Input> decl, bool output);
  void process(Ptr<Expr> decl, bool output);
  void process(Ptr<Decl> decl, bool output);
  void process_val(Ptr<ValDecl> val, bool output);
  void read_file(const char *filename, bool output = false);

  [[noreturn]] inline void quit() { std::exit(0); }

  String m_prompt;
  Ptr<Env<EnvEntry>> m_env;
};


inline Ptr<Env<Type>> Repl::type_env() const
{
  return map_env<Type>(env(), [](Ptr<EnvEntry> x){ return x->type; });
}

inline Ptr<Env<Expr>> Repl::value_env() const
{
  return map_env<Expr>(env(), [](Ptr<EnvEntry> x){ return x->value; });
}


}

#endif /* end of include guard: REPL_HXX_ECGNQUAW */
