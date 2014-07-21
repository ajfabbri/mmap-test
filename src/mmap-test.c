/**
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <sys/sysinfo.h>    /* sysinfo */

#include <time.h>           /* clock_gettime */
        
#include "mmap-test.h"

static void print_stats(struct timespec *start_t, struct timespec *end_t,
    unsigned long num_writes, unsigned long num_iterations)
{
    /* get elapsed wall time */
    struct timespec delta;
    delta.tv_sec = end_t->tv_sec - start_t->tv_sec;
    delta.tv_nsec = end_t->tv_nsec - start_t->tv_nsec;
    /* handle nsec wrapping */
    if (delta.tv_nsec < 0) {
        delta.tv_nsec += 1e9;
        delta.tv_sec--;
    }

    double seconds = delta.tv_sec + (delta.tv_nsec/1e9);
    double iter_per_sec = (double)num_iterations / seconds;
    double mwrite_per_sec = ((double)num_writes/1e6) / seconds;

    printf("%s: Time: %.4f, Mwrites/sec: %.4f, iteration/sec: %.4f\n", __func__,
        seconds, mwrite_per_sec, iter_per_sec);
}


static int mmap_test(enum memory_backing mem)
{
    unsigned long bytes = buffer_size();
    int fd = -1, error = EINVAL;
    void *ptr;
    struct timespec start_t, end_t;

    if (bytes == 0 || bytes < MIN_FREE_RAM) {
        error = ENOMEM;
        goto out;
    }

    switch (mem) {
        case SHM:
            error = shmem_create(bytes, &fd, &ptr);
            break;
        case RAM_FILE:
            error = ramfile_mem_create(bytes, &fd, &ptr);
            break;
    }
    if (error)
        goto out;


    long i = 0;
    char *cp = (char *)ptr;
    char *end = ((char *)ptr) + bytes;
    printf("%s: mapped %lu bytes of memory at %p - %p.\n", __func__, bytes, ptr,
        end);
    unsigned long num_writes = 0;
    clock_gettime(CLOCK_MONOTONIC, &start_t);
    while (i < NUM_ITERATIONS) {
        /* write some value at this memory location */
        *cp = (char)(i & 0xff);
        num_writes++;
    
        cp += TOUCH_STRIDE;

        /* Wrap when we get to the end of memory region */
        if (cp >= ((char *)ptr + bytes)) {
            cp -= bytes;
            i++;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end_t);
    printf("%s: done writing.\n", __func__);

    switch (mem) {
        case SHM:
            shmem_destroy(bytes, fd, ptr);
            break;
        case RAM_FILE:
            ramfile_mem_destroy(bytes, fd, ptr);
            break;
    }

    print_stats(&start_t, &end_t, num_writes, NUM_ITERATIONS);

out:
    if (error)
        printf("%s: failed with error %d %s\n", __func__, error,
            strerror(error));
    return error;
}

static void usage(const char* appname) 
{
    printf("\n** Warning **\n This test can hang your box with VM thrashing."
        " Use at your own risk.\n");
    printf("%s: shmem | tmpfs\n", appname);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{

    if (argc != 2)
        usage(argv[0]);

    enum memory_backing mem_type;
    if (strcmp("shmem", argv[1]) == 0)
        mem_type = SHM;
    else if (strcmp("tmpfs", argv[1]) == 0)
        mem_type = RAM_FILE;
    else
        usage(argv[0]);

    int error = mmap_test(mem_type);
    if (error)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}

// vim: ts=8 et sw=4 sts=4
