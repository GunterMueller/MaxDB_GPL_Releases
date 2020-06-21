/* @(#)pgram/yyput.c

    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end

 */

#ifdef WIN32
#pragma warning (disable : 4033  ) /*RASE err. 1a: warnings.level 2*/
#endif

#include "0.h"
#include "yy.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Structure describing queued listing lines during the forward move
 * of error recovery.  These lines will be stroed by yyoutline during
 * the forward move and flushed by yyoutfl or yyflush when an
 * error occurs or a program termination.
 */
struct	B {					/* EF4: 11-08-86 WB */
	int	Bline;
	int	Bseekp;
	char	*Bfile;
	int	Bseqid;
	struct	B *Bnext;
} *bottled;

/* EF4: 11-08-86 WB
 * LbufHead and LbufTail are used to sbrk new space for the
 * structures describing queued lines.
 */
#define LineBufferLength	20

struct B *LbufHead, *LbufTail;

/*
/*
 * Filename gives the current input file, lastname is
 * the last filename we printed, and lastid is the seqid of the last line
 * we printed, to help us avoid printing
 * multiple copies of lines.
 */
extern	char *filename;
char	*lastname;
int	lastid;

char	hadsome;
char	holdbl;

extern int CardCount;

/*
 * Print the current line in the input line
 * buffer or, in a forward move of the recovery, queue it for printing.
 */
yyoutline()			/* EF4: 11-08-86 WB; new procedure yyoutline */
{
	extern struct B * NewLineBuffer();
	register struct B *bp;

	if (Recovery) {
		bp = NewLineBuffer();
		bp->Bline = yyline;
		bp->Bseekp = yylinpt;
		bp->Bfile = filename;
		bp->Bseqid = yyseqid;
		if (bottled != NIL)
			bp->Bnext = bottled->Bnext, bottled->Bnext = bp;
		else
			bp->Bnext = bp;
		bottled = bp;
		return;
	}
	yyoutfl(yyseqid);
	if (yyseqid != lastid)
		yyprline(charbuf, yyline, filename, yyseqid);
}


