#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <fts.h>
#include <grp.h>
#include <inttypes.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <time.h>
#include <unistd.h>

#include "ls.h"
#include "print_function.h"

#define MODE_STRLEN 12
#define HUMANIZE_FLAGS HN_DECIMAL | HN_NOSPACE | HN_B
#define KB 1024
#define DEF_BLOCKSIZE 512

void
print_permissions(struct stat *fileStat)
{
    char modeString[MODE_STRLEN];
    strmode(fileStat->st_mode,modeString);
    printf("%s",modeString);
}


void print_no_of_links(struct stat *fileStat)
{
    printf("%2d ",fileStat->st_nlink);	
}

void
print_owner(uid_t fileUserId,gid_t fileGroupId,struct opts_holder opts,int *max)
{
    struct passwd *filePasswd = NULL;
    struct group *fileGroup = NULL;
    int userLen;
    int groupLen;
    
    userLen = max[0] + 1;
    groupLen = max[1] + 1;
    if(opts._n){
        (void)printf("%-*d ",userLen,fileUserId);
        (void)printf("%-*d",groupLen,fileGroupId);
    } else {
        if((filePasswd = getpwuid(fileUserId)) != NULL){
            (void)printf("%-*s ",userLen,filePasswd->pw_name);
        } else {
            (void)printf("%-*d ",userLen,fileUserId);
        }
        
        if((fileGroup = getgrgid(fileGroupId)) !=  NULL){
            (void)printf("%-*s",groupLen,fileGroup->gr_name);
        } else {
            (void)printf("%-*d",groupLen,fileGroupId);
        }
    }
}

void
print_bytes(struct stat *fileStat,struct opts_holder opts,int *max)
{
    char bytes[5];
    char charDevice[max[2] + 1];
    int64_t actual_size = (int64_t)fileStat->st_size;
    int maxBytesLength;
    
    maxBytesLength = max[2] + 1;
    if(S_ISCHR(fileStat->st_mode) || S_ISBLK(fileStat->st_mode)){
        (void)snprintf(charDevice,sizeof(charDevice),"%d, %d",\
                major(fileStat->st_rdev),minor(fileStat->st_rdev));
        printf("%*s ",maxBytesLength,charDevice);
        return;
    }
    
    if(opts._h){
        if(humanize_number(bytes,5,actual_size,"",HN_AUTOSCALE, \
                    HUMANIZE_FLAGS)==-1){
            printf("%*ld ",maxBytesLength,fileStat->st_size);
        } else
            (void)printf("%*s ",maxBytesLength,bytes);
    } else {
        (void)printf("%*ld ",maxBytesLength,fileStat->st_size);
    }
}

void
print_time(FTSENT *file,struct opts_holder opts)
{
    struct stat fileStat;
    int i;
    char *timeString;
    
    fileStat = *(file->fts_statp);
    tzset();

    if(opts._c){
        if((timeString = ctime(&fileStat.st_ctime)) == NULL){
            timeString = (char *)fileStat.st_ctime;
        }
    } else if(opts._u){
        timeString = ctime(&fileStat.st_atime);
    } else {
        timeString = ctime(&fileStat.st_mtime);
    }

    for(i=4;i<16;++i)
       putchar(timeString[i]);
    putchar(' ');
}


