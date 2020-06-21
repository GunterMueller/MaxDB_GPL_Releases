/*


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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "AHuff.h"
#include "BitEA.h"

#define END_OF_STREAM   256
#define ESCAPE          257
#define ANZAHL_SYMBOLE  258
#define ANZAHL_TAB_KNOTEN ((ANZAHL_SYMBOLE * 2)-1)
#define WURZEL_KNOTEN   0
#define MAX_GEWICHTUNG  0x8000
#define TRUE            1
#define FALSE           0

typedef struct baum {
    int blatt[ANZAHL_SYMBOLE];
    int naechster_freier;
    struct knten {
        unsigned int gewichtung;
        int eltern_knoten;
        int kind_ist_blatt;
        int kind;
    } knoten[ANZAHL_TAB_KNOTEN];
} BAUM;

BAUM Baum;

static void    InitialisiereBaum       (BAUM*);
static void    CodiereSymbol           (BAUM*, unsigned int, BIT_FILE*);
static int     DecodiereSymbol         (BAUM*, BIT_FILE*);
static void    AktualisiereModell      (BAUM*, int);
static void    BaumAufbauen            (BAUM*);
static void    vertausche_knoten       (BAUM*, int, int);
static void    neuen_knoten_aufnehmen  (BAUM*, int);


/*==========================================================================*/

void KomprimiereDatei
(
    FILE        * eingabe,
    BIT_FILE    * ausgabe
)
{
    int c;
    InitialisiereBaum ( &Baum );
    while ( ( c = getc ( eingabe ) ) != EOF )
    {
        CodiereSymbol ( &Baum, c, ausgabe );
        AktualisiereModell ( &Baum, c );
    }

    CodiereSymbol ( &Baum, END_OF_STREAM, ausgabe );
}

/*==========================================================================*/

void ExpandiereDatei 
(
    BIT_FILE    * eingabe,
    FILE        * ausgabe
)
{
    int c;
    InitialisiereBaum ( &Baum );
    while ( ( c = DecodiereSymbol ( &Baum, eingabe ) ) != END_OF_STREAM )
    {
        if ( putc ( c, ausgabe ) == EOF )
            fataler_fehler("Fehler bei der Zeichenausgabe\n");
        AktualisiereModell ( &Baum, c );
    }
}

/*==========================================================================*/

void InitialisiereBaum
(
    BAUM    * baum
)
{
    int i;
    baum->knoten[WURZEL_KNOTEN].kind = WURZEL_KNOTEN + 1;
    baum->knoten[WURZEL_KNOTEN].kind_ist_blatt = FALSE;
    baum->knoten[WURZEL_KNOTEN].gewichtung = 2;
    baum->knoten[WURZEL_KNOTEN].eltern_knoten = -1;

    baum->knoten[WURZEL_KNOTEN+1].kind = END_OF_STREAM;
    baum->knoten[WURZEL_KNOTEN+1].kind_ist_blatt = TRUE;
    baum->knoten[WURZEL_KNOTEN+1].gewichtung = 1;
    baum->knoten[WURZEL_KNOTEN+1].eltern_knoten = WURZEL_KNOTEN;

    baum->blatt[END_OF_STREAM] = WURZEL_KNOTEN + 1;

    baum->knoten[WURZEL_KNOTEN+2].kind = ESCAPE ;
    baum->knoten[WURZEL_KNOTEN+2].kind_ist_blatt = TRUE;
    baum->knoten[WURZEL_KNOTEN+2].gewichtung = 1;
    baum->knoten[WURZEL_KNOTEN+2].eltern_knoten = WURZEL_KNOTEN;

    baum->blatt[ESCAPE] = WURZEL_KNOTEN + 2;

    baum->naechster_freier = WURZEL_KNOTEN + 3;

    for ( i = 0 ; i < END_OF_STREAM; i++ )
        baum->blatt[ i ] = -1;
}


/*==========================================================================*/

void CodiereSymbol
(
    BAUM            *baum,
    unsigned int    c,
    BIT_FILE        * ausgabe
)
{
    unsigned long code;
    unsigned long aktuelles_bit;
    int code_groesse;
    int aktueller_knoten;

    code = 0;
    aktuelles_bit =1;
    code_groesse = 0;
    aktueller_knoten = baum->blatt[ c ];
    if ( aktueller_knoten == -1 )
        aktueller_knoten = baum->blatt[ ESCAPE ];

    while ( aktueller_knoten != WURZEL_KNOTEN )
    {
        if ( ( aktueller_knoten & 1 ) == 0 )
            code |= aktuelles_bit;
        aktuelles_bit <<=1;
        code_groesse++;
        aktueller_knoten = baum->knoten[ aktueller_knoten ].eltern_knoten;
    }
    OutputBits ( ausgabe, code, code_groesse );
    if ( baum->blatt[ c ] == -1 )
    {
        OutputBits ( ausgabe, (unsigned long) c, 8 );
        neuen_knoten_aufnehmen ( baum, c );
    }
}

/*==========================================================================*/

int DecodiereSymbol
(
    BAUM        * baum,
    BIT_FILE    * eingabe
)
{
    int aktueller_knoten ;
    int c;

    aktueller_knoten = WURZEL_KNOTEN;
    while ( ! baum->knoten[ aktueller_knoten ].kind_ist_blatt )
    {
        aktueller_knoten = baum->knoten[ aktueller_knoten ].kind;
        aktueller_knoten += InputBit ( eingabe );
    }
    c = baum->knoten[ aktueller_knoten ].kind;
    if ( c == ESCAPE )
    {
        c = (int) InputBits ( eingabe, 8 );
        neuen_knoten_aufnehmen ( baum, c );
    }
    return ( c );
}

