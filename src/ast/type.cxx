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

Ptr<Ppr> IdType::ppr(unsigned, bool pos) const
{
  return pos_if(pos, id().ppr(pos), start(), end());
}

Ptr<Ppr> IntType::ppr(unsigned, bool pos) const
{
  return pos_if(pos, "int"_p, start(), end());
}

bool ArrowType::operator==(const Type &other) const
{
  if (type() == other.type()) {
    auto other0 = dynamic_cast<const ArrowType&>(other);
    return left() == other0.left() && right() == other0.right();
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


Ptr<Type> TypeNF::v(Ptr<IdType> id, Ptr<Env<Type>> env)
{
  auto t = env->lookup(id->id());
  return t? t: id;
}

Ptr<Type> TypeNF::v(Ptr<IntType>, Ptr<Env<Type>>)
{
  return ptr<IntType>();
}

Ptr<Type> TypeNF::v(Ptr<ArrowType> ty, Ptr<Env<Type>> env)
{
  auto l = v(ty->left(), env),
       r = v(ty->right(), env);
  return ptr<ArrowType>(l, r);
}

}
