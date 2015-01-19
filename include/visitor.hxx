#ifndef VISITOR_HXX_MDEFBPJA
#define VISITOR_HXX_MDEFBPJA

#include <string>
#include <exception>

namespace miniml
{

struct AbstractVisit final: public std::exception
{
  AbstractVisit(const std::string &cls):
    msg("tried to visit an abstract class " + cls)
  {}
  inline const char *what() const noexcept override { return msg.c_str(); }
  std::string msg;
};

}

#endif /* end of include guard: VISITOR_HXX_MDEFBPJA */
