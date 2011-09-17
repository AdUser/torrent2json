#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <yajl/yajl_common.h>
#include <yajl/yajl_gen.h>

#define BUF_SIZE 4096

char buf[BUF_SIZE];

/* FD's */
int in;
int out;

void (*handler)(int);

void (*select_handler(char chr))(int)
{
  handler = NULL;

  switch ((isdigit(chr) != 0) ? '0' : chr)
  {
    case 'd' : /* dictionary */
      break;
    case 'e' : /* dictionary, integer, or list end */
      break;
    case 'i' : /* integer */
      break;
    case 'l' : /* list */
      break;
    case '0' : /* string */
      break;
    default:
      fprintf(stderr, "Unknown marker: %u\n", chr);
      break;
  }

  return handler;
}

int
main(int argc, char **argv)
{
  int chr = '\0';

  in  = dup(0);
  out = dup(1);

  read(in, &chr, 1);
  if (chr != 'd')
  {
    fprintf(stderr, "Malformed torrent file\n");
    exit(EXIT_FAILURE);
  }

  /* some work here */
  while (true)
  {
    handler = select_handler(chr);

    if (handler == NULL)
      break;

    handler(in);
  }

  exit(EXIT_SUCCESS);
}

