#ifndef SIMPLE_PROF
#define SIMPLE_PROF

#include <time.h> // for clock_gettime and the timespec struct
#include <stdlib.h> // for malloc
#include <math.h> // for sqrt
#include <stdio.h>

typedef struct p_data {
    int max_trials; // the number of trials that can be recorded
    int next_idx; // next available index for writing into deltas
    struct timespec start_time; // updated on each prof_trial_start
    long *deltas;
} p_data;

typedef struct p_stats {
    int num_trials;
    long min;
    long max;
    double avg;
    double stdev;
} p_stats;

p_data prof_init_data(int);
void prof_start_trial(p_data *);
void prof_stop_trial(p_data *);
p_stats prof_get_stats(p_data);
long timespec_delta_in_microseconds(struct timespec, struct timespec);

#endif
