#ifndef TYPE_HXX_PHQ9E7V1
#define TYPE_HXX_PHQ9E7V1

#include "pos.hxx"
#include "ptr.hxx"
#include "dup.hxx"
#include "id.hxx"
#include "ppr.hxx"
#include "env.hxx"
#include "visitor.hxx"
#include <vector>

namespace miniml
{

/// Syntactic kinds of types.
enum class TypeType
{
  ID,
  INT,
  BOOL,
  STRING,
  ARROW,
  TUPLE,
};

/// Base class for types.
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


/// Type names (other than `int`/`string`/`bool`).
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

  inline Ptr<Ppr> ppr(unsigned=0, bool pos = false) const override
  { return ppr::pos_if(pos, id().ppr(pos), start(), end()); }

  inline Ptr<Type> dup() const override
  { return ptr<IdType>(id(), start(), end()); }

  inline Id id() const { return m_id; }

private:
  Id m_id;
};


/// Integer type.
class IntType final: public Type
{
public:
  IntType(const IntType &other) = default;
  IntType(IntType &&other) = default;

  IntType(Pos start = Pos(), Pos end = Pos()): Type(start, end) {}

  inline TypeType type() const override { return TypeType::INT; }

  inline bool operator==(const Type &other) const override
  { return other.type() == type(); }

  inline Ptr<Ppr> ppr(unsigned=0, bool pos = false) const override
  { return ppr::pos_if(pos, "int"_p, start(), end()); }

  inline Ptr<Type> dup() const override
  { return ptr<IntType>(); }
};


/// Boolean type.
class BoolType final: public Type
{
public:
  BoolType(const BoolType &other) = default;
  BoolType(BoolType &&other) = default;

  BoolType(Pos start = Pos(), Pos end = Pos()): Type(start, end) {}

  inline TypeType type() const override { return TypeType::BOOL; }

  inline bool operator==(const Type &other) const override
  { return other.type() == type(); }

  inline Ptr<Ppr> ppr(unsigned=0, bool pos = false) const override
  { return ppr::pos_if(pos, "bool"_p, start(), end()); }

  inline Ptr<Type> dup() const override
  { return ptr<BoolType>(); }
};


/// String type.
class StringType final: public Type
{
public:
  StringType(const StringType &other) = default;
  StringType(StringType &&other) = default;

  StringType(Pos start = Pos(), Pos end = Pos()): Type(start, end) {}

  inline TypeType type() const override { return TypeType::STRING; }

  inline bool operator==(const Type &other) const override
  { return other.type() == type(); }

  inline Ptr<Ppr> ppr(unsigned=0, bool pos = false) const override
  { return ppr::pos_if(pos, "string"_p, start(), end()); }

  inline Ptr<Type> dup() const override
  { return ptr<StringType>(); }
};


/// Arrow type `a -> b`.
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
  { return ptr<ArrowType>(left()->dup(), right()->dup(), start(), end()); }

  /// Domain.
  inline Ptr<Type> left() const { return m_left; }
  /// Range.
  inline Ptr<Type> right() const { return m_right; }

private:
  Ptr<Type> m_left, m_right;
};


/// Tuple type `(a, b, c)`.
class TupleType final: public Type
{
public:
  using Types = std::vector<Ptr<Type>>;

  TupleType(const TupleType&) = default;
  TupleType(TupleType&&) = default;

  TupleType(const std::initializer_list<Ptr<Type>> tys,
            Pos start = Pos(), Pos end = Pos()):
    TupleType(ptr<std::vector<Ptr<Type>>>(tys), start, end)
  {}

  TupleType(Ptr<std::vector<Ptr<Type>>> tys,
            Pos start = Pos(), Pos end = Pos()):
    Type(start, end), m_tys(tys)
  {}

  inline TypeType type() const override { return TypeType::TUPLE; }

  bool operator==(const Type &other) const override;

  Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  inline Ptr<Type> dup() const override
  { return ptr<TupleType>(ptr<Types>(*tys())); }

  /// Get the inner types.
  inline Ptr<Types> tys() const { return m_tys; }

private:
  Ptr<Types> m_tys;
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
        return v(std::dynamic_pointer_cast<T>(t), args...)
      CASE(ID, IdType);
      CASE(INT, IntType);
      CASE(STRING, StringType);
      CASE(BOOL, BoolType);
      CASE(ARROW, ArrowType);
      CASE(TUPLE, TupleType);
#undef CASE
#ifdef __GNUC__
      default: std::abort();
#endif
    }
  }
  virtual Ptr<T> v(Ptr<IdType>, Args...) = 0;
  virtual Ptr<T> v(Ptr<IntType>, Args...) = 0;
  virtual Ptr<T> v(Ptr<BoolType>, Args...) = 0;
  virtual Ptr<T> v(Ptr<StringType>, Args...) = 0;
  virtual Ptr<T> v(Ptr<ArrowType>, Args...) = 0;
  virtual Ptr<T> v(Ptr<TupleType>, Args...) = 0;
};

/// Reduce a type to normal form, substituting names as given.
Ptr<Type> nf(Ptr<Type> t, Ptr<Env<Type>> env);


}

#endif /* end of include guard: TYPE_HXX_PHQ9E7V1 */
