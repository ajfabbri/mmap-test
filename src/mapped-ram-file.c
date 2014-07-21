/**
 * Helpers for getting / releasing mapped memory backed by a file from a ram
 * disk FS.  *Not the way to do shared memory.*
 */
#include <sys/mount.h>
#include <string.h>

#include "mmap-test.h"

#define TMPFS_MOUNT_POINT   "/mnt/tmpfs"
#define RAM_FILE_NAME	    TMPFS_MOUNT_POINT "/mmap-test"

static int mount_tmpfs(const char* mount_path)
{
    int error = mount("none", mount_path, "tmpfs", 0 /*mntflags*/,
        NULL);
    if (error) {
        printf("%s: error %d %s\n", __func__, error, strerror(error));
        return errno;
    else
        return 0;
}

static void umount_tmpfs(const char* mount_path)
{
    int error = umount(mount_path);
    if (error)
        printf("%s: error %d %s\n", __func__, errno, strerror(errno));
}

/** Return 0 on success and set out_fd, out_ptr, or return error. */
int ramfile_mem_create(unsigned long bytes, int *out_fd, void **out_ptr)
{

    /* mount tmpfs */
    int error = mount_tmpfs(TMPFS_MOUNT_POINT);
    if (error)
        goto out;

    /* open a file in tmpfs */
    int fd = open(RAM_FILE_NAME, O_CREAT|O_TRUNC);
    if (fd < 0) {
        printf("%s: open error %d %s\n", __func__, errno, strerror(errno));
        error = errno;
        goto out_umount;
    }

    /* set size of file */
    int error = ftruncate(fd, (off_t)bytes);
    if (error < 0) {
        error = errno;
        goto out_close;
    }

    /* map shared memory into our virt address space */
    ptr = mmap(NULL, bytes, PROT_READ | PROT_WRITE,
            MAP_SHARED /*| MAP_HUGETLB*/, fd, 0);
    if (ptr == (void *)-1) {
        error = errno;
        goto out_close;
    }
        
    *out_ptr = ptr;
    *out_fd = fd;
    goto out;

out_close:
    close(fd);
out_umount:
    umount_tmpfs(TMPFS_MOUNT_POINT);
out:
    return error;
}

void ramfile_mem_destroy(unsigned long bytes, int fd, void *ptr) {

    if (ptr != NULL)
        (void)munmap(ptr, bytes);

    close(fd);

    umount(TMPFS_MOUNT_POINT);
}


// vim: ts=8 et sw=4 sts=4

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

/* shmem.c */
void shmem_destroy(unsigned long bytes, int fd, void *ptr);
int shmem_create(unsigned long bytes, int *out_fd, void **out_ptr);

#ifndef __SHMEM_H__
// vim: ts=8 et sw=4 sts=4
