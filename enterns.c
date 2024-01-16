   #define _GNU_SOURCE
       #include <err.h>
       #include <fcntl.h>
       #include <sched.h>
       #include <stdio.h>
       #include <stdlib.h>
       #include <unistd.h>

       int
       main(int argc, char *argv[])
       {
           int fd;

           if (argc < 6) {
               fprintf(stderr, "%s /proc/PID/ns/FILE cmd args...\n", argv[0]);
               exit(EXIT_FAILURE);
           }

           /* Get file descriptor for namespace; the file descriptor is opened
              with O_CLOEXEC so as to ensure that it is not inherited by the
              program that is later executed. */

           fd = open(argv[1], O_RDONLY | O_CLOEXEC);
           if (fd == -1)
               err(EXIT_FAILURE, "open");

           if (setns(fd, 0) == -1)       /* Join that namespace */
               err(EXIT_FAILURE, "setns");

fd = open(argv[2], O_RDONLY | O_CLOEXEC);
           if (fd == -1)
               err(EXIT_FAILURE, "openb");

           if (setns(fd, 0) == -1)       /* Join that namespace */
               err(EXIT_FAILURE, "setns");

fd = open(argv[3], O_RDONLY | O_CLOEXEC);
           if (fd == -1)
               err(EXIT_FAILURE, "openc");

           if (setns(fd, 0) == -1)       /* Join that namespace */
               err(EXIT_FAILURE, "setns");


fd = open(argv[4], O_RDONLY | O_CLOEXEC);
           if (fd == -1)
               err(EXIT_FAILURE, "openc");

           if (setns(fd, 0) == -1)       /* Join that namespace */
               err(EXIT_FAILURE, "setns");

           system(argv[5]);
       }
