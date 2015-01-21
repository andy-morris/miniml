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

enum class InputType { DECL, EXPR };
struct Input
{
  virtual ~Input() {}
  virtual InputType type() const = 0;
};
struct DeclInput final: public Input
{
  DeclInput(Ptr<Decl> d): decl(d) {}
  inline InputType type() const override { return InputType::DECL; }
  Ptr<Decl> decl;
};
struct ExprInput final: public Input
{
  ExprInput(Ptr<Expr> e): expr(e) {}
  inline InputType type() const override { return InputType::EXPR; }
  Ptr<Expr> expr;
};

struct Parser final
{
  Parser();
  ~Parser();

  struct ParseFail final: public Exception
  {
    ParseFail(const Token *t);
    const char *what() const noexcept override;
    String msg;
  };

  Ptr<Input> parse(const String&)
    throw(ParseFail, Lexer::LexicalError);
  Ptr<Input> parse(const std::vector<Ptr<Token>>&)
    throw(ParseFail);

private:
  void *parser;
};

}

#endif /* end of include guard: PARSER_HXX_RGFUMH5N */
