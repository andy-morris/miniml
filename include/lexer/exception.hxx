#ifndef EXCEPTION_HXX_AJINKVWR
#define EXCEPTION_HXX_AJINKVWR

#include "../exception.hxx"
#include "../pos.hxx"

namespace miniml
{
  struct LexerError: public Exception
  {
    virtual ~LexerError() {}

    /// \return the value of \ref text.
    virtual const char *what() const noexcept override
    { return text.c_str(); }

    /// The text of the exception. \sa what
    std::string text;
    /// The position that the error occurred.
    Pos pos;
  };

  /// Exception thrown when a lexical error is enountered.
  struct LexicalError final: public LexerError
  {
    LexicalError(char c_, Pos pos_);

    /// The character at the position the error occurred.
    char c;
  };

  /// Exception thrown when a numeric escape sequence in a string is too high.
  /// `pos` indicates the whole string literal, though, which is probably not
  /// ideal.
  struct InvalidEscape final: public LexerError
  {
    InvalidEscape(int, Pos pos_);

    /// The attempted escape.
    int index;
  };
}

#endif /* end of include guard: EXCEPTION_HXX_AJINKVWR */
