set -xe
gcc -Wall -Wextra -Wconversion -g -o mathlang main.c cvecs.c -I./ -lm
