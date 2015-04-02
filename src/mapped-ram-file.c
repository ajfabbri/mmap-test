/**
 * Helpers for getting / releasing mapped memory backed by a file from a ram
 * disk FS.
 */

#include <sys/types.h>      /* open() */
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>         /* ftruncate() */
#include <sys/types.h>

#include <sys/mman.h>       /* mmap() */

#include <stdio.h>
#include <sys/mount.h>
#include <errno.h>
#include <string.h>

#include "mmap-test.h"

#define TMPFS_MOUNT_POINT   "/mnt/tmpfs"
#define RAM_FILE_NAME	    TMPFS_MOUNT_POINT "/mmap-test"

static int mount_tmpfs(const char* mount_path)
{
    int error = mount("none", mount_path, "tmpfs", 0 /*mntflags*/,
        NULL);
    if (error) {
        printf("%s: error %d %s\n", __func__, errno, strerror(errno));
        printf("* Make sure you've created a directory at %s\n", mount_path);
        return errno;
    } else
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
    int fd = open(RAM_FILE_NAME, O_CREAT|O_TRUNC|O_RDWR, 0664);
    if (fd < 0) {
        printf("%s: open error %d %s\n", __func__, errno, strerror(errno));
        error = errno;
        goto out_umount;
    }

    /* set size of file */
    error = ftruncate(fd, (off_t)bytes);
    if (error < 0) {
        printf("%s: ftruncate(%d, %ld)  error %d %s\n", __func__, fd, (off_t)bytes,
            errno, strerror(errno));
        error = errno;
        goto out_close;
    }

    /* map shared memory into our virt address space */
    void *ptr = mmap(NULL, bytes, PROT_READ | PROT_WRITE,
            MAP_SHARED /*| MAP_HUGETLB*/, fd, 0);
    if (ptr == (void *)-1) {
        printf("%s: mmap error %d %s\n", __func__, errno, strerror(errno));
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

    // TODO check for errors
    unlink(RAM_FILE_NAME);

    ftruncate(fd, 0);
    close(fd);

    umount(TMPFS_MOUNT_POINT);
    printf("%s: cleaned up.\n", __func__);
}

// vim: ts=8 et sw=4 sts=4
