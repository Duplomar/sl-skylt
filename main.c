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

    Color color;
    color.r = 125;
    color.g = 235;
    color.b = 45;

    Text text;
    text.text = "Ropsten";
    text.text_len = 7;
    text.x = 10;
    text.y = 20;

    draw_text(text, 16, color);

    for (size_t i = 0; i < 1000; i++)
    {
        render();
    }
    
    return 0;

    pthread_t connection_th;
    time_t raw_now;
    Week now;
    struct tm * timeinfo;

    //pthread_create(&connection_th, NULL, connection_handler, NULL);

    do
    {
        time(&raw_now);
        timeinfo = localtime(&raw_now);
        now.day = timeinfo->tm_wday;
        now.hour = timeinfo->tm_hour;
        now.min = timeinfo->tm_min;
        now.sec = timeinfo->tm_sec;

        print_week(now);
        print_departure(departures[get_current_departure_index(now)]);
        return 0;

        if (render())
            active = 0;
    } while (active);
    
    pthread_join(connection_th, NULL);
    return 0;
}