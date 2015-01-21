#include "tc.hxx"

namespace miniml
{

namespace
{
  struct TypeOf final: public ExprVisitor<Type, Ptr<Env<Type>>>
  {
    using ExprVisitor<Type, Ptr<Env<Type>>>::v;

    Ptr<Type> v(Ptr<IdExpr> id, Ptr<Env<Type>> env) override
    {
      auto ty = env->lookup(id->id());
      return ty? ty: throw NotInScope(id->id());
    }

    inline Ptr<Type> v(Ptr<IntExpr>, Ptr<Env<Type>>) override
    {
      return ptr<IntType>();
    }

    inline Ptr<Type> v(Ptr<BoolExpr>, Ptr<Env<Type>>) override
    {
      return ptr<BoolType>();
    }

    Ptr<Type> v(Ptr<AppExpr> e, Ptr<Env<Type>> env) override
    {
      auto ty_f0 = v(e->left(), env);
      auto ty_x = v(e->right(), env);

      if (ty_f0->type() == TypeType::ARROW) {
        auto ty_f = dyn_cast<ArrowType>(ty_f0);
        check_eq(ty_f->left(), ty_x, e->left());
        return ty_f->right();
      } else {
        throw NotArrow(ty_f0);
      }
    }

    Ptr<Type> v(Ptr<LamExpr> e, Ptr<Env<Type>> env) override
    {
      auto inner = ptr<Env<Type>>(env);
      inner->insert(e->var(), e->ty());
      auto t = v(e->body(), inner);
      return ptr<ArrowType>(e->ty(), t);
    }

    inline Ptr<Type> v(Ptr<TypeExpr> e, Ptr<Env<Type>> env) override
    {
      check_eq(v(e->expr(), env), nf(e->ty(), env), e->expr());
      return e->ty();
    }

    Ptr<Type> v(Ptr<BinOpExpr> e, Ptr<Env<Type>> env) override
    {
      auto int_ = ptr<IntType>();
      auto bool_ = ptr<BoolType>();
      switch (e->op()) {
      case BinOp::PLUS:
      case BinOp::MINUS:
      case BinOp::TIMES:
      case BinOp::DIVIDE:
        check_eq(v(e->left(), env), int_, e->left());
        check_eq(v(e->right(), env), int_, e->right());
        return int_;
      case BinOp::LESS:
      case BinOp::LEQ:
      case BinOp::EQUAL:
      case BinOp::GEQ:
      case BinOp::GREATER:
      case BinOp::NEQ:
        check_eq(v(e->left(), env), int_, e->left());
        check_eq(v(e->right(), env), int_, e->right());
        return bool_;
      case BinOp::IFF:
      case BinOp::AND:
      case BinOp::OR:
        check_eq(v(e->left(), env), bool_, e->left());
        check_eq(v(e->right(), env), bool_, e->right());
        return bool_;
      case BinOp::SEQ:
        return v(e->right(), env);
      }
    }

    Ptr<Type> v(Ptr<TupleExpr> es, Ptr<Env<Type>> env) override
    {
      auto ts = ptr<TupleType::Types>();
      ts->reserve(es->exprs()->size());
      for (auto e: *es->exprs()) {
        ts->push_back(v(e, env));
      }
      return ptr<TupleType>(ts);
    }

    Ptr<Type> v(Ptr<DotExpr> e, Ptr<Env<Type>> env) override
    {
      auto ty = v(e->expr(), env);
      auto i = e->index();
      if (ty->type() == TypeType::TUPLE) {
        auto tys = dyn_cast<TupleType>(ty)->tys();
        if (i < tys->size()) {
          return tys->at(i);
        } else {
          throw CannotProject(e, ty, i);
        }
      } else {
        throw CannotProject(e, ty, i);
      }
    }

    Ptr<Type> v(Ptr<BuiltinExpr> e, Ptr<Env<Type>> env) override
    {
      auto ty = e->ty();
      for (auto a: *e->args()) {
        if (ty->type() == TypeType::ARROW) {
          auto aty = dyn_cast<ArrowType>(ty);
          check_eq(v(a, env), aty->left(), a);
          ty = aty->right();
        } else {
          throw NotArrow(ty);
        }
      }
      return ty;
    }

    inline void check_eq(Ptr<Type> s, Ptr<Type> t, Ptr<Expr> e)
    { if (*s != *t) throw Clash(s, t, e); }
  };
}

Ptr<Type> type_of(Ptr<Expr> expr, Ptr<Env<Type>> env)
{ return TypeOf()(expr, env); }

}
