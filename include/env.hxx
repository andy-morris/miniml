#ifndef ENV_HXX_V1MWKBLN
#define ENV_HXX_V1MWKBLN

#include "id.hxx"
#include "ptr.hxx"
#include <functional>
#include <unordered_map>

namespace miniml
{

/// Base class for environments, which are layers of mappings from identifiers
/// to `T`s. On lookup, if an identifier isn't present in one layer, the next
/// one is checked, to model nested scopes.
/// \sa Env
template <typename T>
class EnvBase
{
public:
  virtual ~EnvBase() {}
  /// Look up a name.
  /// \return `nullptr` if the name isn't present.
  virtual Ptr<T> lookup(const Id&) const = 0;
  /// Dump the contents of the environment and any inner ones.
  virtual void debug() const = 0;
};

/// Normal environments backed by a map data structure.
template <typename T>
class Env final: public EnvBase<T>
{
public:
  Env(const Ptr<EnvBase<T>> next): m_next(next) {}
  Env(): Env(nullptr) {}

  Ptr<T> lookup(const Id&) const override;

  /// Add a new binding.
  void insert(const Id&, const Ptr<T>);

  void debug() const
  {
    for (auto p: m_elements) {
      std::cerr << p.first << ": " << *p.second->ppr() << std::endl;
    }
    std::cerr << "=======" << std::endl;
    if (m_next) m_next->debug();
  }

private:
  std::unordered_map<Id, Ptr<T>, std::hash<Id>> m_elements;
  const Ptr<EnvBase<T>> m_next;
};


template <typename T>
Ptr<T> Env<T>::lookup(const Id &id) const
{
  auto it = m_elements.find(id);
  if (it == m_elements.end()) {
    return m_next? m_next->lookup(id): nullptr;
  } else {
    return it->second;
  }
}

template <typename T>
void Env<T>::insert(const Id &id, const Ptr<T> val)
{
  m_elements.insert(std::make_pair(id, val));
}


/// Environments which map some other Env's contents on the fly.
/// Since the function may not be bijective, mapped environments don't support
/// insertion.
template <typename T, typename U>
class MappedEnv final: public EnvBase<T>
{
public:
  /// Function from U to T but with more pointing.
  using Fun = std::function<Ptr<T>(Ptr<U>)>;

  MappedEnv(const Ptr<Env<U>> inner, const Fun func):
    m_inner(inner), m_func(func)
  {}

  /// The backing environment.
  inline Ptr<Env<U>> inner() const { return m_inner; }
  inline Ptr<T> map(Ptr<U> x) const { return m_func(x); }

  inline void debug() const { m_inner->debug(); }

  Ptr<T> lookup(const Id &id) const override;
private:
  const Ptr<Env<U>> m_inner;
  const Fun m_func;
};

template <typename T, typename U>
Ptr<T> MappedEnv<T, U>::lookup(const Id &id) const
{
  auto ret = inner()->lookup(id);
  return ret? map(ret): nullptr;
}

template <typename T, typename U>
Ptr<Env<T>> map_env(const Ptr<Env<U>> inner,
                    typename MappedEnv<T, U>::Fun func)
{
  return ptr<Env<T>>(ptr<MappedEnv<T, U>>(inner, func));
}

}

#endif /* end of include guard: ENV_HXX_V1MWKBLN */
