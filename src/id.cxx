#include "id.hxx"

namespace miniml
{

using namespace ppr;

const std::hash<String> Id::make_hash = std::hash<String>();

Ptr<Ppr> Id::ppr(unsigned, bool pos) const
{ return pos_if(pos, string(*val()), start(), end()); }

}
