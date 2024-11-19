#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "network.h"
#include "schedule.h"
#include "draw.h"

char active = 1;
const long wait_time_ms = 200000000;

int main(int argc, char** argv)
{

    load_departures("schedule.txt");


    if(init_graphics())
        return 1;

    const Color color = {.r = 125, .g = 235, .b = 45};

    pthread_t connection_th;
    time_t raw_now;
    int now;
    struct tm * timeinfo;

    //pthread_create(&connection_th, NULL, connection_handler, NULL);
    
    // Get time
    time(&raw_now);
    timeinfo = localtime(&raw_now);
    now = timeinfo->tm_wday * DAY + 
    timeinfo->tm_hour * HOUR +
    timeinfo->tm_min * MINUTE +
    timeinfo->tm_sec;

    unsigned short current_dep = bin_search_current_departure_index(now);
    Text current_dep_dest = {
        .x = 50, 
        .y = 10, 
        .banner_offset = 0, 
        .text = departures[current_dep].dest_name, 
        .text_len = departures[current_dep].dest_name_size
    };

    Text current_dep_line = {
        .x = 10, 
        .y = 10, 
        .banner_offset = 0, 
        .text = (char*) malloc(6), 
        .text_len = 0
    };
    current_dep_line.text_len = sprintf(current_dep_line.text, "%u", departures[current_dep].line_num);

    Text current_dep_time = {
        .x = 200, 
        .y = 10, 
        .banner_offset = 0, 
        .text = (char*) malloc(11), 
        .text_len = 0
    };

    draw_text(&current_dep_line, 16, color);
    draw_text(&current_dep_dest, 16, color);

    int min_diff, last_min_diff = 0;
    char any_diff = 1;
    struct timespec ts = {.tv_nsec=wait_time_ms, .tv_sec=0};
    do
    {
        // Get time
        time(&raw_now);
        timeinfo = localtime(&raw_now);
        now = timeinfo->tm_wday * DAY + 
        timeinfo->tm_hour * HOUR +
        timeinfo->tm_min * MINUTE +
        timeinfo->tm_sec;
        
        if (now > departures[current_dep].departure_time || abs(now - departures[current_dep].departure_time) > 3 * DAY){
            current_dep++;
            if (current_dep >= num_departures)
                current_dep = 0;

            clear_text_area(&current_dep_line, 16);
            clear_text_area(&current_dep_dest, 16);
            // Update visuals for destination and line number
            current_dep_dest.text = departures[current_dep].dest_name, 
            current_dep_dest.text_len = departures[current_dep].dest_name_size;
            current_dep_line.text_len = sprintf(current_dep_line.text, "%u", departures[current_dep].line_num);

            draw_text(&current_dep_line, 16, color);
            draw_text(&current_dep_dest, 16, color);
            any_diff = 1;
        }

        min_diff = get_time_diff_min(departures[current_dep].departure_time, now);
        if (min_diff != last_min_diff){
            clear_text_area(&current_dep_time, 16);
            current_dep_time.text_len = sprintf(current_dep_time.text, "%u min", min_diff);
            current_dep_time.x = WIDTH - (current_dep_time.text_len - 1) * 16 - letter_dimension;
            draw_text(&current_dep_time, 16, color);
            any_diff = 1;
        }
        last_min_diff = min_diff;
        
        if (handle_events())
            active = 0;
        
        if (any_diff)
            render();
        any_diff = 0;
        nanosleep(&ts, &ts); 
        ts.tv_nsec = wait_time_ms;

    } while (active);
    
    //pthread_join(connection_th, NULL);

    free(current_dep_dest.text);
    free(current_dep_line.text);
    return 0;
}