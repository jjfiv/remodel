set -e -u #-x
scons -s
./remodel -C ex/baz
./remodel -f ex/badRules || [ $? -eq 255 ]

