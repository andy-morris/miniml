#ifndef PARSER_HXX_RGFUMH5N
#define PARSER_HXX_RGFUMH5N

#include "ast.hxx"
#include "token.hxx"
#include "lemon.hxx"

namespace miniml
{

struct Parser
{
  Parser();
  ~Parser();

  Ptr<Expr> parse(const Ptr<String>);

private:
  void *parser;
};

}

#endif /* end of include guard: PARSER_HXX_RGFUMH5N */
