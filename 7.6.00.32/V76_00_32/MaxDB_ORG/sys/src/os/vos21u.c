/*!
  @file           vos21u.c
  @author         FrankH
  @brief          CRYPT
  @see            

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




//
// INCLUDE FILES
//
#include "gos00.h"
#include "heo46.h"
#include "hos21u.h"


//
//  DEFINES
//

#define MOD__  "VOS21UC : "
#define MF__   MOD__"UNDEFINED"

#define ord(x) ((unsigned)(x))

//
//  MACROS
//


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


//
// EXTERNAL VARIABLES
//


//
//  EXPORTED VARIABLES
//


//
// LOCAL VARIABLES
//


//
// LOCAL FUNCTION PROTOTYPES
//

VOID sql21write_crypt ( INT4 zahl, C20 erg );
INT4 sql21read_crypt  ( C20 puffer );
VOID sql21get_name    ( C20 clearname, CRYPTNAME crypt );



//
// ========================== GLOBAL FUNCTIONS ================================
//

VOID sql21write_crypt( INT4 Zahl, C20 Erg )
    {
    INT   i;

    SAPDB_memcpy  (Erg, "00000000000000000000", 20 );

    if (Zahl < 0)
        {
        Zahl = -Zahl;
        Erg[0] = (unsigned char)'-';
        }
    i = 20;
    do
        {
        Erg[i + -1] = (unsigned char ) (ord(48) + Zahl % 10);
        i = i + -1;
        Zahl = Zahl / 10;
        }
    while (!(Zahl == 0 || i == 1));
    }

/*------------------------------*/

INT4 sql21read_crypt( C20 Puffer )
    {
    INT   _sql21read_crypt;
    INT   i;
    INT   ziffer;
    INT4  Zahl;

    Zahl = 0;
    i = 2;
    for (;;)
        {
        ziffer = ord((unsigned char ) Puffer[i + -1]) - ord(48);
        Zahl = Zahl * 10 + ziffer;
        if (i == 20)
            {
            break;
            }
        i += 1;
        }

    if ((int ) Puffer[0] == 45)
        {
        Zahl = -Zahl;
        }
    _sql21read_crypt = Zahl;

    return(_sql21read_crypt);
    }

/*------------------------------*/

VOID sql21put_name( C20 ClearName, CRYPTNAME Crypt )
    {
    INT    i;
    INT4   vp1;
    INT4   vp2;
    INT4   vp3;
    INT4   left;
    INT4   right;

    if ( memcmp (ClearName , "                    ", 20) == 0 ? 1 : 0)
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
            Crypt[i + -1] = ord((unsigned char ) ClearName[i * 3 + -3]) * vp3 + ord((unsigned char ) ClearName[i * 3 + -2]) * vp2 + ord((unsigned char ) ClearName[i * 3 + -1]) * vp1;
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
            if ((char ) (Crypt[i + -1] & 1))
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

VOID sql21get_name( C20 ClearName, CRYPTNAME Crypt )
    {
    INT        i;
    INT4       vp1;
    INT4       vp2;
    INT4       vp3;
    INT4       left;
    INT4       right;
    INT4       exp1;
    INT4       exp2;
    INT4       exp3;
    CRYPTNAME  help;
    CHAR       pw_belegt;

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
        SAPDB_memcpy ( ClearName,"                    ", 20 );
    else
        {
        vp1 = 2;
        vp2 = 521;
        vp3 = 133379;
        SAPDB_memcpy ( help, Crypt, 6 );

        i = 1;

        for (;;)
            {
            if ((char ) (help[i + -1] & 1))
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
            ClearName[i * 3 + -3] = (unsigned char ) exp1;
            exp3 = exp2;
            exp1 = exp3 / vp2;
            exp2 = exp3 % vp2;
            ClearName[i * 3 + -2] = (unsigned char ) exp1;
            exp3 = exp2;
            exp1 = exp3 / vp1;
            exp2 = exp3 % vp1;
            ClearName[i * 3 + -1] = (unsigned char ) exp1;
            if (i == 6)
                break;

            i += 1;
            }
        }
    }

//
// ========================== LOCAL FUNCTIONS =================================
//

//
// =============================== END ========================================
//
