/*!
  @file           veo21.c
  @author         JoergM
  @special area   RTE internal xuser access
  @brief          Crypt routines
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "heo21.h"
#include "SAPDBCommon/SAPDB_string.h"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	__inline
#endif

#define MOD__  "VEO21.C : "
#define MF__   MOD__"UNDEFINED"

#define ord(x) ((unsigned)(x))


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

void 
sql21write_crypt (tsp00_Int4 Zahl, tsp00_C20 Erg)
{
int             i;

    SAPDB_memcpy (Erg, "00000000000000000000", 20);

    if (Zahl < 0)
    {
	Zahl = -Zahl;
	Erg[0] = (unsigned char) '-';
    }
    i = 20;
    do
    {
	Erg[i + -1] = (unsigned char) (ord (48) + Zahl % 10);
	i = i + -1;
	Zahl = Zahl / 10;
    }
    while (!(Zahl == 0 || i == 1));
}

/*------------------------------*/

tsp00_Int4 
sql21read_crypt (tsp00_C20 Puffer)
{
int             _sql21read_crypt;
int             i;
int             ziffer;
tsp00_Int4      Zahl;

    Zahl = 0;
    i = 2;
    for (;;)
    {
	ziffer = ord ((unsigned char) Puffer[i + -1]) - ord (48);
	Zahl = Zahl * 10 + ziffer;
	if (i == 20)
	{
	    break;
	}
	i += 1;
    }

    if ((int) Puffer[0] == 45)
    {
	Zahl = -Zahl;
    }
    _sql21read_crypt = Zahl;

    return (_sql21read_crypt);
}

/*------------------------------*/

void 
sql21put_name (tsp00_C20 ClearName, tsp00_CryptName Crypt)
{
int             i;
tsp00_Int4      vp1;
tsp00_Int4      vp2;
tsp00_Int4      vp3;
tsp00_Int4      left;
tsp00_Int4      right;

    if (memcmp (ClearName, "                    ", 20) == 0 ? 1 : 0)
    {
	i = 1;
	for (;;)
	{
	    Crypt[i + -1] = 0;
	    if (i == 6)
	    {
		break;
	    }
	    i += 1;
	}
    }
    else
    {
	vp1 = 2;
	vp2 = 521;
	vp3 = 133379;
	i = 1;
	for (;;)
	{
	    Crypt[i + -1] = ord ((unsigned char) ClearName[i * 3 + -3]) * vp3 + ord ((unsigned char) ClearName[i * 3 + -2]) * vp2 + ord ((unsigned char) ClearName[i * 3 + -1]) * vp1;
	    if (i == 6)
	    {
		break;
	    }
	    i += 1;
	}
	i = 1;
	for (;;)
	{
	    if (i > 1)
	    {
		left = Crypt[i + -2];
	    }
	    else
	    {
		left = vp3;
	    }
	    Crypt[i + -1] = Crypt[i + -1] + left % 61 * (vp3 * 126 + -1);
	    if (i == 6)
	    {
		break;
	    }
	    i += 1;
	}

	i = 6;
	for (;;)
	{
	    if (i < 5)
	    {
		right = Crypt[i];
	    }
	    else
	    {
		right = vp2;
	    }
	    Crypt[i + -1] = Crypt[i + -1] + right % 61 * (vp3 * 128 + -1);
	    if (i == 1)
	    {
		break;
	    }
	    i -= 1;
	}

	i = 1;
	for (;;)
	{
	    if ((char) (Crypt[i + -1] & 1))
	    {
		Crypt[i + -1] = -Crypt[i + -1];
	    }
	    if (i == 6)
	    {
		break;
	    }
	    i += 1;
	}
    }
}

/*------------------------------*/

void 
sql21get_name (tsp00_C20 ClearName, tsp00_CryptName Crypt)
{
int             i;
tsp00_Int4      vp1;
tsp00_Int4      vp2;
tsp00_Int4      vp3;
tsp00_Int4      left;
tsp00_Int4      right;
tsp00_Int4      exp1;
tsp00_Int4      exp2;
tsp00_Int4      exp3;
tsp00_CryptName help;
char            pw_belegt;

    pw_belegt = 0;

    i = 1;

    for (;;)
    {
	pw_belegt = Crypt[i + -1] != -2 || pw_belegt;
	if (i == 6)
	{
	    break;
	}
	i += 1;
    }

    if (!pw_belegt)
	SAPDB_memcpy (ClearName, "                    ", 20);
    else
    {
	vp1 = 2;
	vp2 = 521;
	vp3 = 133379;
	SAPDB_memcpy (help, Crypt, 6);

	i = 1;

	for (;;)
	{
	    if ((char) (help[i + -1] & 1))
	    {
		help[i + -1] = -help[i + -1];
	    }
	    if (i == 6)
	    {
		break;
	    }
	    i += 1;
	}

	i = 1;
	for (;;)
	{
	    if (i < 5)
		right = help[i];
	    else
		right = vp2;

	    help[i + -1] = help[i + -1] - right % 61 * (vp3 * 128 + -1);
	    if (i == 6)
		break;

	    i += 1;
	}

	i = 6;
	for (;;)
	{
	    if (i > 1)
		left = help[i + -2];
	    else
		left = vp3;

	    help[i + -1] = help[i + -1] - left % 61 * (vp3 * 126 + -1);
	    if (i == 1)
		break;

	    i -= 1;
	}

	i = 1;
	for (;;)
	{
	    exp3 = help[i + -1];
	    exp1 = exp3 / vp3;
	    exp2 = exp3 % vp3;
	    ClearName[i * 3 + -3] = (unsigned char) exp1;
	    exp3 = exp2;
	    exp1 = exp3 / vp2;
	    exp2 = exp3 % vp2;
	    ClearName[i * 3 + -2] = (unsigned char) exp1;
	    exp3 = exp2;
	    exp1 = exp3 / vp1;
	    exp2 = exp3 % vp1;
	    ClearName[i * 3 + -1] = (unsigned char) exp1;
	    if (i == 6)
		break;

	    i += 1;
	}
    }
}


/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
