#include "ptr.hxx"
#include "expr.hxx"
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
