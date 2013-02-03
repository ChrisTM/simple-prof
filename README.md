What is simple-prof?
====================

simple-prof is for students learning to write performant C routines.  It makes
it simple to record the execution times of many trials of a routine and report
simple stats on those measurements (like min, avg, stdev).


How do I use simple-prof?
=========================

Include simple-prof with `#include "simple_prof.h"`, compile with:

    gcc -lrt -lm simple_prof.c your-program.c

Typical usage:

  1. create a structure for storing collected timing data:

         prof_data fn_data = prof_init_data()

  2. call the start and stop functions around your code-block:

         prof_start_trial(&fn_data);
         fn();
         prof_stop_trial(&fn_data);

     You can do this in a loop to collect many timings.

  3. use a function to print the stats:

         prof_print_stats(fn_data);

     or for comma-separated-values:

         prof_print_csv_header();
         prof_print_csv_stats(fn_data);


Other Notes
===========

simple-prof uses a high resolution timer that should be unaffected by changes
to the system clock due to things like NTP jumps and skews.
