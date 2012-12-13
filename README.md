remodel
=======

"make done right" - for [CS691ST@umass](http://plasma.cs.umass.edu/emery/grad-systems)

### Dependencies:
openssl - for md5sum hash calculations
gmake or scons - for building the tool initially
python - for test scripts

### C++ 11 Note:
The code makes heavy use of C++ 11 features, namely the range-based for loop, ``nullptr``, and ``auto`` type inference. Although the standard is finalized, compiler support is not. This code was tested on x86_64 Linux with two compilers:

 - gcc 4.6.3
 - clang 3.1

Other versions should work but were not tested. Newer versions of each may need the flag ``-std=c++0x`` to be changed to ``-std=c++11``

### Build and Test:
    make test

### Invoking Remodel:
Remodel supports a variety of arguments in the flavor of ``gmake`` and a few of new design. Instead of defaulting to interpreting the ``Makefile`` file in the current directory, it looks for a file named ``remodelfile``.

Do note that ``remodel -j3`` is invalid due to argument parsing. The correct command would be ``remodel -j 3``, apparently the ``-j3`` mode is deprecated within POSIX recommendations and gmake itself, but kept for compatibilitty.

### Command Line Arguments:

    -f, --file
      * Read FILE as a remodelfile.
    -C, --directory
      * Change to DIRECTORY before doing anything.
    -j, --jobs
      * Specify the maximum number of jobs. Unlimited if N <= 0.
    --graph
      * Output a Graphviz dot file to FILE
    -w, --wait
      * Wait for lock if there is contention.
    --clean
      * Delete all generated files.
    --remake
      * Delete all generated files and make targets anyway.
    --help, -?, -h
      * Show this help.

### Included Test Cases

Test cases are enumerated within the ``test.py`` file and are contained within the ``ex/`` directory, with the exception of the root ``remodelfile`` and the ``meta`` build.

Inside the ``ex/baz`` folder are files mimicing the example given in the project specification. These are the subject of a few tests. In addition, there are tests for common errors.

### Remodel Error-Checking
Remodel will...
 - complain if told to build a target that is not defined as part of the remodelfile.
 - complain if multiple definitions are given for the same target.
 - detect and abort on circular dependencies.
 - complain if actions are not provided for building targets
 - complain if a target depends on a source file that is not present in the filesystem.
 - detect if it is being invoked on a directory where another remodel process is running. It maintains a lockfile in the .remodel directory.
 - detect if actions do not create the targets specified by the remodelfile
 - detect if build file cannot create the required targets (after building what it can)

