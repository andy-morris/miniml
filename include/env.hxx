#ifndef ENV_HXX_V1MWKBLN
#define ENV_HXX_V1MWKBLN

#include "id.hxx"
#include <unordered_map>

namespace miniml
{

template <typename T>
class Env
{
public:
  Env(const Ptr<Env> next): m_next(next) {}
  Env(): Env(nullptr) {}

  Ptr<T> lookup(const Id&) const;

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
  const Ptr<Env> m_next;
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


}

#endif /* end of include guard: ENV_HXX_V1MWKBLN */
