#ifndef ID_HXX_HQ5DONMJ
#define ID_HXX_HQ5DONMJ

#include "string.hxx"
#include "ptr.hxx"
#include "ppr.hxx"

#include <string>
#include <ostream>
#include <functional>

namespace miniml
{

/// Identifiers keep a hash for quick equality testing.
class Id final
{
public:
  Id(const Id&) = default;
  Id(Id&&) = default;

  Id &operator=(const Id&) = default;
  Id &operator=(Id&&) = default;

  /// Creates an identifier. \a str isn't copied.
  Id(const Ptr<String> str):
    m_val(str),
    m_hash(make_hash(*str))
  { }

  /// Copies a string onto the heap and constructs an identifier from it.
  Id(const String &str):
    Id(ptr<String>(str))
  { }

  /// If the hashes are the same then the values are also checked in case of a
  /// collision.
  /// \return Whether the two identifiers are the same.
  bool operator==(const Id &other) const
  { return m_hash == other.m_hash && m_val == other.m_val; }

  /// \return The value of the identifier.
  const Ptr<String> val() const { return m_val; }

  /// \return The identifier in \ref String form.
  operator const Ptr<String>() const { return val(); }

  /// Pretty prints an identifier.
  /// \relates PprString
  const Ptr<Ppr> ppr() const { return ppr::string(*val()); }

private:
  /// Hash function for \ref String.
  static std::hash<String> make_hash;

  const Ptr<String> m_val;  ///< Value.
  unsigned long m_hash;     ///< Hash.
};


/// Outputs an identifier to the given output stream.
inline OStream& operator<<(OStream &out, const Id &id)
{
  return out << *id.val();
}

}

#endif /* end of include guard: ID_HXX_HQ5DONMJ */
