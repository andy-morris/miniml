#include <string>
#include <iostream>

namespace miniml
{

/// The char type used by \ref miniml::String.
using Char = wchar_t;

/// The string type to be used.
using String = std::wstring;

/// The variety of std::basic_istream that supports \ref String.
using IStream = std::wistream;

/// The variety of std::basic_ostream that supports \ref String.
using OStream = std::wostream;

/// The variety of std::basic_stringstream that supports \ref String.
using SStream = std::wstringstream;
}
