mmap-test
=========

Testing performance of writing to large memory-mapped buffers.

Aaron Fabbri July 2014

Motivation
----------

Initial goal was to show whether or not using shared memory APIs has measurably
smaller overhead than using a memory mapped ram file.

Still useful as examples of how to allocate large chunks of memory via mmap.

How To Build / Run
------------------

1. Clone this repository.

2. Generate build files:

```
  ./autogen.sh
```

3. Build it. 

```
 $ mkdir build && cd build
 $ ../configure
 $ make
```

4. Run the test.

```
$ ./mmap-test 
```

Tuning
------

Currently, most tunable parameters are compile-time constants.  Edit
mmap-test.h.
