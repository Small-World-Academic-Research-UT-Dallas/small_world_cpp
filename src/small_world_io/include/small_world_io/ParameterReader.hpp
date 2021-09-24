#pragma once

#include <string>

/*

This class is a generic representation of a reader for a parameter file. The
functions defined on it all being with get_ and take a string. These functions
will return the named parameter as the given type, and if that parameter does
not exist or is not in the right format, throw an error.

Many possible readers could exist: a test reader that returns canned values,
our main JSON reader, a hypothetical future YAML reader, etc. This abstract
class just wraps them in one interface to make things easier on us.

Right now, all we need are doubles. Strings, integers, etc will follow.

*/

namespace small_world::io {

class ParameterReader {
public:
  virtual ~ParameterReader() {}; // Make it extendable
  virtual double get_double(std::string name) = 0; // Get a paramter as a double
};

}
