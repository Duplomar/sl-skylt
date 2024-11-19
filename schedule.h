#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MINUTE 60
#define HOUR 3600
#define DAY 86400
#define WEEK 604800

struct departure
{
    int departure_time; // Time for departure, in seconds since start of week. Only fields for one week
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
                        departures[curr_dep_index].departure_time = atoi(buffer + char_holder + 1);

                        while (buffer[char_holder] != '.')
                            char_holder--;
                        buffer[char_holder] = 0;
                        departures[curr_dep_index].departure_time += MINUTE * atoi(buffer + char_holder + 1);

                        while (buffer[char_holder] != '.')
                            char_holder--;
                        buffer[char_holder] = 0;
                        departures[curr_dep_index].departure_time += HOUR * atoi(buffer + char_holder + 1);

                        departures[curr_dep_index].departure_time += DAY * atoi(buffer);

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

void print_week(int time)
{
    unsigned short holder;
    holder = time / DAY;
    printf("dag %s, ", weekdays[holder]);
    time -= holder * DAY;
    
    holder = time / HOUR;
    printf("timme %d, ", holder);
    time -= holder * HOUR;

    holder = time / MINUTE;
    printf("minut %d, ", holder);
    time -= holder * MINUTE;

    printf("sekund %d\n", time);
}

void print_departure(Departure departure)
{
    printf("Linje %d mot %s, avgår ", departure.line_num, departure.dest_name);
    print_week(departure.departure_time);
}


int get_time_diff_min(int a, int b)
{
    return (a - b)/MINUTE + 1;
}

unsigned short bin_search_current_departure_index(int current_time)
{
    unsigned short low, high, i;
    low = 0;
    high = num_departures - 1;
    i = high/2;
    while (low < high)
    {   

        int lower_comp = current_time - departures[i].departure_time;
        if (i + 1 >= num_departures)
            return 0;
        int higher_comp = current_time - departures[i + 1].departure_time;

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