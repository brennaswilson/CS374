#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int total_movies = 0; 

struct movie{
    char* title;
    char* year;
    char* language;
    char* rating;
    struct movie *next;

};

struct movie* processMovieFile(char* filePath);

struct movie* createMovie(char* title, char* year, char* language, char* rating){
    struct movie* currMovie = malloc(sizeof(struct movie));

    //insert title
    currMovie->title = calloc(strlen(title) + 1, sizeof(char));
    strcpy(currMovie->title, title);

    //insert year
    currMovie->year = calloc(strlen(year) + 1, sizeof(char));
    strcpy(currMovie->year, year);

    //insert language
    currMovie->language = calloc(strlen(language) + 1, sizeof(char));
    strcpy(currMovie->language, language);

    //insert rating
    currMovie->rating = calloc(strlen(rating) + 1, sizeof(char));
    strcpy(currMovie->rating, rating);  
    return currMovie;

    
}


int main ( int argc, char **argv ){

    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movies.csv\n");
        return EXIT_FAILURE;
    }

    struct movie* head = processMovieFile(argv[1]);

    int option_chosen = 0;
    char year_chosen[10];
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
            scanf("%s", year_chosen);
            struct movie* current = head;
            int movies_printed = 0;

            //go through linked list and print all movie titles with year that matches 
              while (current != NULL) {
                if(strcmp((current->year), year_chosen)==0){
                    printf("%s\n", current->title);
                    movies_printed++;
                }
                current = current->next;
            }
            if (movies_printed == 0){
                printf("No data about movies released in the year %s\n", year_chosen);
            }
            printf("\n");

        }

        if (option_chosen == 2){
            
            struct movie* topMovie = NULL;
            float maxRating = 0.0;

            //go through linked list and print all movie titles with year that matches 
            for(int i = 1900; i<2022; i++){
                struct movie* current = head;
                topMovie = NULL;
                maxRating = 0.0;
                int movie_year;
                while (current != NULL) {
                    float current_rating = strtof(current->rating, NULL);
                    int movie_year = atoi(current->year);
                    if(movie_year == i && current_rating > maxRating){
                        maxRating = current_rating;
                        topMovie = current;
                    }
                    current = current->next;  
                    }
                if(topMovie != NULL){
                        printf("%s %.1f %s\n", topMovie->year, maxRating, topMovie->title);
            }
             
            }
            printf("\n");
        }

        if (option_chosen == 3){
            printf("Enter the language for which you want to see movies: ");
            scanf("%s", language_chosen);
            struct movie* current = head;
            //skip headers
            current = current -> next;
            int movieCounter = 0;
            

            while(current != NULL){
                char* movieLanguage = strdup(current -> language);
                char* current_language = strtok(movieLanguage, ";[]");
                
                while(current_language != NULL){
                    if(strcmp(current_language, language_chosen) == 0){
                        printf("%s %s\n", current->year, current->title);
                        movieCounter ++;
                        break;
                    }
                    current_language = strtok(NULL, ";[]");
                }
                current = current->next;
                }
            if(movieCounter == 0){
                printf("No data about movies released in %s\n", language_chosen);
            }

            printf("\n");

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
 struct movie* processMovieFile(char* filePath){
    char *currLine = NULL;
    size_t len = 0;

    // Open the specified file for reading only
    FILE *movieFile = fopen(filePath, "r");

    // The head of the linked list
    struct movie* head = NULL;
    // The tail of the linked list
    struct movie* tail = NULL;

    // Read the file line by line
    while(getline(&currLine, &len, movieFile) != -1)
    {

        //get title, year, language, and rating
        char* title = strtok(currLine, ",");
        char* year = strtok(NULL, ",");
        char* language = strtok(NULL, ",");
        char* rating = strtok(NULL, ",");

        struct movie* newMovie = createMovie(title, year, language, rating);

        if(head == NULL){
            // This is the first element in the linked link
            // Set the head and the tail to this element
            head = newMovie;
            tail = newMovie;
        } else{
        // This is not the first element. 
        // Add this element to the list and advance the tail
        tail->next = newMovie;
        tail = newMovie;
        }

        total_movies ++;


    }

    // Free the memory allocated by getline for currLine
    free(currLine);
    // Close the file
    fclose(movieFile);
    printf("Processed file %s and parsed data for %d movies\n\n", filePath, total_movies - 1);
    return head;
}


/**
 * 
 */
