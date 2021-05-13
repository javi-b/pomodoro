#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

/**
 * Constants and global variables
 */
#define STR_LEN 128

#define NOTIFICATION_SOUND "/usr/share/pomodoro/notification.mp3"
#define NOTIFICATION_URGENCY "critical" // Can be: low / normal / critical

#define HELP_STRING \
    "Usage: pomodoro [OPTION...]\n" \
    "  -h, --help          display this help and exit\n" \
    "  -v, --verbose       print the notifications to stdout\n" \
    "      --no-sound      no sound notifications\n" \
    "      --no-desktop    no desktop notifications\n"

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

// Flags
int exit_flag, help_flag, verbose_flag, no_sound_flag, no_desktop_flag;

/**
 * Functions
 */
void parse_options (int argc, char *argv[]) {
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'},
        {"no-sound", no_argument, &no_sound_flag, 1},
        {"no-desktop", no_argument, &no_desktop_flag, 1},
        {0, 0, 0, 0} // To terminate array
    };

    int option_index = 0; // 'getopt_long' stores the option index here
    int c; // Option character code

    while (1) {
        c = getopt_long (argc, argv, "hv", long_options, &option_index);

        if (c == -1) {
            // If no more options...
            break;
        }

        switch (c) {
            case 0:
                // If options sets a flag...
                break;
            case 'h':
                help_flag = 1;
                break;
            case 'v':
                verbose_flag = 1;
                break;
            case '?':
                // If an unknown option or an option with a missing
                // required argument is found...
                exit_flag = 1;
                break;
            default:
                abort ();
        }
    }

    if (optind < argc) {
        // If non-option arguments are found...
        exit_flag = 1;
    }
}

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
    if (verbose_flag) {
        printf(PERIOD_MESSAGES[period]);
        printf("\n");
    }

    if (! no_desktop_flag) {
        // System notification using 'notify-send'
        if (command_exists ("notify-send")) {
            char command[STR_LEN];
            strcpy (command, "notify-send --urgency=");
            strcat (command, NOTIFICATION_URGENCY);
            strcat (command, " -a \"Pomodoro\" \"Pomodoro\" \"");
            strcat (command, PERIOD_MESSAGES[period]);
            strcat (command, "\"");
            system (command);
        }
    }

    if (! no_sound_flag) {
        // Sound using 'mpv'
        if (command_exists ("mpv")) {
            char command[STR_LEN];
            strcpy (command, "mpv --vo=null --really-quiet \
                    --loop-playlist=1 --loop=0 ");
            strcat (command, NOTIFICATION_SOUND);
            system (command);
        }
    }
}

/**
 * Main
 */
int main (int argc, char *argv[]) {
    parse_options (argc, argv);

    if (exit_flag) {
        printf ("Try 'pomodoro --help' for more information.\n");
        return 1;
    }

    if (help_flag) {
        printf (HELP_STRING);
        return 0;
    }

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

