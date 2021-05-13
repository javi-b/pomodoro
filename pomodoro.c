#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define STR_LEN 128

#define NOTIFICATION_SOUND "/usr/share/pomodoro/notification.mp3"
#define NOTIFICATION_URGENCY "critical" // Can be: low / normal / critical

#define CICLE 4
#define WORK_TIME 25
#define SHORT_BREAK_TIME 5
#define LONG_BREAK_TIME 30

enum period{work, short_break, long_break} period;
const int PERIOD_TIMES[3] = {WORK_TIME, SHORT_BREAK_TIME, LONG_BREAK_TIME};
const char PERIOD_MESSAGES[3][STR_LEN] = {
    "Time to work, you can do this!",
    "Enjoy a short break :)",
    "You have a longer break now!"
};

int command_exists (char command[STR_LEN]) {
    char which_command[STR_LEN];
    strcpy (which_command, "which ");
    strcat (which_command, command);
    strcat (which_command, " > /dev/null 2>&1");
    if (system (which_command)) {
        fprintf (stderr, "Command '%s' not found.", command);
        return 0;
    }
    return 1;
}

void notify () {
    char command[STR_LEN];

    // System notification using 'notify-send'
    if (command_exists ("notify-send")) {
        strcpy (command, "notify-send --urgency=");
        strcat (command, NOTIFICATION_URGENCY);
        strcat (command, " -a \"Pomodoro\" \"Pomodoro\" \"");
        strcat (command, PERIOD_MESSAGES[period]);
        strcat (command, "\"");
        system (command);
    }

    // Sound using 'mpv'
    if (command_exists ("mpv")) {
        strcpy (command, "mpv --vo=null --really-quiet --loop-playlist=1 \
                --loop=0 ");
        strcat (command, NOTIFICATION_SOUND);
        system (command);
    }
}

int main (int argc, char *argv[]) {
    time_t start_time, current_time;
    double elapsed_time;

    int step = 1;
    period = work;
    start_time = time (NULL);
    notify ();

    while (1) {
        current_time = time (NULL);
        elapsed_time = difftime (current_time, start_time);

        if (elapsed_time >= PERIOD_TIMES[period] * 60) {
            start_time = time (NULL);

            step++;
            if (step % 2 == 1) {
                period = work;
            } else if (step % (CICLE * 2) == 0) {
                period = long_break;
            } else {
                period = short_break;
            }

            notify ();
        }

        sleep (60); // sleeps 1 minute
    }

    return 0;
}

