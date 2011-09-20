#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <yajl/yajl_gen.h>
#include <yajl/yajl_parse.h>

/* FD's */
FILE *in;
FILE *out;

struct yajl_handle_t *handle = NULL;
struct yajl_gen_t *gen = NULL;

static int
write_integer(void *ctx, long long l)
{
  fprintf(out, "i%llie", l);
  return 1;
}

static int
write_dict_start(void *ctx)
{ return fputc('d', out); }

static int
write_string(void *ctx, const unsigned char *str, size_t len)
{ return (!!fprintf(out, "%u:", len) && !!fwrite(str, 1, len, out)); }

static int
write_list_start(void *ctx)
{ return fputc('l', out); }

/* for dictionary, list and integer *
 * end we have the same marker: 'e' */
static int
write_end(void *ctx)
{ return fputc('e', out); return 0; }

yajl_callbacks callbacks = {
  NULL,             /* null    */
  NULL,             /* bool    */
  write_integer,    /* integer */
  NULL,             /* double  */
  NULL,             /* number  */
  write_string,     /* string  */
  write_dict_start, /* start map   */
  NULL,             /* map key     */
  write_end,        /* end map     */
  write_list_start, /* start array */
  write_end         /* end array   */
};

int
main(int argc, char **argv)
{
  /* FILE */ in  = stdin;
  /* FILE */ out = stdout;

  gen = yajl_gen_alloc(NULL);
  yajl_gen_config(gen, yajl_gen_validate_utf8, 0);

  handle = yajl_alloc(&callbacks, NULL, (void *) gen);

  yajl_config(handle, yajl_allow_comments,        1);
  yajl_config(handle, yajl_dont_validate_strings, 1);

  /* some work here */

  yajl_free(handle);

  exit(EXIT_SUCCESS);
}

