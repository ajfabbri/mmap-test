/**
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

/* sysinfo */
#include <sys/sysinfo.h>
        
#include "mmap-test.h"

static int mmap_test(enum memory_backing mem)
{
    unsigned long bytes = buffer_size();;
    int fd = -1, error = EINVAL;
    void *ptr;

    if (bytes == 0) {
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

    printf("%s: mapped %lu bytes of memory.\n", __func__, bytes);

    long i = 0;
    char *cp = (char *)ptr;
    while (i < NUM_ITERATIONS) {
        /* write some value at this memory location */
        *cp = (char)(i & 0xff);
    
        cp += TOUCH_STRIDE;

        /* Wrap when we get to the end of memory region */
        if (cp > ((char *)ptr + bytes)) {
            cp -= bytes;
            i++;
        }
    }
    switch (mem) {
        case SHM:
            shmem_destroy(bytes, fd, ptr);
            break;
        case RAM_FILE:
            ramfile_mem_destroy(bytes, fd, ptr);
            break;
    }
out:
    if (error)
        printf("%s: filed with error %d\n", __func__, error);
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
