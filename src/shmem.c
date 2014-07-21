/**
 * Helpers for getting / releasing shared memory mappings.
 */
#include "mmap-test.h"
#define SHM_NAME	"mmap-test.shm"

/** Return 0 on success and set out_fd, out_ptr, or return error. */
int shmem_create(unsigned long bytes, int *out_fd, void **out_ptr)
{
    /* get shared memory context */
    int fd = shm_open(SHM_NAME, O_CREAT | O_TRUNC | O_RDWR, 0660);
    void *ptr;

    if (fd < 0)
        return errno;

    /* set size of shared memory region */
    int error = ftruncate(fd, (off_t)bytes);
    if (error < 0) {
        shmem_destroy(bytes, fd, NULL);
        return errno;
    }

    /* map shared memory into our virt address space */
    ptr = mmap(NULL, bytes, PROT_READ | PROT_WRITE,
            MAP_SHARED /*| MAP_HUGETLB*/, fd, 0);
    if (ptr == (void *)-1) {
        shmem_destroy(bytes, fd, NULL);
        return errno;
    }
        
    *out_ptr = ptr;
    *out_fd = fd;
    return 0;
}

void shmem_destroy(unsigned long bytes, int fd, void *ptr) {
    (void)shm_unlink(fd);

    if (ptr != NULL)
        (void)munmap(ptr, bytes);
}


// vim: ts=8 et sw=4 sts=4
