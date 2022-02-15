/*
 * morseTranslator.h
 *
 * ECE 315 : Computer Interfacing
 * Part-1 LAB 2
 *
 *  Created on: Jul 26, 2021
 *      Author: Mazen Elbaz, Shyama Gandhi
 */

#ifndef SRC_MORSETRANSLATOR_H_
#define SRC_MORSETRANSLATOR_H_

/*
References:
https://upload.wikimedia.org/wikipedia/commons/b/b5/International_Morse_Code.svg
https://www.electronics-notes.com/articles/ham_radio/morse_code/characters-table-chart.php
A   .-
B   -...
C   -.-.
D   -..
E   .
F   ..-.
G   --.
H   ....
I   ..
J   .---
K   -.-
L   .-..
M   --
N   -.
O   ---
P   .--.
Q   --.-
R   .-.
S   ...
T   -
U   ..-
V   ...-
W   .--
X   -..-
Y   -.--
Z   --..
1   .----
2   ..---
3   ...--
4   ....-
5   .....
6   -....
7   --...
8   ---..
9   ----.
0   -----
Comma --..--
Period .-.-.-
Question mark ..--..
@ symbol .--.-.
Colon ---...
Semicolon -.-.-.
Dash -....-
Apostrophe .----.
Unknown character can be represented by * (starlike symbol)


-> A space will be kept as a space
-> $ sign will be considered as a new line
-> Characters will be separated by | symbol

Example:
            .--|.|.-..|-.-.|---|--|.| .|...-|.|.-.|-.--|---|-.|.|.-.-.-|

            WELCOME EVERYONE.

*/

extern char char_morse_sequence[10];
extern char output_text_sequence[500];
extern int output_length;
extern int char_seq_length;

#define strMatch(str) strcmp(char_morse_sequence, str) == 0

#define addChar(ch) output_text_sequence[output_length++] = ch;

void morseToTextConverter(char morse_char) {
    /* clang-format off */
    if (morse_char == '.' || morse_char == '-') 
        char_morse_sequence[char_seq_length++] = morse_char;

    // a space will be kept the same
    else if (morse_char == ' ') addChar(' ');
    // $ sign entered by the user will indicate a new line
    else if (morse_char == '\r') addChar('\r');
    // if a character delimiter is detected
    else if (morse_char == '|') {
        if (strMatch(".-")) addChar('A'); // letter A
        else if (strMatch("-...")) addChar('B'); // letter B
        else if (strMatch("-.-.")) addChar('C'); // letter C
        else if (strMatch("-..")) addChar('D'); // letter D
        else if (strMatch(".")) addChar('E'); // letter E
        else if (strMatch("..-.")) addChar('F'); // letter F
        else if (strMatch("--.")) addChar('G'); // letter G
        else if (strMatch("....")) addChar('H'); // letter H
        else if (strMatch("..")) addChar('I'); // letter I
        else if (strMatch(".---")) addChar('J'); // letter J
        else if (strMatch("-.-")) addChar('K'); // letter K
        else if (strMatch(".-..")) addChar('L'); // letter L
        else if (strMatch("--")) addChar('M'); // letter M
        else if (strMatch("-.")) addChar('N'); // letter N
        else if (strMatch("---")) addChar('O'); // letter O
        else if (strMatch(".--.")) addChar('P'); // letter P
        else if (strMatch("--.-")) addChar('Q'); // letter Q
        else if (strMatch(".-.")) addChar('R'); // letter R
        else if (strMatch("...")) addChar('S'); // letter S
        else if (strMatch("-")) addChar('T'); // letter T
        else if (strMatch("..-")) addChar('U'); // letter U
        else if (strMatch("...-")) addChar('V'); // letter V
        else if (strMatch(".--")) addChar('W'); // letter W
        else if (strMatch("-..-")) addChar('X'); // letter X
        else if (strMatch("-.--")) addChar('Y'); // letter Y
        else if (strMatch("--..")) addChar('Z'); // letter Z
        else if (strMatch(".----")) addChar('1'); // number 1
        else if (strMatch("..---")) addChar('2'); // number 2
        else if (strMatch("...--")) addChar('3'); // number 3
        else if (strMatch("....-")) addChar('4'); // number 4
        else if (strMatch(".....")) addChar('5'); // number 5
        else if (strMatch("-....")) addChar('6'); // number 6
        else if (strMatch("--...")) addChar('7'); // number 7
        else if (strMatch("---..")) addChar('8'); // number 8
        else if (strMatch("----.")) addChar('9'); // number 9
        else if (strMatch("-----")) addChar('0'); // number 0
        else if (strMatch("--..--")) addChar(','); // comma ',"
        else if (strMatch(".-.-.-")) addChar('.'); // period '.'
        else if (strMatch("..--..")) addChar('?'); // question mark '?'
        else if (strMatch(".--.-.")) addChar('@'); // @ symbol
        else if (strMatch("---...")) addChar(':'); // colon :
        else if (strMatch("-.-.-.")) addChar(';'); // semicolon ;
        else if (strMatch("-....-")) addChar('-'); // dash -
        else if (strMatch(".----.")) addChar('\''); // apostrophe '
        else addChar('*'); // unknown character is represented by *
        /* clang-format on */

        char_seq_length = 0;
        memset(char_morse_sequence, 0, 10);
    }
}

#endif /* SRC_MORSETRANSLATOR_H_ */
