#include <stdio.h>
#include <stdlib.h>

static int getPalindrome(int number) {
    int value = 0;
    while (number > 0) {
        value = value * 10 + (number % 10);
        number /= 10;
    }
    return value;
}

int main(int argc, const char *argv[]) {
    int num = 1;
    if (argc == 2) {
        num = atoi(argv[1]);
    }
    printf("%d, palindrome: %d\n", num, getPalindrome(num));
}