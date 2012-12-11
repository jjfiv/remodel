import os

env = Environment(ENV={'PATH':os.environ['PATH']})
#env["CXX"]="clang++"
env.Append(CXXFLAGS=['-std=c++0x', '-O2', '-Wall', '-Werror'])
#env.Append(CXXFLAGS=['-g'])
env.ParseConfig('pkg-config openssl --libs --cflags');

sources = [
  'src/FileOps.cpp',
  'src/ProcessManager.cpp',
  'src/Token.cpp',
  'src/Parser.cpp',
  'src/Arguments.cpp',

  'src/BuildStep.cpp',
  'src/BuildGraph.cpp',
  'src/BuildRecord.cpp',

  'src/TargetBuilder.cpp',
  ]

program = env.Program('remodel', ['src/main.cpp'] + sources)

# execute test.py on 'scons test'
env.AlwaysBuild(env.Alias('test', [program], 'python test.py'));

