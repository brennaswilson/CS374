#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int randint(){

    int random_index;
    
    
    random_index = rand() % 26;

    return random_index;

}


int main(int argc, char **argv){

    if (argc < 2)
    {
        fprintf(stderr, "Error: Please provide a nonzero key length\n");
        return 1;
    }

    int keylength;
    const char allowed_characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    // add 1 for null terminator 
    keylength = atoi(argv[1]);

    if(keylength < 1){
        fprintf(stderr, "Error: Please provide a nonzero key length\n");
        return 1;
    }


    srand(time(NULL));

    char generated_key[keylength + 1];

    for (int i=0; i < keylength; i++) {
        int rand_index = randint();
        char random_char = allowed_characters[rand_index];
        generated_key[i] = random_char;

    }

    generated_key[keylength] = '\0';

    printf("%s\n", generated_key);

    return 0;
}