#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <string.h>

int
compare(const FTSENT** first,const FTSENT** second)
{   
    return (strcmp((*first)->fts_name,(*second)->fts_name));
}

int
rcompare(const FTSENT** first,const FTSENT** second)
{   
    return (strcmp((*second)->fts_name,(*first)->fts_name));
}

int
atime_cmp(const FTSENT ** first,const FTSENT **second)
{
    if((*first)->fts_statp->st_atime > (*second)->fts_statp->st_atime)
        return -1;
    else if((*first)->fts_statp->st_atime < (*second)->fts_statp->st_atime)
        return 1;
    else
        return compare(first,second);
}

int
ratime_cmp(const FTSENT ** first,const FTSENT **second)
{
    if((*first)->fts_statp->st_atime > (*second)->fts_statp->st_atime)
        return 1;
    else if((*first)->fts_statp->st_atime < (*second)->fts_statp->st_atime)
        return -1;
    else
        return compare(first,second);
}

int
size_cmp(const FTSENT ** first,const FTSENT **second)
{
    if((*first)->fts_statp->st_size > (*second)->fts_statp->st_size)
        return -1;
    else if((*first)->fts_statp->st_size < (*second)->fts_statp->st_size)
        return 1;
    else
        return(compare(first,second));
}

int
rsize_cmp(const FTSENT ** first,const FTSENT **second)
{
    if((*first)->fts_statp->st_size > (*second)->fts_statp->st_size)
        return 1;
    else if((*first)->fts_statp->st_size < (*second)->fts_statp->st_size)
        return -1;
    else
        return(compare(first,second));
}

int
mtime_cmp(const FTSENT ** first,const FTSENT **second)
{
    if((*first)->fts_statp->st_mtime > (*second)->fts_statp->st_mtime)
        return -1;
    else if((*first)->fts_statp->st_mtime < (*second)->fts_statp->st_mtime)
        return 1;
    else
        return compare(first,second);
}

int
rmtime_cmp(const FTSENT ** first,const FTSENT **second)
{
    if((*first)->fts_statp->st_mtime > (*second)->fts_statp->st_mtime)
        return 1;
    else if((*first)->fts_statp->st_mtime < (*second)->fts_statp->st_mtime)
        return -1;
    else
        return compare(first,second);
}

int
ctime_cmp(const FTSENT ** first,const FTSENT **second)
{
    if((*first)->fts_statp->st_ctime >= (*second)->fts_statp->st_ctime)
        return -1;
    else if((*first)->fts_statp->st_ctime < (*second)->fts_statp->st_ctime)
        return 1;
    else
        return compare(first,second);
}

int
rctime_cmp(const FTSENT ** first,const FTSENT **second)
{
    if((*first)->fts_statp->st_ctime > (*second)->fts_statp->st_ctime)
        return 1;
    else if((*first)->fts_statp->st_ctime < (*second)->fts_statp->st_ctime)
        return -1;
    else
        return compare(first,second);
}
