#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "user.c"
#include "crypto.c"
#include "utils.c"

const unsigned int logo[256] = {10, 32, 10, 9, 95, 95, 32, 32, 32, 95, 95, 95, 95, 95, 95, 95, 32, 32, 95, 95, 95, 32, 32, 95, 95, 95, 32, 32, 95, 95, 95, 10, 9, 92, 32, 92, 32, 47, 32, 47, 95, 32, 32, 32, 95, 124, 47, 32, 95, 32, 92, 32, 124, 32, 32, 92, 47, 32, 32, 124, 10, 9, 32, 92, 32, 86, 32, 47, 32, 32, 124, 32, 124, 32, 47, 32, 47, 95, 92, 32, 92, 124, 32, 46, 32, 32, 46, 32, 124, 10, 9, 32, 47, 32, 32, 32, 92, 32, 32, 124, 32, 124, 32, 124, 32, 32, 95, 32, 32, 124, 124, 32, 124, 92, 47, 124, 32, 124, 10, 9, 47, 32, 47, 94, 92, 32, 92, 95, 124, 32, 124, 95, 124, 32, 124, 32, 124, 32, 124, 124, 32, 124, 32, 32, 124, 32, 124, 10, 9, 92, 47, 32, 32, 32, 92, 47, 92, 95, 95, 95, 47, 92, 95, 124, 32, 124, 95, 47, 92, 95, 124, 32, 32, 124, 95, 47, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10};

// flushes anything from stdin
// until its EOF || \n
void flush_stdin_until_newline()
{
  char ch;
  while (ch = getchar() != '\n' && ch != EOF)
    continue;
}

// polls and serves menus
// returns any %d choice
int poll_menu(char **menu_items, size_t total_menu_items)
{
  int choice;
  // loops thru whilst prints
  for (int i = 0; i < total_menu_items; i++)
    printf("\tPress  %d to %s\n", i + 1, menu_items[i]);
  printf("\tPress -1 to Exit\n");
  printf("\n> ");

  // scans %d
  scanf(" %d", &choice);

  // flushes \n
  flush_stdin_until_newline();

  // returns choice %d
  return choice;
}

// polls and let user
// select one of its
// pre-encrypted files
// and puts **output
int poll_user_files(char *username, char **output)
{
  int choice;
  char **user_files;

  // calls and saves count
  // puts **user_files
  int count = list_all_user_files(username, &user_files);

  // if no files in user_dir
  if (count < 1)
  {
    printf("\n\t0 files found\n\n");
    return -1;
  }

  // as long as choice is
  // not b/w 1-count, runs
  while (choice < 1 || choice > count)
  {
    // loops thru the options
    for (int i = 0; i < count; i++)
      printf("\tPress  %d select %s\n", i + 1, user_files[i]);

    printf("\tPress -1 to Exit\n");
    printf("\n(%d-%d) > ", 1, count);
    scanf(" %d", &choice);

    // flushes \n
    flush_stdin_until_newline();

    // killswitch
    if (choice == -1)
      break;
  }

  // copies to output
  // the subsequent index
  if (choice > 0 && choice <= count)
    strcpy(*output, user_files[choice - 1]);

  // stay clean
  for (int i = 0; i < count; i++)
    free(user_files[i]);
  free(user_files);
  return choice;
}

