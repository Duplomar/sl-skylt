#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "network.h"
#include "schedule.h"
#include "draw.h"

char active = 1;


int main(int argc, char** argv)
{
    load_departures("schedule.txt");

    if(init_graphics())
        return 1;

    const Color color = {.r = 125, .g = 235, .b = 45};

    pthread_t connection_th;
    time_t raw_now;
    Week now;
    struct tm * timeinfo;

    //pthread_create(&connection_th, NULL, connection_handler, NULL);

    Departure* current_dep = &departures[get_current_departure_index(now)];
    Text current_dep_dest = {
        .x = 50, 
        .y = 10, 
        .banner_offset = 0, 
        .text = current_dep->dest_name, 
        .text_len = current_dep->dest_name_size
    };

    Text current_dep_line = {
        .x = 10, 
        .y = 10, 
        .banner_offset = 0, 
        .text = (char*) malloc(6), 
        .text_len = 0
    };
    current_dep_line.text_len = sprintf(current_dep_line.text, "%u", current_dep->line_num);

    Text current_dep_time = {
        .x = 200, 
        .y = 10, 
        .banner_offset = 0, 
        .text = (char*) malloc(11), 
        .text_len = 0
    };

    do
    {
        // Get time
        time(&raw_now);
        timeinfo = localtime(&raw_now);
        now.day = timeinfo->tm_wday;
        now.hour = timeinfo->tm_hour;
        now.min = timeinfo->tm_min;
        now.sec = timeinfo->tm_sec;

        print_week(now);
        print_week(current_dep->departure_time);

        current_dep_time.text_len = sprintf(current_dep_time.text, "%u min", get_time_diff_min(current_dep->departure_time, now));
        
        draw_text(&current_dep_line, 16, color);
        draw_text(&current_dep_dest, 16, color);
        draw_text(&current_dep_time, 16, color);
        if (render())
            active = 0;
    } while (active);
    
    //pthread_join(connection_th, NULL);

    free(current_dep_dest.text);
    free(current_dep_line.text);
    return 0;
}