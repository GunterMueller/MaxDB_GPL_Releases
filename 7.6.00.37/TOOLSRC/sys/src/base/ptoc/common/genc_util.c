/* @(#)common/genc_util.c

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
/* JH   10.06.91        GenPName : Change variable name from  VAR to (*VAR) */
/* JH   ??              Some Debug-Functions (DumpDump)                     */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "opcode.i"
#include "globals.ci"
#include "stab.h"
#include <stdlib.h>

#ifdef RASN /*RASN4 file pref used */
extern char tok_first_type [80];
#endif

#define ANYSIDE 0
#define LEFT    1
#define RIGHT   2
#define IfthenelseOp (LastOp+1)

extern GetString (int , char *);

char *Key[] = {
  "auto",
  "break",
  "case",
  "char",
  "continue",
  "default",
  "do",
  "double",
  "else",
  "entry",
  "enum",
  "extern",
  "float",
  "for",
  "goto",
  "if",
  "int",
  "long",
  "register",
  "return",
  "short",
  "sizeof",
  "static",
  "struct",
  "switch",
  "typedef",
  "union",
  "unsigned",
  "void",
  "while",
  (char *)0,
};


static struct gen_names {
	char *prefix;
	int   counter;
} gen_names[] = {
	"Dummy", 	0,	
	"Temp",  	0,
	"Module", 	0,		
	"Proc", 	0,		
	"Func", 	0,		
	"ForwardProc", 	0,		
	"ForwardFunc", 	0,		
	"Type", 	0,		
	"Con", 		0,			
	"Var", 		0,			
	"Label", 	0,		
	"Elem",		0,	
	"Recvar",	0,	
	"PackedType", 	0,		
	"Parm", 	0,	
	"SysProc", 	0,	
	"TypeName", 	0,	
	"CBlock", 	0,	
	"CBlockElement",0,
	"FuncParm", 	0,	
	"ProcParm", 	0,	
	"Asm", 		0,		
	"Block", 	0,	
	"Entry", 	0,	
	"NameList", 	0,
};


/*JH*/ GenPName(Idx)
/*JH*/ int Idx;
/*JH*/ {
/*JH*/     int i,suf,len;
/*JH*/     char name[1000],buf[100];
/*JH*/     buf[0] = '_';
/*JH*/
/*JH*/     name[0] = '(';
/*JH*/     name[1] = '*';
/*JH*/
/*JH*/     GetString(STab[Idx].SName,&name[2]);
/*JH*/     len = strlen(name);
/*JH*/
/*JH*/     strcat( name , ")" ) ;
/*JH*/     len++ ;
/*JH*/
/*JH*/     return(EnterString(name,len));
/*JH*/ }

GenName(Idx)
int Idx;
{
  char name[1000];
  int namelen;
  struct gen_names *gnp;

  gnp = &gen_names[STab[Idx].SKind];

  for(;;) {
#ifdef RASN /* RASN4 ins. file pref. in gen.ed types */
      if ( RAS_control ['0'] )
	sprintf(name,"ptoc_%s_%4.4s_%d",gnp->prefix, tok_first_type, gnp->counter++);
      else
#endif
      sprintf(name,"ptoc_%s%d",gnp->prefix, gnp->counter++);
      namelen=strlen(name);
      if (FindString(name,namelen) == -1)
          return(EnterString(name,namelen));
  }
}

BuildUnique(Idx,Suffix)
int Idx,Suffix;
{
    int i,suf,len;
    char name[1000],buf[100];	
    buf[0] = '_';

    GetString(STab[Idx].SName,name);
    while(Suffix > 0) {
	suf = STab[Suffix].SName;
        GetString(abs(suf),&buf[1]);
        strcat(name,buf);
        if (suf < 0)
             break;
        Suffix = STab[Suffix].SParent;
    }
    len = strlen(name);
    i = 0;
    while(FindString(name,strlen(name)) != -1)
    	sprintf(&name[len],"_%d",i++);

    return(EnterString(name,strlen(name)));
}


BuildTypeName(Idx,Suffix)
int Idx,Suffix;
{
    int i,suf,len;
    char name[1000],buf[100];	
    buf[0] = '_';

    name[0] = 't';
    name[1] = '_';

    GetString(STab[Idx].SName,&name[2]);
    len = strlen(name);
    i = 0;
    while(FindString(name,strlen(name)) != -1)
    	sprintf(&name[len],"_%d",i++);

    /* 1.5.95 if 't_' is not given to 'FindString' above, it returns an error;
     * so we drop it on output only - let's try */
    return(EnterString(name+2,strlen(name)-2));
}

