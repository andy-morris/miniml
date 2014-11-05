#include "lexer.hxx"
#include "token.hxx"

namespace miniml
{

using TokType = Token::Type;

#define ATOMIC(t) Token::atomic<TokType::t>()

%%{
machine Lexer;
alphtype char;

idstart  = alpha | "_";
idletter = idstart | digit | "'";
id       = idstart idletter*;

int = ('+'|'-')? [0-9]+;

token := |*
  "fn"  => { push(ATOMIC(FN)); };
  "=>"  => { push(ATOMIC(ARROW)); };
  "->"  => { push(ATOMIC(TYARROW)); };
  "("   => { push(ATOMIC(LPAR)); };
  ")"   => { push(ATOMIC(RPAR)); };
  ":"   => { push(ATOMIC(COLON)); };
  id    => {
    push(ptr<IdToken>(ts, te - ts));
  };
  int   => {
    std::string str(ts, te - ts);
    std::stringstream s(str); long x;
    s >> x;
    push(ptr<IntToken>(x));
  };
  space+;
  "//" . [^\n] . "\n";
*|;
}%%


%% write data;

Lexer::Lexer(const Ptr<String> str) throw(Lexer::LexicalError):
  data(str)
{
  p = begin = str->data();
  eof = pe = str->data() + str->size();
  %% write init;
  %% write exec;

  if (cs == Lexer_error)
    throw LexicalError(*p, p - begin);
}

Lexer::LexicalError::LexicalError(char c_, ptrdiff_t pos_):
  c(c_), pos(pos_)
{
  std::stringstream s;
  s << "Lexical error at '" << c << "', position " << pos;
  text = s.str();
}

}
