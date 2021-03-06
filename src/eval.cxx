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
      if (e->type() == ExprType::BUILTIN) {
        return v(e->dup(), env);
      } else {
        return v(e, env);
      }
    }

    Ptr<Expr> v(Ptr<AppExpr> x, ENV env) override
    {
      auto l = v(x->left(),  env);
      auto r = v(x->right(), env);

      Ptr<LamExpr> lam; Ptr<BuiltinExpr> bi;

      switch (l->type()) {
      case ExprType::LAM:
        lam = dyn_cast<LamExpr>(l);
        return v(lam->apply(r), lam->env());
      case ExprType::BUILTIN:
        bi = dyn_cast<BuiltinExpr>(l);
        if (bi->need_arg()) {
          bi->give_arg(r);
          return v(bi, env);
        } // else fall thru
      default:
        return ptr<AppExpr>(l, r);
      }
    }

    inline Ptr<Expr> v(Ptr<IntExpr> x, ENV) override { return x; }

    inline Ptr<Expr> v(Ptr<BoolExpr> x, ENV) override { return x; }

    inline Ptr<Expr> v(Ptr<StringExpr> x, ENV) override { return x; }

    Ptr<Expr> v(Ptr<LamExpr> x, ENV env) override
    {
      x->set_env(env);
      return x;
    }

    Ptr<Expr> v(Ptr<IfExpr> x, ENV env) override
    {
      if (lit<ExprType::BOOL, BoolExpr, bool>(v(x->cond(), env))) {
        return v(x->thenCase(), env);
      } else {
        return v(x->elseCase(), env);
      }
    }

    Ptr<Expr> v(Ptr<BinOpExpr> x, ENV env) override
    {
      using namespace std;

      auto l = v(x->left(), env), r = v(x->right(), env);

      auto to_int = lit<ExprType::INT, IntExpr, long>;
      auto to_bool = lit<ExprType::BOOL, BoolExpr, bool>;

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
      case BinOp::SEQ:     return r;
#ifdef __GNUC__
      default:             std::abort();
#endif
      }
    }

    inline Ptr<Expr> v(Ptr<TypeExpr> x, ENV env) override
    { return v(x->expr(), env); }

    Ptr<Expr> v(Ptr<TupleExpr> x, ENV env) override
    {
      auto es = ptr<TupleExpr::Exprs>();
      es->reserve(x->exprs()->size());
      for (auto e: *x->exprs()) {
        es->push_back(v(e, env));
      }
      return ptr<TupleExpr>(es);
    }

    Ptr<Expr> v(Ptr<DotExpr> x, ENV env) override
    {
      auto y = v(x->expr(), env);
      auto i = x->index();
      if (y->type() == ExprType::TUPLE) {
        auto tup = dyn_cast<TupleExpr>(y);
        assert(tup->exprs()->size() > i);
        return tup->exprs()->at(i);
      } else {
        return ptr<DotExpr>(y, i);
      }
    }

    Ptr<Expr> v(Ptr<BuiltinExpr> x, ENV env) override
    {
      if (x->need_arg()) {
        return x;
      } else {
        return v(x->run(), env);
      }
    }
  };
}

Ptr<Expr> eval(Ptr<Expr> e, Ptr<Env<Expr>> env)
{
  Eval ev; return ev(e, env);
}

}
