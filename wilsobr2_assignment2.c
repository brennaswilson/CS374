#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "movies.h"

struct movie{
    char* title;
    int year;
    char* language;
    float rating;
    struct movie* next;

};

    // struct movie* createMovie(char* title, int* year, char* language, int* rating){
    //     struct movie* currMovie = malloc(sizeof(struct movie));
    //     // Allocate memory for the movie title in the structure
    //     currMovie->title = calloc(strlen(title) + 1, sizeof(char));
    //     // Copy the value of name into the member name in the structure
    //     strcpy(currMovie->title, title);

    //     // Allocate memory for member year in the structure
    //     currMovie->year = calloc(1, sizeof(int));
    //     // Copy the value of year into the member year in the structure
    //     strcpy(currMovie->year, year);

    //     // Allocate memory for member language in the structure
    //     currMovie->language = calloc(strlen(language) + 1, sizeof(char));
    //     // Copy the value of year into the member year in the structure
    //     strcpy(currMovie->language, language);

    //     // Allocate memory for member year in the structure
    //     currMovie->rating = calloc(strlen(rating) + 1, sizeof(int));
    //     // Copy the value of year into the member year in the structure
    //     strcpy(currMovie->rating, rating);

    //     // Set the next node to NULL in the newly created customer entry
    //     currMovie->next = NULL;
    //     return currMovie;

    // }


int main ( int argc, char **argv ){
    char title[40];
    int year;
    char language[10];
    int rating;


    // The head of the linked list
    struct movie* head = NULL;
    // The tail of the linked list
    struct movie* tail = NULL;
    //pointer to whole list 
    struct movie* movies = NULL;

    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movies.csv\n");
        return EXIT_FAILURE;
    }
    processMovieFile(argv[1]);
    
    int option_chosen = 0;
    int year_chosen;
    char language_chosen[86];

    while (option_chosen != 4){
        printf("1. Show movies released in the specified year \n");
        printf("2. Show highest rated movie for each year \n");
        printf("3. Show the title and year of release of all movies in a specific language \n");
        printf("4. Exit from the program \n\n");
        printf("Enter a choice from 1 to 4: ");
        scanf("%d", &option_chosen);

        if (option_chosen == 1){
            printf("Enter the year for which you want to see movies: ");
            scanf("%d", &year_chosen);

        }

        if (option_chosen == 2){
            printf("placeholder");
        }

        if (option_chosen == 3){
            printf("Enter the language for which you want to see movies: ");
            scanf("%s", language_chosen);
        }

        if (option_chosen > 4){
            printf("You entered an incorrect choice. Try again.\n\n");
        }


    }
    return 0;
    return EXIT_SUCCESS;
}














/*
 * Function: processMovieFile
 *   Opens a file, reads and prints each line
 *   filePath: path to the file
 * 
 *  This function shows sample code that opens a file, then in a loop reads and prints each line in that file.
 *  You can use this code however you want in your Prog Assignment 2.
 */
 void processMovieFile(char* filePath){
    char *currLine = NULL;
    size_t len = 0;
    int total_lines = 0;
    char **data = NULL;

    // Open the specified file for reading only
    FILE *movieFile = fopen(filePath, "r");

    // Read the file line by line
    while(getline(&currLine, &len, movieFile) != -1)
    {
        char *current_title;
        char current_year;
        char *current_lang;
        char current_rating;
        
        struct movie *currMovie = malloc(sizeof(struct movie) * (total_lines + 1));

        //get title
        char* token = strtok(currLine, ",");
        current_title = token;
        currMovie->title = calloc(strlen(token) + 1, sizeof(char));
        strcpy(currMovie->title, token);

        //get year
        token = strtok(NULL, ",");
        currMovie[total_lines].year = atoi(token);


        //get language
        token = strtok(NULL, ",");
        currMovie->language = calloc(strlen(token), sizeof(char));
        strcpy(currMovie -> language, token);

        //get rating
        token = strtok(NULL, ",");
        currMovie[total_lines].rating = atoi(token);

        currMovie-> next = NULL;
        total_lines ++;


    }


    // Free the memory allocated by getline for currLine
    free(currLine);
    // Close the file
    fclose(movieFile);
    printf("\nProcessed file %s and parsed data for %d movies\n", filePath, total_lines);
}


/**
 * 
 */
