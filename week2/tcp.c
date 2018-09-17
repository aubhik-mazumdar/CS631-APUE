/*
 * CS-631: Advanced Programming in Unix
 * HW#1: trivially copy a file
 * AUTHOR: Aubhik Mazumdar
 * CWID: 10430935
 * FILE: tcp.c
 * usage: ./tcp file1 file2
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#ifndef BUFFSIZE
#define BUFFSIZE 32678
#endif

static void usage(void);

/*
 * This program trivially copies a file to another file specified in the command-line
 * arguments. It returns 0(EXIT_SUCCESS) on success and 1(EXIT_FAILURE) on failure.
 */
int
main(int argc,char **argv) {
    int readfd;
    int writefd;
    int dirfd;
    int n;
    char buf[BUFFSIZE];
    char *bname;
    char *readPath;
    char *writePath;
    struct stat writeStat;
    struct stat readStat;

    setprogname(argv[0]);

    /* Parse arguments, must be = 3 */
    if (argc != 3) {
        usage();
    }

    /* Fetch paths to read and write from command line arguments */
    readPath = argv[1];
    writePath = argv[2];


    /* Get status for read file */
    if (stat(readPath, &readStat) == -1) {
        (void)fprintf(stderr,"Unable to get %s stat: %s\n",readPath,strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    /* Get status for write file, if it fails, we should ignore the error */
    (void)stat(writePath, &writeStat);

    
    /* Check if reading and writing to the same file */
    if ((writeStat.st_dev == readStat.st_dev) && (writeStat.st_ino == readStat.st_ino)) {
        (void)fprintf(stderr,"ERROR: Trying to write to same file: %s\n",writePath);
        exit(EXIT_FAILURE);
    }


    if ((readfd = open(readPath,O_RDONLY)) == -1) {
        (void)fprintf(stderr,"Unable to open %s for reading:%s\n",readPath,strerror(errno));
        exit(EXIT_FAILURE);
    }


    /*
     * Check if write path is a directory, in which case
     * we will have to modify the write path 
     */
    if (S_ISDIR(writeStat.st_mode) == 1) {

        /* Get a file decriptor to the directory to use openat with */
        if((dirfd = open(writePath,O_RDONLY)) == -1) {
            (void)fprintf(stderr,"The write directory %s could not be opened: %s",writePath,strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* Get the filename from the read file path and modify the write path */
        bname = basename(readPath);
        strcat(writePath,"/");
        strcat(writePath,bname);

        /* 
         * Open the file for writing in the correct directory 
         * with the same permissions as the file we are copying.
         * Create it if it does not exist and truncate it in case 
         * it already contains data
         */
        if ((writefd = openat(dirfd,bname,O_WRONLY | O_CREAT | O_TRUNC,readStat.st_mode)) == -1) {
            (void)fprintf(stderr,"The write file %s could not be opened: %s\n",writePath,strerror(errno));
            exit(EXIT_FAILURE);
        }

    } else{
        /* Open file for writing directly, if path given is to a file */
        if ((writefd = open(writePath,O_WRONLY | O_CREAT | O_TRUNC,readStat.st_mode)) == -1) {
            (void)fprintf(stderr,"Unable to open %s for writing: %s\n",writePath,strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    /*
     * Perform the actual reading and writing with the set file descriptors 
     */
    while ((n = read(readfd,buf,BUFFSIZE)) > 0) {
        if (write(writefd,buf,n)!=n) {
            (void)fprintf(stderr,"Unable to open %s : %s\n",writePath,strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    
    /* Make sure the write completed correctly i.e last read returned 0 (EOF) */
    if (n!=0) {
        (void)fprintf(stderr,"Unable to read file %s correctly: %s\n",readPath,strerror(errno));
        exit(EXIT_FAILURE);
    }
    (void)close(readfd);
    (void)close(writefd);

    return EXIT_SUCCESS;
}

static void
usage(void)
{
    (void)fprintf(stderr, "usage: %s source_path destination_path \n", getprogname());
    exit(EXIT_FAILURE);
}
