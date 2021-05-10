#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define CICLE 4
#define WORK_TIME 25
#define SHORT_BREAK_TIME 5
#define LONG_BREAK_TIME 30

enum period{work, short_break, long_break} period;
const int PERIOD_TIMES[3] = {WORK_TIME, SHORT_BREAK_TIME, LONG_BREAK_TIME};
const char PERIOD_MESSAGES[3][128] = {
    "Time to work, you can do this!",
    "Enjoy a short break :)",
    "You have a longer break now!"
};

void notify() {
    printf("%s\n", PERIOD_MESSAGES[period]);

    char command[128];
    strcpy(command, "notify-send \"");
    strcat(command, PERIOD_MESSAGES[period]);
    strcat(command, "\"");
    system(command);
}

int main(int argc, char *argv[]) {
    time_t start_time, current_time;
    double elapsed_time;

    int step = 1;
    period = work;
    start_time = time(NULL);
    notify();

    while(1) {
        current_time = time(NULL);
        elapsed_time = difftime(current_time, start_time);

        if(elapsed_time >= PERIOD_TIMES[period] * 60) {
            start_time = time(NULL);

            step++;
            if(step % 2 == 1) {
                period = work;
            } else if(step % (CICLE * 2) == 0) {
                period = long_break;
            } else {
                period = short_break;
            }

            notify();
        }

        sleep(60); // sleeps 1 minute
    }

    return 0;
}
