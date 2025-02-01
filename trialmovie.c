#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE_LENGTH 100
#define MAX_GENRE_LENGTH 50

// Structure to represent a movie
struct Movie {
    char title[MAX_TITLE_LENGTH];
    char year;
    char genre[MAX_GENRE_LENGTH];
    char rating;
};

int main() {
    FILE *fp;
    char line[256]; 
    struct Movie *movies = NULL;
    int numMovies = 0;

    // Open the CSV file
    fp = fopen("movies_sample_1.csv", "r");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    // Read the header line (if any)
    fgets(line, sizeof(line), fp);

    // Read and parse each line
    while (fgets(line, sizeof(line), fp)) {
        // Allocate memory for a new movie
        movies = realloc(movies, (numMovies + 1) * sizeof(struct Movie));

        // Parse the line using strtok
        char *token = strtok(line, ",");
        strncpy(movies[numMovies].title, token, MAX_TITLE_LENGTH);

        token = strtok(NULL, ",");
        movies[numMovies].year = atoi(token);

        token = strtok(NULL, ",");
        strncpy(movies[numMovies].genre, token, MAX_GENRE_LENGTH);

        token = strtok(NULL, ",");
        movies[numMovies].rating = atof(token);

        numMovies++;
    }

    // Close the file
    fclose(fp);

    // Print the movie data
    for (int i = 0; i < numMovies; i++) {
        printf("Title: %s\n", movies[i].title);
        printf("Year: %d\n", movies[i].year);
        printf("Genre: %s\n", movies[i].genre);
        printf("Rating: %.1f\n", movies[i].rating);
        printf("\n");
    }

    // Free the allocated memory
    free(movies);

    return 0;
}