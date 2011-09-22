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

#define BUF_SIZE (64 * 1024)

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
  write_string,     /* map key     */
  write_end,        /* end map     */
  write_list_start, /* start array */
  write_end         /* end array   */
};

void
check_yajl_error(struct yajl_handle_t *handle, unsigned char *buf, size_t read)
{
  unsigned char *yajl_error = NULL;

  if ((yajl_error = yajl_get_error(handle, 1, buf, read)) != NULL)
  {
    fwrite(buf, read, 1, stderr);
    fputc('\n', stderr);
    yajl_free_error(handle, yajl_error);
  }
}

int
main(int argc, char **argv)
{
  static unsigned char buf[BUF_SIZE];
  size_t read = 0;
  /* FILE */ in  = stdin;
  /* FILE */ out = stdout;

  gen = yajl_gen_alloc(NULL);
  yajl_gen_config(gen, yajl_gen_validate_utf8, 0);

  handle = yajl_alloc(&callbacks, NULL, (void *) gen);

  yajl_config(handle, yajl_allow_comments,        1);
  yajl_config(handle, yajl_dont_validate_strings, 1);

  while ((read = fread(buf, 1, (BUF_SIZE - 1), in)) != 0)
  {
    buf[read] = '\0';

    if (yajl_parse(handle, buf, read) != yajl_status_ok)
    {
      check_yajl_error(handle, buf, read);
      break;
    }
  }

  if (yajl_complete_parse(handle) != yajl_status_ok)
    check_yajl_error(handle, buf, read);

  yajl_free(handle);

  exit(EXIT_SUCCESS);
}

