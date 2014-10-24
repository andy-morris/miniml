#ifndef ID_HXX_HQ5DONMJ
#define ID_HXX_HQ5DONMJ

#include "ptr.hxx"

#include <string>
#include <ostream>
#include <functional>

namespace miniml
{

/**
 * Identifiers. They keep a hash for quick equality testing.
 */
class Id final
{
public:
  using String = std::wstring;

  Id(const Ptr<String> str):
    m_val(str),
    m_hash(make_hash(*str))
  { }

  bool operator==(const Id &other) const
  { return m_hash == other.m_hash && m_val == other.m_val; }

  const Ptr<String> val() const { return m_val; }

  operator const Ptr<String>() const { return val(); }

private:
  static std::hash<String> make_hash;

  const Ptr<String> m_val;
  unsigned long m_hash;
};


inline std::wostream& operator<<(std::wostream &out, const Id &id)
{
  return out << *id.val();
}


}

#endif /* end of include guard: ID_HXX_HQ5DONMJ */
