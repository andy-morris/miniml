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

enum class InputType { DECL, EXPR, MODULE };
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

  Ptr<Input> parse(const String&);
  Ptr<Input> parse(const std::vector<Ptr<Token>>&);

private:
  void *parser;
};

}

#endif /* end of include guard: PARSER_HXX_RGFUMH5N */
