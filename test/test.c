#include <stdio.h>
#include <stdlib.h>

#include "../user.c"
#include "../crypto.c"
#include "../utils.c"

int test_crypt_file()
{
  int ret;
  const int offset = 70;
  char input_file[] = "hello.txt";
  char encrypted_file[] = "encrypted.txt";
  char output_file[] = "output.txt";

  ret = crypt_file(input_file, encrypted_file, offset);

  if (ret == 0)
  {
    printf("successfully encrypted file.\n");
  }
  else
  {
    printf("failed to encrypt file.\n");
  }

  ret = crypt_file(encrypted_file, output_file, -offset);

  if (ret == 0)
  {
    printf("successfully decrypted file.\n");
  }
  else
  {
    printf("failed to decrypt file.\n");
  }

  return ret;
}

void test_crypt_str()
{
  char password[] = "12345678";
  char *encrypted_password = crypt_str(password, user_pwd_key);
  printf("encrypted password = %s\n", encrypted_password);

  char *decrypted_password = crypt_str(encrypted_password, -user_pwd_key);
  printf("decrypted password = %s\n", decrypted_password);

  free(encrypted_password);
  free(decrypted_password);
}

int test_list_dir()
{
  struct dirent **container;
  int count = list_dir(".", &container);
  if (count < 0)
  {
    printf("error reading dir\n");
    return 1;
  }

  for (int i = 0; i < count; i++)
  {
    // d_type: 8(file) | 4(dir)
    if (container[i]->d_type == 8)
      printf("entry[%d] = %s\n", i + 1, container[i]->d_name);
  }

  return count;
}

int test_file_io()
{
  char *content = read_file("hello.txt");
  printf("reads: %s\n", content);

  int ret = write_file("hello_e.txt", content);
  if (ret == 0)
  {
    printf("file written\n");
  }
  else
  {
    printf("cannot write file\n");
  }

  free(content);
  return ret;
}

void test_list_all_user_files()
{
  char **output;
  int total_files = list_all_user_files("foo", &output);
  for (int i = 0; i < total_files; i++)
  {
    printf("file[%d] = %s\n", i, output[i]);
  }
  free(output);
}

void test_user()
{
  // printf("create user: %d\n", create_user("xusr", "123"));
  // printf("user exists: %d\n", assert_user("xusr", "123"));
  // printf("encrypt ret: [%d]\n", crypt_user_file("xusr", "123", "hello.txt", "hello", 1));
  // printf("decrypt ret: [%d]\n", crypt_user_file("xusr", "123", "", "hello", 0));
}

int main(void)
{
  test_user();
}