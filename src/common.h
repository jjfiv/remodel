#ifndef _COMMON_H
#define _COMMON_H

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <iostream>

#include <map>
#include <set>

#include <fstream>
#include <sstream>

#include <cassert>

// debugging fun
#define show(var) \
  std::cout << __FILE__ << ":" << __LINE__ << ": " << #var << " = " << var << "\n"


#endif

