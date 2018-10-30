#ifndef _CMP_H_ 
#define _CMP_H_
#include "ls.h"
int atime_cmp(const FTSENT **,const FTSENT **);
int ratime_cmp(const FTSENT **,const FTSENT **);
int size_cmp(const FTSENT **,const FTSENT **);
int rsize_cmp(const FTSENT **,const FTSENT **);
int mtime_cmp(const FTSENT **,const FTSENT **);
int rmtime_cmp(const FTSENT **,const FTSENT **);
int ctime_cmp(const FTSENT **,const FTSENT **);
int rctime_cmp(const FTSENT **,const FTSENT **);
int compare(const FTSENT **,const FTSENT **);
int rcompare(const FTSENT **,const FTSENT **);

#endif
