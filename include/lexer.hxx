#ifndef LEXER_HXX_FHKS2ENC
#define LEXER_HXX_FHKS2ENC

#include "token.hxx"
#include "string.hxx"
#include "exception.hxx"
#include <vector>

namespace miniml
{

/// Lexer class that wraps a
/// <a href="http://www.colm.net/open-source/ragel/">Ragel</a>
/// generated state machine.
class Lexer
{
public:
  /// Exception thrown when a lexical error is enountered.
  struct LexicalError: public Exception
  {
    virtual ~LexicalError() {}

    LexicalError(char c_, Pos pos_);

    /// \return the value of \ref text.
    virtual const char *what() const noexcept override
    { return text.c_str(); }

    /// The text of the exception. \sa what
    std::string text;
    /// The character at the position the error occurred.
    char c;
    /// The position that the error occurred.
    Pos pos;
  };


  /// Creates a Lexer over the given string.
  Lexer(const String&) throw(LexicalError);

  /// \return The tokens read.
  std::vector<Ptr<Token>> &tokens()
  { return m_tokens; }

private:
  // values used by Ragel {
  int cs, act;
  const Char *p, *begin, *pe, *eof, *ts, *te;
  // }

  Pos start, end;

  /// The tokens.
  std::vector<Ptr<Token>> m_tokens;

  /// Add a token to the vector.
  void push(Ptr<Token> &&tok);
};

}

#endif /* end of include guard: LEXER_HXX_FHKS2ENC */
