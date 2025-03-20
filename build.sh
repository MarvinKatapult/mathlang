set -xe
gcc -Wall -Wextra -Wconversion -g -o simple_language main.c cvecs.c -I./ -lm