struct B *
NewLineBuffer()
{
    register struct B *new_space;

    new_space = LbufHead;
    if (new_space >= LbufTail) {
#if OLD
	new_space = (struct B *) sbrk (LineBufferLength * sizeof(struct B));
	if ((int) new_space != -1) {
#else
	new_space = (struct B *) calloc (LineBufferLength , sizeof(struct B));
	if ( new_space ) {
#endif
			/*
			 *  IST 1995-01-19
			 *  The line buffer will have big gaps!
			 */
	    LbufHead = new_space;
	    LbufTail = new_space + LineBufferLength;
	    return (new_space);
	}
	else
	    cerror("sbrk failed: No space for line buffer structure");
    }
    else {
	LbufHead++;
	return (new_space);
    }
}			/* End of NewLineBuffer */



/*
 * Flush all the bottled output.
 */
yyflush()
{

	yyoutfl(32767);
}

/*
 * Flush the listing to the sequence id toseqid
 */
yyoutfl(toseqid)
	int toseqid;
{
	register struct B *bp;

	bp = bottled;
	if (bp == NIL)
		return;
	bp = bp->Bnext;
	while (bp->Bseqid <= toseqid) {
		yygetline(bp->Bfile, bp->Bseekp, bp->Bline, bp->Bseqid);
		if (bp->Bnext == bp) {
			bottled = NIL;
			break;
		}
		bp = bp->Bnext;
		bottled->Bnext = bp;
	}
}

FILE	*yygetunit = NULL;
char	*yygetfile;

/*
 * Yysync guarantees that the line associated
 * with the current token was the last line
 * printed for a syntactic error message.
 */
yysync()
{

	yyoutfl(yyeseqid);
	if (lastid != yyeseqid)
		yygetline(yyefile, yyseekp, yyeline, yyeseqid);
}

yySsync()
{

	yyoutfl(OY.Yyeseqid);
	if (lastid != OY.Yyeseqid)
		yygetline(OY.Yyefile, OY.Yyseekp, OY.Yyeline, OY.Yyeseqid);
}

/*
 * Yygetline gets a line from a file after we have
 * lost it.  The pointer efile gives the name of the file,
 * seekp its offset in the file, and eline its line number.
 * If this routine has been called before the last file
 * it worked on will be open in yygetunit, with the files
 * name being given in yygetfile.  Note that this unit must
 * be opened independently of the unit in use for normal i/o
 * to this file; if it were a dup seeks would seek both files.
 */
yygetline(efile, seekp, eline, eseqid)
	char *efile;
	int seekp, eline, eseqid;
{
	extern int preprocessed;
	register int cnt;
	register char *bp;
	register int i;
	char buf[CBSIZE + 1];

	if (lastid == eseqid)
		return;
	if (eseqid == yyseqid) {
		bp = charbuf;
		yyprtd++;
	} else {
		bp = buf;
		if (efile != yygetfile) {
			if ( yygetunit != (FILE *) NULL )
			    (void) fclose( yygetunit );
			yygetfile = efile;
			yygetunit = fopen( yygetfile , "r" );
			if ( yygetunit == (FILE *) NULL)
				/* J.Bruehe 91-Aug-01  compatible types */
oops:
				perror(yygetfile), pexit(DIED);
		}
		if (preprocessed) {
			for(i = eline; i > 0; i--) {
				if( NULL == fgets(bp, CBSIZE, yygetunit)) 
					/* J.Bruehe 91-Aug-01  types */
		    		    printf("Error in reading file %s\n",
					yygetfile), pexit(DIED);
			}
		} else {
		    if (fseek( yygetunit , (long) seekp , 0 ) < 0)
			goto oops;
		    cnt = fread( bp , sizeof( * bp ) , CBSIZE, yygetunit );
		    if (cnt == 0 && ferror(yygetunit) != 0)
		    printf("Error in reading file %s\n",yygetfile), pexit(DIED);
		    bp[cnt] = 0;
		}
/*
 *              if ( fseek( yygetunit , (long) seekp , 0 ) < 0)
 *                      goto oops;
 *              cnt = fread( bp , sizeof( * bp ) , CBSIZE , yygetunit );
 *              if (cnt < 0)
 *                      goto oops;
 *              bp[cnt] = 0;
 */
	}
	yyprline(bp, eline, efile, eseqid);
}

yyretrieve()
{

	yygetline(OY.Yyefile, OY.Yyseekp, OY.Yyeline, OY.Yyeseqid);
}

/*
 * Print the line in the character buffer which has
 * line number line.  The buffer may be terminated by a new
 * line character or a null character.  We process
 * form feed directives, lines with only a form feed character, and
 * suppress numbering lines which are empty here.
 */
yyprline(buf, line, file, ident)
	register char *buf;
	int line;
	char *file;
	int ident;
{

	lastid = ident;
	if (buf[0] == '\f' && buf[1] == '\n') {
		printf("\f\n");
		hadsome = 0;
		holdbl = 0;
		return;
	}
	if (holdbl) {
		pchr('\n');
		holdbl = 0;
	}
	if (buf[0] == '\n')
		holdbl = 1;
	else {
		yysetfile(file);
		yyprintf(buf, line);
	}
	hadsome = 1;
}

yyprintf(cp, line)
	register char *cp;
	int line;
{
	printf("%6d  ", line);
	while (*cp != 0 && *cp != '\n')
		pchr(graphic(*cp++));
	pchr('\n');
}

graphic(ch)
	register CHAR ch;
{

	switch (ch) {
		default:
			if (ch >= ' ')
				return (ch);
		case 0177:
			return ('?');
		case '\n':
		case '\t':
			return (ch);
	}
}

extern	int nopflg;

char	printed = 1;
/*
 * Set the current file name to be file,
 * printing the name, or a header on a new
 * page if required.
 * there is another yysetfile in error.c
 * this one is for PI and PXP that one is for PI1
 */
yysetfile(file)
	register char *file;
{
	if (lastname == file)
		return;
	if (file == filename && opt('n') && (printed & 02) == 0) {
		printed |= 02;
		yyputfn(file);
	} else
		yyputfn(file);
	lastname = file;
}

/*
 * Put out an include file name
 * if an error occurs but the name has
 * not been printed (or if another name
 * has been printed since it has).
 */
yyputfn(cp)
	register char *cp;
{
	extern int outcol;

	if (cp == lastname && printed)
		return;
	lastname = cp;
	printed = 1;
	gettime( cp );
	printf("%s  %s:\n" , myctime( (int *) (&tvec) ) , cp );
	hadsome = 1;
}
