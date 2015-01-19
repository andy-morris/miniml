#include "id.hxx"

namespace miniml
{

std::hash<String> Id::make_hash;
using namespace ppr;


Ptr<Ppr> Id::ppr(unsigned, bool pos) const
{ return pos_if(pos, string(*val()), start(), end()); }

}
