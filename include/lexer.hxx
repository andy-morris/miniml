#ifndef LEXER_HXX_FHKS2ENC
#define LEXER_HXX_FHKS2ENC

#include "token.hxx"
#include "string.hxx"
#include "exception.hxx"
#include <vector>
#include <deque>

namespace miniml
{

/// Lexer class that wraps a
/// <a href="http://www.colm.net/open-source/ragel/">Ragel</a>
/// generated state machine.
class Lexer
{
public:
  /// Creates a Lexer over the given string.
  Lexer(const String&);

  /// \return The tokens read.
  std::vector<Ptr<Token>> tokens() const;

private:
  // values used by Ragel {
  int cs, act;
  const Char *p, *begin, *pe, *eof, *ts, *te;
  // }

  Pos start, end;

  /// The tokens.
  std::deque<Ptr<Token>> m_tokens;

  /// Add a token to the vector.
  void push(Ptr<Token> &&tok);
};

}

#endif /* end of include guard: LEXER_HXX_FHKS2ENC */
