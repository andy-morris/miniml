#ifndef ID_HXX_HQ5DONMJ
#define ID_HXX_HQ5DONMJ

#include "ptr.hxx"
#include "ppr.hxx"

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
  Id(const Ptr<String> str):
    m_val(str),
    m_hash(make_hash(*str))
  { }

  bool operator==(const Id &other) const
  { return m_hash == other.m_hash && m_val == other.m_val; }

  const Ptr<String> val() const { return m_val; }

  operator const Ptr<String>() const { return val(); }

  const Ptr<Ppr> ppr() const { return ppr::string(*val()); }

private:
  static std::hash<String> make_hash;

  const Ptr<String> m_val;
  unsigned long m_hash;
};


inline OStream& operator<<(OStream &out, const Id &id)
{
  return out << *id.val();
}


}

#endif /* end of include guard: ID_HXX_HQ5DONMJ */
