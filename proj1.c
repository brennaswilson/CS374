#include <math.h>
#include <stdio.h>


/**
* Ask the user for two positive integers.
* Print all the multiples of the first integer between 1 and the second integer
*/


float const pi = 3.14159265359; 

float average (int a, int b, int c) {
    float avg;
    avg = (a + b + c)/3.0;
    return avg;
}

float volume (R, ha, hb) {
    float volume;
    float a_value;
    a_value = (sqrt(R*R - ha*ha))
    b_value = (sqrt(R*R - hb*hb))
    volume = ((1/6)*(pi)(ha-hb))(3*(a_value * a_value) + 3*(b_value * b_value) + (ha-hb))
    return volume;
}

int main()
{
    int spherical_segments = 0;
    float R;
    float ha; 
    float hb;


    while (spherical_segments < 2 || spherical_segments > 10) {
        printf("How many spherical segments you want to evaluate [2-10]? ");
        scanf("%d", &spherical_segments);
    }

    int boundary = 1;
    
    while (boundary <= spherical_segments) {

        printf("Obtaining data for spherical segment number %i.\n", boundary);
        printf("What is the radius of the sphere (R)?");
        scanf("%f", &R);
        printf("What is the height of the top area of the spherical segment (ha)? ");
        scanf("%f", &ha);
        printf("What is the height of the bottom area of the spherical segment (hb)?");
        scanf("%f", &hb);
        

        printf("Entered data: R = %.2f ha = %.2f hb = %.2f.\n", R, ha, hb);

        if ( 0 < R && 0 < ha && 0 < hb && hb<=ha && ha<=R) {
            current_volume = volume(R, ha, hb)
            printf("Total Surface Area = 697.43 Volume = %.2f.\n", current_volume)
            boundary ++;
        }

        else if (ha < hb) {
            printf("Invalid Input: ha = %.2f hb = %.2f. ha must be greater than or equal to hb.\n", ha, hb);
        }
        else if (R < ha) {
            printf("Invalid Input: R = %.2f ha = %.2f. R must be greater than or equal to ha.\n", R, ha);
        }

    }
    
    
    return 0;
}


