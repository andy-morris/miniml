#include "ast/decl.hxx"
#include "tc.hxx"

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

Ptr<Type> ValDecl::type_of(Ptr<Env<Type>> env) const
{
  if (ty()) {
    auto ty_ = nf(ty(), env);
    check_eq(ty_, miniml::type_of(def(), env), def());
    return ty_;
  } else {
    return miniml::type_of(def(), env);
  }
}

}
