/** ***********************************************************
    @file logthread.c
    @author Snow Tempest
    @date Aug 31, 2022
    @brief  Program using threads to find the log of a value between 0 - 2

C program that uses pthreads to calculate the ln(x) of a value between
0 and 2. The program takes in 3 inputs:The value to calculate ln(x) for,
the number of threads to run, and the number of iterations.

Compile by: gcc -Wall prog1.c -o prog1 -lpthread -lm
    or by makefile.

Compiler: gcc
Company: Me

*************************************************************** **/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <string.h>

/* Function Declarations */
void exitMsg();
void * log_thread_function(void *data);

/* Global variables */
pthread_mutex_t LOCK = PTHREAD_MUTEX_INITIALIZER;
double x;
int MAX_THREADS;
int ITERATIONS;
double LOG_VAL = 0;

/** **************************************************************
    @brief log pthread function

    The function is used as a pthread function using the pthread function
    prototype. This function will be used to have the threads calculate
    an approximate value for the ln(x) value.

    @param[in] value - void * pointer which points to the value used in the threads
    @return Returns - void *
**************************************************************** */

void * log_thread_function(void *value) {

    int initial = *((int*)value);
    int i = 0;
    double rlog = 0;

    for (i = 0; i < ITERATIONS; i++) {

        int n = initial + i * MAX_THREADS;

        if (n % 2 == 0) {
            rlog -= pow((x-1), n) /  n;

        }
        else {
            rlog += pow((x-1), n) /  n;
        }
    }

    if(pthread_mutex_lock(&LOCK) != 0) {
        printf("Error requesting lock\n");
    }
        
    LOG_VAL += rlog;
    pthread_mutex_unlock(&LOCK);

    pthread_exit(value);
}

int main (int argc, char *argv[]) {

    if (argc != 4) {
        printf("Not enough arguments given.\nPlease input the 3 arguments needed.\n");
        exitMsg();
    }
    else {

        int i;
        x = atof(argv[1]);
        MAX_THREADS = atoi(argv[2]);
        ITERATIONS = atoi(argv[3]);

        if (x <= 0 || x > 2) {
            printf("Please enter a correct x value for ln(x) between 0 - 2\n");
            exitMsg();
        }

        pthread_t threads[MAX_THREADS];
        int iterations[MAX_THREADS];

        if(pthread_mutex_init(&LOCK, NULL) != 0) {
            printf("Error initializing the lock!\n");
        }

        for (i = 0; i < MAX_THREADS; i++) {
            iterations[i] = i + 1;
            int thread_create_status = pthread_create(&threads[i], NULL,
                log_thread_function, ((void*) &iterations[i]));

            if(thread_create_status != 0) {
                fprintf(stderr, "pthread_create() has failed: %s\n", strerror(errno));
            }
        }

        for (i = 0; i < MAX_THREADS; i++) {
            int * thread_retval;
            int join_retval;

            join_retval = pthread_join(threads[i], (void**) (&thread_retval));

            if (join_retval != 0)
            {
                fprintf(stderr, "pthread_join() has failed: %s\n", strerror(errno));
            }
        }


        //Get rid of lock.
        pthread_mutex_destroy(&LOCK);

        printf("Value Calculated by the threads: %.14f\n", LOG_VAL);
        printf("Value Calculated by the math function: %.14f\n\n", log(x));

        printf("%.14f\n", LOG_VAL);
        printf("%.14f\n", log(x));
        return 0;
    }

}

/** **************************************************************
    @brief exit function

    The function simply tells the user that the program is exiting for
    whatever reason that may be.
**************************************************************** */
void exitMsg() {

    printf("Program will now exit!\n");
    exit(0);
}