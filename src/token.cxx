#include "token.hxx"

namespace miniml
{

OStream &operator<<(OStream &out, const Pos &p)
{
  return out << p.line << ':' << p.col;
}


void IdToken::out(OStream &o) const
{
  o << "ID(" << *id << ")";
}

void IntToken::out(OStream &o) const
{
  o << "INT(" << val << ")";
}


#define ATOMIC_OUT(t, str) \
  template<> \
  void AtomicToken<Token::Type::t>::out(OStream &o) const \
  { o << str; }

ATOMIC_OUT(FN, "'fn'")
ATOMIC_OUT(ARROW, "'=>'")
ATOMIC_OUT(TYARROW, "'->'")
ATOMIC_OUT(LPAR, "'('")
ATOMIC_OUT(RPAR, "')'")
ATOMIC_OUT(COLON, "':'")
ATOMIC_OUT(PLUS, "'+'")
ATOMIC_OUT(MINUS, "'-'")
ATOMIC_OUT(TIMES, "'*'")
ATOMIC_OUT(DIVIDE, "'/'")
ATOMIC_OUT(VAL, "'val'")
ATOMIC_OUT(EQ, "'='")
ATOMIC_OUT(TRUE, "'true'")
ATOMIC_OUT(FALSE, "'false'")
ATOMIC_OUT(AND, "'&&'")
ATOMIC_OUT(OR, "'||'")
ATOMIC_OUT(IFF, "'<->'")
ATOMIC_OUT(LESS, "'<'")
ATOMIC_OUT(LEQ, "'<='")
ATOMIC_OUT(EQUAL, "'=='")
ATOMIC_OUT(GEQ, "'>='")
ATOMIC_OUT(GREATER, "'>'")
ATOMIC_OUT(NEQ, "'!='")
ATOMIC_OUT(SEQ, "';'")
ATOMIC_OUT(COMMA, "','")
ATOMIC_OUT(DOT, "'.'")
#undef ATOMIC_OUT

OStream &operator<<(OStream &out, const Token &tok)
{
  tok.out(out);
  return out;
}

}
