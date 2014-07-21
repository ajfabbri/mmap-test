/**
 *
 */
#ifndef __SHMEM_H__
#define __SHMEM_H__

#define USE_RAM_FRACTION    0.9
#define NUM_ITERATIONS      1000

#define TOUCH_STRIDE        4095

enum memory_backing {
    SHM,
//    ANON,
    RAM_FILE,
//    ROOT_FS_FILE
};

/* mapped-ram-file.c */
int ramfile_mem_create(unsigned long bytes, int *out_fd, void **out_ptr);
void ramfile_mem_destroy(unsigned long bytes, int fd, void *ptr);

/* shmem.c */
void shmem_destroy(unsigned long bytes, int fd, void *ptr);
int shmem_create(unsigned long bytes, int *out_fd, void **out_ptr);

#ifndef __SHMEM_H__
// vim: ts=8 et sw=4 sts=4
