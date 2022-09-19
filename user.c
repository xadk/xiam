#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "user.h"
#include "utils.h"
#include "crypto.h"

char *create_user_dir_path(char *username)
{
  char *user_dir_path = malloc(256 * sizeof(char));
  // creates user dir path
  // using `root_user_dir`,
  // `dir_sep` and
  // `username`

  sprintf(
      user_dir_path,
      "%s%c%s",
      root_user_dir,
      dir_sep,
      username);

  // note: pointer
  // must be freed from
  // the caller end
  return user_dir_path;
}

char *create_user_credentials_path(char *username)
{
  // creates `user_dir_path`
  char *user_dir_path = create_user_dir_path(username);

  // creates user_credentials_file
  // using `dir_sep`
  // and `user_dir_path`

  char *user_credentials_file = malloc(512 * sizeof(char));

  // creates path for user credentials
  sprintf(
      user_credentials_file,
      "%s%c.usr",
      user_dir_path,
      dir_sep);

  // clear pts from heap
  free(user_dir_path);

  // note: pointer
  // must be freed from
  // the caller end
  return user_credentials_file;
}

char *create_user_decrypted_files_path(char *username)
{
  char *user_dir_path = create_user_dir_path(username);
  // creates `user_dir_decrypted_files`
  // with incremented length after inital
  // 256 bytes of `user_dir_path` and
  // strlen of dirname + 1
  char *user_dir_decrypted_files = malloc(256 + strlen(str_decrypted_dir_name) + 1);
  sprintf(
      user_dir_decrypted_files,
      "%s%c%s",
      user_dir_path,
      dir_sep,
      str_decrypted_dir_name);

  // garbage cleaning
  free(user_dir_path);

  // note: pointer
  // must be freed from
  // the caller end
  return user_dir_decrypted_files;
}

int create_user(char *username, char *password, int overwrite)
{
  int ret;

  // creates `user_dir_path`
  char *user_dir_path = create_user_dir_path(username);

  // checks if overwriting is enabled
  // otherwise will make sure that
  // user doesn't exists already
  if (overwrite == 0 && path_exists(user_dir_path) == 0)
  {
    printf("\n\t[err]: username %s already exists.\n\n", username);
    free(user_dir_path);
    return -2;
  }

  // makes user_dir if !exist
  ret = mkdir_p(user_dir_path);

  if (ret != 0)
  {
    printf("failed to create user dir: %s\n", user_dir_path);
    return 1;
  }

  // creates `user_dir_decrypted_files`
  char *user_dir_decrypted_files = create_user_decrypted_files_path(username);

  // makes user_dir/decrypted_files if !exist
  ret = mkdir_p(user_dir_decrypted_files);
  if (ret != 0)
  {
    printf(
        "failed to create user dir for decrypted files: %s\n",
        user_dir_decrypted_files);
    return 1;
  }

  // creates path for user credentials
  char *user_credentials_file = create_user_credentials_path(username);

  // encrypts the password
  // by traditional ascii
  // values swap

  char *encrypted_password = crypt_str(password, user_pwd_key);

  // flushes user credentials
  // into user_credentials_file
  ret = write_file(user_credentials_file, encrypted_password);

  // free vars from heap
  free(user_dir_path);
  free(user_dir_decrypted_files);
  free(user_credentials_file);
  free(encrypted_password);

  // finally return write_file output
  return ret;
}

int assert_user(char *username, char *password)
{
  int ret;

  // creates `user_dir_path`
  char *user_dir_path = create_user_dir_path(username);

  // check if user directory exists
  if (path_exists(user_dir_path) < 0)
  {
    free(user_dir_path);
    return -1;
  }

  // creates path for user credentials
  char *user_credentials_file = create_user_credentials_path(username);

  // check if  user_credentials_file exists
  if (path_exists(user_credentials_file) < 0)
  {
    free(user_dir_path);
    free(user_credentials_file);
    return -1;
  }

  // reads user_credentials_file
  char *user_credentials_file_content = read_file(
      user_credentials_file);

  // decrypts the password
  // by traditional ascii
  // values swap

  char *decrypted_password = crypt_str(user_credentials_file_content, -user_pwd_key);

  // core: checking if pwd is OK
  ret = strcmp(decrypted_password, password);

  // free vars from heap
  free(user_dir_path);
  free(user_credentials_file);
  free(user_credentials_file_content);
  free(decrypted_password);

  // finally return write_file output
  return ret;
}

