#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MAX_RED 12
#define MAX_GREEN 13
#define MAX_BLUE 14

size_t get_game_id(char *line, long int *id)
{
    /* 0 1 2 3 4 5 ... ret
     * G a m e _        :
     */
    size_t index = 5;
    while (line[index] != ':' && line[index] != '\0') {
        ++index;
    }
    size_t buf_size = index - 5;
    char buf[buf_size+1];
    buf[buf_size] = '\0';
    for (size_t i = 0; i < buf_size; ++i) {
        buf[i] = line[i + 5];
    }

    *id = atol(buf);
    return index;
}

size_t get_number(char *input, long int *num)
{
    size_t index = 0;
    
    /* Trim the beginning until we have a digit */
    while ('\0' != input[index] && 0 == isdigit(input[index])) {
        ++index;
    }
    if ('\0' == input[index]) {
        return 0;
    }

    size_t beginning = index;
    while (0 != isdigit(input[index])) {
        ++index;
    }
    size_t buf_size = index - beginning;
    char buf[buf_size+1];
    buf[buf_size] = '\0';
    for (size_t i = 0; i < buf_size; ++i) {
        buf[i] = input[beginning + i];
    }

    *num = atol(buf);
    return index;
}

char process_input(char *input, long int *reds, long int *greens, 
        long int *blues)
{
    char good_game = 1;
    size_t index = 0;
    while ('\0' != input[index]) {
        long int num = 0;
        size_t offset = get_number(input+index, &num);
        if (0 == offset) {
            /* Could not find any remaining number, game is over and safe */
            return good_game;
        }
        index += offset+1;
        
        switch (input[index]) {
            /* Let's suppose the input is sound, right? */
            case 'r':
                if (MAX_RED < num) {
                    good_game = 0;
                }
                if (num > *reds) {
                    *reds = num;
                }
                index += 3;
                break;
            case 'g':
                if (MAX_GREEN < num) {
                    good_game = 0;
                }
                if (num > *greens) {
                    *greens = num;
                }
                index += 5;
                break;
            case 'b':
                if (MAX_BLUE < num) {
                    good_game = 0;
                }
                if (num > *blues) {
                    *blues = num;
                }
                index += 4;
                break;
            default:
                ++index;
        }
    }

    return good_game;
}

/*
 *  Returns game's ID if it is valid
 */
void parse_game(char *line, long int *id, long int *power)
{
    size_t offset = get_game_id(line, id);

    char *input = line + offset;
    long int reds = LONG_MIN;
    long int greens = LONG_MIN;
    long int blues = LONG_MIN;

    char res = process_input(input, &reds, &greens, &blues);
    if (0 == res) {
        //fprintf(stderr, "Incorrect game %ld\n", *id);
        *id = 0;
    }
    *power = reds * greens * blues;

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
    long int total = 0;
    long int total_power = 0;
    while (-1 != getline(&line, &line_len, file)) {
        long int id = 0;
        long int power = 0;
        parse_game(line, &id, &power);
        total += id;
        total_power += power;
    }

    if (-1 == fclose(file)) {
        fprintf(stderr, "Could not close file input: %s\n", strerror(errno));
    }
    fprintf(stdout, "Done, total=%ld, power=%ld\n", total, total_power);
    return 0;
}
