#include "ppr.hxx"

#include <memory>
#include <iostream>
#include <string>

using namespace std;
using namespace miniml;

int main()
{
  Ptr<Ppr> ppr = u8"안녕"_p * +"world"_p + ("boop"_p >> 1);
  cout << *ppr << endl;
}
