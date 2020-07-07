/*!
  @file           ven18.c
  @author         JoergM
  @brief          Client RunTime: sqlcharsetname
  @see            


                Find &lt;charsetname> corresponding to DBTERM or TERM

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end



\endif
*/

#define MOD__ "ven18.c:"

#include "gen00.h"
#include "hen40.h"
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include "geo007_1.h"
#include "heo01.h"
#include "heo46.h"

/*
 *  DECLARATION OF GLOBAL PROCEDURES/FUNCTIONS (Entrys)
 */
void sqlcharsetname ( tsp00_KnlIdentifier CharSetName );

/*
 *  DECLARATION OF EXTERNAL PROCEDURES/FUNCTIONS (Entrys)
 */
extern char     *getenv ();

/*
 *  DECLARATION OF INTERNAL PROCEDURES/FUNCTIONS (Entrys)
 */
static int		sqlBlackLen( char *Pstring );

#define CHARSETNAME_ENV		"DBCHARSET"
#define CHARSETNAME_FNAME	"term/charsetnames"
#define C_SPACE	' '

/* sqlcharsetname
 * File has 2 words per line: <terminal type> and <charsetname>.
 * The <terminal type> is DBTERM. If DBTERM is not set, use TERM.
 * Caller wants <charsetname> copied to given buffer (pad on right with
 *		spaces, do NOT terminate with null char)
 */
#undef MF__
#define MF__ MOD__"sqlcharsetname"
void
sqlcharsetname( 
tsp00_KnlIdentifier CharSetName )		/* copy of <charsetname> */
{
	char			readbuf[256];
	char			*Pc;			/* moveable ptr to readbuf */
	char			*Ptermtype;		/* DBTERM or TERM */
	int				termlen;
	int				charsetlen;
	tsp00_Pathc 	fname;			/* charsetname file name */
	FILE			*Pfile;			/* charsetname file id */
        tsp00_Pathc     terminfoPath;
        tsp01_RteError  rteError;

    SAPDB_memset( CharSetName, ' ', sizeof (tsp00_KnlIdentifier));

	/* If DBCHARSET is in environment, use this as the charsetname */
	Pc = getenv ( CHARSETNAME_ENV );
	if ( Pc )
	{
        DBG3 (( MF__, "%s = '%s' ", CHAR_SET_ENV_VAR, Pc));
        eo46CtoP ( CharSetName, Pc, sizeof(tsp00_KnlIdentifier) );
        DBGOUT;
        return;
	}

        /* PTS 1108088 */
	/* Build filename */
        if ( !sqlGetIndependentTerminfoPath(terminfoPath, true, &rteError) )
        {
          DBG3 (( MF__, "IndependentTerminfoPath not found:(%d)%s (%d)%s", 
                  rteError.RteErrCode, rteError.RteErrText,
                  rteError.OsErrCode, rteError.OsErrText ));
          DBGOUT;
	  return;
	}

    /* PTS 1105566 */
    if ( strlen(CHARSETNAME_FNAME) + strlen (terminfoPath) < sizeof ( fname ) ) 
      strcat(strcpy((char *)fname, terminfoPath), CHARSETNAME_FNAME);
    else
    {
 	  MSGD(( -11117, ERR_TYPE, "CHARSET ", "Path to long\n" ));
      return;
    }

    /* If DBTERM is not set, use TERM */
	if(!(Ptermtype = getenv("DBTERM")))
	{
		DBG1(( MF__,"warning: DBTERM not set\n" ));
		if(!(Ptermtype = getenv("TERM")))
		{
    		MSGD(( -11117, ERR_TYPE, "CHARSET ", "DBTERM and TERM not set\n" ));
			return;
		}
	}

	/* Open charsetname file for reading */
	if(!(Pfile = fopen((char *)fname, "r")))
	{
        /* PTS 1106084 */
		DBG1(( MF__, "open charset %s error, %s\n", fname, sqlerrs() ));
		return;
	}

	/* For each line, 1st word is <term type> and 2nd word is <charsetname> */
	termlen = strlen(Ptermtype);
	Pc = fgets(readbuf, sizeof(readbuf), Pfile);
	while(Pc)
	{
		/* Skip white space */
		while(isspace(*Pc))
		{
			Pc++;
		}
		if(termlen == sqlBlackLen(Pc) && !memcmp(Pc, Ptermtype, termlen))
		{
			/* 1st word matches our termtype: skip 1st word and white space */
			Pc += termlen;
			while(isspace(*Pc))
			{
				Pc++;
			}

			/* Look at 2nd word */
			if( !sqlBlackLen(Pc) )
			{
				/* Can't find 2nd word: try next line */
				MSGD(( -11317, INFO_TYPE, "CHARSET ", "charset <term type> is '%s'\n", Ptermtype ));
				DBG1(( -11317, INFO_TYPE, "CHARSET ", "warning: <charsetname> is missing\n" ));
			}
			else
			{
                eo46CtoP ( CharSetName, Pc, sizeof(tsp00_KnlIdentifier) );
				break;
			}
		}
		/* Get next line */
		Pc = fgets(readbuf, sizeof(readbuf), Pfile);
	}

	/* Close file */
	(void)fclose(Pfile);
}

static int
sqlBlackLen(
char *Pstring)
{
	char	*Pc;

	for(Pc = Pstring; *Pc && !isspace(*Pc); Pc++)
	{
		;
	}
	return(Pc - Pstring);
}
