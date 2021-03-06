import subprocess, os, sys

def do_cmd(cmd):
  """Runs a shell command using subprocess module, silently, returns rc, stdout, stderr tuple """
  proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
  out, err = proc.communicate()
  rc = proc.returncode
  return rc, out, err

def test(cmd, exprc=0):
  rc, out, err = do_cmd(cmd)
  if(rc != exprc):
    print "- " + cmd
    print(out)
    print(err)
    sys.exit(-1);
  print "+ %3d <- %s" % (rc, cmd)
  #print os.path.exists(".remodel/lock") # for debugging locks

def run():
  test("./remodel -C ex/baz --clean")
  test("./remodel -C ex/baz")
  test("./remodel -C ex/baz noSuchTarget", 255)
  
  test("./remodel -C ex/multarget --remake")

  test("./remodel --remake")
  test("./remodel -f meta") # build itself!

  for bad_file in [
      "ex/badRules",
      "ex/circularDeps",
      "ex/missingAction",
      "ex/noSuchFile",
      ]:
    test("./remodel -f %s" % bad_file, 255)

  print("All tests passed!")

if __name__ == '__main__':
  run()

