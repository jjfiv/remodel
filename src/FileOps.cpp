#include "FileOps.h"

#include "str++.h"

#include <cstdio>
#include <openssl/md5.h>

const size_t HashBufferSize = 1024;

string fileSignature(const string &path) {
  string data;
  MD5_CTX ctx;
  unsigned char csum[MD5_DIGEST_LENGTH];

  if(MD5_Init(&ctx) == 0)
    return "MD5_Init error";

  FILE *fp = fopen(path.c_str(), "rb");
  if(!fp)
    return "File Doesn't Exist";

  char buffer[1024];
  while(!feof(fp)) {
    size_t len = fread(buffer, 1, sizeof(buffer), fp);

    if(len <= 0)
      break;
    
    if(MD5_Update(&ctx, buffer, len) == 0)
      return "MD5_Update error";
  }

  if(MD5_Final(csum, &ctx) == 0)
    return "MD5_Final error";

  string result;
  result.reserve(2*MD5_DIGEST_LENGTH);

  for(int i=0; i<MD5_DIGEST_LENGTH; i++) {
    char ascii[3];
    snprintf(ascii, sizeof(ascii), "%02x", csum[i]);
    result += ascii;
  }

  return result;
}
