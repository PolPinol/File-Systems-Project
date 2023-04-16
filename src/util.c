#include "../include/util.h"

char *format_time(time_t t) {
  static char buf[64];
  strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Y", localtime(&t));
  return buf;
}
