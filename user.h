#ifndef USER_H
#define USER_H

unsigned char root_user_dir[] = "system_users";
const unsigned char dir_sep = '/';
const unsigned int magic_number = 0x5f;
const unsigned int user_pwd_key = 0x7f;
const unsigned char str_decrypted_dir_name[] = "decrypted_files";

// creates path for
// user data dir
char *create_user_dir_path(char *username);

// creates path for user
// crendentials dir
char *create_user_credentials_path(char *username);

// creates path for user's
// decrypted_files dir
char *create_user_decrypted_files_path(char *username);

// creates a new user
int create_user(char *username, char *password, int overwrite);

// confirms user credentials
int assert_user(char *username, char *password);

// lists all users
// fills up output ptr
int list_all_user_files(char *username, char ***output);

// generates random offset using
// username, password & magic number
int generate_offset(char *username, char *password);

// encrypts/decrypts
// using ascii char
// swaps, swicthes
// by do_encrypt
int crypt_user_file(
    char *username,
    char *password,
    char *input_path,
    char *filename,
    int do_encrypt);

#endif