BuildL0Name(Idx,Suffix)
int Idx,Suffix;
{
    int i,suf,len;
    char name[1000],buf[100];	
    buf[0] = '_';

    name[0] = 'l';
    name[1] = '0';
    name[2] = '_';

    GetString(STab[Idx].SName,&name[3]);
    len = strlen(name);
    i = 0;
    while(FindString(name,strlen(name)) != -1)
    	sprintf(&name[len],"_%d",i++);

    return(EnterString(name,strlen(name)));
}

int UnCKeyword(index)
int index;    /* StrTable-Index */
{
  char s[100];
  int i,len;
  GetString (index, s);
  for (i=0; Key[i]; i++)
    if (strcmp(s,Key[i])==0) {
	for(;;) {
		len = strlen(s);
		s[len++] = '_'; s[len] = 0;
		if (FindString(s,len) == -1)
		    return(EnterString(s,len));
	}
    }
  return(index);
}


int SpecFunction(index)
int index;    /* StrTable-Index */
  {
  char s[1000];
  GetString (index, s);
  if (!strcmp(s,"_FNIL"))
    return(1);
  if (!strcmp(s,"_UNIT"))
    return(2);
  return(0);
  }

int ToSymtabIdx(p)
SYMTAB *p;
{
	if (p)
	    return(p - STab);
	else
	    return(0);	
}	

long_to_string(low,high,s,l)
int  low,high;
char *s;
int *l;
{
#if OSF1
  union {
     long x;
     struct {
        int  low,high;
     } y;
  } v;

  v.y.low = low;
  v.y.high= high;

  sprintf(s,"%ld",v.x);
#else
  sprintf(s,"%d",low);
#endif
  *l = strlen(s);
}

double_to_string(low,high,s,l)
int  low,high;
char *s;
int *l;
{
  union {
     double x;
     struct {
        int  low,high;
     } y;
  } v;

  v.y.low = low;
  v.y.high= high;

  sprintf(s,"%.20e",v.x);
  *l = strlen(s);
}


/* prepare symbol table for initialized data output */

extern int	datacnt;
extern struct datafield {
	int symfield, valuefield;
} *datafile;


Ref_STab_datafile()
{
  int entry;
  int Idx, Idx_alt;

  entry = 0;
  Idx_alt = 0;
  while (entry < datacnt)
    {
    Idx = (datafile + entry)->symfield;
    if (Idx != Idx_alt)
      {
      if (Idx_alt != 0)
	STab[Idx_alt].SFeld2 = entry - 1;   /* Ende altes Intervalls   */
      STab[Idx].SFeld1 = entry;             /* Anfang neues Intervalls */
      Idx_alt = Idx;
      }
    entry++;
    }
  STab[Idx_alt].SFeld2 = entry - 1;   /* Ende altes Intervalls   */
}



int DataNode(entry)
int entry;
{
  return ((datafile + entry)->valuefield);
}


