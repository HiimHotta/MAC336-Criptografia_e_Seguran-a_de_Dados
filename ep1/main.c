/* main.c
*
* Daniel Hotta <daniel.y.hotta at gmail>
*
* File to implement algorithm K128 and encrypt files.
*
* Para compilar:
* gcc -Wall -ansi -o main main.c
*
* Para rodar:
* ./main <arquivo_entrada> <arquivo_saida>
*
* References:
* FUNCTION: add ()
* https://stackoverflow.com/questions/22026777/how-to-add-bits-using-bitwise
*
* FUNCTION: isLE ()
* https://stackoverflow.com/questions/12791864/c-program-to-check-little-vs-big-endian/12792301#12792301
*
* FUNCTION: op3 ()
* https://stackoverflow.com/questions/18473134/how-to-add-each-byte-of-an-8-byte-long-integer
*
* FUNCTION: rotate () or circular_shift ()
* https://www.geeksforgeeks.org/rotate-bits-of-an-integer/
*
* HELPER: Printf int64
* https://stackoverflow.com/questions/9225567/how-to-print-a-int64-t-type-in-c
*
*/
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef uint64_t byte_t;

typedef unsigned char byte;

// number of rounds defined in the exercise
#define R 12

// 64 bits for each entry
#define INT_BITS 64
#define NUM_BYTES 8
#define MOD 257

// pre-calculated values for K128
byte_t expo [256];
byte_t loga [256];

// initialize global variables and check parameters
void init () {
    if (argc < 6) {
        printf ("There are 4 modes:\n")
        printf ("Mode 1 (To Encrypt files)      : programa -c -i <input file> -o <output file> -p <password> -a\n");
        printf ("Mode 2 (To Decrypt files)      : programa -d -i <input file> -o <output file> -p <password>   \n");
        printf ("Mode 3 (Calculate Randomness 1): programa -1 -i <input file> -p <password> -a\n");
        printf ("Mode 4 (Calculate Randomness 2): programa -2 -i <input file> -p <password> -a\n");
        exit (1);
    }


    // previous value of expo % MOD
    byte_t prev = 1;
    expo[0] = 1;
    loga[expo[0]] = 0;

    for (int i = 1; i < 256; i++) {
        prev = (45 * prev) % 257;
        expo[i] = prev % 256;
        loga[expo[i]] = i;
    }
}

// check if computer is little endian (LE - 1) or big endian (BE - 0)
byte_t isLE () {
    int n = 1;

    if (*(char *) &n == 1)
        return 1;

    return 0;
}

// Function to left rotate x by n bits
byte_t leftRotate (byte_t x, byte_t n) {    
    if (n == 0)
        return x;

    n = n % INT_BITS;
    
    return (x << n) | (x >> (INT_BITS - n));
}
 
//Function to right rotate x by n bits
int rightRotate (byte_t x, byte_t n) {
    if (n == 0)
        return x;

    n = n % INT_BITS;
    
    return (x >> n) | (x << (INT_BITS - n));
}

// sum (a + b) (both 64 bits) and take module 2^64
byte_t add (byte_t a, byte_t b) {
    byte_t carry  = a & b;

    byte_t result = a ^ b;

    while (carry != 0) {
        byte_t shiftedcarry = carry << 1;

        carry = result & shiftedcarry;

        result ^= shiftedcarry;
    }

    return result;
}

// convert string to int
byte_t toInt (char *s){    
    byte_t ans = 0 ;
    
    byte_t p = 1;
    for (int i = 0; i < 16; i++) {
        ans += s[i] * p;
        p *= 256;
    }
        
    return ans ;
}

// convert byte_t array to char array
char* toString (byte_t *num, int size) {
    char *ans = malloc (size);
    
    for (int i = 0; i < size; i++ ) {
        for (int j = 0; j < 16; j++) {
            ans[i * NUM_BYTES + j] = (char) (num[i] % 256);
            num[i] /= 256;
        }
    }
        
    return ans;
}

// operation 3 of K128 (dot)
byte_t op3 (byte_t b, byte_t c) {
    byte *b_bytes = calloc (NUM_BYTES, sizeof (byte));
    byte *c_bytes = calloc (NUM_BYTES, sizeof (byte));

    for (int i = 0; i < NUM_BYTES; i++) {
        b_bytes [i] = 0xff & (b >> i * NUM_BYTES);
        c_bytes [i] = 0xff & (c >> i * NUM_BYTES);
    }

    byte_t p = 1, res = 0;

    for (int i = 0; i < NUM_BYTES; i++) {
        res += expo[b_bytes[i]] ^ expo[c_bytes[i]] * p;
        p *= 256;
    }

    return res;
}

