#include "pos.hxx"

namespace miniml
{
namespace ppr
{

  Ptr<Ppr> pos_if(bool b, Ptr<Ppr> ppr, Pos start, Pos end)
  {
    return b? hcat({ppr, Span {start, end}.ppr()}) : ppr;
  }

}
}
