// simple-prof is for students learning to write performant functions in C.
//
// Needs to be linked with RT; use -lrt.
//
// Typical usage involves creating a structure to store timing data for each
// trial. Then, for each trial call the stopwatch-like start and stop functions
// around your desired function. Finally, call the function to calculate simple
// timing states from the data recorded in the timing data structure.
//
// simple-prof uses a high resolution timer that should be unaffected by changes
// to the system clock due to things like NTP jumps and skews.

#include <time.h> // for clock_gettime and the timespec struct
#include <stdio.h>

typedef struct p_data {
    int num_trials;
    // used to determine which index to save timespecs into
    int current_trial;
    struct timespec *start_times;
    struct timespec *end_times;

    // TODO: Do timespec diffing on prof_trial_end (and only keep a deltas
    // array and single start timespec)? Or keep start and end timespec arrays
    // and do all the time diffing later?

    // timespec deltas are stored here, in nanoseconds
    long int *deltas;
} p_data;

typedef struct p_stats {
    // units are in terms of nanoseconds
    int min;
    int max;
    double avg;

    double stdev;
} p_stats;

// might be good to check in prof_create that clock_gettime won't error
p_data prof_create(int);
void prof_start_trial(p_data);
void prof_end_trial(p_data);
p_stats prof_get_stats(p_data);


int main() {
    // just a test
    struct timespec res;
    clock_getres(CLOCK_MONOTONIC, &res);
    printf("Timer resolution is %ld", res.tv_nsec);

    return 1;
};
