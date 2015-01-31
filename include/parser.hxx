#ifndef PARSER_HXX_RGFUMH5N
#define PARSER_HXX_RGFUMH5N

#include "ast.hxx"
#include "token.hxx"
#include "lemon.hxx"
#include "lexer.hxx"
#include "exception.hxx"
#include <vector>

namespace miniml
{

/// Different possibilities for the parser to produce.
enum class InputType { DECL, EXPR, MODULE };
struct Input
{
  virtual ~Input() {}
  virtual InputType type() const = 0;
};
/// The input was a declaration.
struct DeclInput final: public Input
{
  DeclInput(Ptr<Decl> d): decl(d) {}
  inline InputType type() const override { return InputType::DECL; }
  Ptr<Decl> decl;
};
/// The input was an expression.
struct ExprInput final: public Input
{
  ExprInput(Ptr<Expr> e): expr(e) {}
  inline InputType type() const override { return InputType::EXPR; }
  Ptr<Expr> expr;
};
/// The input was a module.
struct ModuleInput final: public Input
{
  using Decls = std::vector<Ptr<Decl>>;
  ModuleInput(Id n, Ptr<Decls> ds):
    name(n), decls(ds)
  {}
  inline InputType type() const override { return InputType::MODULE; }
  Id name;
  Ptr<Decls> decls;
};


/// Wrapper for a <a href="http://www.hwaci.com/sw/lemon/">lemon</a> parser.
struct Parser final
{
  Parser();
  ~Parser();

  /// Exception when parsing fails.
  struct ParseFail final: public Exception
  {
    ParseFail(const Token *t);
    const char *what() const noexcept override;
    String msg;
  };

  /// Lex & parse a string. \sa Lexer
  Ptr<Input> parse(const String&);
  /// Parse a token stream that was already produced.
  Ptr<Input> parse(const std::vector<Ptr<Token>>&);

private:
  void *parser; ///< The object lemon produces.
};

}

#endif /* end of include guard: PARSER_HXX_RGFUMH5N */
