#ifndef PANIC_HXX_SDTLKBF0
#define PANIC_HXX_SDTLKBF0

#include <iostream>
#include <cstdlib>

namespace miniml
{

[[noreturn]] void panic(const char *msg)
{
  std::cerr << msg << std::endl;
  std::exit(-1);
}

}

#endif /* end of include guard: PANIC_HXX_SDTLKBF0 */
