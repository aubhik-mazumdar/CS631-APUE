/*
 * HW#2 - command(3)
 * AUTHOR: amazumda
 * DATE: 10-27-2018
 * TIME TAKEN: 1hour
 */


#include <sys/types.h>
#include <sys/wait.h>


#include <errno.h>
#include <fcntl.h>
#include <paths.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;
/*
 * If command(3) is modified, we can use this function to modify the buffer
 * so that it stores the contents of stdout and stderr. For now, I have written
 * the function with the side effect of printing the stdout and stderr
 * (not good practice)
 *
 */
/*
void
change(char **outbuf,char *outstring,int outlen,char **errbuf,char *errstring,int errlen){
    int i;
    *outbuf = malloc(outlen * sizeof(char));
    *errbuf = malloc(errlen * sizeof(char));
    if(*outbuf == NULL || *errbuf == NULL){
        fprintf(stderr,"malloc failed\n");
        exit(EXIT_FAILURE);
    }
    for(i=0;i<outlen;i++)
        (*outbuf)[i] = outstring[i];
    for(i=0;i<errlen;i++)
        (*errbuf)[i] = errstring[i];
}
*/

int
command(const char *string,char *outbuf,int outlen,
        char *errbuf, int errlen)
{
    int outfd[2],errfd[2];
    sigset_t newmask,oldmask;
    pid_t pid;
    const char *argp[] = {"sh", "-c", NULL, NULL};
    char outstring[outlen];
    char errstring[errlen];
    
    argp[2] = string;

    if(string == NULL){
        if (access(_PATH_BSHELL, X_OK) == 0)
                return 1;
        return 0;
    }

    if(pipe(outfd) < 0){
        fprintf(stderr,"stdout pipe creation failed:%s\n",strerror(errno));
        return EXIT_FAILURE;
    }
    
    if(pipe(errfd) < 0){
        fprintf(stderr,"stderr pipe creation failed:%s\n",strerror(errno));
        return EXIT_FAILURE;
    }
    
    if(signal(SIGINT,SIG_IGN) == SIG_ERR){
        fprintf(stderr,"SIGINT ignore error:%s\n",strerror(errno));
        return EXIT_FAILURE;
    }

    if(signal(SIGQUIT,SIG_IGN) == SIG_ERR){
        fprintf(stderr,"SIGQUIT ignore error:%s\n",strerror(errno));
        return EXIT_FAILURE;
    }

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGCHLD);

    if(sigprocmask(SIG_BLOCK,&newmask,&oldmask) < 0){
        fprintf(stderr,"SIG_BLOCK error:%s\n",strerror(errno));
        return EXIT_FAILURE;
    }

    if((pid = vfork()) == -1){
        fprintf(stderr,"fork failed:%s\n",strerror(errno));
        return -1;
    }

    if(pid == 0){
        (void)close(outfd[0]);
        (void)close(errfd[0]);
        if(dup2(outfd[1],STDOUT_FILENO) < 0){
            fprintf(stderr,"error while mapping stdout:%s\n",strerror(errno));
            return EXIT_FAILURE;
        }

        if(dup2(errfd[1],STDERR_FILENO) < 0){
            fprintf(stderr,"error while mapping stderr:%s\n",strerror(errno));
            return EXIT_FAILURE;
        }

        execve(_PATH_BSHELL,__UNCONST(argp),environ);
        (void)close(outfd[1]);
        (void)close(errfd[1]);
    } else {
        (void)close(outfd[1]);
        (void)close(errfd[1]);
        if(read(outfd[0],outstring,outlen) == -1){
            fprintf(stderr,"error while reading stdout:%s\n",strerror(errno));
            return EXIT_FAILURE;
        }

        if(read(errfd[0],errstring,errlen) == -1){
            fprintf(stderr,"error while reading stderr:%s\n",strerror(errno));
            return EXIT_FAILURE;
        }
        outbuf = outstring;
        errbuf = errstring;
        printf("stdout:\n%s\n",outbuf);
        printf("stderr:\n%s\n",errbuf);
    }

    (void)wait(NULL);
    return EXIT_SUCCESS;
}

int
main()
{
    char out[BUFSIZ],err[BUFSIZ];
    if(command("ls -l",out,BUFSIZ,err,BUFSIZ) == -1){
        perror("command");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
