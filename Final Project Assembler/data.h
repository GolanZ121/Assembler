#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define Min(x, y) (((x) < (y)) ? (x) : (y))/* A macro that returns the smaller number between two given numbers */
#define MAX_FILENAME_LENGTH 150/* The longest length of a name of a given file */
#define ROW_LENGTH 80/* The longest length of a line in a given file */
#define MAX_FILE_LENGTH 1024/* The biggest size of memory - instructionArray and dataArray */
#define FIRST_SIX 63/* Used in extract in convert. 63 = 000000111111(binary) */
#define INIT_MEM_POS 100/*the initial position of encoded instructions in memory */
#define ASCII_SUB 48/* 48(ascii) = 0(int). when subtracting 48 from a integer in ascii we get the actual integer */
#define MAX_BITS 2048/* A number cannot be longer than 11 bits = 2^11 = 2048 */
#define MAX_LABEL_LENGTH 31/* The biggest length of a name of a label */

enum {
    INTEGER_ADDRESSING = 1,
    LABEL_ADDRESSING = 3,
    REGISTER_ADDRESSING = 5
};
