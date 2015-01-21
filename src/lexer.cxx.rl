#include "lexer.hxx"
#include "token.hxx"

namespace miniml
{

using TokType = Token::Type;

#define ATOMIC(t) Token::atomic<TokType::t>(start, end)

%%{
machine Lexer;
alphtype char;

action ws   { end = start += fc; }
action bump { end += fc; }

idstart  = (alpha | "_");
idletter = (idstart | digit | "'");

ID      = (idstart idletter*) $bump;
INT     = ('~'? [0-9]+) $bump;
FN      = "fn" $bump;
ARROW   = "=>" $bump;
TYARROW = "->" $bump;
LPAR    = "(" $bump;
RPAR    = ")" $bump;
COLON   = ":" $bump;
PLUS    = "+" $bump;
MINUS   = "-" $bump;
TIMES   = "*" $bump;
DIVIDE  = "/" $bump;
VAL     = "val" $bump;
EQ      = "=" $bump;
TRUE    = "true" $bump;
FALSE   = "false" $bump;
AND     = "&&" $bump;
OR      = "||" $bump;
IFF     = "<->" $bump;
LESS    = "<" $bump;
LEQ     = "<=" $bump;
EQUAL   = "==" $bump;
GEQ     = ">=" $bump;
GREATER = ">" $bump;
NEQ     = "!=" $bump;
SEQ     = ";" $bump;
COMMA   = "," $bump;
WS      = (space+ | ("//" . [^\n] . "\n")) $ws;

token := |*
  FN      => { push(ATOMIC(FN)); };
  ARROW   => { push(ATOMIC(ARROW)); };
  TYARROW => { push(ATOMIC(TYARROW)); };
  LPAR    => { push(ATOMIC(LPAR)); };
  RPAR    => { push(ATOMIC(RPAR)); };
  COLON   => { push(ATOMIC(COLON)); };
  PLUS    => { push(ATOMIC(PLUS)); };
  MINUS   => { push(ATOMIC(MINUS)); };
  TIMES   => { push(ATOMIC(TIMES)); };
  DIVIDE  => { push(ATOMIC(DIVIDE)); };
  VAL     => { push(ATOMIC(VAL)); };
  EQ      => { push(ATOMIC(EQ)); };
  TRUE    => { push(ATOMIC(TRUE)); };
  FALSE   => { push(ATOMIC(FALSE)); };
  AND     => { push(ATOMIC(AND)); };
  OR      => { push(ATOMIC(OR)); };
  IFF     => { push(ATOMIC(IFF)); };
  LESS    => { push(ATOMIC(LESS)); };
  LEQ     => { push(ATOMIC(LEQ)); };
  EQUAL   => { push(ATOMIC(EQUAL)); };
  GEQ     => { push(ATOMIC(GEQ)); };
  GREATER => { push(ATOMIC(GREATER)); };
  NEQ     => { push(ATOMIC(NEQ)); };
  SEQ     => { push(ATOMIC(SEQ)); };
  COMMA   => { push(ATOMIC(COMMA)); };
  ID      => { push(ptr<IdToken>(ts, te - ts, start, end)); };
  INT     => {
    std::string str(ts, te - ts);
    if (str[0] == '~') { str[0] = '-'; }
    long x = std::stol(str);
    push(ptr<IntToken>(x, start, end));
  };
  WS;
*|;
}%%


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-const-variable"
%% write data;
#pragma clang diagnostic pop


Lexer::Lexer(const String &str) throw(Lexer::LexicalError)
{
  p = begin = str.data();
  eof = pe = str.data() + str.size();
  %% write init;
  %% write exec;

  if (cs == Lexer_error)
    throw LexicalError(*p, end);
}

Lexer::LexicalError::LexicalError(char c_, Pos pos_):
  c(c_), pos(pos_)
{
  std::stringstream s;
  s << pos << ": lexical error at char '" << c << "'";
  text = s.str();
}


void Lexer::push(Ptr<Token> &&tok)
{
  m_tokens.push_back(tok);
  start = end;
}

}
