/* COLIN HUGHES, 223C */
#include "grep.h"
int main(int argc, char *argv[]) {
	zero = (unsigned *)malloc(nlall*sizeof(unsigned));
	tfname = mkdtemp(tmpXXXXX);
	init();
	if (argc < 3) {
		printf("Error: Invalid number of arguments\n");
		return 0;
	}
	const char* search_for = argv[1];
	int i = 2;
	while ( i < argc) {
		process_dir(argv[i], search_for, search_file); // search_file: fn that reads and searches
		++i; //increment to next argument and start again
	}
	return 0;
}
void search_file(const char* filename_, const char* searchfor) {
	printf("\n"); printf("processing %s...\n", filename_); 
	int c;
	setnoaddr();
	filename(filename_);
	init();
	addr2 = zero;
	if ((io = open(file, 0)) < 0) {
		lastc = '\n';
		error(file);
	}
	setwide();
	squeeze(0);
	ninbuf = 0;
	c = zero != dol;
	append(getfile, addr2);
	exfile();
	fchange = c;
	search(searchfor);
}
void process_dir(const char* dir, const char* searchfor, void (*fp)(const char*, const char*)) {
  if (strchr(dir, '*') == NULL) {  search_file(dir, searchfor);  return; }  // search one flie or search a directory of files using glob()
	glob_t results;
	memset(&results, 0, sizeof(results));
	glob(dir, 0, NULL, &results);
	printf("processing files in %s...\n\n", dir);
  for (int i = 0; i < results.gl_pathc; ++i) {
	const char* filename = results.gl_pathv[i];
	fp(filename, searchfor);    // function ptr to function that reads and searches a file
  }
  globfree(&results);
}
void printcommand(void) {  
	int c;  char lastsep;
	for (;;) {  
		unsigned int* a1;
		if (pflag) { 
			pflag = 0;  
			addr1 = addr2 = dot;  
			print(); 
		}  
		c = '\n';
		for (addr1 = 0;;) {
			lastsep = c;
			a1 = address();
			c = getchr();
			if (c != ',' && c != ';') { break; }  
			if (lastsep==',') { error(Q); }
			if (a1==0) {  
				a1 = zero+1;  
				if (a1 > dol) { a1--; }  }  
				addr1 = a1;  
				if (c == ';') { dot = a1; }
		}
	if (lastsep != '\n' && a1 == 0) { a1 = dol; }
	if ((addr2 = a1)==0) { given = 0;  addr2 = dot;  } else { given = 1; }  if (addr1==0) { addr1 = addr2; }
	switch(c) {
		case 'p':  case 'P':  newline();  print();  continue;
		case EOF:  default:  return;
		}
	}
}
int getch_(void) {  
  char c = (bufp > 0) ? buf[--bufp] : getchar();  
  lastc = c & 0177;
  return lastc;
}
void ungetch_(int c) { 
	if (bufp >= BUFSIZE) { printf("ungetch: overflow\n"); }
	else { buf[bufp++] = c; } 
}
void search(const char* re) {  
	char buf[GBSIZE];  
	snprintf(buf, sizeof(buf), "/%s\n", re);  // / and \n very important 
	// printf("g%s", buf);  
	const char* p = buf + strlen(buf) - 1;
	while (p >= buf) { ungetch_(*p--); }  global(1);
}
void print(void) {
	unsigned int *a1;
	nonzero();
	a1 = addr1;
	do {
		if (listn) { putchr('\t'); }
		gputs(ggetline(*a1++));
	} while (a1 <= addr2);
	dot = addr2;
	listf = listn = pflag = 0;
}
unsigned int * address(void) {
	int sign;
	unsigned int *a, *b;
	int opcnt, nextopand;
	int c;
	nextopand = -1;
	sign = 1;
	opcnt = 0;
	a = dot;
	do {
		do c = getchr(); while (c==' ' || c=='\t');
		if ('0'<=c && c<='9') {
			peekc = c;
			if (!opcnt) { a = zero; }
			a += sign*getnum();
		} else switch (c) {
		case '$':
			a = dol;
			/* fall through */
		case '.':
			if (opcnt) {error(Q);}
			break;
		case '\'':
			c = getchr();
			if (opcnt || c<'a' || 'z'<c) { error(Q);}
			a = zero;
			do a++; while (a<=dol && names[c-'a']!=(*a&~01));
			break;
		case '?':
			sign = -sign; /* fall through */
		case '/':
			compile(c);
			b = a;
			for (;;) {
				a += sign;
				if (a<=zero) {a = dol;}
				if (a>dol) {a = zero;}
				if (execute(a)) {break;}
				if (a==b) {error(Q);}
			}
			break;
		default:
			if (nextopand == opcnt) {
				a += sign;
				if (a<zero || dol<a)
					continue;       /* error(Q); */
			}
			if (c!='+' && c!='-' && c!='^') {
				peekc = c;
				if (opcnt==0) {a = 0;}
				return (a);
			}
			sign = 1;
			if (c!='+') {sign = -sign;}
			nextopand = ++opcnt;
			continue;
		}
		sign = 1;
		opcnt++;
	} while (zero<=a && a<=dol);
	error(Q);
	/*NOTREACHED*/
	return 0;
}
int getnum(void) {
	int r, c;
	r = 0;
	while ((c=getchr())>='0' && c<='9') {r = r*10 + c - '0';}
	peekc = c;
	return (r);
}
void setwide(void) {
	if (!given) {
		addr1 = zero + (dol>zero);
		addr2 = dol;
	}
}
void setnoaddr(void) { if (given) {error(Q);} }
void nonzero(void) { squeeze(1);}
void squeeze(int i) { if (addr1<zero+i || addr2>dol || addr1>addr2) { error(Q);} }
void newline(void) {
	int c;
	if ((c = getchr()) == '\n' || c == EOF) { return;}
	if (c=='p' || c=='l' || c=='n') {
		pflag++;
		if (c=='l') { listf++;}
		else if (c=='n') { listn++;}
		if ((c=getchr())=='\n') { return;}
	}
	error(Q);
}
void filename(const char* name) {
	char *p1, *p2;
	int c;
	int comm = 'e';
	strcpy(file, name);
	strcpy(savedfile, name);
}
void exfile(void) { io = -1; }
void error(char *s) { printf("problem found, passed %s\n", s); }
int getchr(void) {
	char c;
	if ((lastc = peekc)) {
		peekc = 0;
		return(lastc);
	}
	if (globp) {
		if ((lastc = *globp++) != 0)
			return(lastc);
		globp = 0;
		return(EOF);
	}
	if (( c = getch_() ) <= 0) { return(lastc = EOF);}
	lastc = c&0177;
	return(lastc);
}
int getfile(void) {
	int c;
	char *lp, *fp;
	lp = linebuf;
	fp = nextip;
	do {
		if (--ninbuf < 0) {
			if ((ninbuf = read(io, genbuf, LBSIZE)-1) < 0) {
				if (lp>linebuf) {
					gputs("'\\n' appended");
					*genbuf = '\n';
				}
				else { return(EOF); }
			}
			fp = genbuf;
			while(fp < &genbuf[ninbuf]) { if (*fp++ & 0200) { break;} }
			fp = genbuf;
		}
		c = *fp++;
		if (c=='\0') { continue; }
		if (c&0200 || lp >= &linebuf[LBSIZE]) {
			lastc = '\n';
			error(Q);
		}
		*lp++ = c;
	} while (c != '\n');
	*--lp = 0;
	nextip = fp;
	return(0);
}
int append(int (*f)(void), unsigned int *a) {
	unsigned int *a1, *a2, *rdot;
	int nline, tl;
	nline = 0;
	dot = a;
	while ((*f)() == 0) {
		if ((dol-zero)+1 >= nlall) {
			unsigned *ozero = zero;
			nlall += 1024;
			if ((zero = (unsigned *)realloc((char *)zero, nlall*sizeof(unsigned)))==NULL) { error("MEM?");}
			dot += zero - ozero;
			dol += zero - ozero;
		}
		tl = putline();
		nline++;
		a1 = ++dol;
		a2 = a1+1;
		rdot = ++dot;
		while (a1 > rdot) { *--a2 = *--a1;}
		*rdot = tl;
	}
	return(nline);
}
char *ggetline(unsigned int tl) {
	char *bp, *lp;
	int nl;
	lp = linebuf;
	bp = getblock(tl, READ);
	nl = nleft;
	tl &= ~((BLKSIZE/2)-1);
	while ((*lp++ = *bp++))
		if (--nl == 0) {
			bp = getblock(tl+=(BLKSIZE/2), READ);
			nl = nleft;
		}
	return(linebuf);
}
int putline(void) {
	char *bp, *lp;
	int nl;
	unsigned int tl;
	fchange = 1;
	lp = linebuf;
	tl = tline;
	bp = getblock(tl, WRITE);
	nl = nleft;
	tl &= ~((BLKSIZE/2)-1);
	while ((*bp = *lp++)) {
		if (*bp++ == '\n') {
			*--bp = 0;
			break;
		}
		if (--nl == 0) {
			bp = getblock(tl+=(BLKSIZE/2), WRITE);
			nl = nleft;
		}
	}
	nl = tline;
	tline += (((lp-linebuf)+03)>>1)&077776;
	return(nl);
}
char *getblock(unsigned int atl, int iof) {
	int bno, off;	
	bno = (atl/(BLKSIZE/2));
	off = (atl<<1) & (BLKSIZE-1) & ~03;
	if (bno >= NBLK) {
		lastc = '\n';
		error(T);
	}
	nleft = BLKSIZE - off;
	if (bno==iblock) {
		ichanged |= iof;
		return(ibuff+off);
	}
	if (bno==oblock) { return(obuff+off);}
	if (iof==READ) {
		if (ichanged) { blkio(iblock, ibuff, write);}
		ichanged = 0;
		iblock = bno;
		blkio(bno, ibuff, read);
		return(ibuff+off);
	}
	if (oblock>=0) { blkio(oblock, obuff, write);}
	oblock = bno;
	return(obuff+off);
}
void blkio(int b, char *buf, int (*iofcn)(int, char*, int)) {
	lseek(tfile, (long)b*BLKSIZE, 0);
	if ((*iofcn)(tfile, buf, BLKSIZE) != BLKSIZE) { error(T);}
}
void init(void) { dot = dol = zero; }
void global(int k) {
	char *gp;
	int c;
	unsigned int *a1;
	char globuf[GBSIZE];
	if (globp) { error(Q);}
	setwide();
	squeeze(dol>zero);
	if ((c=getchr())=='\n') { error(Q);}
	compile(c);
	gp = globuf;
	while ((c = getchr()) != '\n') {
		if (c==EOF) { error(Q);}
		if (c=='\\') {
			c = getchr();
			if (c!='\n') { *gp++ = '\\';}
		}
		*gp++ = c;
		if (gp >= &globuf[GBSIZE-2]) { error(Q);}
	}
	if (gp == globuf) { *gp++ = 'p';}
	*gp++ = '\n';
	*gp++ = 0;
	for (a1=zero; a1<=dol; a1++) {
		*a1 &= ~01;
		if (a1>=addr1 && a1<=addr2 && execute(a1)==k) { *a1 |= 01;}
	}
	for (a1=zero; a1<=dol; a1++) {
		if (*a1 & 01) {
			*a1 &= ~01;
			dot = a1;
			globp = globuf;
			printcommand();
			a1 = zero;
		}
	}
}
void compile(int eof) {
	int c;
	char *ep;
	char *lastep;
	char bracket[NBRA], *bracketp;
	int cclcnt;
	ep = expbuf;
	bracketp = bracket;
	if ((c = getchr()) == '\n') {
		peekc = c;
		c = eof;
	}
	if (c == eof) {
		if (*ep==0) { error(Q);}
		return;
	}
	nbra = 0;
	if (c=='^') {
		c = getchr();
		*ep++ = CCIRC;
	}
	peekc = c;
	lastep = 0;
	for (;;) {
		if (ep >= &expbuf[ESIZE]) { cerror();}
		c = getchr();
		if (c == '\n') {
			peekc = c;
			c = eof;
		}
		if (c==eof) {
			if (bracketp != bracket) { cerror();}
			*ep++ = CEOF;
			return;
		}
		if (c!='*') { lastep = ep;}
		switch (c) {
		case '\\':
			if ((c = getchr())=='(') {
				if (nbra >= NBRA)
					cerror();
				*bracketp++ = nbra;
				*ep++ = CBRA;
				*ep++ = nbra++;
				continue;
			}
			if (c == ')') {
				if (bracketp <= bracket) { cerror();} 
				*ep++ = CKET;
				*ep++ = *--bracketp;
				continue;
			}
			if (c>='1' && c<'1'+NBRA) {
				*ep++ = CBACK;
				*ep++ = c-'1';
				continue;
			}
			*ep++ = CCHR;
			if (c=='\n') { cerror();}
			*ep++ = c;
			continue;
		case '.': *ep++ = CDOT; continue;
		case '\n': cerror();
		case '*':
			if (lastep==0 || *lastep==CBRA || *lastep==CKET) { defchar(ep, c); }
			*lastep |= STAR;
			continue;
		case '$':
			if ((peekc=getchr()) != eof && peekc!='\n') {defchar(ep, c);}
			*ep++ = CDOL;
			continue;
		case '[':
			*ep++ = CCL;
			*ep++ = 0;
			cclcnt = 1;
			if ((c=getchr()) == '^') {
				c = getchr();
				ep[-2] = NCCL;
			}
			do {
				if (c=='\n') { cerror();}
				if (c=='-' && ep[-1]!=0) {
					if ((c=getchr())==']') {
						*ep++ = '-';
						cclcnt++;
						break;
					}
					while (ep[-1]<c) {
						*ep = ep[-1]+1;
						ep++;
						cclcnt++;
						if (ep>=&expbuf[ESIZE])
							cerror();
					}
				}
				*ep++ = c;
				cclcnt++;
				if (ep >= &expbuf[ESIZE]) { cerror();}
			} while ((c = getchr()) != ']');
			lastep[1] = cclcnt;
			continue;
		default:
			*ep++ = CCHR;
			*ep++ = c;
		}
	}
}
void cerror(void) { expbuf[0] = nbra = 0; }
void defchar(char* ep, int c) {
	*ep++ = CCHR;
	*ep++ = c;
}
int execute(unsigned int *addr) {
	char *p1, *p2;
	int c;
	for (c=0; c<NBRA; c++) {
		braslist[c] = 0;
		braelist[c] = 0;
	}
	p2 = expbuf;
	if (addr == (unsigned *)0) {
		if (*p2==CCIRC) { return(0);}
		p1 = loc2;
	} else if (addr==zero) { return(0); }
	else { p1 = ggetline(*addr); }
	if (*p2==CCIRC) {
		loc1 = p1;
		return(advance(p1, p2+1));
	}/* fast check for first character */
	if (*p2==CCHR) {
		c = p2[1];
		do {
			if (*p1!=c) { continue;}
			if (advance(p1, p2)) {
				loc1 = p1;
				return(1);
			}
		} while (*p1++);
		return(0);
	}/* regular algorithm */
	do {
		if (advance(p1, p2)) {
			loc1 = p1;
			return(1);
		}
	} while (*p1++);
	return(0);
}
int advance(char *lp, char *ep) {
	char *curlp;
	int i;
	for (;;) switch (*ep++) {
	case CCHR: if (*ep++ == *lp++) {continue; } return(0);
	case CDOT: if (*lp++) { continue; } return(0);
	case CDOL: if (*lp==0) { continue; } return(0);
	case CEOF: loc2 = lp; return(1);
	case CCL: if (cclass(ep, *lp++, 1)) { ep += *ep; continue; } return(0);
	case NCCL: if (cclass(ep, *lp++, 0)) { ep += *ep; continue; } return(0);
	case CBRA: braslist[*ep++] = lp; continue;
	case CKET: braelist[*ep++] = lp; continue;
	case CBACK:
		if (braelist[i = *ep++]==0) { error(Q); }
		if (backref(i, lp)) { lp += braelist[i] - braslist[i]; continue;}
		return(0);
	case CBACK|STAR:
		if (braelist[i = *ep++] == 0) {error(Q); }
		curlp = lp;
		while (backref(i, lp)) { lp += braelist[i] - braslist[i];}
		while (lp >= curlp) {
			if (advance(lp, ep)) { return(1);}
			lp -= braelist[i] - braslist[i];
		}
		continue;
	case CDOT|STAR:
		curlp = lp;
		while (*lp++) {;}
		return(star(lp, ep, curlp));
	case CCHR|STAR:
		curlp = lp;
		while (*lp++ == *ep) {;}
		ep++;
		return(star(lp, ep, curlp));
	case CCL|STAR: case NCCL|STAR:
		curlp = lp;
		while (cclass(ep, *lp++, ep[-1]==(CCL|STAR))) {;}
		ep += *ep;
		return(star(lp, ep, curlp));
	default: error(Q);
	}
}
int star(char* lp, char* ep, char* curlp) { do {if (advance(--lp, ep)) { return(1);} } while (lp > curlp); return(0);}
int backref(int i, char *lp) {
	char *bp;
	bp = braslist[i];
	while (*bp++ == *lp++)
		if (bp >= braelist[i]) { return(1); }
	return(0);
}
int cclass(char *set, int c, int af) {
	int n;
	if (c==0) { return(0); }
	n = *set++;
	while (--n) { if (*set++ == c) { return(af); } }
	return(!af);
}
void gputs(char *sp) {
	col = 0;
	while (*sp) { putchr(*sp++); }
	putchr('\n');
}
void putchr(int ac) {
	char *lp;
	int c;
	lp = linp;
	c = ac;
	if (listf) {
		if (c=='\n') {
			if (linp!=line && linp[-1]==' ') {
				*lp++ = '\\';
				*lp++ = 'n';
			}
		} else {
			if (col > (72-4-2)) {
				col = 8;
				*lp++ = '\\';
				*lp++ = '\n';
				*lp++ = '\t';
			}
			col++;
			if (c=='\b' || c=='\t' || c=='\\') {
				*lp++ = '\\';
				if (c=='\b') { c = 'b';}
				else if (c=='\t') { c = 't';}
				col++;
			} else if (c<' ' || c=='\177') {
				*lp++ = '\\';
				*lp++ =  (c>>6)    +'0';
				*lp++ = ((c>>3)&07)+'0';
				c     = ( c    &07)+'0';
				col += 3;
			}
		}
	}
	*lp++ = c;
	if(c == '\n' || lp >= &line[64]) {
		linp = line;
		write(oflag?2:1, line, lp-line);
		return;
	}
	linp = lp;
}