// Generate Subkeys, *Key is a array with 2 elements
// of  64 bits
byte_t* subkey_generator (byte_t *key) {
    // separa o espaço para L e k
    byte_t *L = calloc (4 * R + 3, sizeof (byte_t));

    byte_t *k = calloc (4 * R + 3, sizeof (byte_t));

    // iterador e variavel do pseudocodigo
    int i, j;

    // 1 - copia os dados para L0 e L1

    L[0] = key[0];
    L[1] = key[1];

    // 2 - 

    for (j = 2; j < 4 * R + 3; j++) 
        L[j] = add (L[j - 1], 0x9e3779b97f4a7151);
        
    // 3 -
    k[0] = 0x8aed2a6bb7e15162;

    // 4 -
    for (j = 1; j < 4 * R + 3; j++)
        k[j] = add (k[j - 1], 0x7c159e3779b97f4a);
        
    // 5 - 
    i = 0; j = 0;

    byte_t A, B;

    A = 0; B = 0;
    
    
    // 6 -
    for (int s = 1; s < 4 * R + 4; s++) {
        // a)
        k[i] = leftRotate (add (B, add (A, k[i])), 3);

        A = k[i];

        i++;

        // b)
        L[j] = leftRotate (add (A, add (B, L[j])), add (A, B));

        B = L[j];

        j++;
    }

    return k;
}


// part 1 of algorithm K128 t0 X_a
byte_t part1_xa (byte_t xa, byte_t ka) {
    return op3 (xa, ka);
}

// part 1 of algorithm K128 to X_b
byte_t part1_xb (byte_t xb, byte_t kb) {
    return add (xb, kb);
}

// part 2 of algorithm K128 for X_e
byte_t part2_xe (byte_t xe, byte_t xf, byte_t ke, byte_t kf) {
    byte_t y1 = xe ^ xf;
    byte_t y2 = op3 (add (op3 (ke, y1), y1), kf);
    byte_t z  = add (op3 (ke, y1), y2);

    return xe ^ z;
}

// part 2 of algorithm K128 to X_f
byte_t part2_xf (byte_t xe, byte_t xf, byte_t ke, byte_t kf) {
    byte_t y1 = xe ^ xf;
    byte_t y2 = op3 (add (op3 (ke, y1), y1), kf);
    byte_t z  = add (op3 (ke, y1), y2);

    return xf ^ z;
}

// Last operation, transformation T to X_e
byte_t T_xe (byte_t xf, byte_t ke) {
    return op3 (xf, ke);
}

// Last operation, transformation T to X_f
byte_t T_xf (byte_t xe, byte_t kf) {
    return add (xe, kf);
}

byte_t* K128 (byte_t *input, byte_t *password) {
    byte_t *keys = subkey_generator (password);
    byte_t *x = calloc (2, sizeof (byte_t));

    // FALTA ATRIBUIR VALORES PARA METADE DE CADA
    byte_t aux;

    x[0] = input[0]; x[1] = input[1];

    // parte 2
    for (int i = 0; i < R; i++) {
        // part 1
        x[0] = part1_xa (x[0], keys[4 * i + 1]);       
        x[1] = part1_xb (x[1], keys[4 * i + 2]);
        
        // part 2
        aux = part2_xe (x[0], x[1], keys[4 * i + 3], keys[4 * i + 4]);      
        x[1] = part2_xf (x[0], x[1], keys[4 * i + 3], keys[4 * i + 4]);        
        x[0] = aux;       
    }
    
    // transformação T

    aux  = T_xe (x[1], keys [4 * R + 1]);
    x[1] = T_xf (x[0], keys [4 * R + 2]);
    x[0] = aux;

    return x;
}

int test () {
    init ();
    byte_t *password = malloc (2 * sizeof (byte_t));
    byte_t *plainText = malloc (2 * sizeof (byte_t));
    
    password[0] = toInt ("11111111");
    password[1] = toInt ("FFFFFFFF");
    
    plainText[0] = toInt ("AAAAAAAA");
    plainText[1] = toInt ("BBBBBBBB");
    
    /*
    for (int i = 1; i <= 50; i++) {
        printf ("%d: %" PRIu64 "\n", i, k[i]);
    }
    */
    
    byte_t *x = K128 (plainText, password);
    
    printf ("0: %" PRIu64 "\n", x[0]);
    printf ("1: %" PRIu64 "\n", x[1]);
    
    printf ("%s", toString (x, 2));
    //printf ("\n%d: %" PRIu64 "\n", 1, op3 (input[1], k[1]));
    

    return 0;
}

void mode_1 (char input_name[], byte_t *password, char output_name[]) {
    FILE *input_file;

    input_file = fopen (input_name, "r");

    if (input_file == NULL) {
        printf ("Input file %s not found.nn", input_name);
        exit (1);
    }

    while((c = fgetc (input_file)) != EOF) {
        fread  (file_bytes, sizeof (*file_bytes), file_size, input_file);

        fwrite (file_bytes, sizeof (*file_bytes), file_size, output_file);
    }

    fclose (input_file);
}

void mode_2 () {
    
}

void mode_3 () {
    
}

void mode_4 () {
    
}

int main (int argc, char **argv) {
    init ();

    if (strcmp (argv [1], "-c")) {
        mode_1 ();
    }

    else if (strcmp (argv [1], "-d")) {
        mode_2 ();
    }

    else if (strcmp (argv [1], "-1")) {
        mode_3 ();
    }

    else if (strcmp (argv [1], "-2")) {
        mode_4 ();
    }

    else 
        printf ("Use a valid command!\n");

    return 0;
}