import os

env = Environment()
#env["CXX"]="clang++"
env.Append(CXXFLAGS=['-std=c++0x', '-O2', '-Wall', '-Werror'])
env.Append(CXXFLAGS=['-g'])
env.Append(CCFLAGS=['-O2'])
env.ParseConfig('pkg-config openssl --libs --cflags');

sources = [
  'src/FileOps.cpp',
  'src/Process.cpp',
  'src/ProcessManager.cpp',
  ]

env.Program('remodel', ['src/main.cpp'] + sources)

