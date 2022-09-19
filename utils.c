#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "utils.h"

char *read_file(char *input_path)
{
  FILE *fps;
  fps = fopen(input_path, "r");

  // if files doesn't open,
  // return empty string
  if (fps == NULL)
  {
    return "";
  }

  // seeks to the end of file
  fseek(fps, 0L, SEEK_END);
  // takes size of the file
  // since its seeked to the end
  size_t file_size = ftell(fps);
  // seeks back to start position
  fseek(fps, 0L, SEEK_SET);

  // allocate memory in the heap
  // by total size of the file + 1
  // +1 is for string terminator '\0'
  char *buf = malloc((file_size * sizeof(char)) + 1);

  // keeps record of seek point
  int seeker = 0;
  // during reading each character
  // is seeked throughout
  // and this is its definition
  char ch;

  // while file isn't seeked to the end,
  // keep seeking
  while ((ch = fgetc(fps)) != EOF)
  {
    // sets char at seek position
    // to the buffer
    buf[seeker] = ch;
    // passes on seeker gutter
    seeker++;
  }

  // string terminator
  // for whom we assigned
  // one extra byte in malloc ^
  buf[file_size] = '\0';

  // lastly close file (important)
  fclose(fps);
  // and return buffer
  // note: its a pointer
  return buf;
}

int write_file(char *output_path, char *content)
{
  FILE *fpt;
  char ch;

  // opens file `output_path`
  // for writing
  fpt = fopen(output_path, "w");

  // if file ptr `fpt` is NULL
  // then exit with exit code 1
  if (fpt == NULL)
  {
    return 1;
  }

  // calculates the size of string
  size_t content_length = strlen(content);

  // read string and copy
  // to the output file
  // until reaches end

  for (int i = 0; i < content_length; i++)
  {
    ch = content[i];
    // if string reaches end
    // then break loop
    if (!ch)
    {
      break;
    }
    // puts each character read
    // from `content` string to
    // output_file ptr `fpt`
    fputc(ch, fpt);
  }

  // important: close file
  // once done writing
  fclose(fpt);

  // return 0 as no error
  return 0;
}

int list_dir(char *dir_path, struct dirent ***container)
{
  struct dirent **file_list;
  int count;

  // scans the dir `dir_path`
  // and return count of entry
  count = scandir(
      dir_path,
      &file_list,
      NULL,
      alphasort);

  // if count < 0, means potential error
  if (count < 0)
  {
    return count;
  }

  // fills the container
  *container = file_list;

  return count;
}

int mkdir_p(char *dir_path)
{
  // check if `dir_path` exists
  // if so, it makes one
  if (path_exists(dir_path) < 0)
  {
// Windows
#if _WIN32
    return mkdir(dir_path);
// Unix
#else
    return mkdir(dir_path, 0700);
#endif
  }
  else
  {
    // exists already; no error;
    return 0;
  }

  // neither exists
  // nor can be created
  // raises err
  return 1;
}

int path_exists(char *path)
{
  // `struct stat` object from <sys/stat>
  // is a container to receive result
  // from `stat` function call
  struct stat st = {0};

  // if `path` doesn't exists
  // also applies this stat to
  // local var `struct stat st`
  // given it a ptr
  return stat(path, &st);
}

void free_dirent(struct dirent ***container, size_t count)
{
  // frees all of the
  // dirent contents
  // sub pointers
  for (int i = 0; i < count; i++)
    free((*container)[i]);
  // then dirent[] itself
  free((*container));
}