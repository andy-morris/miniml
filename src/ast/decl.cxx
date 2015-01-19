#include "ast/decl.hxx"

namespace miniml
{

using namespace ppr;

Ptr<Ppr> ValDecl::ppr(unsigned, bool pos) const
{
  return vcat({hcat({pos_if(pos, "val"_p, start(), end()),
                     +name().ppr(pos),
                     +'='_p}),
               def()->ppr(0, pos) >> 1});
}

}
