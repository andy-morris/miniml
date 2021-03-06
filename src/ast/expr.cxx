#include "token.hxx"
#include "ast/expr.hxx"
#include <sstream>
#include <cassert>

namespace miniml
{

using namespace ppr;
using std::unordered_set;

Ptr<Ppr> IdExpr::ppr(unsigned, bool pos) const
{
  return pos_if(pos, id().ppr(), start(), end());
}


Ptr<Ppr> IntExpr::ppr(unsigned, bool pos) const
{
  return pos_if(pos, num(val()), start(), end());
}


Ptr<Ppr> BoolExpr::ppr(unsigned, bool pos) const
{
  return pos_if(pos, bool_(val()), start(), end());
}


Ptr<Ppr> StringExpr::ppr(unsigned, bool pos) const
{
  return pos_if(pos, hcat({'"'_p, string(escaped(*val())), '"'_p}),
                start(), end());
}


Ptr<Ppr> AppExpr::ppr(unsigned prec, bool pos) const
{
  return pos_if(pos,
                parens_if(prec > 10 || pos,
                          hcat({left()->ppr(10, pos),
                                +right()->ppr(11, pos)})),
                start(), end());
}


Ptr<Ppr> LamExpr::ppr(unsigned prec, bool pos) const
{
  return pos_if(pos,
                parens_if(prec > 0 || pos,
                          vcat({hcat({"fn ("_p, var().ppr(pos),
                                      ':'_p,
                                      +ty()->ppr(pos), ')'_p, +"=>"_p}),
                                (body()->ppr(pos) >> 1)})),
                start(), end());
}

Ptr<Ppr> IfExpr::ppr(unsigned prec, bool pos) const
{
  return pos_if(pos,
                parens_if(prec > 10 || pos,
                          vcat({hcat({"if "_p, cond()->ppr(10, pos)}),
                                thenCase()->ppr(10, pos) >> 1,
                                elseCase()->ppr(10, pos) >> 1})),
                start(), end());
}


Ptr<Ppr> TypeExpr::ppr(unsigned prec, bool pos) const
{
  return pos_if(pos,
                parens_if(prec > 0 || pos,
                          hcat({expr()->ppr(pos),
                                ':'_p,
                                +ty()->ppr(pos)})),
                start(), end());
}

Ptr<Ppr> BinOpExpr::ppr(unsigned pr, bool pos) const
{
  unsigned oprec = prec(op()),
           lprec = assoc(op()) == OpAssoc::LEFT?  oprec : oprec + 1,
           rprec = assoc(op()) == OpAssoc::RIGHT? oprec : oprec + 1;
  return pos_if(pos,
                parens_if(pr > oprec || pos,
                          hcat({left()->ppr(lprec, pos),
                                +name(op()),
                                +right()->ppr(rprec, pos)})),
                start(), end());
}


void BuiltinExpr::give_arg(Ptr<Expr> arg)
{
  assert(need_arg());
  args()->push_back(arg);
}

Ptr<Expr> BuiltinExpr::run()
{
  assert(!need_arg());
  return effect()(*args());
}

Ptr<Expr> BuiltinExpr::dup() const
{
  auto d = ptr<BuiltinExpr>(ty()->dup(), effect(), arity(), start(), end());
  for (auto a: *args()) {
    d->give_arg(a->dup());
  }
  return d;
}

Ptr<Ppr> TupleExpr::ppr(unsigned, bool pos) const
{
  auto pprs = ptr<std::list<Ptr<Ppr>>>();
  for (auto e: *exprs()) {
    pprs->push_back(e->ppr(0, pos));
    pprs->push_back(", "_p);
  }
  if (pprs->size() > 0) pprs->pop_back(); // final comma
  return pos_if(pos, parens_if(true, hcat(pprs)), start(), end());
}


Ptr<Ppr> DotExpr::ppr(unsigned, bool pos) const
{
  return pos_if(pos,
                hcat({expr()->ppr(), '.'_p, ppr::num(index())}),
                start(), end());
}


namespace
{
  /// Free variables
  struct FV: public ExprVisitor<unordered_set<Id>>
  {
    using ExprVisitor<unordered_set<Id>>::v;

    typedef Ptr<unordered_set<Id>> Ret;

    Ret v(Ptr<IdExpr> i) override { return single(i->id()); }
    Ret v(Ptr<IntExpr>) override { return Ret(); }
    Ret v(Ptr<BoolExpr>) override { return Ret(); }
    Ret v(Ptr<StringExpr>) override { return Ret(); }

    Ret v(Ptr<AppExpr> e) override
    {
      auto s1 = v(e->left()), s2 = v(e->right());
      s1->insert(s2->begin(), s2->end());
      return s1;
    }

    Ret v(Ptr<LamExpr> e) override
    {
      auto s = v(e->body());
      s->erase(e->var());
      return s;
    }

    Ret v(Ptr<IfExpr> e) override
    {
      auto s = ptr<unordered_set<Id>>(),
           sc = v(e->cond()),
           st = v(e->thenCase()),
           se = v(e->elseCase());
      s->insert(sc->begin(), sc->end());
      s->insert(st->begin(), st->end());
      s->insert(se->begin(), se->end());
      return s;
    }

    Ret v(Ptr<TypeExpr> e) override
    {
      return v(e->expr());
    }

