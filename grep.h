/* COLIN HUGHES, 223C */

#include <signal.h>
#include <setjmp.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <glob.h>
/* make BLKSIZE and LBSIZE 512 for smaller machines */
#define	BLKSIZE	4096
#define	NBLK	2047
//#define	NULL	0
#define	FNSIZE	128
#define	LBSIZE	4096
#define	ESIZE	256
#define	GBSIZE	256
#define	NBRA	5
//#define	EOF	-1
#define	KSIZE	9
#define	CBRA	1
#define	CCHR	2
#define	CDOT	4
#define	CCL	6
#define	NCCL	8
#define	CDOL	10
#define	CEOF	11
#define	CKET	12
#define	CBACK	14
#define	CCIRC	15
#define	STAR	01
char	Q[]	= "";
char	T[]	= "TMP";
#define	READ	0
#define	WRITE	1
#define BUFSIZE 100
char buf[BUFSIZE];  
int bufp = 0;
char	line[70];
char	*linp	= line;
int	peekc;
int	lastc;
char	savedfile[FNSIZE];
char	file[FNSIZE];
char	linebuf[LBSIZE];
char	expbuf[ESIZE+4];
int	given;
unsigned int	*addr1, *addr2;
unsigned int	*dot, *dol, *zero;
char	genbuf[LBSIZE];
char	*nextip;
int	ninbuf;
int	io;
int	pflag;
long	lseek(int, long, int);
int	open(char *, int);
int	creat(char *, int);
int	read(int, char*, int);
int	write(int, char*, int);
int	wait(int *);
int	unlink(char *);
int	oflag;
int	listf;
int	listn;
int	col;
char	*globp;
int	tfile	= -1;
int	tline;
char	*tfname;
char	*loc1;
char	*loc2;
char	ibuff[BLKSIZE];
int	iblock	= -1;
char	obuff[BLKSIZE];
int	oblock	= -1;
int	ichanged;
int	nleft;
char	WRERR[]	= "WRITE ERROR";
int	names[26];
int	anymarks;
char	*braslist[NBRA];
char	*braelist[NBRA];
int	nbra;
int	subnewa;
int	subolda;
int	fchange;
int	wrapp;
int	bpagesize = 20;
unsigned nlall = 128;
char	*mkdtemp(char *);
char	tmpXXXXX[50] = "/tmp/eXXXXX";
int star(char* lp, char* ep, char* curlp);
void cerror(void);
void defchar(char* ep, int c);
void search_file(const char* filename_, const char* searchfor);
void process_dir(const char* dir, const char* searchfor, void (*fp)(const char*, const char*));
char *getblock(unsigned int atl, int iof);
char *ggetline(unsigned int tl);
int advance(char *lp, char *ep);
int append(int (*f)(void), unsigned int *a);
int backref(int i, char *lp);
void blkio(int b, char *buf, int (*iofcn)(int, char*, int));
int cclass(char *set, int c, int af);
void printcommand(void);
void compile(int eof);
void error(char *s);
int execute(unsigned int *addr);
void exfile(void);
void filename(const char* name);
int getchr(void);
int getfile(void);
int getnum(void);
void global(int k);
void init(void);
unsigned int *address(void);
void newline(void);
void nonzero(void);
void print(void);
void putchr(int ac);
int putline(void);
void gputs(char *sp);
void setwide(void);
void setnoaddr(void);
void squeeze(int i);
int getch_(void);
void ungetch_(int c);
void search(const char* re);
typedef void	(*SIG_TYP)(int);
SIG_TYP	oldhup;
SIG_TYP	oldquit;
/* these two are not in ansi, but we need them */
#define	SIGHUP	1	/* hangup */
#define	SIGQUIT	3	/* quit (ASCII FS) */
