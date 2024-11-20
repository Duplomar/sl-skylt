#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "network.h"
#include "schedule.h"
#include "draw.h"

char active = 1;
const long wait_time_ms = 100000000;
const short letter_spacing = 16;

int main(int argc, char **argv)
{

    load_departures("schedule.txt");

    if (init_graphics())
        return 1;

    const Color color = {.r = 125, .g = 235, .b = 45};

    pthread_t connection_th;
    time_t raw_now;
    int now;
    struct tm *timeinfo;

    // pthread_create(&connection_th, NULL, connection_handler, NULL);

    // Get time
    time(&raw_now);
    timeinfo = localtime(&raw_now);
    now = timeinfo->tm_wday * DAY + timeinfo->tm_hour * HOUR + timeinfo->tm_min * MINUTE + timeinfo->tm_sec;

    unsigned short current_dep = bin_search_current_departure_index(now);
    TextStrip text_strips[3];
    for (unsigned short i = 0; i < 3; i++)
    {
        Text destination_text = {
            .x = 5,
            .y = 2 + i * (5 + letter_spacing),
            .letter_spacing = letter_spacing,
            .text = departures[(current_dep + i) % num_departures].dest_name,
            .text_len = departures[(current_dep + i) % num_departures].dest_name_size,
            .color = color
        };

        Text line_text = {
            .x = 5 + (letter_spacing + 1) * 2,
            .y = 2 + i * (5 + letter_spacing),
            .letter_spacing = letter_spacing,
            .text = (char*) malloc(7),
            .text_len = 0,
            .color = color
        };
        line_text.text_len = sprintf(line_text.text, "%.7u", departures[(current_dep + i) % num_departures].line_num);

        text_strips[i].destination = destination_text;
        text_strips[i].line_num = line_text;
        text_strips[i].rolling = 0;
    }
    text_strips[2].rolling = 1;

    for (unsigned short i = 0; i < 3; i++)
        draw_text_strip(&text_strips[i]);
        
    int min_diff, last_min_diff = 0;
    char any_diff = 1;
    struct timespec ts = {.tv_nsec = wait_time_ms, .tv_sec = 0};
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

            for (unsigned short i = 0; i < 3; i++)
            {
                text_strips[i].texts[0].x = 50;
                text_strips[i].texts[0].y = 2 + i * (5 + letter_spacing);
                text_strips[i].texts[0].letter_spacing = letter_spacing;
                text_strips[i].texts[0].text = departures[(current_dep + i) % num_departures].dest_name;
                text_strips[i].texts[0].text_len = departures[(current_dep + i) % num_departures].dest_name_size;
                text_strips[i].texts[0].color = color;
                
                text_strips[i].num_texts = 1;
                text_strips[i].rolling = 0;
                draw_text_strip(&text_strips[i]);
            }
            any_diff = 1;
        }


        for (unsigned short i = 0; i < 3; i++){
            if(text_strips[i].rolling){
                draw_text_strip(&text_strips[i]);
                any_diff = 1;
            }
        }

        last_min_diff = min_diff;

        if (handle_events())
            active = 0;

        if (any_diff)
            render();

        nanosleep(&ts, &ts);
        ts.tv_nsec = wait_time_ms;

    } while (active);

    // pthread_join(connection_th, NULL);

    return 0;
}