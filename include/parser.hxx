#ifndef PARSER_HXX_RGFUMH5N
#define PARSER_HXX_RGFUMH5N

#include "ast.hxx"
#include "token.hxx"
#include "lemon.hxx"
#include <vector>

namespace miniml
{

struct Parser final
{
  Parser();
  ~Parser();

  Ptr<Decl> parse(const Ptr<String>);
  Ptr<Decl> parse(const std::vector<Ptr<Token>>&);

private:
  void *parser;
};

}

#endif /* end of include guard: PARSER_HXX_RGFUMH5N */
