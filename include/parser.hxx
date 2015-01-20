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

struct Parser final
{
  Parser();
  ~Parser();

  struct ParseFail final: public Exception
  {
    ParseFail(Ptr<Token> t);
    const char *what() const noexcept override;
    Ptr<Token> tok;
    String msg;
  };

  Ptr<Decl> parse(const String&)
    throw(ParseFail, Lexer::LexicalError);
  Ptr<Decl> parse(const std::vector<Ptr<Token>>&)
    throw(ParseFail);

  struct ParseFailInternal final: public Exception
  {
    const char *what() const noexcept override;
  };

private:
  void *parser;
};

}

#endif /* end of include guard: PARSER_HXX_RGFUMH5N */
