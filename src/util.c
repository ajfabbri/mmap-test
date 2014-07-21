/**
 * Various helper functions.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <sys/sysinfo.h>    /* sysinfo */

#include "mmap-test.h"

/** Return 0 on success and set total_ram, free_ram, or return error. */
int get_system_ram(unsigned long *total_ram, unsigned long *free_ram)
{
    struct sysinfo info;
    int error = sysinfo(&info);
    if (error < 0)
        return errno;

    *total_ram = info.totalram;
    *free_ram = info.freeram;

    return 0;
}

const char *memory_backing_to_string(enum memory_backing m)
{
    if (m == SHM)
        return "shmem";
    else if (m == RAM_FILE)
        return "ram_file";
    else
        return "";
}

/* How much RAM to use for test? */
unsigned long buffer_size(void) {
#if 1
    unsigned long total = 0, free = 0;
    int error = get_system_ram(&total, &free);
    if (error)
        return 0;
    /* Tweak this to get desired amount of memory pressure. */
    /* Currently I do a weighted average of free and total memory. */
/*    unsigned long use = (unsigned long)
        ((float)total + ((float)free *2)) / 3.0;
        */
    unsigned long use =  (unsigned long)((float)free * USE_RAM_FRACTION); 
    printf("%s: %luMB total, %luMB free\n -> Use %luMB.\n", __func__,
        byte_to_mb(total), byte_to_mb(free), byte_to_mb(use));
    return use;
#else
    return 1024000;
#endif
}


// vim: ts=8 et sw=4 sts=4
