/**
 *
 */
#ifndef __MEMMAP_TEST_H__
#define __MEMMAP_TEST_H__


/* Compile-time constants. */

/** Use this much of free ram. */
#define USE_RAM_FRACTION    0.70

/** If there is less than this much free ram, exit! */
#define MIN_FREE_RAM        (1000 * 1000 * 200)

/** Each iteration touches all pages in the buffer. */  
#define NUM_ITERATIONS      (10*1000)

/** One word shy of regular page size, for cruelty. */
#define TOUCH_STRIDE        4092


/* Static inlines. */

static inline unsigned long byte_to_mb(unsigned long bytes) {
    return bytes / (1<<20);
}


/* Shared data types */

enum memory_backing {
    SHM,
//    ANON,
    RAM_FILE,
//    ROOT_FS_FILE
};


/* Exposed APIs. */

/* mapped-ram-file.c */
int ramfile_mem_create(unsigned long bytes, int *out_fd, void **out_ptr);
void ramfile_mem_destroy(unsigned long bytes, int fd, void *ptr);

/* shmem.c */
void shmem_destroy(unsigned long bytes, int fd, void *ptr);
int shmem_create(unsigned long bytes, int *out_fd, void **out_ptr);

/* util.c */
int get_system_ram(unsigned long *total_ram, unsigned long *free_ram);
const char *memory_backing_to_string(enum memory_backing m);
unsigned long buffer_size(void);

#endif /* __MMAP_TEST_H__ */
// vim: ts=8 et sw=4 sts=4