    Ret v(Ptr<BinOpExpr> e) override
    {
      auto s1 = v(e->left()), s2 = v(e->right());
      s1->insert(s2->begin(), s2->end());
      return s1;
    }

    Ret v(Ptr<TupleExpr> es) override
    {
      auto s1 = ptr<unordered_set<Id>>();
      for (auto e: *es->exprs()) {
        auto s2 = v(e);
        s1->insert(s2->begin(), s2->end());
      }
      return s1;
    }

    inline Ret v(Ptr<DotExpr> e) override { return v(e->expr()); }

    Ret v(Ptr<BuiltinExpr> e) override
    {
      Ret fv;
      for (auto arg: *e->args()) {
        auto fv0 = v(arg);
        fv->insert(fv0->begin(), fv0->end());
      }
      return fv;
    }

    static Ret single(const Id x)
    {
      auto set = ptr<unordered_set<Id>>();
      set->insert(x);
      return set;
    }
  };

  struct Subst:
    public ExprVisitor<Expr, const Id, Ptr<Expr>, Ptr<unordered_set<Id>>>
  {
    using ExprVisitor<Expr, const Id, Ptr<Expr>, Ptr<unordered_set<Id>>>::v;

    Ptr<Expr> v(Ptr<IdExpr> e, const Id x, Ptr<Expr> arg, FV::Ret) override
    {
      return e->id() == x? arg : e;
    }

    Ptr<Expr> v(Ptr<AppExpr> e, const Id x, Ptr<Expr> arg, FV::Ret fv) override
    {
      return ptr<AppExpr>(v(e->left(), x, arg, fv),
                          v(e->right(), x, arg, fv));
    }

    Ptr<Expr> v(Ptr<IfExpr> e, const Id x, Ptr<Expr> arg, FV::Ret fv) override
    {
      return ptr<IfExpr>(v(e->cond(), x, arg, fv),
                         v(e->thenCase(), x, arg, fv),
                         v(e->elseCase(), x, arg, fv));
    }

    Ptr<Expr> v(Ptr<IntExpr> e, const Id, Ptr<Expr>, FV::Ret) override
    { return e; }

    Ptr<Expr> v(Ptr<BoolExpr> e, const Id, Ptr<Expr>, FV::Ret) override
    { return e; }

    Ptr<Expr> v(Ptr<StringExpr> e, const Id, Ptr<Expr>, FV::Ret) override
    { return e; }


    Ptr<Expr> v(Ptr<LamExpr> e, const Id x, Ptr<Expr> arg, FV::Ret fv) override
    {
      // capture-avoiding substitution (λx.e)[y:=e']:
      //   - if x = y, do nothing
      //   - else
      //     - if x ∈ FV(e'), use (λx'.e[x:=x']) instead for fresh x'
      //     - subst body as normal

      if (x == e->var()) return e;

      // id = x, id2 = x'
      auto id = e->var(), id2 = e->var();
      if (fv->find(x) != fv->end()) {
        // find fresh variable
        unsigned i = 0;
        while (fv->find(id) != fv->end())
          id2 = id.suffix(i++);
      }

      auto body =
        id == id2?
          e->body()
        : v(e->body(), id, ptr<IdExpr>(id2), FV::single(id2));

      return ptr<LamExpr>(id2, e->ty(), v(body, x, arg, fv));
    }

    Ptr<Expr> v(Ptr<TypeExpr> e, const Id x, Ptr<Expr> arg, FV::Ret fv) override
    {
      return ptr<TypeExpr>(v(e->expr(), x, arg, fv), e->ty());
    }

    Ptr<Expr> v(Ptr<BinOpExpr> e, const Id x, Ptr<Expr> arg, FV::Ret fv)
      override
    {
      return ptr<BinOpExpr>(e->op(),
                            v(e->left(), x, arg, fv),
                            v(e->right(), x, arg, fv));
    }

    Ptr<Expr> v(Ptr<TupleExpr> e, const Id x, Ptr<Expr> arg, FV::Ret fv)
      override
    {
      auto es = ptr<TupleExpr::Exprs>();
      es->reserve(e->exprs()->size());
      for (auto elt: *e->exprs()) {
        es->push_back(v(elt, x, arg, fv));
      }
      return ptr<TupleExpr>(es);
    }

    inline Ptr<Expr> v(Ptr<DotExpr> e, const Id x, Ptr<Expr> arg, FV::Ret fv)
      override
    {
      return ptr<DotExpr>(v(e->expr(), x, arg, fv), e->index());
    }

    Ptr<Expr> v(Ptr<BuiltinExpr> e, const Id x, Ptr<Expr> arg, FV::Ret fv)
      override
    {
      auto expr = ptr<BuiltinExpr>(e->ty(), e->effect(), e->arity());
      for (auto a: *e->args()) {
        expr->give_arg(v(a, x, arg, fv));
      }
      return expr;
    }
  };
}

Ptr<unordered_set<Id>> fv(const Ptr<Expr> expr)
{ return FV()(expr); }

Ptr<Expr> Expr::subst(const Id var, const Ptr<Expr> expr)
{
  return Subst()(dup(), var, expr, fv(expr));
}

Ptr<Expr> LamExpr::apply(const Ptr<Expr> arg) const
{
  auto fv = FV()(arg);
  return Subst()(body(), var(), arg, fv);
}

}