/*==========================================================================*/

void AktualisiereModell
(
    BAUM    * baum,
    int     c
)
{
    int aktueller_knoten ;
    int neuer_knoten;

    if ( baum->knoten[ WURZEL_KNOTEN ].gewichtung == MAX_GEWICHTUNG )
        BaumAufbauen( baum );
    aktueller_knoten = baum->blatt[ c ];
    while ( aktueller_knoten != -1 )
    {
        baum->knoten[ aktueller_knoten ].gewichtung++;
        for (   neuer_knoten = aktueller_knoten; 
                neuer_knoten > WURZEL_KNOTEN; 
                neuer_knoten-- 
            )
            if ( baum->knoten[ neuer_knoten - 1].gewichtung >= 
                 baum->knoten[ aktueller_knoten ].gewichtung )
                break;
        if ( aktueller_knoten != neuer_knoten )
        {
            vertausche_knoten ( baum, aktueller_knoten , neuer_knoten );
            aktueller_knoten = neuer_knoten;
        }
        aktueller_knoten  = baum->knoten[ aktueller_knoten ].eltern_knoten;
    }
}

/*==========================================================================*/

void BaumAufbauen
(
    BAUM * baum
)
{
    int i, j, k;
    unsigned int gewichtung;

    j = baum->naechster_freier - 1;

    for ( i = j; i >= WURZEL_KNOTEN; i-- )
    {
        if ( baum->knoten[ i ].kind_ist_blatt )
        {
            baum->knoten[ j ] = baum->knoten[ i ];
            baum->knoten[ j ].gewichtung = ( baum->knoten[ j ].gewichtung + 1 ) / 2;
            j--;
        }
    }

    for( i = baum->naechster_freier - 2; j >= WURZEL_KNOTEN; i -= 2, j-- )
    {
        k = i + 1;
        baum->knoten[ j ].gewichtung = baum->knoten[ i ].gewichtung + 
                                       baum->knoten[ k ].gewichtung;
        gewichtung = baum->knoten[ j ].gewichtung;
        baum->knoten[ j ].kind_ist_blatt = FALSE;
        for ( k = j + 1; gewichtung < baum->knoten[ k ].gewichtung; k++ );
            k--;
        memmove ( &baum->knoten[ j ], &baum->knoten[ j + 1 ], 
                  ( k - j ) * sizeof ( struct knten ) );
        baum->knoten[ k ].gewichtung = gewichtung;
        baum->knoten[ k ].kind = i;
        baum->knoten[ k ].kind_ist_blatt = FALSE;
    }

    for ( i = baum->naechster_freier - 1; i >= WURZEL_KNOTEN; i-- )
    {
        if ( baum->knoten[ i ].kind_ist_blatt )
        {
            k = baum->knoten[ i ].kind;
            baum->blatt[ k ] = i;
        }
        else
        {
            k = baum->knoten[ i ].kind;
            baum->knoten[ k ].eltern_knoten = baum->knoten[ k + 1 ].eltern_knoten = i;
        }
    }
}


/*==========================================================================*/

void vertausche_knoten
(
    BAUM    * baum,
    int     i,
    int     j
)
{
    struct knten temp;

    if ( baum->knoten[ i ].kind_ist_blatt )
        baum->blatt[baum->knoten[ i ].kind] = j;
    else
    {
        baum->knoten[ baum->knoten[ i ].kind ].eltern_knoten = j;
        baum->knoten[ baum->knoten[ i ].kind + 1 ].eltern_knoten = j;
    }
    if ( baum->knoten[ j ].kind_ist_blatt )
        baum->blatt[ baum->knoten[ j ].kind ]=i;
    else
    {
        baum->knoten[ baum->knoten[ j ].kind ].eltern_knoten = i;
        baum->knoten[ baum->knoten[ j ].kind +  1 ].eltern_knoten = i;
    }
    temp = baum->knoten[ i ];
    baum->knoten[ i ] = baum->knoten[ j ];
    baum->knoten[ i ].eltern_knoten = temp.eltern_knoten;
    temp.eltern_knoten = baum->knoten[ j ].eltern_knoten;
    baum->knoten[ j ] = temp;
}

/*==========================================================================*/

void neuen_knoten_aufnehmen
(
    BAUM    * baum,
    int     c
)
{
    int geringster_knoten;
    int neuer_knoten;
    int null_gewichtungs_knoten;

    geringster_knoten = baum->naechster_freier - 1;
    neuer_knoten = baum->naechster_freier;
    null_gewichtungs_knoten = baum->naechster_freier + 1;
    baum->naechster_freier += 2;
    baum->knoten[ neuer_knoten ] = baum->knoten[ geringster_knoten ];
    baum->knoten[ neuer_knoten ].eltern_knoten = geringster_knoten;
    baum->blatt[ baum->knoten[ neuer_knoten ].kind ] = neuer_knoten;
    baum->knoten[ geringster_knoten ].kind = neuer_knoten;
    baum->knoten[ geringster_knoten ].kind_ist_blatt = FALSE;

    baum->knoten[ null_gewichtungs_knoten ].kind = c;
    baum->knoten[ null_gewichtungs_knoten ].kind_ist_blatt = TRUE;
    baum->knoten[ null_gewichtungs_knoten ].gewichtung = 0;
    baum->knoten[ null_gewichtungs_knoten ].eltern_knoten = geringster_knoten;
    baum->blatt[ c ] = null_gewichtungs_knoten;
}

