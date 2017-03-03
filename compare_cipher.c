#include <stdio.h>
#include <stdlib.h>

typedef int bool;
#define true 1
#define false 0

void printText(FILE *src, const int CHAR_COUNT_PER_LINE)
{
    rewind(src);
    int temp = 0;
    int char_count = 0;
    while (1)
    {
        temp = fgetc(src);
        if (temp == EOF) break;
        printf("%c", temp);
        if (++char_count % CHAR_COUNT_PER_LINE == 0) printf("\n");
    }
    printf("\n\n");
    rewind(src);
    return;
}

int main(int argc, const char *argv[])
{
    char src1fname[256];
    char src2fname[256];
    if (argc != 3)
    {
        fprintf(stderr, "\n\nUsage: %s sourcefile targetfile\n\n",
                argv[0]);
        exit(0);
    }
    // filename input check
    sprintf(src1fname, "%s", argv[1]);
    FILE *src1 = fopen(src1fname, "r");
    if (src1 == NULL) {
        fprintf(stderr, "\n\nError opening file %s\nTry again\n\n", src1fname);
        exit(0);
    }

    // filename output check
    sprintf(src2fname, "%s", argv[2]);
    FILE *src2 = fopen(src2fname, "r");
    if (src2 == NULL) {
        fprintf(stderr, "\n\nError opening file %s\nTry again\n\n", src2fname);
        exit(0);
    }

    int temp_src1 = 0;
    int temp_src2 = 0;
    bool is_same_cipher = true;
    int char_count = 0;
    const int CHAR_COUNT_PER_LINE = 64;
    printf("\n");
    printText(src1, CHAR_COUNT_PER_LINE);

    char_count = 0;
    while (1)
    {
        temp_src1 = fgetc(src1);
        temp_src2 = fgetc(src2);
        if (temp_src1 == EOF || temp_src2 == EOF) break;
        if (temp_src1 != temp_src2)
        {
            printf("_");
            is_same_cipher = false;
        }
        else
        {
            printf("%c", temp_src1);
        }
        if (++char_count % CHAR_COUNT_PER_LINE == 0) printf("\n");
    }
    printf("\n\n");
    printText(src2, CHAR_COUNT_PER_LINE);

    if (is_same_cipher) fprintf(stderr, "\nCorrect!!\n\n\n");
    else fprintf(stderr, "\nThat's too bad...\n\n\n");

    return 0;
}
