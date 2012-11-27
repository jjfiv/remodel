#include "FileOps.h"

#include <cstdio>
#include <openssl/md5.h>

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

  if(MD5_Init(&ctx) == 0)
    return "MD5_Init error";

  FILE *fp = fopen(path.c_str(), "rb");
  if(!fp)
    return "File Doesn't Exist";

  char buffer[HashBufferSize];
  while(!feof(fp)) {
    size_t len = fread(buffer, 1, sizeof(buffer), fp);

    if(len <= 0)
      break;
    
    if(MD5_Update(&ctx, buffer, len) == 0)
      return "MD5_Update error";
  }

  if(MD5_Final(csum, &ctx) == 0)
    return "MD5_Final error";

  return stringFromData(csum, MD5_DIGEST_LENGTH);
}


