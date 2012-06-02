/* Example of using simple-prof.
 * Compile with:
 *     gcc -lrt -lm simple-prof.c example.c
 */

#include "simple_prof.h"

void function_a() {
    int i;
    int sum = 0;
    for (i = 0; i < 10000; i++) { sum += i; }
}

void function_b() {
    int i;
    double prod = 1;
    for (i = 0; i < 10000; i++) { prod *= i; }
}

int main() {
    int trial_num;
    // Create a p_data for each thing you want to time. 
    p_data data_a = prof_init_data();
    p_data data_b = prof_init_data();

    // Record a number timings for each thing.
    for (trial_num = 0; trial_num < 1000; trial_num++) {
        prof_start_trial(&data_a);
        function_a();
        prof_stop_trial(&data_a);

        prof_start_trial(&data_b);
        function_b();
        prof_stop_trial(&data_b);
    }

    // Print the results!
    printf("function_a's results:\n");
    prof_print_stats(data_a);

    printf("\nfunction_b's results:\n");
    prof_print_stats(data_b);

    printf("\nfunction_a is %lf times faster than function_b.\n",
            data_b.avg / data_a.avg);

    return 0;
};