// serves submenu for
// logged-in user
int handle_user_menu(char *username, char *password)
{
  // list of actions
  char user_menu[][64] = {
      "List files",
      "Encrypt file",
      "Decrypt file",
      "Show file stat",
      "Update password",
      "Return to menu"};

  // just creates pointer with appropriate
  // size, explained in handle_main_menu already
  size_t total_menu_items = sizeof(user_menu) / sizeof(user_menu[0]);
  char **menu_items = malloc(total_menu_items * sizeof(char *));
  for (int i = 0; i < total_menu_items; i++)
  {
    menu_items[i] = malloc(64 * sizeof(char));
    strcpy(menu_items[i], user_menu[i]);
  }
  // check @handle_main_menu ^

  // extra padding-bottom
  printf("\n\n");

  int ret_code = 0;

  while (1)
  {
    // loops thru polling and
    // returns any %d choice
    int choice = poll_menu(menu_items, total_menu_items);

    // killswitch
    if (choice == -1)
    {
      ret_code = -1;
      goto exit_user_menu;
    }

    // as long as choice is valid
    // and b/w a valid range
    if (choice > 0 && choice <= total_menu_items)
    {
      switch (choice)
      {
        // lists all user files (pre-encrypted)
      case 1:
      {
        // calls user listing function and
        // prints number of files
        char **user_files;
        int count = list_all_user_files(username, &user_files);
        if (count < 1)
          printf("\t0 files found\n\n");

        else
        {
          // and displays in a proper
          // table format ranging a-Z
          // of filenames
          printf("\t%d files found\n\n", count);
          printf("\t%-20s %-20s\n", "Index", "Filename");
          for (int i = 0; i < count; i++)
            printf("\t%-20d %-20s\n", i, user_files[i]);
          printf("\n\n");
        }

        // cleanliness is next to godliness
        for (int i = 0; i < count; i++)
          free(user_files[i]);
        free(user_files);
        break;
      }

      // encrypts any text file
      // from path to :
      // system_user/{user}/{alias}
      case 2:
      {
        // takes inputs
        char input_fp[64];
        char alias[64];

        // gets path
        printf("\n\tPath: ");
        scanf(" %64s", input_fp);

        // flushes \n
        flush_stdin_until_newline();

        // gets alias
        printf("\tAlias: ");
        scanf(" %64s", alias);

        // flushes \n
        flush_stdin_until_newline();

        printf("\n\tencrypting...\n");

        // asserts input file
        // must exist before it attempts
        // to encrypt
        if (path_exists(input_fp) != 0)
        {
          printf("\n\t[err] cannot find file: %s\n\n\n", input_fp);
          continue;
        }

        // calls to user's function
        // to encrypt file
        int ret = crypt_user_file(
            username,
            password,
            input_fp,
            alias,
            1);

        if (ret != 0)
        {
          printf("\n\t[err] failed to encrypt this file\n\n\n");
          continue;
        }
        else
          printf("\n\t[done ✓] saved encrypted file: %s\n\n\n", alias);

        break;
      }

      // decrypt pre-encrypted
      // text files to:
      // system_user/{user}/decrypted_files/{alias}
      case 3:
      {
        // polls thru all pre-encrypted
        // files and lets user choose one
        // (a valid choice ofc) || -1
        char *choosen_file_name = malloc(512 * sizeof(char));
        int choice = poll_user_files(username, &choosen_file_name);

        // killswitch
        if (choice == -1)
        {
          printf("\n\n");
          // a must cleanup
          free(choosen_file_name);
          continue;
        }

        printf("\n\tdecrypting...\n");

        // calls to user's function
        // to decrypt file
        // doesn't require input_path
        // argument
        int ret = crypt_user_file(
            username,
            password,
            "",
            choosen_file_name,
            0);

        if (ret != 0)
        {
          printf("\n\t[err] failed to decrypt this file\n\n\n");
          // a followup cleanup
          free(choosen_file_name);
          continue;
        }

        else
          printf("\n\t[done ✓] saved decrypted file: %s\n\n\n", choosen_file_name);

        // a must:
        // a fallback cleanup
        free(choosen_file_name);

        break;
      }

      // polls user to select a file
      // and then prints a pretty table
      // with {struct stat} details of
      // that file
      case 4:
      {
        // polls user and let them choose a file
        // (a valid choice %d ofc) || -1
        char *choosen_file_name = malloc(512 * sizeof(char));
        int choice = poll_user_files(username, &choosen_file_name);

        // entertains a clean exit
        if (choice < 0)
          continue;

        // creates `user_dir_path`
        char *user_dir_path = create_user_dir_path(username);

        // creates cipher text file path
        char cipher_text_fp[512];
        sprintf(
            cipher_text_fp,
            "%s%c%s",
            user_dir_path,
            dir_sep,
            choosen_file_name);

        // please:
        // always clean on time
        free(user_dir_path);

        // struct object to store
        // the stat results, throws
        // err if files doesn't exit
        struct stat st = {0};
        if (stat(cipher_text_fp, &st) != 0)
        {
          printf("\n\tthe file [%s] no longer exists\n", cipher_text_fp);
          continue;
        }

        // time objects to store
        // creation and modification
        // times of a file
        struct tm mt, ct;

        ct = *(gmtime(&st.st_ctime));
        mt = *(gmtime(&st.st_mtime));

        // creation and modication time
        // but formatted
        char ctf[64], mtf[64];

        // creation time formatted
        sprintf(
            ctf,
            "%d:%d %d/%d/%d",
            ct.tm_hour,
            ct.tm_min,
            ct.tm_mon + 1,
            ct.tm_mday,
            (ct.tm_year + 1900) - 2000);

        // modification time formatted
        sprintf(
            mtf,
            "%d:%d %d/%d/%d",
            mt.tm_hour,
            mt.tm_min,
            mt.tm_mon + 1,
            mt.tm_mday,
            (mt.tm_year + 1900) - 2000);

        // prints table header
        printf(
            "\n%-20s %-30s %-17s %-17s %-10s\n",
            "Filename (alias)",
            "Cipher path",
            "Creation",
            "Modification",
            "Size (bytes)");

        // prints table body
        printf(
            "%-20s %-30s %-17s %-17s %-10ld\n\n\n",
            choosen_file_name,
            cipher_text_fp,
            ctf,
            mtf,
            st.st_size);

        // killswitch
        if (choice == -1)
        {
          printf("\n\n");
          // a must cleanup
          free(choosen_file_name);
          continue;
        }

        // a fallback cleanup
        free(choosen_file_name);

        break;
      }

      // lets users to change their
      // passwordsm, prompts user
      // old password, assets; then
      // updates new password before
      // redirects to same submenu
      case 5:
      {
        char old_password[64];
        char new_password[64];

        // gets old password
        printf("\n\tOld Password: ");
        scanf(" %64s", old_password);

        // flushes until \n
        flush_stdin_until_newline();

        // gets new password
        printf("\tNew Password: ");
        scanf(" %64s", new_password);

        // flushes until \n
        flush_stdin_until_newline();

        // if ! assert then flush err
        if (strcmp(old_password, password) == 0)
        {

          // if all OK, overwrites .usr file
          // sitting in:
          // system_users/{user}/.usr
          int ret = create_user(username, new_password, 1);
          if (ret != 0)
            printf("\n\t[err] failed to update password\n\n\n");
          printf("\n\t[done ✓] updated password\n\n\n");
        }
        // fallback err printing
        else
          printf("\n\t[err] invalid old password\n\n\n");

        break;
      }

      // just returns back
      // to the main menu
      case 6:
        ret_code = 0;
        goto exit_user_menu;
        break;

      default:
        continue;
      }
    }
  }

exit_user_menu:
  // freeing heap
  for (int i = 0; i < total_menu_items; i++)
  {
    free(menu_items[i]);
  }
  free(menu_items);
  return ret_code;
}

