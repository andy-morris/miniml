#include "ast/type.hxx"

namespace miniml
{

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
    return left() == other0.left() && right() == other0.right();
  }
  return false;
}

}
