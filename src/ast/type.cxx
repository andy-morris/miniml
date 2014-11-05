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

Ptr<Ppr> IdType::ppr(unsigned) const
{
  return id()->ppr();
}


bool ArrowType::operator==(const Type &other) const
{
  if (type() == other.type()) {
    auto other0 = dynamic_cast<const ArrowType&>(other);
    return left() == other0.left() && right() == other0.right();
  }
  return false;
}

Ptr<Ppr> ArrowType::ppr(unsigned prec) const
{
  return parens_if(prec > 0,
      left()->ppr(1) * +"->"_p * +right()->ppr(0));
}

}
