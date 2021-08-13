/*
   Reads message in ascii (from msg.txt) and outputs ciphertext in hex (to ctext.txt)

   Key is provided in hex (in key.txt)

   Assumes plaintext has length an integer number of bytes,
   with padding implemented as described in class
*/

#include "aes.h"
#include <stdio.h>

int main()
{
  unsigned char MBlock[16];
  unsigned char CBlock_prev[16];
  unsigned char CBlock_cur[16];
  unsigned char Key[16];
  unsigned char next;
  AES_KEY AESkey;

  FILE *fpIn = fopen("msg.txt", "r");
  FILE *fpOut = fopen("ctext.txt", "w");
  FILE *fpKey = fopen("key.txt", "r");
  FILE *randfile = fopen("/dev/random", "r");

  for (int i = 0; i <= 15; i++)
  {
    fscanf(fpKey, "%2X", (unsigned int*)&Key[i]);
  }

  fclose(fpKey);

  AES_set_encrypt_key((const unsigned char *)Key, 128, &AESkey);

  // Generate a random IV
  for (int i = 0; i < 16; i++)
  {
    fscanf(randfile, "%c", &next);
    CBlock_prev[i] = next;
  }

  // write the IV as the first block of the ciphertext
  int i;
  for (i = 0; i < 16; i++)
  {
    fprintf(fpOut, "%X", CBlock_prev[i] / 16), fprintf(fpOut, "%X", CBlock_prev[i] % 16);
  }

  while (1)
  {
    // read up to a full block of the message
    for (i = 0; i < 16; i++)
    {
      if (fscanf(fpIn, "%c", &MBlock[i]) == EOF)
        break;
    }

    if (i == 0)
      break;

    // if less than a full block is read, append appropriate padding
    if (i < 16)
    {
      for (int j = i; j < 16; j++)
      {
        MBlock[j] = 16 - i;
      }
    }

    // XOR the message block with the previous ciphertext block...
    for (int j = 0; j < 16; j++)
    {
      MBlock[j] ^= CBlock_prev[j];
    }

    // ...and evaluate AES on the result
    AES_encrypt((const unsigned char *)MBlock, CBlock_cur, (const AES_KEY *)&AESkey);

    // write the result as the next block of the ciphertext
    for (int j = 0; j < 16; j++)
    {
      fprintf(fpOut, "%X", CBlock_cur[j] / 16), fprintf(fpOut, "%X", CBlock_cur[j] % 16);
      CBlock_prev[j] = CBlock_cur[j];
    }

    if (i < 16)
      break;
  }

  // if the message length is a multiple of the block length, we still need to add a full block of padding,
  // XOR this with the previous ciphertext block, evaluate AES on the result,
  // and write the result as the final block of the ciphertext
  if (i == 0)
  {
    for (int j = 0; j < 16; j++)
    {
      MBlock[j] = 16;
      MBlock[j] ^= CBlock_prev[j];
    }

    AES_encrypt((const unsigned char *)MBlock, CBlock_cur, (const AES_KEY *)&AESkey);

    for (int j = 0; j < 16; j++)
    {
      fprintf(fpOut, "%X", CBlock_cur[j] / 16), fprintf(fpOut, "%X", CBlock_cur[j] % 16);
      //CBlock_prev[j] = CBlock_cur[j];
    }
  }

  fclose(fpOut), fclose(fpIn);
}
