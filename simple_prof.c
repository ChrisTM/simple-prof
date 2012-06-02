#include "simple_prof.h"

struct prof_data prof_init_data() {
    struct prof_data data;
    data.n = 0;
    return data;
}

void prof_start_trial(struct prof_data *data) {
    clock_gettime(CLOCK_MONOTONIC, &data->start_time);
}

void prof_stop_trial(struct prof_data *data) {
    struct timespec stop_time;
    clock_gettime(CLOCK_MONOTONIC, &stop_time);

    long duration = timespec_delta_in_microseconds(
            stop_time,
            data->start_time);
    data->n += 1;


    // initialize data's values when the first datapoint is recorded
    if (data->n == 1) {
        data->min = duration;
        data->max = duration;
        data->avg = duration;
        data->variance = 0.0;
        data->sum_sqrs = 0.0;
    } else {
        // compute running mean and variance
        double residual = duration - data->avg;
        data->avg += residual / data->n;
        data->sum_sqrs += residual * (duration - data->avg);
        data->variance = data->sum_sqrs / data->n;

        if (duration < data->min) { data->min = duration; }
        if (duration > data->max) { data->max = duration; }
    }
}

// Our timer is in nanoseconds (10^-9 s). Only ~4.2 seconds worth of
// nanoseconds can fit in a long. This is too short for representing run-times.
// To work around this, we can either store runtimes...
//   1. with nanosecond precision in a long long (plenty of time fits) or,
//   2. with microsecond (10^-6 s) precision in a long (~1.2 hours fits),
//
// We somewhat arbitrarily use option 2, partly because we think microsecond
// resolution will be more convenient than nanosecond resolution for typical
// uses of this profiler.
long timespec_delta_in_microseconds(struct timespec time_a, struct timespec time_b) {
    long microseconds = (time_a.tv_sec - time_b.tv_sec) * 1000000; // 10^6µs/1s
    microseconds += (time_a.tv_nsec - time_b.tv_nsec) / 1000.0; // 1µs/10^3ns
    return microseconds;
}

void prof_print_stats(struct prof_data data) {
    printf("    N: %d\n", data.n);
    printf("  Min: %4ld µs\n", data.min);
    printf("  Max: %4ld µs\n", data.max);
    printf("  Avg: %8.3lf µs\n", data.avg);
    printf("StDev: %8.3lf\n", sqrt(data.variance));
}


/* For creating output suitable for easily importing into a spreadsheet, use
 * these csv routines to generate comma-separated output. */

void prof_print_csv_stats(struct prof_data data) {
    printf("%d,%ld,%ld,%lf,%lf\n",
            data.n,
            data.min,
            data.max,
            data.avg,
            sqrt(data.variance));
}

void prof_print_csv_header() {
    printf("N,Min(µs),Max(µs),Avg(µs),StDev\n");
}
