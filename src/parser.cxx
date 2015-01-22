#include "parser.hxx"
#include "lexer.hxx"
#include <cstdlib>

using namespace miniml;


void *MiniMLParserAlloc(void* (*)(size_t));
void MiniMLParserFree(void*, void(*)(void*));
void MiniMLParser(void*, int, Token*, Input**);

#ifndef NDEBUG
void MiniMLParserTrace(FILE*, char*);
#endif

namespace
{
  int token_id(const Token &tok);
}

namespace miniml
{

Parser::ParseFail::ParseFail(const Token *t)
{
  if (t) {
    SStream s;
    s << "parse error at token " << *t << *t->start().ppr();
    msg = s.str();
  } else {
    msg = "parse error at end of declaration";
  }
}

const char *Parser::ParseFail::what() const noexcept
{
  return msg.data();
}



Parser::Parser():
  parser(MiniMLParserAlloc(&std::malloc))
{ }

Ptr<Input> Parser::parse(const String &input)
{
  return parse(Lexer(input).tokens());
}

Ptr<Input> Parser::parse(const std::vector<Ptr<Token>>& toks)
{
  Input *input = nullptr;

#ifndef NDEBUG
  // lemon forgot a const :(
  MiniMLParserTrace(stderr, const_cast<char*>("parser: "));
#endif

  Token *tok;
  for (auto it = begin(toks); it != end(toks); ++it) {
    tok = it->get();
    MiniMLParser(parser, token_id(*tok), tok, &input);
  }
  MiniMLParser(parser, 0, nullptr, &input);

  return Ptr<Input>(input);
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
      CASE(STRING);
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
      CASE(TRUE);
      CASE(FALSE);
      CASE(AND);
      CASE(OR);
      CASE(IFF);
      CASE(LESS);
      CASE(LEQ);
      CASE(EQUAL);
      CASE(GEQ);
      CASE(GREATER);
      CASE(NEQ);
      CASE(SEQ);
      CASE(COMMA);
      CASE(DOT);
#undef CASE
    }
  }
}
