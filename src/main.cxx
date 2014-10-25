#include "ppr.hxx"

#include <memory>
#include <iostream>
#include <string>

using namespace std;
using namespace miniml;

int main()
{
  auto ppr = L"hello"_p * +L"world"_p + (L"boop"_p >> 1);
  wcout << *ppr << endl;
}
