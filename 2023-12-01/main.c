#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>


char *numbers[9] = {
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
};

size_t buf_len = 5;
char buf[6];

int str_is_number(char *buf)
{
    for (unsigned i = 0; i < 9; ++i) {
        if (strstr(buf, numbers[i])) {
            return i + 1;
        }
    }
    return -1;
}

size_t get_first_number(char *line, int *res)
{
    size_t index = 0;
    char c[2];
    c[1] = '\0';
    while ('\n' != line[index] && '\0' != line[index]) {
        c[0] = line[index];

        if (index < buf_len) {
            for (unsigned i = 0; i <= index; ++i) {
                buf[i] = line[i];
            }
        } else {
            for (unsigned i = buf_len; i > 0; --i) {
                unsigned write_position = buf_len - i;
                unsigned read_position = index - i + 1;

                buf[write_position] = line[read_position];
            }
        }

        int num = str_is_number(buf);
        if (num > 0) {
            *res = num;
            break;
        }

        if (isdigit(c[0])) {
            num = atoi(c);
            *res = num;
            break;
        }
        ++index;
    }
    return index;
}

size_t get_second_number(char *line, size_t len, int *res)
{
    size_t index = len;
    char c[2];
    int num = -1;
    c[1] = '\0';
    while (0 != index) {
        c[0] = line[index];

        if (isdigit(c[0])) {
            num = atoi(c);
            *res = num;
            return index;
        }

        for (unsigned i = 0; i < buf_len; ++i) {
            unsigned write_position = i;
            unsigned read_position = index + i;

            if (read_position > len) {
                buf[write_position] = ' ';
            } else {
                buf[write_position] = line[read_position];
            }
        }

        num = str_is_number(buf);
        if (num > 0) {
            *res = num;
            return index;
        }

        --index;
    }

    c[0] = line[0];
    if (isdigit(c[0])) {
        num = atoi(c);
        *res = num;
    }

    return index;
}


int line_value(char *line, size_t alloc_len)
{
    size_t line_len = strnlen(line, alloc_len);
    size_t index = 0;
    int first = INT_MIN;
    int second = INT_MIN;

    /* Reset buf */
    for (size_t i = 0; i < buf_len; ++i) {
        buf[i] = '\0';
    }
    buf[buf_len+1]='\0';

    index = get_first_number(line, &first);
    /*
     *  If no numbers were found in a line, then skip the second step.
     *  Likewise, if the number was only found in the last position, then
     *  we can skip the second step.
     */
    if (index > line_len) {
        return 0;
    } else if (index == line_len) {
        return first*1e1 + first;
    }

    /* Reset buf */
    for (size_t i = 0; i < buf_len; ++i) {
        buf[i] = '\0';
    }

    get_second_number(line, line_len, &second);

    return first*1e1 + second;
}

int main()
{
    FILE *file;
    file = fopen("input", "r");
    if (NULL == file) {
        fprintf(stderr, "Could not open file input: %s\n", strerror(errno));
        return -1;
    }

    char* line;
    size_t line_len = 0;
    int64_t count = 0;
    while (-1 != getline(&line, &line_len, file)) {
        int value = line_value(line, line_len);
        if (0 > value) {
            fprintf(stderr, "Found line with no number, skipping ...\n");
            continue;
        }
        count += value;
    }

    if (-1 == fclose(file)) {
        fprintf(stderr, "Could not close file input: %s\n", strerror(errno));
    }
    fprintf(stdout, "Done, %lu\n", count);
    return 0;
}
