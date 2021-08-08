#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define DATA_SIZE 256

char *read_file(const char *fileName)
{
    FILE *fp = fopen(fileName, "r");

    if (!fp)
    {
        perror("Error opening the file");
        return NULL;
    }

    // Get file size
    fseek(fp, 0L, SEEK_END);
    long int sz = ftell(fp);
    rewind(fp);

    char *buf = (char *)malloc(sz * sizeof(char));
    if (!buf)
    {
        perror("Failed to allocated memory");
        fclose(fp);
        return NULL;
    }

    if (fread(buf, 1, sz, fp) != sz)
    {
        perror("Error reading the file");
        free(buf);
        fclose(fp);
        return NULL;
    }

    fclose(fp);

    return buf;
}

float compute_frequency_std_dev(const char *text, unsigned int keyLength)
{
    size_t len = strlen(text);
    unsigned int tot = len / keyLength;
    float freq[DATA_SIZE];
    for (int i = 0; i < DATA_SIZE; ++i)
    {
        freq[i] = 0.0;
    }

    for (size_t i = keyLength; i < len; i += keyLength)
    {
        freq[(int)text[i]] += 1.0;
    }

    float res = 0;
    for (size_t i = 0; i < DATA_SIZE; ++i)
    {
        res += freq[i] * freq[i] / ((float)(tot * tot));
    }

    return res;
}

int ascii_to_hex(char c)
{
    int num = (int)c;
    if (num < 58 && num > 47)
        return num - 48;
    if (num < 103 && num > 96)
        return num - 87;
    if (num < 71 && num > 64)
        return num - 55;
    return num;
}

float get_score(unsigned char key, unsigned int keyIndex, unsigned int keyLength, const char *fileName)
{
    FILE *fp = fopen(fileName, "r");
    if (!fp)
    {
        perror("Failed to open file");
        return -1.0;
    }

    unsigned int i = 0;
    float score = 0.0;
    while (1)
    {
        unsigned char ch1, ch2;
        if (fscanf(fp, "%c", &ch1) == EOF || fscanf(fp, "%c", &ch2) == EOF)
            break;
        int c1 = ascii_to_hex(ch1);
        int c2 = ascii_to_hex(ch2);
        int ch = c1 << 4 | c2;
        if (i % keyLength == keyIndex)
        {
            int dec_ch = (int)(ch ^ key);
            int ok = 0;
            if (islower(dec_ch))
            {
                score += 1.0;
                ok = 1;
            }
            if (isupper(dec_ch))
            {
                score += 0.1;
                ok = 1;
            }
            if (isspace(dec_ch))
            {
                score += 0.7;
                ok = 1;
            }
            if (ok == 0 && dec_ch != ',' && dec_ch != '.')
            {
                fclose(fp);
                return -1.0;
            }
        }
        ++i;
    }

    fclose(fp);
    return score;
}

void decrypt_message(const char *fileName, unsigned char *keys, unsigned int keyLen)
{
    FILE *fp = fopen(fileName, "r");
    if (!fp)
    {
        perror("Failed to open file");
        return;
    }

    unsigned int i = 0;
    while (1)
    {
        unsigned char ch1, ch2;
        if (fscanf(fp, "%c", &ch1) == EOF || fscanf(fp, "%c", &ch2) == EOF)
            break;
        int c1 = ascii_to_hex(ch1);
        int c2 = ascii_to_hex(ch2);
        int ch = c1 << 4 | c2;
        int dec_ch = (int)(ch ^ keys[i % keyLen]);
        printf("%c", dec_ch);
        ++i;
    }

    fclose(fp);
}

int main()
{
    char *buf = read_file("ctext.txt");
    if (!buf)
    {
        perror("Failed to read file");
        return 1;
    }

    float std_dev[13];
    for (unsigned int i = 1; i <= 13; ++i)
    {
        std_dev[i - 1] = compute_frequency_std_dev(buf, i);
    }

    float max_value = 0.0;
    unsigned int max_index = 0;
    for (unsigned int i = 1; i <= 13; ++i)
    {
        if (std_dev[i - 1] > max_value)
        {
            max_value = std_dev[i - 1];
            max_index = i;
        }
    }

    printf("Most likely key size is: %u\n", max_index);
    free(buf);

    // Try all keys
    unsigned char* keys = malloc(max_index*sizeof(unsigned char));
    if (!keys)
        return 1;
    for (unsigned int i = 0; i < max_index; ++i)
    {
        float max_score = -2.0;
        unsigned int max_key = 0;
        for (unsigned int key = 0; key < DATA_SIZE; ++key)
        {
            float score = get_score((unsigned char)key, i, max_index, "ctext.txt");
            if (score > max_score)
            {
                max_score = score;
                max_key = key;
            }
        }

        printf("key position: %u, and of value: %u, score: %f\n", i, max_key, max_score);
        keys[i] = max_key;
    }

    decrypt_message("ctext.txt", keys, max_index);
}