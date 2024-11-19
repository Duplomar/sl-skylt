#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct week
{
    char sec;
    char min;
    char hour;
    char day;
};
typedef struct week Week;

struct departure
{
    Week departure_time; // Time for departure. Only fields for one week
    unsigned short line_num;
    unsigned char dest_name_size;
    char* dest_name;
};
typedef struct departure Departure;

// Sorted array of departures for one month. Will loop when end is reached
unsigned short num_departures;
Departure* departures; 

enum read_state{line_number, destination_name, departures_time};
void load_departures(const char* filename)
{
    FILE* file_ptr = fopen(filename, "r");
    if (file_ptr)
    {
        if(fscanf(file_ptr, "%hu", &num_departures) == EOF)
            return;
        
        departures = (Departure*) malloc(sizeof(Departure) * num_departures);
        unsigned short curr_dep_index = 0;

        const unsigned char buffer_size = 64;
        char buffer[buffer_size + 1];
        buffer[buffer_size] = 0;
        unsigned char buff_index = 0;

        enum read_state state = line_number; 
        short char_holder;
        while ((char_holder = fgetc(file_ptr)) != EOF && curr_dep_index < num_departures)
        {
            if (char_holder == ';' || buff_index >= buffer_size)
            {
                buffer[buff_index] = 0;
                switch (state)
                {
                    case line_number:
                        departures[curr_dep_index].line_num = atoi(buffer);
                        state = destination_name;
                        break;
                        
                    case destination_name:
                        char_holder = 0;
                        while (buffer[char_holder] == ' ' || buffer[char_holder] == '\n')
                            char_holder++;

                        departures[curr_dep_index].dest_name = (char*) malloc(buff_index - char_holder + 1);
                        departures[curr_dep_index].dest_name_size = buff_index - char_holder;
                        strcpy(departures[curr_dep_index].dest_name, buffer + char_holder);
                        state = departures_time;
                        break;

                    case departures_time:
                        char_holder = buff_index - 1;
                        while (buffer[char_holder] != '.')
                            char_holder--;
                        buffer[char_holder] = 0;
                        departures[curr_dep_index].departure_time.sec = atoi(buffer + char_holder + 1);

                        while (buffer[char_holder] != '.')
                            char_holder--;
                        buffer[char_holder] = 0;
                        departures[curr_dep_index].departure_time.min = atoi(buffer + char_holder + 1);

                        while (buffer[char_holder] != '.')
                            char_holder--;
                        buffer[char_holder] = 0;
                        departures[curr_dep_index].departure_time.hour = atoi(buffer + char_holder + 1);

                        departures[curr_dep_index].departure_time.day = atoi(buffer);

                        curr_dep_index++;
                        state = line_number;
                    default:
                        break;
                } 
                buff_index = 0;

            }
            else
                buffer[buff_index] = char_holder;
            buff_index++;
        
        }
    }
}


const char* weekdays[] = {
    "söndag",
    "måndag",
    "tisdag",
    "onsdag",
    "torsdag",
    "fredag",
    "lördag",
};

void print_week(Week week)
{
    printf("dag %s, timme %d, minut %d, sekund %d\n", weekdays[week.day], week.hour, week.min, week.sec);
}

void print_departure(Departure departure)
{
    printf("Linje %d mot %s, avgår ", departure.line_num, departure.dest_name);
    print_week(departure.departure_time);
}

//Naive comparison of a and b, based on month only
short compare_time(Week a, Week b)
{
    if (a.day != b.day)
        return a.day - b.day;
    
    if (a.hour != b.hour)
        return a.hour - b.hour;

    if (a.min != b.min)
        return a.min - b.min;

    return a.sec - b.sec;
}

int get_time_diff_min(Week a, Week b)
{
    int diff = (a.day * 1440 + a.hour * 60 + a.min);
    diff -= (b.day * 1440 + b.hour * 60 + b.min);
    return diff;
}


unsigned short lin_search_current_departure_index(Week current_time, unsigned short i)
{
    if (compare_time(current_time, departures[i].departure_time) < 0)
        return i;

    while(i < num_departures - 1)
    {
        short lower_comp = compare_time(current_time, departures[i].departure_time);
        i++;
        short higher_comp = compare_time(current_time, departures[i].departure_time);

        if (lower_comp > 0 && higher_comp < 0)
            return i;
    } 
    return 0;

}

unsigned short bin_search_current_departure_index(Week current_time)
{
    unsigned short low, high, i;
    low = 0;
    high = num_departures - 1;
    i = high/2;
    while (low < high)
    {   

        short lower_comp = compare_time(current_time, departures[i].departure_time);
        if (i + 1 >= num_departures)
            return 0;
        short higher_comp = compare_time(current_time, departures[i + 1].departure_time);

        if (lower_comp < 0)
            high = i;
        else if (higher_comp > 0)
            low = i + 1;
        else
        {
            return i + 1;
        }
        i = (high + low)/2;
    }
    return 0;
}