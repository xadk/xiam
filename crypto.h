#ifndef CRYPTO_H
#define CRYPTO_H

// encrypts/decrypts file (to/from) using +-(offset)
int crypt_file(char *input_file, char *output_file, int offset);

// encrypts/decrypts string (to/from) using +-(offset)
char *crypt_str(char *input_str, int offset);

#endif