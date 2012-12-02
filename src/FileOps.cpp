#include "FileOps.h"

#include <cstdio>
#include <openssl/md5.h>
#include <sys/stat.h>
#include <sys/types.h>


const size_t HashBufferSize = 1024;

static string stringFromData(const void *ptr, size_t numBytes) {
  // get byte pointer
  const unsigned char *cptr = (const unsigned char*) ptr;
  
  // create output string big enough
  string result;
  result.reserve(2*numBytes);

  for(size_t i=0; i<numBytes; i++) {
    char ascii[3];
    snprintf(ascii, sizeof(ascii), "%02x", cptr[i]);
    ascii[2] = 0;
    result += ascii;
  }

  return result;
}

string fileSignature(const string &path) {
  MD5_CTX ctx;
  unsigned char csum[MD5_DIGEST_LENGTH];

  assert(MD5_Init(&ctx) != 0);

  FILE *fp = fopen(path.c_str(), "rb");
  if(!fp)
    return "";

  char buffer[HashBufferSize];
  while(!feof(fp)) {
    size_t len = fread(buffer, 1, sizeof(buffer), fp);

    if(len <= 0)
      break;
    
    assert(MD5_Update(&ctx, buffer, len) != 0);
  }

  assert(MD5_Final(csum, &ctx) != 0);

  return stringFromData(csum, MD5_DIGEST_LENGTH);
}

bool changeDirectory(const string &path) {
  int rc = chdir(path.c_str());
  if(rc == 0) return true;

  perror("chdir");
  show(path);
  return false;
}

bool canOpenFile(const string &path) {
  FILE *fp = fopen(path.c_str(), "r");
  if(fp != 0) {
    fclose(fp);
    return true;
  }
  return false;
}

bool createDirectory(const string &path) {
  int rc = mkdir(path.c_str(), 0777);
  if(rc == 0) return true;

  int err = errno;
  if(err == EEXIST)
    return true;

  perror("mkdir");
  show(path);
  return false;
}

bool removeFile(const string &path) {
  int rc = remove(path.c_str());
  if(rc == 0) return true;

  int err = errno;

  if(err == ENOENT) {
    // file already did not exist; not an error
    return false;
  }

  perror("remove");
  return false;
}

static string temporaryName() {
  string buffer(L_tmpnam, '\0');

  // tmpnam returns the pointer to a temporary unique name
  char* result = tmpnam(&buffer[0]);
  assert(result == &buffer[0]);

  // trim any extra zeroes because I'm crazy like that
  return string(buffer.c_str());
}

bool myLockFile(const string &path) {
  // open lock file and make sure it's ours
  pid_t lockOwner;
  std::ifstream fp(path);

  if(!(fp >> lockOwner))
    return false;

  return lockOwner == getpid();
}

bool createLockfile(const string &path) {
  const string tmpName = temporaryName();
  
  // write pid to a temporary file
  std::ofstream fp(tmpName);
  fp << getpid() << "\n";
  fp.close();

  // hard link file to lock location
  int rc = link(tmpName.c_str(), path.c_str());
  if( rc != 0 ) {
    int err = errno;

    if(err != EEXIST) {
      perror("link");
    }

    removeFile(tmpName);
    return false;
  }

  // remove/unlink our temporary file
  removeFile(tmpName);
  return myLockFile(path);
}

