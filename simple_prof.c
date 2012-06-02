// What is simple-prof?
// ====================
// simple-prof is for students learning to write performant C routines.
// It makes it simple to record the execution times of many trials of a routine
// and report simple stats on those measurements (min, avg, stdev).
//
// How do I use simple-prof?
// =========================
// Include simple-prof with `#include "simple_prof.h"`, compile with:
//     gcc -lrt -lm simple_prof.c your-program.c
// 
// Typical usage:
//   1. create a structure to store runs of timing data:
//
//      p_data func_a_data = prof_init_data(NUM_TRIALS)
//
//   2. for each trial, call the start and stop functions around your
//   code-block:
//
//      prof_start_trial(&fn_data);
//      fn();
//      prof_stop_trial(&fn_data);
//
//   3. calculate some stats from the collected data:
//
//      p_stats fn_stats = prof_get_stats(fn_data);
//
//   4. optionally use this convienence function to print the stats:
//
//      prof_print_stats(fn_stats); 
//
// simple-prof uses a high resolution timer that should be unaffected by
// changes to the system clock due to things like NTP jumps and skews.

#include "simple_prof.h"

p_data prof_init_data(int max_trials) {
    p_data data;
    data.max_trials = max_trials;
    data.next_idx = 0;
    data.deltas = malloc(sizeof(long) * max_trials); // in microseconds

    if (data.deltas == NULL) {
        fprintf(stderr, "Could not allocate memory for a `p_data` with %d many trials.", 
                max_trials);
        exit(1);
    }

    return data;
}

void prof_start_trial(p_data *data) {
    if (data->next_idx >= data->max_trials) {
        fprintf(stderr, "The p_data structure ran out of space for measurements.\n");
        exit(1);
    }

    // keep this line at the bottom of the function so that it's most
    // temporally local to the work that comes after this function returns.
    clock_gettime(CLOCK_MONOTONIC, &data->start_time);
}

void prof_stop_trial(p_data *data) {
    struct timespec stop_time;
    clock_gettime(CLOCK_MONOTONIC, &stop_time);
    data->deltas[data->next_idx] = timespec_delta_in_microseconds(
            stop_time, data->start_time);
    data->next_idx++;
}

// Our timer is in nanoseconds (10^-9 s). Only ~4.2 seconds worth of
// nanoseconds can fit in a long. This is too short for representing run-times.
// To work around this, we can either store runtimes...
//   1. with nanosecond precision in a long long (plenty of time fits) or,
//   2. with microsecond (10^-6 s) precision in a long (~1.2 hours fits),
//
// We somewhat arbitrarily use option 2, partly because we think microsecond
// resolution will be more convienent than nanosecond resolution for typical
// uses of this profiler.
long timespec_delta_in_microseconds(struct timespec time_a, struct timespec time_b) {
    long microseconds = (time_a.tv_sec - time_b.tv_sec) * 1000000; // 10^6µs/1s
    microseconds += (time_a.tv_nsec - time_b.tv_nsec) / 1000.0; // 1µs/10^3ns
    return microseconds;
}

p_stats prof_get_stats(p_data data) {
    p_stats stats;
    // since the user may not have recorded as many trials as the p_data can
    // hold, we look at the next_idx value to see how many deltas were
    // actually recorded.
    stats.num_trials = data.next_idx;

    stats.min = data.deltas[0];
    stats.max = data.deltas[0];
    stats.avg = data.deltas[0] / (double) stats.num_trials;

    int i;
    for (i = 1; i < stats.num_trials; i++) {
        if (data.deltas[i] < stats.min) { stats.min = data.deltas[i]; }
        if (data.deltas[i] > stats.max) { stats.max = data.deltas[i]; }
        stats.avg += data.deltas[i] / (double) stats.num_trials;
    }

    // find sum of squares
    double ss = 0;
    for (i = 0; i < stats.num_trials; i++) {
        ss += (data.deltas[i] - stats.avg) * (data.deltas[i] - stats.avg);
    }
    stats.stdev = sqrt(ss / stats.num_trials);

    return stats;
}

void prof_print_stats(p_stats stats) {
    printf("    N: %d\n", stats.num_trials);
    printf("  Min: %4ld µs\n", stats.min);
    printf("  Max: %4ld µs\n", stats.max);
    printf("  Avg: %8.3lf µs\n", stats.avg);
    printf("StDev: %8.3lf\n", stats.stdev);
}


/* For creating output suitable for easily importing into a spreadsheet, use
 * these csv routines to generate comma-separated output. */

void prof_print_csv_header() {
    printf("N,Min(µs),Max(µs),Avg(µs),StDev\n");
}

void prof_print_csv_stats(p_stats stats) {
    printf("%d,%ld,%ld,%lf,%lf\n",
            stats.num_trials,
            stats.min,
            stats.max,
            stats.avg,
            stats.stdev);
}
