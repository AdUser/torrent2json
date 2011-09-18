#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <yajl/yajl_gen.h>


/* FD's */
FILE *in;
FILE *out;

struct yajl_gen_t *gen = NULL;

/* object handler prototype */
void (*handler)(char);

void
get_integer(char chr)
{ /* 'chr' always 'i' in this case */
  int c = '\0';
  bool cont = true;
  long int value = 0; /* signed */

  while (cont)
  {
    if (!cont) break;

    c = fgetc(in);
    switch ((isdigit(c) != 0) ? '0' : c) /* little hack here */
    {
      case '0' :
        value *= 10; /* (0 * 10) -> 0, so it works correct */
        value += c - '0';
        break;
      case '-' :
      value *= -1;
        break;
      case 'e' : /* integer end marker */
      default  : /* or garbled data    */
        cont = false;
        break;
    }
  }

  yajl_gen_integer(gen, value);
}

void
get_string(char chr)
{
  /* in this case 'chr' first digit of string lenght */
  unsigned int len = (unsigned char) chr - '0';
  int c = '\0';
  int i = 0;
  unsigned char *buf = NULL;

  /* first, get string lenght */
  while (isdigit((c = fgetc(in))))
    len *= 10, len += c - '0';
  /* 'c' now should contain ':' */

  if ((buf = malloc(sizeof(char) * len + 1)) == NULL)
  {
    fprintf(stderr, "Can't allocate memory, exiting.\n");
    exit(EXIT_FAILURE);
  }

  memset(buf, '\0', len + 1);
  for (i = 0; i <= len; i++)
    *(buf + i * sizeof(char)) = fgetc(in);

  yajl_gen_string(gen, buf, len);

  free(buf);
}

void (*select_handler(char chr))(char)
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
  const unsigned char *buf = NULL;
  size_t len = 0;

  in  = stdin;
  out = stdout;

  gen = yajl_gen_alloc(NULL);
  yajl_gen_config(gen, yajl_gen_beautify,         1);
  yajl_gen_config(gen, yajl_gen_indent_string, "  ");

  while ((chr = fgetc(in)) != EOF);
  {
    handler = select_handler(chr);
    handler(chr);
  }

  yajl_gen_get_buf(gen, &buf, &len);
  fprintf(out, "%s", buf);
  yajl_gen_free(gen);

  exit(EXIT_SUCCESS);
}

