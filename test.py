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
    print(out)
    print(err)
    sys.exit(-1);

def run():
  test("./remodel -C ex/baz")
  test("./remodel -C ex/baz stupid", 255)
  test("./remodel -f ex/badRules", 255)
  print("All tests passed!")

if __name__ == '__main__':
  run()

