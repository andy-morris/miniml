#ifndef ID_HXX_HQ5DONMJ
#define ID_HXX_HQ5DONMJ

#include "pos.hxx"
#include "string.hxx"
#include "ptr.hxx"
#include "ppr.hxx"

#include <string>
#include <ostream>

namespace miniml
{

/// Identifiers keep a hash for quick equality testing.
class Id final: public Pretty, public HasPos
{
public:
  Id(const Id&) = default;
  Id(Id&&) = default;

  Id &operator=(const Id&) = default;
  Id &operator=(Id&&) = default;

  /// Creates an identifier. \a str isn't copied.
  Id(const Ptr<String> str, Pos start = Pos(), Pos end = Pos()):
    HasPos(start, end), m_val(str), m_hash(make_hash(*str))
  { }

  /// Copies a string onto the heap and constructs an identifier from it.
  Id(const String &str, Pos start = Pos(), Pos end = Pos()):
    Id(ptr<String>(str), start, end)
  { }

  Id(const Id &other, Pos start, Pos end = Pos()):
    Id(other.val(), start, end)
  { }

  /// If the hashes are the same then the values are also checked in case of a
  /// collision.
  /// \return Whether the two identifiers are the same.
  inline bool operator==(const Id &other) const
  { return m_hash == other.m_hash && *m_val == *other.m_val; }

  /// \return The value of the identifier.
  inline Ptr<String> val() const { return m_val; }

  inline size_t hash() const { return m_hash; }

  /// Pretty prints an identifier.
  /// \relates PprString
  Ptr<Ppr> ppr(unsigned=0, bool=false) const override;

  template <typename T>
  Id suffix(T suf);

private:
  /// Hash function for \ref String.
  const static std::hash<String> make_hash;

  Ptr<String> m_val;  ///< Value.
  size_t m_hash;     ///< Hash.
};

template <typename T>
inline Id Id::suffix(T suf)
{
  auto str = ptr<String>(*m_val + std::to_string(suf));
  return Id(str, start(), end());
}


/// Outputs an identifier to the given output stream.
inline OStream& operator<<(OStream &out, const Id &id)
{ return out << *id.val(); }

inline Id operator"" _i(const char *s, size_t) { return Id(s); }

}


namespace std
{

template <>
struct hash<miniml::Id>
{
  typedef miniml::Id argument_type;
  typedef size_t result_type;
  size_t operator()(const miniml::Id &i) const { return i.hash(); }
};

}

#endif /* end of include guard: ID_HXX_HQ5DONMJ */
