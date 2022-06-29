/* hexdump.c
*
* Thales Paiva <thalespaiva em gmail>
*
* Simples hexdump para ilustrar como ler um arquivo binario para um vetor.
* Escreve no arquivo_saida o arquivo_entrada XOR FF, byte a byte.
*
* Para compilar:
* gcc -Wall -ansi -o hexdump hexdump.c
*
* Para rodar:
* ./hexdump <arquivo_entrada> <arquivo_saida>
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_NAME_SIZE 100

typedef unsigned char string;

long get_file_size (char file_name[]);

void read_file_to_array (char file_name[], byte_t file_bytes[], long file_size);

void write_array_to_file (char file_name[], byte_t file_bytes[], long file_size);
