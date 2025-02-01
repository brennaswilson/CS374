#include <math.h>
#include <stdio.h>


/**
* Ask the user for two positive integers.
* Print all the multiples of the first integer between 1 and the second integer
*/

float total_volume;
float total_surface;
int spherical_segments = 0;

float surface_area(float R, float ha, float hb){
    float surface_area;
    float a_value;
    float b_value;
    float h_value; 
    float top_surface;
    float bottom_surface;
    float lateral_surface;
    a_value = (sqrt(R*R - ha*ha));
    b_value = (sqrt(R*R - hb*hb));
    h_value = (ha - hb);
    top_surface = (3.14159265359 * (a_value*a_value));
    bottom_surface = (3.14159265359 * (b_value*b_value));
    lateral_surface = 2.0 * 3.14159265359 * R * h_value;
    surface_area = (top_surface + bottom_surface + lateral_surface);
    return surface_area;
}

float volume (float R, float ha, float hb) {
    float volume;
    float a_value;
    float b_value;
    a_value = (sqrt(R*R - ha*ha));
    b_value = (sqrt(R*R - hb*hb));
    volume = ((1.0/6.0)* 3.14159265359 *(ha-hb)) * (3.0*(a_value * a_value) + 3.0*(b_value * b_value) + (ha-hb)*(ha-hb));
    return volume;
}

int main()
{
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
            float current_volume;
            float current_surface;
            current_volume = volume(R, ha, hb);
            total_volume += current_volume;
            current_surface = surface_area(R, ha, hb);
            total_surface += current_surface;
            printf("Total Surface Area = %.2f Volume = %.2f.\n", current_surface, current_volume);
            boundary ++;
        }

        else if (ha < hb) {
            printf("Invalid Input: ha = %.2f hb = %.2f. ha must be greater than or equal to hb.\n", ha, hb);
        }
        else if (R < ha) {
            printf("Invalid Input: R = %.2f ha = %.2f. R must be greater than or equal to ha.\n", R, ha);
        }

    }

    printf("Total average results:");
    float average_volume;
    float average_surface;
    average_surface = total_surface / spherical_segments;
    average_volume = total_volume /spherical_segments;
    printf("Average Surface Area = %.2f  Average Volume = %.2f .\n", average_surface, average_volume);

    return 0;
}


