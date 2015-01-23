#include "init_env.hxx"
#include <cassert>

namespace miniml
{
namespace
{
  using Args = BuiltinExpr::Args;

  template <typename T>
  inline std::initializer_list<Ptr<T>> empty()
  { return std::initializer_list<Ptr<T>>(); }

  inline Ptr<Type> arr(Ptr<Type> l, Ptr<Type> r)
  { return ptr<ArrowType>(l, r); }

  Ptr<Type> int_ = ptr<IntType>();
  Ptr<Type> string_ = ptr<StringType>();
  Ptr<Type> bool_ = ptr<BoolType>();
  Ptr<Type> unit = ptr<TupleType>(empty<Type>());

  Ptr<Expr> UNIT = ptr<TupleExpr>(empty<Expr>());

  inline long INT(Ptr<Expr> e)
  {
    assert(e->type() == ExprType::INT);
    return dyn_cast<IntExpr>(e)->val();
  }

  inline String STRING(Ptr<Expr> e)
  {
    assert(e->type() == ExprType::STRING);
    return *dyn_cast<StringExpr>(e)->val();
  }

  inline Ptr<Expr> STRING(String &&e)
  { return ptr<StringExpr>(std::forward<String>(e)); }

  inline Ptr<EnvEntry> builtin(Ptr<Type> ty, unsigned arity,
                               BuiltinExpr::Effect eff)
  {
    return ptr<EnvEntry>(ty, ptr<BuiltinExpr>(ty, eff, arity));
  }

  inline Ptr<EnvEntry> builtin(Ptr<Type> ty,
                               std::function<Ptr<Expr>(Ptr<Expr>)> f)
  {
    return builtin(ty, 1, [f](Args a) { return f(a[0]); });
  }

  inline Ptr<EnvEntry> builtin(Ptr<Type> ty, std::function<void(Ptr<Expr>)> f)
  {
    return builtin(ty, 1, [f](Args a) { f(a[0]); return UNIT; });
  }

  inline Ptr<EnvEntry> builtin(Ptr<Type> ty,
                               std::function<Ptr<Expr>(Ptr<Expr>,Ptr<Expr>)> f)
  {
    return builtin(ty, 2, [f](Args a) { return f(a[0], a[1]); });
  }

  inline Ptr<EnvEntry> builtin(Ptr<Type> ty, std::function<void()> f)
  {
    return builtin(ty, 1, [f](Args) { f(); return UNIT; });
  }
}


Ptr<Env<EnvEntry>> init_val_env()
{
  auto env = ptr<Env<EnvEntry>>();
  env->insert("newline"_i,
              builtin(arr(unit, unit), [] { std::cout << std::endl; }));
  env->insert("string_int"_i,
              builtin(arr(int_, string_),
                      [] (Ptr<Expr> e) {
                        return STRING(std::to_string(INT(e)));
                      }));
  env->insert("print"_i,
              builtin(arr(string_, unit),
                      [] (Ptr<Expr> e) { std::cout << STRING(e); }));
  env->insert("app"_i,
              builtin(arr(string_, arr(string_, string_)),
                      [] (Ptr<Expr> s, Ptr<Expr> t) {
                        return STRING(STRING(s) + STRING(t));
                      }));
  return env;
}
}
