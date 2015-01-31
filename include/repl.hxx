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

/// Read-eval-print loop.
class Repl final
{
public:
  Repl();

  /// Run the repl then exit.
  [[noreturn]] void run();

  /// Prompt when expecting user input
  inline String prompt() const { return m_prompt; }
  inline void set_prompt(String prompt) { m_prompt = prompt; }

  /// Current environment.
  inline Ptr<Env<EnvEntry>> env() const { return m_env; }
  /// Types in #env().
  inline Ptr<Env<Type>> type_env() const;
  /// Values in #env().
  inline Ptr<Env<Expr>> value_env() const;

private:
  /// Prompt for another line of input.
  void prompt_line(String&);
  /// Split at the first ';;'
  std::pair<String, String> get_next(String);
  /// Try to parse an input and then #process() it.
  bool try_parse_process(const String &input, bool output = true);
  void process(Ptr<Input> decl, bool output);
  /// Evaluate an expression and output its value and type.
  void process(Ptr<Expr> decl, bool output);
  /// Add a declaration to the environment.
  void process(Ptr<Decl> decl, bool output);
  void process_val(Ptr<ValDecl> val, bool output);
  /// Read a file and #process() its contents.
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
