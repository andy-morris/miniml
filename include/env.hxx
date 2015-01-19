#ifndef ENV_HXX_V1MWKBLN
#define ENV_HXX_V1MWKBLN

#include "id.hxx"
#include "ptr.hxx"
#include <functional>
#include <unordered_map>

namespace miniml
{

template <typename T>
class EnvBase
{
public:
  virtual ~EnvBase() {}
  virtual Ptr<T> lookup(const Id&) const = 0;
};

template <typename T>
class Env final: public EnvBase<T>
{
public:
  Env(const Ptr<EnvBase<T>> next): m_next(next) {}
  Env(): Env(nullptr) {}

  virtual Ptr<T> lookup(const Id&) const override;

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


template <typename T, typename U>
class MappedEnv final: public EnvBase<T>
{
public:
  /// Function from U to T but with more pointing.
  using Fun = std::function<Ptr<T>(Ptr<U>)>;

  MappedEnv(const Ptr<Env<U>> inner, const Fun func):
    m_inner(inner), m_func(func)
  {}

  inline Ptr<Env<U>> inner() const { return m_inner; }
  inline Ptr<T> map(Ptr<U> x) const { return m_func(x); }

  virtual Ptr<T> lookup(const Id &id) const override;
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
