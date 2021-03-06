#include <ctype.h>
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

void (*select_handler(int))(int);

void
usage(int exitcode)
{
  fprintf(stderr, "\
Usage: torrent2json [<options>]\n\
Where options are:\n\
  -h            This help.\n\
  -i <file>     Input file.   (Default: read from stdin)\n\
  -o <file>     Output file.  (Default: write to stdout)\n\
  -c            Compact json. (Default: no)\n");
  exit(exitcode);
}

void
hexencode(unsigned char *buf, unsigned int len) {
  unsigned char *encbuf = NULL;
  unsigned int   enclen = sizeof(char) * (len * 3 + 3); /* "hex" + "FF," x len */
  unsigned int i, j;
  unsigned int a, b;

  if ((encbuf = malloc(enclen)) == NULL) {
    fprintf(stderr, "Can't allocate memory, exiting.\n");
    exit(EXIT_FAILURE);
  }

  memcpy(encbuf, "hex", 3);

  for (i = 0, j = 3; i < len; i++, j += 3) {
    a = (buf[i] & 0xF0) >> 4;
    b = (buf[i] & 0x0F) >> 0;
    encbuf[j + 0] = ',';
    encbuf[j + 1] = (a > 9) ? 'A' + (a - 10) : '0' + a;
    encbuf[j + 2] = (b > 9) ? 'A' + (b - 10) : '0' + b;
  }

  encbuf[3] = ':';
  yajl_gen_string(gen, encbuf, enclen);

  free(encbuf);
}

/* handlers */
void
get_integer(int chr)
{ /* 'chr' always 'i' in this case */
  int c = '\0';
  bool cont = true;
  bool negative = false;
  long int value = 0; /* signed */

  while (cont)
  {
    c = fgetc(in);
    switch ((isdigit(c) != 0) ? '0' : c) /* little hack here */
    {
      case '0' :
        value *= 10; /* (0 * 10) -> 0, so it works correct */
        value += c - '0';
        break;
      case 'e' : /* integer end marker */
        cont = false;
        break;
      case '-' :
        if (value == 0) /* true if '-' - first char after 'i' */
        {
          negative = true;
          break;
        } /* else we consider it as garbage  */
      default  :
        fprintf(stderr, "Garbage after integer: %i%c<\n", value, c);
        exit(EXIT_FAILURE);
        break;
    }
  }

  if (negative) value *= -1;
  yajl_gen_integer(gen, value);
}

void
get_string(int chr)
{
  /* in this case 'chr' first digit of string lenght */
  unsigned int len = (unsigned char) chr - '0';
  int c = '\0';
  unsigned int i = 0;
  unsigned char *buf = NULL;
  unsigned int hex = 0;

  /* first, get string lenght */
  while (isdigit((c = fgetc(in))))
    len *= 10, len += c - '0';
  /* 'c' now should contain ':' */

  if ((buf = malloc(sizeof(char) * (len + 1))) == NULL)
  {
    fprintf(stderr, "Can't allocate memory, exiting.\n");
    exit(EXIT_FAILURE);
  }

  memset(buf, '\0', sizeof(char) * (len + 1));
  for (i = 0; i < len; i++) {
    buf[i] = fgetc(in);
    if (buf[i] < 0x20) /* string has control chars */
      hex = 1;
  }

  if (hex == 1) {
    hexencode(buf, len);
  } else {
    yajl_gen_string(gen, buf, len);
  }

  free(buf);
}

void
get_dict(int chr)
{ /* 'chr' always 'd' in this case */
  void (*handler)(int) = NULL;
  int c = '\0';

  yajl_gen_map_open(gen);

  while ((c = fgetc(in)) != 'e')
  {
    handler = select_handler(c);
    if (handler) handler(c);
  }

  yajl_gen_map_close(gen);
}

void
get_list(int chr)
{ /* 'chr' always 'l' in this case */
  void (*handler)(int) = NULL;
  int c = '\0';

  yajl_gen_array_open(gen);

  while ((c = fgetc(in)) != 'e')
  {
    handler = select_handler(c);
    if (handler) handler(c);
  }

  yajl_gen_array_close(gen);
}

/* general purpose functions */
void (*select_handler(int chr))(int)
{
  switch ((isdigit(chr) != 0) ? '0' : chr)
  {
    case 'd' : /* dictionary */
      return get_dict;
      break;
    case 'i' : /* integer */
      return get_integer;
      break;
    case 'l' : /* list */
      return get_list;
      break;
    case '0' : /* string */
      return get_string;
      break;
    default:
      fprintf(stderr, "Unknown marker: %u\n", chr);
      break;
  }

  return NULL;
}

int
main(int argc, char **argv)
{
  void (*handler)(int) = NULL;
  int c = '\0';
  const unsigned char *buf = NULL;
  char opt = '\0';
  size_t len = 0;

  /* FILE */ in  = stdin;
  /* FILE */ out = stdout;

  if ((gen = yajl_gen_alloc(NULL)) == NULL)
  {
    fprintf(stderr, "Can't allocate json generator. Exiting.");
    exit(EXIT_FAILURE);
  }

  yajl_gen_config(gen, yajl_gen_beautify,         1);
  yajl_gen_config(gen, yajl_gen_indent_string, "\t");

  while ((opt = getopt(argc, argv, "hi:o:c")) != -1)
    switch (opt)
    {
      case 'h' :
        usage(EXIT_SUCCESS);
        break;
      case 'i' :
        if ((in = fopen(optarg, "r")) == NULL)
        {
          fprintf(stderr, "Can't open input file '%s'. Exiting.", optarg);
          exit(EXIT_FAILURE);
        }
        break;
      case 'o' :
        if ((out = fopen(optarg, "w")) == NULL)
        {
          fprintf(stderr, "Can't open output file '%s'. Exiting.", optarg);
          exit(EXIT_FAILURE);
        }
        break;
      case 'c' :
        yajl_gen_config(gen, yajl_gen_beautify,       0);
        break;
      default  :
        usage(EXIT_FAILURE);
        break;
    }

  while ((c = fgetc(in)) != EOF)
  {
    handler = select_handler(c);
    if (handler) handler(c);
  }

  yajl_gen_get_buf(gen, &buf, &len);
  if (len > 0 && buf) fprintf(out, "%s", buf);
  yajl_gen_free(gen);

  exit(EXIT_SUCCESS);
}