Precedence(op,l,a)
int op;
int *l, *a;
{

   switch (op) {
	case NopOp:		*l=-1; *a=LEFT ; break;		/* node */
	case LnoOp:		*l= 0; *a=LEFT ; break;		/* fake */
	case LoadOp:		*l=-1; *a=LEFT ; break;		/*      */
	case StoreOp:		*l= 1; *a=RIGHT; break;		/* LongStmnt */
	case StorepOp:		*l=-1; *a=LEFT ; break;		/* UnaryExp */
	case StorepadrOp:	*l=13; *a=RIGHT; break;		/* UnaryExp */
	case LitOp:		*l=15; *a=LEFT ; break;		/* node */
	case LoadadrOp:		*l=13; *a=RIGHT; break;		/* UnaryExp */
	case LoadstrOp:		*l=15; *a=LEFT ; break;		/* node */
	case AddOp:		*l=11; *a=LEFT ; break;		/* BinaryExp */
	case SubtOp:		*l=11; *a=LEFT ; break;		/* BinaryExp */
	case MultOp:		*l=12; *a=LEFT ; break;		/* BinaryExp */
	case DivOp:		*l=12; *a=LEFT ; break;		/* BinaryExp */
/*	case RangeOp:		*l= 0; *a=LEFT ; break;		/* UnaryExp */
	case UminusOp:		*l=13; *a=RIGHT; break;		/* UnaryExp */
	case OrOp:		*l= 3; *a=LEFT ; break;		/* BinaryExp */
	case XorOp:		*l= 3; *a=LEFT ; break;		/* BinaryExp */
	case AndOp:		*l= 4; *a=LEFT ; break;		/* BinaryExp */
	case NotOp:		*l=13; *a=RIGHT; break;		/* UnaryExp */
	case ShlOp:		*l=10; *a=LEFT ; break;		/* BinaryExp */
	case ShrOp:		*l=10; *a=LEFT ; break;		/* BinaryExp */
	case EqOp:		*l= 8; *a=LEFT ; break;		/* BinaryExp */
	case LtOp:		*l= 9; *a=LEFT ; break;		/* BinaryExp */
	case GtOp:		*l= 9; *a=LEFT ; break;		/* BinaryExp */
	case LeOp:		*l= 9; *a=LEFT ; break;		/* BinaryExp */
	case GeOp:		*l= 9; *a=LEFT ; break;		/* BinaryExp */
	case NeOp:		*l= 8; *a=LEFT ; break;		/* BinaryExp */
	case CallOp:		*l=14; *a=LEFT ; break;		/* LongStmnt */
/*	case RtrnOp:		*l= 0; *a=LEFT ; break;		/* spec */
/*	case RtrnxOp:		*l= 0; *a=LEFT ; break;		/* LongStmnt */
/*	case ProcOp:		*l= 0; *a=LEFT ; break;		/* Proc */
	case Sys0Op:		*l=14; *a=LEFT ; break;		/* LongStmnt */
	case LitSizeOp:		*l=15; *a=LEFT ; break;		/* node */
/*	case LitOffsetOp:	*l= 0; *a=LEFT ; break;		/* node */
/*	case AddiOp:		*l= 0; *a=LEFT ; break;		/* UnaryExp */
/*	case SubtiOp:		*l= 0; *a=LEFT ; break;		/* UnaryExp */
/*	case AddixOp:		*l= 0; *a=LEFT ; break;		/* UnaryExp */
/*	case SubtixOp:		*l= 0; *a=LEFT ; break;		/* UnaryExp */
/*	case MultxOp:		*l= 0; *a=LEFT ; break;		/* UnaryExp */
/*	case ContinueOp:	*l= 0; *a=LEFT ; break;		/* spec */
/*	case GotoOp:		*l= 0; *a=LEFT ; break;		/* spec */
	case ModOp:		*l=12; *a=LEFT ; break;		/* BinaryExp */
	case BAndOp:		*l= 7; *a=LEFT ; break;		/* BinaryExp */
	case BOrOp:		*l= 5; *a=LEFT ; break;		/* BinaryExp */
	case BXorOp:		*l= 6; *a=LEFT ; break;		/* BinaryExp */
	case BNotOp:		*l=13; *a=RIGHT; break;		/* UnaryExp */
	case ConvertOp:		*l=13; *a=RIGHT; break;		/* UnaryExp */
	case IdOp:		*l=15; *a=LEFT ; break;		/* node */
	case SubscriptOp:	*l=14; *a=LEFT ; break;		/* BinaryExp */
	case ElementOp:		*l=14; *a=LEFT ; break;		/* BinaryExp */
	case DerefOp:		*l=13; *a=RIGHT; break;		/* UnaryExp */
/*	case LabelOp:		*l= 0; *a=LEFT ; break;		/* spec */
/*	case CaseOp:		*l= 0; *a=LEFT ; break;		/* Case */
/*	case DefaultOp:		*l= 0; *a=LEFT ; break;		/* Case */
/*	case BreakOp:		*l= 0; *a=LEFT ; break;		/* spec */
	case PrefixOp:		*l=13; *a=RIGHT; break;		/* LongStmnt */
	case PostfixOp:		*l=13; *a=RIGHT; break;		/* LongStmnt */
	case ModuloOp:		*l=12; *a=LEFT ; break;		/* BinaryExp */
/*	case FileOp:		*l= 0; *a=LEFT ; break;		/* spec */
/*	case AsmOp:		*l= 0; *a=LEFT ; break;		/* spec */
/*	case RepeatOp:		*l= 0; *a=LEFT ; break;		/* BinaryExp */
/*	case ComplexOp:		*l= 0; *a=LEFT ; break;		/* BinaryExp */
/*	case StaticLinkOp:	*l= 0; *a=LEFT ; break;		/* node */
	case RegOp:		*l=14; *a=LEFT ; break;		/* node */
/*	case EntryOp:		*l= 0; *a=LEFT ; break;		/* spec */
	case StoreadrOp:	*l=13; *a=RIGHT; break;		/* LongStmnt */
	case ListOp:		*l= 0; *a=LEFT ; break;		/* ExpList */
	case CommaOp:		*l= 0; *a=LEFT ; break;		/* ExpList */
/*	case PushOp:		*l= 0; *a=LEFT ; break;		/* spec */
/*	case PopOp:		*l= 0; *a=LEFT ; break;		/* spec */
/*	case PushAdrOp:		*l= 0; *a=LEFT ; break;		/* spec */
/*	case PopAdrOp:		*l= 0; *a=LEFT ; break;		/* spec */
/*	case BeginOp:		*l= 0; *a=LEFT ; break;		/* spec */
/*	case EndOp:		*l= 0; *a=LEFT ; break;		/* spec */
	case IfthenelseOp:	*l= 2; *a=RIGHT; break;		/* ifthenelse */
	otherwise:		*l=-1; *a=LEFT ; break;
      }
}


