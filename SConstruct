import os

env = Environment()
#env["CXX"]="clang++"
env.Append(CXXFLAGS=['-std=c++0x', '-O2', '-Wall', '-Werror'])
env.Append(CXXFLAGS=['-g'])
env.Append(CCFLAGS=['-O2'])
#env.Append(LIBS=['rt'])

sources = [
  ]

env.Program('remodel', ['src/main.cpp'] + sources)

