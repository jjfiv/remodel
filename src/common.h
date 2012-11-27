#ifndef _COMMON_H
#define _COMMON_H

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <iostream>
using std::cout;
using std::cerr;

// debugging fun
#define show(var) \
  cout << __FILE__ << ":" << __LINE__ << ": " << #var << " = " << var << "\n"


#endif

