#include "ast/type.hxx"

namespace miniml
{

using namespace ppr;

bool IdType::operator==(const Type &other) const
{
  if (type() == other.type())
    return id() == dynamic_cast<const IdType&>(other).id();
  return false;
}


bool ArrowType::operator==(const Type &other) const
{
  if (type() == other.type()) {
    auto other0 = dynamic_cast<const ArrowType&>(other);
    return *left() == *other0.left() && *right() == *other0.right();
  }
  return false;
}

Ptr<Ppr> ArrowType::ppr(unsigned prec, bool pos) const
{
  return pos_if(pos,
                parens_if(prec > 0 || pos,
                          hcat({left()->ppr(1, pos),
                                +"->"_p,
                                +right()->ppr(0, pos)})),
                start(), end());
}


bool TupleType::operator==(const Type &other) const
{
  if (type() == other.type()) {
    auto other0 = dynamic_cast<const TupleType&>(other);
    auto it1 = tys()->begin();
    auto it2 = other0.tys()->begin();
    for (; it1 != tys()->end() && it2 != other0.tys()->end();
           ++it1, ++it2) {
      if (*it1 != *it2) return false;
    }
    return (it1 == tys()->end() && it2 == other0.tys()->end());
  }
  return false;
}

Ptr<Ppr> TupleType::ppr(unsigned, bool pos) const
{
  auto pprs = ptr<std::list<Ptr<Ppr>>>();
  for (auto e: *tys()) {
    pprs->push_back(e->ppr(0, pos));
    pprs->push_back(", "_p);
  }
  if (pprs->size() > 0) pprs->pop_back(); // final comma
  return pos_if(pos, parens_if(true, hcat(pprs)), start(), end());
}


namespace
{
  struct TypeNF final: public TypeVisitor<Type, Ptr<Env<Type>>>
  {
    using TypeVisitor<Type, Ptr<Env<Type>>>::v;

    Ptr<Type> v(Ptr<IdType> id, Ptr<Env<Type>> env) override
    {
      auto t = env->lookup(id->id());
      return t? v(t, env): id;
    }

    Ptr<Type> v(Ptr<IntType>, Ptr<Env<Type>>) override
    {
      return ptr<IntType>();
    }

    Ptr<Type> v(Ptr<BoolType>, Ptr<Env<Type>>) override
    {
      return ptr<BoolType>();
    }

    Ptr<Type> v(Ptr<StringType>, Ptr<Env<Type>>) override
    {
      return ptr<StringType>();
    }

    Ptr<Type> v(Ptr<ArrowType> ty, Ptr<Env<Type>> env) override
    {
      auto l = v(ty->left(), env),
           r = v(ty->right(), env);
      return ptr<ArrowType>(l, r);
    }

    Ptr<Type> v(Ptr<TupleType> ty, Ptr<Env<Type>> env) override
    {
      auto tys = ptr<std::vector<Ptr<Type>>>();
      tys->reserve(ty->tys()->size());
      for (auto t: *ty->tys()) {
        tys->push_back(v(t, env));
      }
      return ptr<TupleType>(tys);
    }
  };


}

Ptr<Type> nf(Ptr<Type> t, Ptr<Env<Type>> env)
{ return TypeNF()(t, env); }

}
