#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

// Text formatting constants
#define RESET "\033[0m"
#define BOLD "\033[1m"

#define STR_LEN 128 // Default length for strings

// Help command string
#define HELP_STRING \
    "Usage: pomodoro [OPTION...]\n\n" \
    "  -h, --help          display this help and exit\n\n" \
    "  -v, --verbose       print notifications to stdout\n" \
    "      --no-sound      no sound notifications\n" \
    "      --no-desktop    no desktop notifications\n\n" \
    "  -w, --work=MIN      set Work period time to MIN " \
            "(default 25)\n" \
    "  -s, --short=MIN     set Short Break period time to MIN " \
            "(default 5)\n" \
    "  -l, --long=MIN      set Long Break period time to MIN " \
            "(default 30)\n"

#define NOTIFICATION_SOUND "/usr/share/pomodoro/notification.mp3"
#define NOTIFICATION_URGENCY "critical" // Can be: low / normal / critical

#define CICLE 4 // Amount of work periods until long break
#define WORK_TIME 25 // Default work period time
#define SHORT_BREAK_TIME 5 // Default short break period time
#define LONG_BREAK_TIME 30 // Default long break period time

enum period{work, short_break, long_break};

int PERIOD_TIMES[3] = {WORK_TIME, SHORT_BREAK_TIME, LONG_BREAK_TIME};
const char PERIOD_NAMES[3][STR_LEN] = {
    "Work period",
    "Short Break period",
    "Long Break period"
};
const char PERIOD_MESSAGES[3][STR_LEN] = {
    "Time to work, you can do this!",
    "Enjoy a short break :)",
    "You have a longer break now!"
};

// Flags
int exit_flag, help_flag, verbose_flag, no_sound_flag, no_desktop_flag;

/**
 * Sets the time for 'period' to the number in 'str' if possible. If not,
 * keeps the previous time.
 */
void set_period_time (int period, const char *str) {
    int time = (int) strtol (str, NULL, 10);
    if (time > 0) {
        PERIOD_TIMES[period] = time;
        printf ("%s time set to %d min.\n",
                PERIOD_NAMES[period], PERIOD_TIMES[period]);
    } else {
        fprintf (stderr, "'%s' min is not a valid period time.\n", str);
        fprintf (stderr, "%s time is still %d min.\n",
                PERIOD_NAMES[period], PERIOD_TIMES[period]);
    }
}

/**
 * Parses the command options and make the necessary changes accordingly.
 */
void parse_options (int argc, char *argv[]) {
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'},
        {"no-sound", no_argument, &no_sound_flag, 1},
        {"no-desktop", no_argument, &no_desktop_flag, 1},
        {"work", required_argument, 0, 'w'},
        {"short-break", required_argument, 0, 's'},
        {"long-break", required_argument, 0, 'l'},
        {0, 0, 0, 0} // To terminate array
    };

    int option_index = 0; // 'getopt_long' stores the option index here
    int c; // Option character code

    while (1) {
        c = getopt_long (argc, argv, "hvw:s:l:", long_options,
                &option_index);

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
            case 'w':
                set_period_time (work, optarg);
                break;
            case 's':
                set_period_time (short_break, optarg);
                break;
            case 'l':
                set_period_time (long_break, optarg);
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

/**
 * Returns boolean value depending if 'command' exists in the user's
 * system.
 */
int command_exists (char command[STR_LEN]) {
    char which_command[STR_LEN * 2];
    snprintf (which_command, STR_LEN * 2, "which %s > /dev/null 2>&1",
            command);
    if (system (which_command)) {
        fprintf (stderr, "Command '%s' not found.", command);
        return 0;
    }
    return 1;
}

/**
 * Notifies the user about the change of the Pomodoro period. It can notify
 * in different ways depending on the options set.
 */
void notify (int period) {
    char title[STR_LEN * 2];
    snprintf (title, STR_LEN * 2, "%s of %d min",
            PERIOD_NAMES[period], PERIOD_TIMES[period]);
    char body[STR_LEN];
    strcpy (body, PERIOD_MESSAGES[period]);

    if (verbose_flag) {
        // Output to stdout
        printf ("\n" BOLD "  %s\n" RESET "  %s\n\n", title, body);
    }

    if (! no_desktop_flag) {
        // System notification using 'notify-send'
        if (command_exists ("notify-send")) {
            char command[STR_LEN * 4];
            snprintf (command, STR_LEN * 4,
                    "notify-send --urgency=%s -a Pomodoro \"%s\" \"%s\"",
                    NOTIFICATION_URGENCY, title, body);
            system (command);
        }
    }

    if (! no_sound_flag) {
        // Sound using 'mpv'
        if (command_exists ("mpv")) {
            char command[STR_LEN * 2];
            snprintf (command, STR_LEN * 2, "mpv -vo=null --really-quiet \
                    --loop-playlist=1 --loop=0 %s", NOTIFICATION_SOUND);
            system (command);
        }
    }
}

/**
 * Main.
 * Loops through the Pomodoro periods and notifies the user accordingly.
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
    int period = work;
    start_time = time (NULL);
    notify (period);

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

            notify (period);
        }

        sleep (60); // sleeps 1 minute
    }

    return 0;
}

