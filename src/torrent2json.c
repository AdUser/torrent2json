#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  int chr;
  int fd = dup(0);

  while (true)
  {
    chr = read(fd, &chr, 1);
    switch (chr)
    {
      case 'd' : /* dictionary */
/*        print_dict(fd);*/
        break;
      case 'i' : /* integer */
        break;
      case 'l' : /* list */
        break;
      case '1' :
      case '2' :
      case '3' :
      case '4' :
      case '5' :
      case '6' :
      case '7' :
      case '8' :
      case '9' : /* string */
        break;
      default:
        fprintf(stderr, "Unknown marker: %u\n", chr);
        break;
    }
    break;
  }

  exit(0);
}

