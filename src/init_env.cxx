#include "init_env.hxx"
#include <cassert>

namespace miniml
{
namespace
{
  using Args = BuiltinExpr::Args;

  inline Ptr<Type> arr(Ptr<Type> l, Ptr<Type> r)
  { return ptr<ArrowType>(l, r); }

  Ptr<Type> int_ = ptr<IntType>();

  inline long INT(Ptr<Expr> e)
  {
    assert(e->type() == ExprType::INT);
    return dyn_cast<IntExpr>(e)->val();
  }

  inline Ptr<Expr> INT(long e) { return ptr<IntExpr>(e); }

  inline Ptr<EnvEntry> builtin(Ptr<Type> ty, unsigned arity,
                               BuiltinExpr::Effect eff)
  {
    return ptr<EnvEntry>(ty, ptr<BuiltinExpr>(ty, eff, arity));
  }

  inline Ptr<EnvEntry> builtin(Ptr<Type> ty, std::function<void(Ptr<Expr>)> f)
  {
    return builtin(ty, 1, [f](Args a) { f(a[0]); return INT(0); });
  }

  inline Ptr<EnvEntry> builtin(Ptr<Type> ty,
                               std::function<void(Ptr<Expr>, Ptr<Expr>)> f)
  {
    return builtin(ty, 2, [f](Args a) { f(a[0], a[1]); return INT(0); });
  }

  inline Ptr<EnvEntry> builtin(Ptr<Type> ty, std::function<void()> f)
  {
    return builtin(ty, 1, [f](Args) { f(); return INT(0); });
  }
}


Ptr<Env<EnvEntry>> init_val_env()
{
  auto env = ptr<Env<EnvEntry>>();
  env->insert("print_int"_i,
              builtin(arr(int_, int_),
                      [](Ptr<Expr> e) { std::cout << INT(e); }));
  env->insert("newline"_i,
              builtin(arr(int_, int_), [] { std::cout << std::endl; }));
  // TODO
  return env;
}


Ptr<Env<Type>> init_type_env()
{
  auto env = ptr<Env<Type>>();
  // TODO
  return env;
}
}
