#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crypto.h"

int crypt_file(char *input_file, char *output_file, int offset)
{
  char ch;

  // file pointer (source/target)
  FILE *fps, *fpt;

  /**
   * @brief File reading/writing
   * r = read
   * r+ = r/w file; must exist
   * w = write
   * w+ = creates empty files for r/w
   * rb = read binary
   * wb = write binary
   * rb+ = r/w binary file; must exist
   * wb+ = r/w binary file; creates
   * a = appends; creates
   * a+ = reads + appends; creates
   * ab+ = r/w + appends; creates;
   * *t = action for text files
   * *t+ = action for text files; creates
   */

  // reading input file into `fps`
  fps = fopen(input_file, "r");
  if (fps == NULL)
  {
    return 1;
  }

  // writing output file into `fpt`
  fpt = fopen(output_file, "w");
  if (fpt == NULL)
  {
    return 1;
  }

  // EOF = End Of File
  // Signals when final seeking/reading ends
  while ((ch = fgetc(fps)) != EOF)
  {
    // o + 1 = p
    ch = ch + offset;
    fputc(ch, fpt);
  }

  // important to close your files
  // once you're done
  fclose(fps);
  fclose(fpt);

  // lastly return 0 (signal OK)
  return 0;
}

char *crypt_str(char *input_str, int offset)
{
  // calculates size of
  // input string
  size_t str_length = strlen(input_str);

  // allocates memory in heap
  // by string length + 1
  char *buf = malloc((str_length * sizeof(char)) + 1);

  // loops by the string length
  char ch;
  for (int i = 0; i < str_length; i++)
  {
    // swaps offset from ascii value
    ch = input_str[i] + offset;
    if (!ch)
    {
      break;
    }
    // adds swapped value to string buffer
    buf[i] = ch;
  }

  // a null terminator for
  // who we allocated 1 extra
  // byte in `buf`
  buf[str_length] = '\0';

  // finally return buffer
  // note its a pointer
  return buf;
}