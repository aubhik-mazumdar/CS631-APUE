#ifndef _LS_H
#define _LS_H
#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
struct opts_holder{
	bool _A;
	bool _a;
	bool _c;
	bool _C;
	bool _d;
	bool _F;
	bool _f;
	bool _h;
	bool _i;
	bool _k;
	bool _l;
	bool _n;
	bool _q;
	bool _R;
	bool _r;
	bool _S;
	bool _s;
	bool _t;
	bool _u;
	bool _w;
	bool _x;
	bool _1;
};

typedef struct node{
    FTSENT *data;
    struct node *next;
} *node; 


#endif
