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
};

/// Abstract base class for expressions.
class Expr
{
public:
  virtual ~Expr() {}

  /// Get the type of the expression.
  /// \see ExprType
  virtual ExprType type() = 0;

  /// Pretty print an expression.
  /// \param[in] prec The outermost precedence for inserting brackets.
  /// \related Ppr
  virtual Ptr<Ppr> ppr(unsigned prec = 0) = 0;
};


/// Identifier expression.
class IdExpr final: public Expr
{
public:
  /// \param[in] i The identifier this expression represents.
  IdExpr(const Ptr<Id> i): m_id(i) {}

  /// \return ExprType::ID
  ExprType type() { return ExprType::ID; }

  /// \param prec Ignored, since variables are always atomic.
  Ptr<Ppr> ppr(unsigned prec = 0);

  /// \return The identifier this expression stands for.
  const Ptr<Id> id() const { return m_id; }

private:
  const Ptr<Id> m_id;   ///< Identifier value.
};


/// Application expression.
class AppExpr final: public Expr
{
public:
  AppExpr(const Ptr<Expr> left, const Ptr<Expr> right):
    m_left(left), m_right(right)
  {}

  /// \return ExprType::APP
  ExprType type() { return ExprType::APP; }

  Ptr<Ppr> ppr(unsigned prec = 0);

  /// \return The left part (operator).
  const Ptr<Expr> left() const { return m_left; }
  /// \return The right part (operand).
  const Ptr<Expr> right() const { return m_right; }

private:
  const Ptr<Expr> m_left;   ///< Operator.
  const Ptr<Expr> m_right;  ///< Operand.
};


/// Lambda term (function) expression.
class LamExpr final: public Expr
{
public:
  LamExpr(const Ptr<Id> var, const Ptr<Expr> body):
    m_var(var), m_body(body)
  {}

  /// \return ExprType::LAM
  ExprType type() { return ExprType::LAM; }

  Ptr<Ppr> ppr(unsigned prec = 0);

  /// \return The bound variable.
  const Ptr<Id> var() const { return m_var; }
  /// \return The body of the term.
  const Ptr<Expr> body() const { return m_body; }

private:
  const Ptr<Id> m_var;      ///< Bound variable.
  const Ptr<Expr> m_body;   ///< Body.
};

}

#endif /* end of include guard: EXPR_HXX_SPN8H6I7 */
