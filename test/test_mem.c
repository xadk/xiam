#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#include "../utils.c"

void cleaner(struct dirent ***c2, size_t count)
{
  printf("sizeof(struct dirent***) = %ld\n", sizeof(*c2));

  for (int i = 0; i < count; i++)
    free((*c2)[i]);
  free((*c2));
}

void test_mem_leak()
{
  struct dirent **container;
  char dirs[2][64] = {".", "include"};
  int count = list_dir(dirs[0], &container);
  struct dirent **c2;

  int cycle = 0;
  for (;;)
  {
    printf("\n\ncycle: %d\n", cycle);
    printf("sizeof(container) = %ld\n\n", sizeof(container));

    c2 = malloc(count * sizeof(struct dirent *));

    for (int i = 0; i < count; i++)
      c2[i] = malloc(sizeof(struct dirent));

    cleaner(&c2, count);

    usleep(100 * 1000);
    cycle++;
  }
}

void put_string_arr(char ***bucket, int count)
{
  int str_size = 512;
  char **container = malloc(count * sizeof(char *));

  for (int i = 0; i < count; i++)
  {
    container[i] = malloc(str_size * sizeof(char));
    strcpy(container[i], "hello world!");
  }

  *bucket = container;
}

void test_mem2()
{
  int cycle = 0;
  for (;;)
  {
    char **str_arr;
    int count = 5;
    put_string_arr(&str_arr, count);

    for (int i = 0; i < count; i++)
    {
      free(str_arr[i]);
    }
    free(str_arr);

    usleep(100 * 1000);
    printf("cycle: %d\n", cycle);
    cycle++;
  }
}

int main(void)
{

  // test_mem_leak();
  test_mem2();
  return 0;
}