// action for handling
// login and signups
int user_action(int login_only)
{
  // requires input for
  // username and password
  // since both mandatory
  // for signin + signup
  char username[64];
  char password[64];

  // gets username
  printf("\n\tUsername: ");
  scanf(" %64s", username);

  // flushes \n
  flush_stdin_until_newline();

  // gets password
  printf("\tPassword: ");
  scanf(" %64s", password);

  // flush last \n
  flush_stdin_until_newline();

  int ret;

  // signin
  if (login_only)
  {
    // if user exists then
    // followup with submenu
    if ((ret = assert_user(username, password)) == 0)
      return handle_user_menu(username, password);

    // otherise err
    printf("\n\t[err] invalid username or password\n\n");
  }

  // signup
  // if creates user
  // then followup with submenu
  else if ((ret = create_user(username, password, 0)) == 0)
    return handle_user_menu(username, password);

  return 1;
}

// serves main menu
// for entry point
int handle_main_menu()
{
  // list of menu items
  // each string is 64 bytes
  char main_menu[][64] = {
      "Sign In",
      "Sign Up",
      "List Users",
  };

  // calculates total number
  // of main menu items
  size_t total_menu_items = sizeof(main_menu) / sizeof(main_menu[0]);

  // allocates nth length of
  // main menu array into
  // the heap memory
  char **menu_items = malloc(total_menu_items * sizeof(char *));
  // loops over each array
  // member to allocates char*
  // to it
  for (int i = 0; i < total_menu_items; i++)
  {
    // allocates each string
    // of 64 bytes to each
    // array member and copies
    // string content into it
    menu_items[i] = malloc(64 * sizeof(char));
    strcpy(menu_items[i], main_menu[i]);
  }

  // inifinite loop until
  // returns -1
  while (1)
  {
    // polls main menu items
    // & awaits user input
    // returns any %d choice
    int choice = poll_menu(menu_items, total_menu_items);

    // kill switch
    if (choice == -1)
      goto exit_main_menu;

    // as long as choice
    // is valid and inbounds
    // possible choice range
    if (choice > 0 && choice <= total_menu_items)
    {
      switch (choice)
      {
      case 1:
        // call user action
        // with signal 'login'
        if (user_action(1) == -1)
          goto exit_main_menu;
        break;

      case 2:
        // call user action
        // with signal 'signup'
        if (user_action(0) == -1)
          goto exit_main_menu;
        break;

      case 3:
      {
        // lists system_dir
        // and prints all dirs

        // container to receive direct
        // from list_dir
        struct dirent **container;
        int count = list_dir(root_user_dir, &container);
        int valid_dirs_idx = 0;
        if (count > 0)
        {
          printf("\tActive users:\n");
          for (int i = 0; i < count; i++)
          {
            // checks if a dir (d_type=D_TYPE_DIR)
            // makes sure no '.' || '..'
            if (container[i]->d_type == D_TYPE_DIR &&
                !strcmp(container[i]->d_name, ".") == 0 &&
                !strcmp(container[i]->d_name, "..") == 0)
              // then prints each user
              // properly to stdout
              printf("\t[%d] %s\n", valid_dirs_idx + 1, container[i]->d_name) &&
                  valid_dirs_idx++;
          }
          printf("\n\n");
        }

        else
        {
          // fallback if count < 0
          // which means list_dir dint
          // run properly
          printf("\n\t[err] failed to list users\n");
        }
        // makes sure to
        // dispose after use
        free_dirent(&container, count);
        break;
      }

      default:
        continue;
      }
    }
  }

  /**
   * garbage collection
   */

exit_main_menu:
  for (int i = 0; i < total_menu_items; i++)
  {
    free(menu_items[i]);
  }
  free(menu_items);

  return 0;
}

int main(void)
{
  // logo
  for (int i = 0; i < sizeof(logo) / sizeof(logo[0]); i++)
    printf("%c", logo[i]);
  printf("\n\n");

  /**
   * @brief makes system_users directory
   * if not exists, it creates;
   */
  int ret;
  ret = mkdir_p(root_user_dir);
  if (ret != 0)
  {
    printf("cannot create dir: %s\n", root_user_dir);
    perror("error, program exits");
    return ret;
  }

  /**
   * Entry point to main menu interface
   */
  handle_main_menu();

  return 0;
}