int list_all_user_files(char *username, char ***output)
{
  // creates `user_dir_path`
  char *user_dir_path = create_user_dir_path(username);

  // direct container stores files
  // as `struct dirent *`
  struct dirent **container;
  // fills `container` and
  // returns file/dir count
  int count = list_dir(user_dir_path, &container);

  // loops through all files and
  // increments the total_files
  // by if !.usr file
  int total_files = 0;
  for (int i = 0; i < count; i++)
    if (container[i]->d_type == D_TYPE_FILE && strcmp(container[i]->d_name, ".usr") != 0)
      total_files++;

  // allocating array of strings length
  // with total_files
  char **result = malloc(total_files * sizeof(char *));

  // loops through valid files
  // increments active_file++
  int active_file = 0;
  for (int i = 0; i < count; i++)
    if (container[i]->d_type == D_TYPE_FILE && strcmp(container[i]->d_name, ".usr") != 0)
    {
      // buf_len of filename
      size_t buf_len = strlen(container[i]->d_name);
      // allocates strings memoring to array member
      result[active_file] = malloc((buf_len * sizeof(char)) + 1);
      // fills array member of string by filename
      strcpy(result[active_file], container[i]->d_name);
      // null terminator is a must
      result[active_file][buf_len] = '\0';
      // increments
      active_file++;
    }

  // updates *output object
  // sets ptr to `char **result`
  *output = result;

  // free vars from heap
  free(user_dir_path);
  free_dirent(&container, count);

  // lastly valid file count
  return total_files;
}

int generate_offset(char *username, char *password)
{
  /**
   * @brief dynamically assigning offset
   * number based on the username
   */

  // uses username
  int stock_number = 0;
  for (int i = 0; i < strlen(username); i++)
    if (username[i])
      stock_number = stock_number + username[i];

  // also uses password
  for (int j = 0; j < strlen(password); j++)
    if (password[j])
      stock_number = stock_number + password[j];

  // then makes up a unique offset
  int offset = stock_number * magic_number % 0x7f;
  return offset;
}

int crypt_user_file(
    char *username,
    char *password,
    char *input_path,
    char *filename,
    int do_encrypt)
{
  /**
   * @brief dynamically assigning offset
   * number based on the username
   */
  int ret;

  int offset = generate_offset(username, password);

  // creates `user_dir_path`
  char *user_dir_path = create_user_dir_path(username);
  char cipher_text_fp[512];
  sprintf(
      cipher_text_fp,
      "%s%c%s",
      user_dir_path,
      dir_sep,
      filename);

  // encrytion flag
  if (do_encrypt)
  {
    printf("\t%s > %s [%do]\n", input_path, cipher_text_fp, offset);
    ret = crypt_file(input_path, cipher_text_fp, offset);
  }
  else
  {
    // creates `user_dir_decrypted_files`
    char *user_dir_decrypted_files = create_user_decrypted_files_path(username);
    char user_dir_decipher_file[512];
    sprintf(
        user_dir_decipher_file,
        "%s%c%s",
        user_dir_decrypted_files,
        dir_sep,
        filename);

    // cleaning
    free(user_dir_decrypted_files);

    printf("\t%s > %s [%do]\n", cipher_text_fp, user_dir_decipher_file, -offset);
    // decryption
    ret = crypt_file(cipher_text_fp, user_dir_decipher_file, -offset);
  }

  // heap cleaning
  free(user_dir_path);

  // finally
  return ret;
}