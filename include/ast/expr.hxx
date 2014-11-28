#ifndef EXPR_HXX_SPN8H6I7
#define EXPR_HXX_SPN8H6I7

#include "pos.hxx"
#include "ppr.hxx"
#include "id.hxx"
#include "ptr.hxx"
#include "ast/type.hxx"
#include "visitor.hxx"

namespace miniml
{

/// Type of expression.
enum class ExprType
{
  ID,   ///< Identifier
  APP,  ///< Application
  LAM,  ///< Lambda term
  INT,  ///< Integer literal
  TYPE, ///< Typed expression
};

/// Abstract base class for expressions.
class Expr: public Pretty, public HasPos
{
public:
  virtual ~Expr() {}
  /// Get the (syntactic) type of the expression.
  /// \see ExprType
  virtual ExprType type() const = 0;
protected:
  Expr(Pos start = Pos(), Pos end = Pos()): HasPos(start, end) {}
};


/// Identifier expression.
class IdExpr final: public Expr
{
public:
  IdExpr(const IdExpr&) = default;
  IdExpr(IdExpr&&) = default;

  /// \param[in] id The identifier this expression represents.
  IdExpr(const Ptr<Id> id, Pos start = Pos(), Pos end = Pos()):
    Expr(start, end), m_id(id)
  {}

  /// \return ExprType::ID
  virtual ExprType type() const override { return ExprType::ID; }

  /// \param prec Ignored, since variables are always atomic.
  virtual Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  /// \return The identifier value.
  Ptr<Id> id() const { return m_id; }

private:
  Ptr<Id> m_id;   ///< Identifier value.
};


/// Integer literals.
class IntExpr final: public Expr
{
public:
  IntExpr(const IntExpr&) = default;
  IntExpr(IntExpr&&) = default;

  /// \param[in] val The value of this literal.
  IntExpr(long val, Pos start = Pos(), Pos end = Pos()):
    Expr(start, end), m_val(val)
  {}

  /// \return ExprType::INT
  virtual ExprType type() const override { return ExprType::INT; }

  /// \param prec Ignored, since literals are always atomic.
  virtual Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  /// \return The value of this literal.
  long val() const { return m_val; }

private:
  long m_val;
};


/// Application expression.
class AppExpr final: public Expr
{
public:
  AppExpr(const AppExpr&) = default;
  AppExpr(AppExpr&&) = default;

  AppExpr(const Ptr<Expr> left, const Ptr<Expr> right,
          Pos start = Pos(), Pos end = Pos()):
    Expr(start, end), m_left(left), m_right(right)
  {}

  /// \return ExprType::APP
  virtual ExprType type() const override { return ExprType::APP; }

  virtual Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  /// \return The left part (operator).
  Ptr<Expr> left() const { return m_left; }
  /// \return The right part (operand).
  Ptr<Expr> right() const { return m_right; }

private:
  Ptr<Expr> m_left;   ///< Operator.
  Ptr<Expr> m_right;  ///< Operand.
};


/// Lambda term (function) expression.
class LamExpr final: public Expr
{
public:
  LamExpr(const LamExpr&) = default;
  LamExpr(LamExpr&&) = default;

  LamExpr(const Ptr<Id> var, const Ptr<Type> ty, const Ptr<Expr> body,
          Pos start = Pos(), Pos end = Pos()):
    Expr(start, end), m_var(var), m_ty(ty), m_body(body)
  {}

  /// \return ExprType::LAM
  virtual ExprType type() const override { return ExprType::LAM; }

  virtual Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  /// \return The bound variable.
  Ptr<Id> var() const { return m_var; }
  /// \return The argument type.
  Ptr<Type> ty() const { return m_ty; }
  /// \return The body of the term.
  Ptr<Expr> body() const { return m_body; }

private:
  Ptr<Id> m_var;      ///< Bound variable.
  Ptr<Type> m_ty;     ///< Argument type.
  Ptr<Expr> m_body;   ///< Body.
};


/// Expressions with type annotations.
class TypeExpr final: public Expr
{
public:
  TypeExpr(const TypeExpr&) = default;
  TypeExpr(TypeExpr&&) = default;

  TypeExpr(const Ptr<Expr> expr, const Ptr<Type> ty,
          Pos start = Pos(), Pos end = Pos()):
    Expr(start, end), m_expr(expr), m_ty(ty)
  {}

  /// \return ExprType::TYPE
  ExprType type() const override { return ExprType::TYPE; }

  virtual Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  /// \return The inner expression.
  Ptr<Expr> expr() const { return m_expr; }
  /// \return The assigned type.
  Ptr<Type> ty() const { return m_ty; }

private:
  Ptr<Expr> m_expr;   ///< Expression.
  Ptr<Type> m_ty;   ///< Assigned type.
};

template <typename T, typename... Args>
struct ExprVisitor
{
  virtual Ptr<T> operator()(Ptr<Expr> e, Args... args)
  {
    switch (e->type()) {
#define CASE(x,t) \
      case ExprType::x: \
        return operator()(std::dynamic_pointer_cast<t>(e), args...);
      CASE(ID, IdExpr)
      CASE(APP, AppExpr)
      CASE(LAM, LamExpr)
      CASE(INT, IntExpr)
      CASE(TYPE, TypeExpr)
#undef CASE
    }
  }

  virtual Ptr<T> operator()(Ptr<IdExpr>, Args...) = 0;
  virtual Ptr<T> operator()(Ptr<AppExpr>, Args...) = 0;
  virtual Ptr<T> operator()(Ptr<IntExpr>, Args...) = 0;
  virtual Ptr<T> operator()(Ptr<LamExpr>, Args...) = 0;
  virtual Ptr<T> operator()(Ptr<TypeExpr>, Args...) = 0;
};

}

#endif /* end of include guard: EXPR_HXX_SPN8H6I7 */
