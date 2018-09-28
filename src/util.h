#ifndef OPENVRT_FIRMWARE_UTIL_H
#define OPENVRT_FIRMWARE_UTIL_H

char *make_null_terminated_string(char *src, size_t len)
{
  char *buf = (char *) malloc(sizeof(char) * (len + 1));
  for (int i = 0; i < len; i++) {
    buf[i] = src[i];
  }
  buf[len] = '\0';
  return buf;
}

#endif //OPENVRT_FIRMWARE_UTIL_H
