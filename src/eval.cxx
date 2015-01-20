#include "eval.hxx"
#include <functional>
#include <cassert>

namespace miniml
{

namespace
{
  using ENV = Ptr<Env<Expr>>;

  template <ExprType ty, typename T, typename E>
  inline E lit(Ptr<Expr> e)
  {
    assert(e->type() == ty);
    return dyn_cast<T>(e)->val();
  }

  struct Eval final: public ExprVisitor<Expr, ENV>
  {
    using ExprVisitor<Expr, ENV>::v;

    Ptr<Expr> v(Ptr<IdExpr> x, ENV env) override
    {
      auto e = env->lookup(x->id());
      assert(e);
      return v(e, env);
    }

    Ptr<Expr> v(Ptr<AppExpr> x, ENV env) override
    {
      auto l = v(x->left(),  env);
      auto r = v(x->right(), env);
      if (l->type() == ExprType::LAM) {
        auto ll = dyn_cast<LamExpr>(move(l));
        return v(ll->apply(r), ll->env());
      } else {
        return ptr<AppExpr>(l, r);
      }
    }

    inline Ptr<Expr> v(Ptr<IntExpr> x, ENV) override { return x; }

    inline Ptr<Expr> v(Ptr<BoolExpr> x, ENV) override { return x; }

    Ptr<Expr> v(Ptr<LamExpr> x, ENV env) override
    {
      x->set_env(env);
      return x;
    }

    Ptr<Expr> v(Ptr<BinOpExpr> x, ENV env) override
    {
      using namespace std;

      auto l = v(x->left(), env), r = v(x->right(), env);

      auto to_int = lit<ExprType::INT, IntExpr, long>;
      auto to_bool = lit<ExprType::INT, BoolExpr, bool>;

      auto int_op =
        [&](function<long(long, long)> f) {
          return ptr<IntExpr>(f(to_int(l), to_int(r)));
        };
      auto bool_op =
        [&](function<bool(bool, bool)> f) {
          return ptr<BoolExpr>(f(to_bool(l), to_bool(r)));
        };
      auto int_cmp =
        [&](function<bool(long, long)> f) {
          return ptr<BoolExpr>(f(to_int(l), to_int(r)));
        };

      switch (x->op()) {
      case BinOp::PLUS:    return int_op(plus<long>());
      case BinOp::MINUS:   return int_op(minus<long>());
      case BinOp::TIMES:   return int_op(multiplies<long>());
      case BinOp::DIVIDE:  return int_op(divides<long>());
      case BinOp::AND:     return bool_op(logical_and<bool>());
      case BinOp::OR:      return bool_op(logical_or<bool>());
      case BinOp::IFF:     return bool_op(equal_to<bool>());
      case BinOp::LESS:    return int_cmp(less<long>());
      case BinOp::LEQ:     return int_cmp(less_equal<long>());
      case BinOp::EQUAL:   return int_cmp(equal_to<long>());
      case BinOp::GEQ:     return int_cmp(greater_equal<long>());
      case BinOp::GREATER: return int_cmp(greater<long>());
      case BinOp::NEQ:     return int_cmp(not_equal_to<long>());
      }
    }

    inline Ptr<Expr> v(Ptr<TypeExpr> x, ENV env) override
    { return v(x->expr(), env); }
  };
}

Ptr<Expr> eval(Ptr<Expr> e, Ptr<Env<Expr>> env)
{
  Eval ev; return ev(e, env);
}

}
