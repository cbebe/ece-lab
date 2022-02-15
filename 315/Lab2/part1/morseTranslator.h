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


void morseToTextConverter(char morse_char){

	if(morse_char == '.' || morse_char == '-'){
		char_morse_sequence[char_seq_length++] = morse_char;
	}

	//a space will be kept the same
	else if(morse_char == ' '){
		output_text_sequence[output_length++] = ' ';
	}

	//$ sign entered by the user will indicate a new line
	else if(morse_char == '\r'){
		output_text_sequence[output_length++] = '\r';
	}

	//if a character delimiter is detected
	else if(morse_char == '|' ){

		//letter A
		if(strcmp(char_morse_sequence, ".-") == 0){
			output_text_sequence[output_length++] = 'A';
		}

		//letter B
		else if(strcmp(char_morse_sequence, "-...") == 0){
			output_text_sequence[output_length++] = 'B';
		}

		//letter C
		else if(strcmp(char_morse_sequence, "-.-.") == 0){
			output_text_sequence[output_length++] = 'C';
		}

		//letter D
		else if(strcmp(char_morse_sequence, "-..") == 0){
			output_text_sequence[output_length++] = 'D';
		}

		//letter E
		else if(strcmp(char_morse_sequence, ".") == 0){
			output_text_sequence[output_length++] = 'E';
		}

		//letter F
		else if(strcmp(char_morse_sequence, "..-.") == 0){
			output_text_sequence[output_length++] = 'F';
		}

		//letter G
		else if(strcmp(char_morse_sequence, "--.") == 0){
			output_text_sequence[output_length++] = 'G';
		}

		//letter H
		else if(strcmp(char_morse_sequence, "....") == 0){
			output_text_sequence[output_length++] = 'H';
		}

		//letter I
		else if(strcmp(char_morse_sequence, "..") == 0){
			output_text_sequence[output_length++] = 'I';
		}

		//letter J
		else if(strcmp(char_morse_sequence, ".---") == 0){
			output_text_sequence[output_length++] = 'J';
		}

		//letter K
		else if(strcmp(char_morse_sequence, "-.-") == 0){
			output_text_sequence[output_length++] = 'K';
		}

		//letter L
		else if(strcmp(char_morse_sequence, ".-..") == 0){
			output_text_sequence[output_length++] = 'L';
		}

		//letter M
		else if(strcmp(char_morse_sequence, "--") == 0){
			output_text_sequence[output_length++] = 'M';
		}

		//letter N
		else if(strcmp(char_morse_sequence, "-.") == 0){
			output_text_sequence[output_length++] = 'N';
		}

		//letter O
		else if(strcmp(char_morse_sequence, "---") == 0){
			output_text_sequence[output_length++] = 'O';
		}

		//letter P
		else if(strcmp(char_morse_sequence, ".--.") == 0){
			output_text_sequence[output_length++] = 'P';
		}

		//letter Q
		else if(strcmp(char_morse_sequence, "--.-") == 0){
			output_text_sequence[output_length++] = 'Q';
		}

		//letter R
		else if(strcmp(char_morse_sequence, ".-.") == 0){
			output_text_sequence[output_length++] = 'R';
		}

		//letter S
		else if(strcmp(char_morse_sequence, "...") == 0){
			output_text_sequence[output_length++] = 'S';
		}

		//letter T
		else if(strcmp(char_morse_sequence, "-") == 0){
			output_text_sequence[output_length++] = 'T';
		}

		//letter U
		else if(strcmp(char_morse_sequence, "..-") == 0){
			output_text_sequence[output_length++] = 'U';
		}

		//letter V
		else if(strcmp(char_morse_sequence, "...-") == 0){
			output_text_sequence[output_length++] = 'V';
		}

		//letter W
		else if(strcmp(char_morse_sequence, ".--") == 0){
			output_text_sequence[output_length++] = 'W';
		}

		//letter X
		else if(strcmp(char_morse_sequence, "-..-") == 0){
			output_text_sequence[output_length++] = 'X';
		}

		//letter Y
		else if(strcmp(char_morse_sequence, "-.--") == 0){
			output_text_sequence[output_length++] = 'Y';
		}

		//letter Z
		else if(strcmp(char_morse_sequence, "--..") == 0){
			output_text_sequence[output_length++] = 'Z';
		}

		//number 1
		else if(strcmp(char_morse_sequence, ".----") == 0){
			output_text_sequence[output_length++] = '1';
		}

		//number 2
		else if(strcmp(char_morse_sequence, "..---") == 0){
			output_text_sequence[output_length++] = '2';
		}

		//number 3
		else if(strcmp(char_morse_sequence, "...--") == 0){
			output_text_sequence[output_length++] = '3';
		}

		//number 4
		else if(strcmp(char_morse_sequence, "....-") == 0){
			output_text_sequence[output_length++] = '4';
		}

		//number 5
		else if(strcmp(char_morse_sequence, ".....") == 0){
			output_text_sequence[output_length++] = '5';
		}

		//number 6
		else if(strcmp(char_morse_sequence, "-....") == 0){
			output_text_sequence[output_length++] = '6';
		}

		//number 7
		else if(strcmp(char_morse_sequence, "--...") == 0){
			output_text_sequence[output_length++] = '7';
		}

		//number 8
		else if(strcmp(char_morse_sequence, "---..") == 0){
			output_text_sequence[output_length++] = '8';
		}

		//number 9
		else if(strcmp(char_morse_sequence, "----.") == 0){
			output_text_sequence[output_length++] = '9';
		}

		//number 0
		else if(strcmp(char_morse_sequence, "-----") == 0){
			output_text_sequence[output_length++] = '0';
		}

		//comma ',"
		else if(strcmp(char_morse_sequence, "--..--") == 0){
			output_text_sequence[output_length++] = ',';
		}

		//period '.'
		else if(strcmp(char_morse_sequence, ".-.-.-") == 0){
			output_text_sequence[output_length++] = '.';
		}

		//question mark '?'
		else if(strcmp(char_morse_sequence, "..--..") == 0){
			output_text_sequence[output_length++] = '?';
		}

		//@ symbol
		else if(strcmp(char_morse_sequence, ".--.-.") == 0){
			output_text_sequence[output_length++] = '@';
		}

		//colon :
		else if(strcmp(char_morse_sequence, "---...") == 0){
			output_text_sequence[output_length++] = ':';
		}

		//semicolon ;
		else if(strcmp(char_morse_sequence, "-.-.-.") == 0){
			output_text_sequence[output_length++] = ';';
		}

		//dash -
		else if(strcmp(char_morse_sequence, "-....-") == 0){
			output_text_sequence[output_length++] = '-';
		}

		//apostrophe '
		else if(strcmp(char_morse_sequence, ".----.") == 0){
			output_text_sequence[output_length++] = '\'';
		}

		//unknown character is represented by *
		else{
			output_text_sequence[output_length++] = '*';
		}

		char_seq_length = 0;
		memset(char_morse_sequence, 0, 10);
	}
}

#endif /* SRC_MORSETRANSLATOR_H_ */
