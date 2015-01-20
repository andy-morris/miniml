#ifndef REPL_HXX_ECGNQUAW
#define REPL_HXX_ECGNQUAW

#include "string.hxx"
#include "ptr.hxx"
#include "ast.hxx"
#include "env.hxx"

namespace miniml
{

struct EnvEntry final
{
  EnvEntry(Ptr<Type> t, Ptr<Expr> v): type(t), value(v) {}

  Ptr<Type> type;
  Ptr<Expr> value;
};

class Repl final
{
public:
  Repl(OStream &out, IStream &in);

  [[noreturn]] void run();

  inline String prompt() const { return m_prompt; }
  inline void set_prompt(String prompt) { m_prompt = prompt; }

  inline Ptr<Env<EnvEntry>> env() const { return m_env; }
  inline Ptr<Env<Type>> type_env() const;
  inline Ptr<Env<Expr>> value_env() const;

  inline OStream &out() { return m_out; }
  inline IStream &in() { return m_in; }

private:
  void prompt_line(String&);
  std::pair<String, String> get_next(String);
  void process(Ptr<Decl> decl);

  [[noreturn]] inline void quit() { std::exit(0); }

  String m_prompt;
  Ptr<Env<EnvEntry>> m_env;

  OStream &m_out;
  IStream &m_in;
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
