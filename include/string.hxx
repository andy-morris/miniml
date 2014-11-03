#include <string>
#include <iostream>
#include <iterator>

namespace miniml
{

/// The char type used by \ref miniml::String.
using Char = char;

/// The string type to be used.
using String = std::string;
using StringIterator = String::iterator;

/// The variety of std::basic_istream that supports \ref String.
using IStream = std::istream;
using StreamBuf = std::streambuf;
using IStreamBufIterator = std::istreambuf_iterator<Char>;

/// The variety of std::basic_ostream that supports \ref String.
using OStream = std::ostream;

/// The variety of std::basic_stringstream that supports \ref String.
using SStream = std::stringstream;
}
