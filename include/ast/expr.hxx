#ifndef EXPR_HXX_SPN8H6I7
#define EXPR_HXX_SPN8H6I7

#include "ppr.hxx"
#include "id.hxx"
#include "ptr.hxx"

namespace miniml
{

/// Type of expression.
enum class ExprType
{
  ID,   ///< Identifier
  APP,  ///< Application
  LAM,  ///< Lambda term
  INT,  ///< Integer literal
};

/// Abstract base class for expressions.
class Expr
{
public:
  virtual ~Expr() {}

  /// Get the (syntactic) type of the expression.
  /// \see ExprType
  virtual ExprType type() const = 0;

  /// Pretty print an expression.
  /// \param[in] prec The outermost precedence for inserting brackets.
  /// \related Ppr
  virtual Ptr<Ppr> ppr(unsigned prec = 0) const = 0;
};


/// Identifier expression.
class IdExpr final: public Expr
{
public:
  IdExpr(const IdExpr&) = default;
  IdExpr(IdExpr&&) = default;

  /// \param[in] id The identifier this expression represents.
  IdExpr(const Ptr<Id> id): m_id(id) {}

  /// \return ExprType::ID
  ExprType type() const { return ExprType::ID; }

  /// \param prec Ignored, since variables are always atomic.
  Ptr<Ppr> ppr(unsigned prec = 0) const;

  /// \return The identifier value.
  const Ptr<const Id> id() const { return m_id; }

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
  IntExpr(long val): m_val(val) {}

  /// \return ExprType::INT
  ExprType type() const { return ExprType::INT; }

  /// \param prec Ignored, since literals are always atomic.
  Ptr<Ppr> ppr(unsigned prec = 0) const;

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

  AppExpr(const Ptr<Expr> left, const Ptr<Expr> right):
    m_left(left), m_right(right)
  {}

  /// \return ExprType::APP
  ExprType type() const { return ExprType::APP; }

  Ptr<Ppr> ppr(unsigned prec = 0) const;

  /// \return The left part (operator).
  const Ptr<const Expr> left() const { return m_left; }
  /// \return The right part (operand).
  const Ptr<const Expr> right() const { return m_right; }

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

  LamExpr(const Ptr<Id> var, const Ptr<Expr> body):
    m_var(var), m_body(body)
  {}

  /// \return ExprType::LAM
  ExprType type() const { return ExprType::LAM; }

  Ptr<Ppr> ppr(unsigned prec = 0) const;

  /// \return The bound variable.
  const Ptr<const Id> var() const { return m_var; }
  /// \return The body of the term.
  const Ptr<const Expr> body() const { return m_body; }

private:
  Ptr<Id> m_var;      ///< Bound variable.
  Ptr<Expr> m_body;   ///< Body.
};

}

#endif /* end of include guard: EXPR_HXX_SPN8H6I7 */
