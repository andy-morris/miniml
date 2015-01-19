#ifndef TYPE_HXX_PHQ9E7V1
#define TYPE_HXX_PHQ9E7V1

#include "pos.hxx"
#include "ptr.hxx"
#include "dup.hxx"
#include "id.hxx"
#include "ppr.hxx"
#include "env.hxx"
#include "visitor.hxx"

namespace miniml
{

enum class TypeType
{
  ID,
  INT,
  ARROW,
};

class Type: public Pretty, public HasPos, public Dup<Type>
{
public:
  virtual ~Type() {}
  virtual TypeType type() const = 0;
  virtual bool operator==(const Type &other) const = 0;
  inline bool operator!=(const Type &other) const { return !(*this == other); }
protected:
  inline Type(Pos start = Pos(), Pos end = Pos()): HasPos(start, end) {}
};


class IdType final: public Type
{
public:
  IdType(const IdType &other) = default;
  IdType(IdType &&other) = default;

  IdType(Id id, Pos start = Pos(), Pos end = Pos()):
    Type(start, end), m_id(id)
  {}

  inline TypeType type() const override { return TypeType::ID; }

  bool operator==(const Type &other) const override;

  Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  inline Ptr<Type> dup() const override
  { return ptr<IdType>(id(), start(), end()); }

  inline Id id() const { return m_id; }

private:
  Id m_id;
};


class IntType final: public Type
{
public:
  IntType(const IntType &other) = default;
  IntType(IntType &&other) = default;

  IntType(Pos start = Pos(), Pos end = Pos()): Type(start, end) {}

  inline TypeType type() const override { return TypeType::INT; }

  inline bool operator==(const Type &other) const override
  { return other.type() == type(); }

  Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  inline Ptr<Type> dup() const override
  { return ptr<IntType>(); }
};


class ArrowType final: public Type
{
public:
  ArrowType(const ArrowType &other) = default;
  ArrowType(ArrowType &&other) = default;

  ArrowType(Ptr<Type> left, Ptr<Type> right,
            Pos start = Pos(), Pos end = Pos()):
    Type(start, end), m_left(left), m_right(right)
  {}

  inline TypeType type() const override { return TypeType::ARROW; }

  bool operator==(const Type &other) const override;

  Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  inline Ptr<Type> dup() const override
  { return ptr<ArrowType>(left(), right(), start(), end()); }

  inline Ptr<Type> left() const { return m_left; }
  inline Ptr<Type> right() const { return m_right; }

private:
  Ptr<Type> m_left, m_right;
};


template <typename T, typename... Args>
struct TypeVisitor
{
  inline Ptr<T> operator()(Ptr<Type> t, Args... args)
  { return v(t, args...); }

  virtual Ptr<T> v(Ptr<Type> t, Args... args)
  {
    switch (t->type()) {
#define CASE(x,T) \
      case TypeType::x: \
        return v(std::dynamic_pointer_cast<T>(t), args...);
      CASE(ID, IdType)
      CASE(INT, IntType)
      CASE(ARROW, ArrowType)
#undef CASE
    }
  }
  virtual Ptr<T> v(Ptr<IdType>, Args...) = 0;
  virtual Ptr<T> v(Ptr<IntType>, Args...) = 0;
  virtual Ptr<T> v(Ptr<ArrowType>, Args...) = 0;
};

Ptr<Type> nf(Ptr<Type> t, Ptr<Env<Type>> env);


}

#endif /* end of include guard: TYPE_HXX_PHQ9E7V1 */