int PAR(Oop,Side,Iop)
int Oop,Side,Iop;
{
    int olevel,ilevel,oassoc,iassoc;

    Precedence(Oop,&olevel,&oassoc);
    Precedence(Iop,&ilevel,&iassoc);

    return (olevel > ilevel || (olevel == ilevel && iassoc != Side));
}

setbit(v,flag)
int *v; int flag;
{
	*v |= flag;
}

clearbit(v,flag)
int *v; int flag;
{
	*v &= ~flag;
}

testbit(v,flag)
int v,flag;
{
	return((v & flag) != 0);
}

/* some functions from codeservice.c used in front end */

double double_to_double(p1, p2)
int p1, p2;
{
    union {
        struct {
            int i1, i2;
        } i;
        double d;
    } kludge;
    kludge.i.i1 = p1;
    kludge.i.i2 = p2;
    return(kludge.d);
}

long double_to_long(p1, p2)
int p1, p2;
{
#if OSF1
    union {
        struct {
            int i1, i2;
        } i;
        long l;
    } kludge;
    kludge.i.i1 = p1;
    kludge.i.i2 = p2;
    return(kludge.l);
#else
    return ((long)p1);
#endif
}

unsigned long double_to_ulong(p1, p2)
int p1, p2;
{
#if OSF1
    union {
        struct {
            int i1, i2;
        } i;
        unsigned long ul;
    } kludge;
    kludge.i.i1 = p1;
    kludge.i.i2 = p2;
    return(kludge.ul);
#else
    return ((unsigned long)p1);
#endif
}

FILE *ObjFile = NULL;
char *ObjectFileName;

cgenerror( s, a, b, c ) char *s; { /* compiler error: die */
   fprintf( stderr, "compiler error: " );
   fprintf( stderr, s, a, b, c );
   fprintf( stderr, "\n" );
   fflush ( stderr );
#if unix
   kill(getpid(),3);	/* and produce a core */
#else
   s = 0 ;
   *s = '\0' ; /* memory fault */
#endif
   sql__exit(1);
}

OpenObjFile (ObjFileName)
	char	*ObjFileName;
{
	ObjectFileName = ObjFileName;
	ObjFile = fopen (ObjFileName, "w+");
}

/*
 * This is bad....
 * The ObjFile is closed twice. Here and during sql__exit()
 * Since sql__exit is always called this Routine is not only
 * not needed, but would lead to an error...
 */
/*
 *  CloseObjFile ()
 *  {
 *      fclose (ObjFile);
 *  }
 */

#include "../libpc/h00vars.h"

rediroutput(f)
struct iorec *f;
{
    extern FILE *ObjFile;
    if (f->fbuf) fclose (f->fbuf); /* remove tempfile */
    f->fbuf = ObjFile;
}


void
ProcessFillOpt(unsigned char *po)
{

	/* process the 2 hex digits after '-f' option */
	FillByte = strtol(&po[2], 0, 16);

	FillWord = (FillByte << 24) | (FillByte << 16) |
			(FillByte << 8) | FillByte;
}

void
ProcessFillForVar(unsigned char *po)
{
	unsigned char	padding_byte;

	/* process the 2 hex digits after '-g' option */
	padding_byte = strtol(&po[2], 0, 16);
	FillForVar = (padding_byte << 24) | (padding_byte << 16) |
			(padding_byte << 8) | padding_byte;
}

int
GenConstName(constant)
int constant;
{
  char name[100];

  if (constant < 0)
    sprintf(name,"C__%d", -constant);
  else
    sprintf(name,"C_%d", constant);
  return EnterString(name,strlen(name));
}

int
GenCharName(character)
int character;
{
  char name[100];

  if (isalnum(character))
    sprintf(name,"C_%c", character);
  else
    sprintf(name,"C_%03o", character);
  return EnterString(name,strlen(name));
}

int
GenConDclName(stix)
int stix;
{
  char name[100];

  sprintf(name,"C_%s", GetStringPtr(STab [stix].SName));
  return EnterString(name,strlen(name));
}

#ifdef RASX /*RAS06*/
int
NameInNames ( NameIdx, Names )
int NameIdx;
char * Names;
{
  char name [ 100 ], namex [ 100 ];
  GetString(NameIdx,&name[0]);
  sprintf ( namex, ",%s,", name );
  return ( NULL != strstr ( Names, namex ) );
}
#endif
