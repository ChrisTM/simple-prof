#ifndef SIMPLE_PROF
#define SIMPLE_PROF

#include <time.h> // for clock_gettime and the timespec struct
#include <stdlib.h> // for malloc
#include <math.h> // for sqrt
#include <stdio.h>

struct prof_data {
    struct timespec start_time; // updated on each prof_trial_start
    int n; // the number of trials that have been recorded
    long min;
    long max;
    double avg;
    double sum_sqrs; // a slight variant used for Knuth-Welford online variance
    double variance;
};

long timespec_delta_in_microseconds(struct timespec, struct timespec);

struct prof_data prof_init_data();
void prof_start_trial(struct prof_data *);
void prof_stop_trial(struct prof_data *);

void prof_print_stats(struct prof_data);
void prof_print_csv_stats(struct prof_data);
void prof_print_csv_header();

#endif
