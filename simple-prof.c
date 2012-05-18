// simple-prof is for students learning to write performant functions in C.
//
// Needs to be linked with RT; use -lrt.
//
// Typical usage involves creating a structure to store timing data for each
// trial. Then, for each trial call the stopwatch-like start and stop functions
// around your desired function. Finally, call the function to calculate simple
// timing states from the data recorded in the timing data structure.
//
// simple-prof uses a high resolution timer that should be unaffected by
// changes to the system clock due to things like NTP jumps and skews.

#include <time.h> // for clock_gettime and the timespec struct
#include <stdlib.h> // for malloc
#include <stdio.h>

typedef struct p_data {
    int num_trials;
    // used to determine which index to save timespecs into
    int current_trial_idx;
    struct timespec *start_times;
    struct timespec *end_times;
} p_data;

typedef struct p_stats {
    // units are in terms of nanoseconds
    int num_trials;
    long min;
    long max;
    double avg;

    double stdev;
} p_stats;

p_data prof_init_data(int);
void prof_start_trial(p_data *);
void prof_end_trial(p_data *);
p_stats prof_get_stats(p_data);
long timespec_delta_in_microseconds(struct timespec, struct timespec);


p_data prof_init_data(int num_trials) {
    p_data data;
    data.num_trials = num_trials;
    data.current_trial_idx = 0;
    data.start_times = malloc(sizeof(struct timespec) * num_trials);
    data.end_times = malloc(sizeof(struct timespec) * num_trials);

    if (data.start_times == NULL || data.end_times == NULL) {
        fprintf(stderr, "Could not allocate memory for a `p_data` of %d many trials.", 
                num_trials);
        exit(1);
    }

    return data;
}

void prof_start_trial(p_data *data) {
    if (data->current_trial_idx >= data->num_trials) {
        fprintf(stderr, "The p_data structure ran out of space for measurements.\n");
        exit(1);
    }

    // keep this line at the bottom of the function so that it's most
    // temporally local to the work that comes after this function returns.
    clock_gettime(CLOCK_MONOTONIC, &data->start_times[data->current_trial_idx]);
}

void prof_end_trial(p_data *data) {
    clock_gettime(CLOCK_MONOTONIC, &data->end_times[data->current_trial_idx]);
    data->current_trial_idx++;
}

long timespec_delta_in_microseconds(struct timespec time_a, struct timespec time_b) {
    long microseconds = (time_a.tv_sec - time_b.tv_sec) * 1000000; // 10^6µs/1s
    microseconds += (time_a.tv_nsec - time_b.tv_nsec) / 1000.0; // 1µs/10^3ns
    return microseconds;
}

p_stats prof_get_stats(p_data data) {
    // Our timer is in nanoseconds (10^-9 s). Only ~4.2 seconds worth of
    // nanoseconds can fit in a long. This is too short for representing
    // run-times. We could store the nanoseconds as a long long, or instead use
    // microsecond (10^-6 s) precision with a long. A long can represent up to
    // ~1.2 hours in nanosecond resolution.
    p_stats stats;
    stats.num_trials = data.num_trials;

    long first_delta = timespec_delta_in_microseconds(
            data.end_times[0],
            data.start_times[0]);
    stats.min = first_delta;
    stats.max = first_delta;
    long long sum = first_delta;

    int i;
    for (i=1; i<data.current_trial_idx; i++) {
        long delta = timespec_delta_in_microseconds(
                data.end_times[i],
                data.start_times[i]);

        if (delta < stats.min) { stats.min = delta; }
        if (delta > stats.max) { stats.max = delta; }
        sum += delta;
    }
    stats.avg = sum / (double) data.num_trials;

    return stats;
}

void prof_print_stats(p_stats stats) {
    printf("N:   %d\n", stats.num_trials);
    printf("Min: %ld µs\n", stats.min);
    printf("Avg: %lf µs\n", stats.avg);
    printf("Max: %ld µs\n", stats.max);
}



void function_a() {
    int i;
    int sum = 0;
    for (i=0; i<1000; i++) { sum += i; }
}

void function_b() {
    int i;
    double prod = 1;
    for (i=0; i<1000; i++) { prod *= i; }
}

int main() {
    // Example usage of the prof_ routines...

    int num_trials = 10000;

    // Create a p_data for each thing you want to time. 
    // Timing data will be saved to these structures.
    p_data data_a = prof_init_data(num_trials);
    p_data data_b = prof_init_data(num_trials);

    // Record a number timings for each thing.
    int trial_num;
    for (trial_num = 0; trial_num < num_trials; trial_num++) {
        prof_start_trial(&data_a);
        function_a();
        prof_end_trial(&data_a);

        prof_start_trial(&data_b);
        function_b();
        prof_end_trial(&data_b);
    }

    // Calculate some simple stats from the recorded trial timings.
    p_stats stats_a = prof_get_stats(data_a);
    p_stats stats_b = prof_get_stats(data_b);

    // Print some results!
    printf("function_a's results:\n");
    prof_print_stats(stats_a);

    printf("\nfunction_b's results:\n");
    prof_print_stats(stats_b);

    printf("\nfunction_a is %lf times faster than function_b.\n",
            stats_b.avg / stats_a.avg);

    return 0;
};
