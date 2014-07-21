/**
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* shm_*() */
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

/* sysinfo */
#include <sys/sysinfo.h>
        

#include "mmap-test.h"

#define SHM_NAME	"mmap-test.shm"
#define USE_RAM_FRACTION    0.9
#define NUM_ITERATIONS      1000

#define TOUCH_STRIDE        4095


/** Return 0 on success and set total_ram, free_ram, or return error. */
static int get_system_ram(unsigned long *total_ram, unsigned long *free_ram)
{
    struct sysinfo info;
    int error = sysinfo(&info);
    if (error < 0)
        return errno;

    *total_ram = info.totalram;
    *free_ram = info.freeram;
    return 0;
}

static unsigned long buffer_size(void) {
    unsigned long total, free;
    int error = get_system_ram(&total, &free);
    if (error)
        return 0;
    return (unsigned long)((float)free * USE_RAM_FRACTION);
}

static int mmap_test(enum memory_backing mem)
{
    unsigned long bytes = buffer_size();;
    if (bytes == 0)
        return ENOMEM;

    int fd = -1, error = EINVAL;
    void *ptr;

    switch (mem) {
        case SHM:
            error = shmem_create(bytes, &fd, &ptr);
            break;
        case RAM_FILE:
            error = ramfile_mem_create(bytes, &fd, &ptr);
            break;
    }
    if (error)
        return error;

    long i = 0;
    char *cp = (char *)ptr;
    while (i < NUM_ITERATIONS) {
        /* write some value at this memory location */
        *cp = (char)(i & 0xff);
    
        cp += TOUCH_STRIDE;

        /* Wrap when we get to the end of memory region */
        if (cp > (ptr + bytes)) {
            cp -= bytes;
            i++;
        }
    }
    switch (mem) {
        case SHM:
            error = shmem_destroy(bytes, fd, ptr);
            break;
        case RAM_FILE:
            error = ramfile_mem_destroy(bytes, fd, ptr);
            break;
    }
    if (error)
        return error;
}

static void usage(const char* appname) 
{
    printf("%s: shmem | tmpfs\n", appname);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{

    if (argc != 2)
        usage();

    enum memory_backing mem_type;
    if (strcmp("shmem", argv[1]) == 0)
        mem_type = SHM;
    else if (strcmp("tmpfs", argv[1]) == 0)
        mem_type = RAM_FILE;
    else
        usage();

    int error = mmap_test(mem_type);
    if (error)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}

// vim: ts=8 et sw=4 sts=4
