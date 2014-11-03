#ifndef LEXER_HXX_FHKS2ENC
#define LEXER_HXX_FHKS2ENC

#include "id.hxx"
#include "string.hxx"
#include <vector>
#include <stdexcept>

namespace miniml
{

struct Token;

/// Lexer class that wraps a
/// <a href="http://www.colm.net/open-source/ragel/">Ragel</a>
/// generated state machine.
class Lexer
{
public:
  /// Exception thrown when a lexical error is enountered.
  struct LexicalError: public std::exception
  {
    LexicalError(char c_, ptrdiff_t pos_);

    /// \return the value of \ref text.
    virtual const char *what() const noexcept override
    { return text.c_str(); }

    /// The text of the exception. \sa what
    std::string text;
    /// The character at the position the error occurred.
    char c;
    /// The position that the error occurred.
    ptrdiff_t pos;
  };


  /// Creates a Lexer over the given string.
  Lexer(const Ptr<String>) throw(LexicalError);

  /// \return The tokens read.
  std::vector<Ptr<Token>> &tokens()
  { return m_tokens; }

private:
  // values used by Ragel {
  int cs, act;
  const Char *p, *begin, *pe, *eof, *ts, *te;
  // }

  /// The string being read.
  Ptr<String> data;

  /// The tokens.
  std::vector<Ptr<Token>> m_tokens;

  /// Add a token to the vector.
  void push(Ptr<Token> &&tok) { m_tokens.push_back(tok); }
};


/// Abstract class for tokens.
struct Token
{
  enum class Type;                ///< Token types.
  virtual Type type() const = 0;  ///< What type of token is this?

  virtual ~Token() {}

  /// Make an atomic token.
  template <Type ty> static Ptr<Token> atomic();

  /// Whether a token type is atomic (needs no data).
  static constexpr bool is_atomic(Type);

  virtual void out(OStream &) const = 0;
};


enum class Token::Type
{
  ID,     ///< identifier
  INT,    ///< integer literal
  FN,     ///< `fn`
  ARROW,  ///< `=>`
};


OStream &operator<<(OStream &out, const Token &tok);



constexpr bool Token::is_atomic(Token::Type ty)
{
  using Type = Token::Type;
  return !(ty == Type::ID || ty == Type::INT);
}


struct IdToken final: public Token
{
  IdToken(Ptr<Id> id_): id(id_) {}
  IdToken(const Char *start, ptrdiff_t size):
    IdToken(ptr<Id>(ptr<String>(start, size)))
  {}

  Ptr<Id> id;

  virtual Token::Type type() const override { return Token::Type::ID; }

  virtual void out(OStream &) const;
};


struct IntToken final: public Token
{
  IntToken(long val_): val(val_) {}
  long val;

  virtual Token::Type type() const override { return Token::Type::INT; }

  virtual void out(OStream &) const;
};


template <Token::Type Ty>
class AtomicToken final: public Token
{
public:
  virtual Token::Type type() const { return Ty; }

static_assert(Token::is_atomic(Ty),
              "tried to make an atomic token of a nonatomic type");

  virtual void out(OStream &) const;
};


template <Token::Type Ty>
Ptr<Token> Token::atomic()
{
  return ptr<AtomicToken<Ty>>();
}


}
#endif /* end of include guard: LEXER_HXX_FHKS2ENC */
