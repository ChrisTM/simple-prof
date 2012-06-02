/* Example of using simple-prof.
 * Compile with:
 *     gcc -lrt -lm simple-prof.c example.c
 */

#include "simple_prof.h"

void function_a() {
    int i;
    int sum = 0;
    for (i = 0; i < 1000; i++) { sum += i; }
}

void function_b() {
    int i;
    double prod = 1;
    for (i = 0; i < 1000; i++) { prod *= i; }
}

int main() {
    int num_trials = 1000;

    // Create a p_data for each thing you want to time. 
    // Timing data will be saved to these structures.
    p_data data_a = prof_init_data(num_trials);
    p_data data_b = prof_init_data(num_trials);

    // Record a number timings for each thing.
    int trial_num;
    for (trial_num = 0; trial_num < num_trials; trial_num++) {
        prof_start_trial(&data_a);
        function_a();
        prof_stop_trial(&data_a);

        prof_start_trial(&data_b);
        function_b();
        prof_stop_trial(&data_b);
    }

    // Calculate simple stats from the recorded trial timings.
    p_stats stats_a = prof_get_stats(data_a);
    p_stats stats_b = prof_get_stats(data_b);

    // Print the results!
    printf("function_a's results:\n");
    prof_print_stats(stats_a);

    printf("\nfunction_b's results:\n");
    prof_print_stats(stats_b);

    printf("\nfunction_a is %lf times faster than function_b.\n",
            stats_b.avg / stats_a.avg);

    return 0;
};