void
print_pathname(FTSENT *file,struct opts_holder opts)
{
    int i;
    int namelen;
    int rdlinkLen;
    struct stat *fileStat;
    char buf[MAXPATHLEN + 1];
    char path[MAXPATHLEN + 1];
    char modeString[MODE_STRLEN];
    char * name;
    
    name = file->fts_name;
    fileStat = file->fts_statp;
    namelen = (int)file->fts_namelen;
    
    if(opts._q && !opts._w){
        for(i=0;i<namelen;i++){
            if(isprint(name[i])==0)
                putchar('?');
            else
                putchar(name[i]);
        }
    }

    if(opts._w)
        printf("%s",name);

    (void)strmode(fileStat->st_mode,modeString);
    
    if(opts._F){
        switch((char)modeString[0]){
            case 'd': 
                putchar('/');
                break;
            case 'l': 
                putchar('@');
                (void)snprintf(path,sizeof(path),"%s/%s", \
                        file->fts_accpath,file->fts_name);
                
                if((rdlinkLen = readlink(path,buf,sizeof(buf)-1)) == -1){
                    fprintf(stderr,"READLINK ERROR:\n");
                    break;
                } else {
                    buf[rdlinkLen] = '\0';
                    printf(" -> %s",buf);
                }
                break;
            case 'w': 
                putchar('%');
                break;
            case 's': 
                putchar('=');
                break;
            case 'p': 
                putchar('|');
                break;
            default:
                if(modeString[3]=='x' || modeString[6]=='x' || modeString[9]=='x')
                    putchar('*');
                break;
        }
    }
    
    if(opts._l && !opts._F){
        if((char)modeString[0]=='l'){
            (void)snprintf(path,sizeof(path),"%s/%s",file->fts_accpath,file->fts_name);
            if((rdlinkLen = readlink(path,buf,sizeof(buf)-1)) == -1)
                fprintf(stderr,"READLINK ERROR:");
            else
                buf[rdlinkLen] = '\0';
                printf(" -> %s",buf);
        }
    }
    printf("\n");
}

void
print_function(node head,struct opts_holder opts,int *max)
{
    FTSENT *file;
    node p;
    char bytes[5];
    int64_t temp;
    long blocksize;
    int prefix;
    struct stat *fileStat;
    
    p = head;
    (void)getbsize(NULL,&blocksize);
    
    if(!opts._d && (opts._l || opts._s)){
        if(opts._h){
            temp = max[3];
            if(humanize_number(bytes,5,temp,"",HN_AUTOSCALE,HUMANIZE_FLAGS)!= -1){
                printf("total %s\n",bytes);    
            } else {
                temp /= blocksize;
                printf("total %ld\n",temp);
            }
        } else if(opts._k){
            temp = max[3]*DEF_BLOCKSIZE;
            if((prefix = humanize_number(bytes,5,temp,"",HN_AUTOSCALE,HN_NOSPACE))!=-1){
                printf("total ");
                for(int i=0;i<prefix-1;i++)
                    putchar(bytes[i]);
                putchar('\n');
            }
        } else {
            temp = max[3] * DEF_BLOCKSIZE;
            temp /= blocksize;
            printf("total %ld\n",temp);
        }
    }

    while(p != NULL){
        file = p->data;
        if(file->fts_statp){
            fileStat = file->fts_statp;
        } else {
            if(stat(file->fts_name,fileStat) == -1){
                p = p->next;
                continue;
            }
        }  

        if(opts._i){
            printf("%ld ",fileStat->st_ino);
        }


        if(opts._s){
            temp = fileStat->st_blocks * DEF_BLOCKSIZE;
            if(opts._h){
                if(humanize_number(bytes,5,(int64_t)temp,"", \
                            HN_AUTOSCALE,HUMANIZE_FLAGS)!= -1){
                    printf("%*s ",max[4],bytes);    
                } else {
                    temp /= blocksize;
                    printf("%*d ",max[4],(int)temp);
                }
            } else {
                if(opts._k)
                    printf("%*d ",max[4],(int)(temp/KB));
                else 
                    printf("%*d ",max[4],(int)(temp/blocksize));
            }
        }

        if(opts._l){
            print_permissions(fileStat);
            print_no_of_links(fileStat);
            print_owner(fileStat->st_uid,fileStat->st_gid,opts,max);
            print_bytes(fileStat,opts,max);
            print_time(file,opts);
            print_pathname(file,opts);
        } else {
            print_pathname(file,opts);
        }
        p = p->next;
    }
}
