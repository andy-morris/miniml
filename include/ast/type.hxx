#ifndef TYPE_HXX_PHQ9E7V1
#define TYPE_HXX_PHQ9E7V1

#include "ptr.hxx"
#include "id.hxx"
#include "ppr.hxx"

namespace miniml
{

enum class TypeType
{
  ID,
  ARROW,
};

class Type: public Pretty
{
public:
  virtual ~Type() {}

  virtual TypeType type() const = 0;

  virtual bool operator==(const Type &other) const = 0;
};


class IdType final: public Type
{
public:
  IdType(const IdType &other) = default;
  IdType(IdType &&other) = default;

  IdType(Ptr<Id> id): m_id(id) {}

  virtual TypeType type() const override { return TypeType::ID; }

  virtual bool operator==(const Type &other) const override;

  virtual Ptr<Ppr> ppr(unsigned prec = 0) const override;

  const Ptr<Id> id() const { return m_id; }

private:
  Ptr<Id> m_id;
};

class ArrowType final: public Type
{
public:
  ArrowType(const ArrowType &other) = default;
  ArrowType(ArrowType &&other) = default;

  ArrowType(Ptr<Type> left, Ptr<Type> right):
    m_left(left), m_right(right)
  {}

  virtual TypeType type() const override { return TypeType::ARROW; }

  virtual bool operator==(const Type &other) const override;

  virtual Ptr<Ppr> ppr(unsigned prec = 0) const override;

  Ptr<Type> left() const { return m_left; }
  Ptr<Type> right() const { return m_right; }

private:
  Ptr<Type> m_left, m_right;
};


}

#endif /* end of include guard: TYPE_HXX_PHQ9E7V1 */
