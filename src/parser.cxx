#include "parser.hxx"
#include "lexer.hxx"
#include <cstdlib>

using namespace miniml;


void *MiniMLParserAlloc(void* (*)(size_t));
void MiniMLParserFree(void*, void(*)(void*));
void MiniMLParser(void*, int, Token*, Decl**);

#ifndef NDEBUG
void MiniMLParserTrace(FILE*, char*);
#endif

namespace
{
  int token_id(const Token &tok);
}

namespace miniml
{

Parser::Parser()
{
  parser = MiniMLParserAlloc(&std::malloc);
}

Ptr<Decl> Parser::parse(const Ptr<String> input)
{
  return parse(Lexer(input).tokens());
}

Ptr<Decl> Parser::parse(const std::vector<Ptr<Token>>& toks)
{
  Decl *decl = nullptr;

#ifndef NDEBUG
  // lemon forgot a const :(
  MiniMLParserTrace(stderr, const_cast<char*>("parser: "));
#endif

  for (auto tok: toks) {
    MiniMLParser(parser, token_id(*tok), tok.get(), &decl);
  }
  MiniMLParser(parser, 0, nullptr, &decl);

  return Ptr<Decl>(decl);
}

Parser::~Parser()
{
  MiniMLParserFree(parser, &std::free);
}

}

namespace
{
  int token_id(const Token &tok)
  {
    using Ty = Token::Type;
    switch (tok.type()) {
#define CASE(t) case Ty::t: return TOK_ ## t
      CASE(ID);
      CASE(INT);
      CASE(FN);
      CASE(ARROW);
      CASE(TYARROW);
      CASE(LPAR);
      CASE(RPAR);
      CASE(COLON);
      CASE(PLUS);
      CASE(MINUS);
      CASE(TIMES);
      CASE(DIVIDE);
      CASE(VAL);
      CASE(EQ);
#undef CASE
    }
  }
}
