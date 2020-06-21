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
#include <stdarg.h>
#include <stdlib.h>

#include "BitEA.h"

/*==========================================================================*/

BIT_FILE        * OpenOutputBitFile 
(
    char * name
)
{
    BIT_FILE * bit_file;
    bit_file = (BIT_FILE *) calloc(1, sizeof(BIT_FILE));
    if ( bit_file == NULL ) return (bit_file);
    bit_file->file = fopen(name, "wb");
    bit_file->akt_byte = 0;
    bit_file->maske = 0x80;
    return (bit_file);
}

/*==========================================================================*/

BIT_FILE * OpenInputBitFile
(
    char * name
)
{
    BIT_FILE * bit_file;
    bit_file = (BIT_FILE *) calloc(1, sizeof(BIT_FILE));
    if ( bit_file == NULL ) return (bit_file);
    bit_file->file = fopen(name, "rb");
    bit_file->akt_byte = 0;
    bit_file->maske = 0x80;
    return (bit_file);
}

/*==========================================================================*/

void CloseOutputBitFile 
(
    BIT_FILE * bit_file
)
{
    if ( bit_file->maske != 0x80 )
    if ( putc(bit_file->akt_byte, bit_file->file ) != bit_file->akt_byte)
        fataler_fehler("Fataler Fehler in CloseOutputBitFile!\n");          
    fclose(bit_file->file);
    free((char*)bit_file);
}

/*==========================================================================*/

void CloseInputBitFile 
(
    BIT_FILE *bit_file
)
{
    fclose(bit_file->file);
    free((char*)bit_file);
}

/*==========================================================================*/

void OutputBit
(
    BIT_FILE * bit_file,
    int bit
)
{
    if ( bit ) 
        bit_file->akt_byte |= bit_file->maske; 
    bit_file->maske >>= 1;
    if ( bit_file->maske == 0 )
    {
        if ( putc(bit_file->akt_byte, bit_file->file) != bit_file->akt_byte )
            fataler_fehler("Fataler Fehler in OutputBit!\n");          
        bit_file->akt_byte = 0;
        bit_file->maske = 0x80;
    }
}

/*==========================================================================*/

void OutputBits 
(
    BIT_FILE *bit_file,
    unsigned long code,
    int zaehler
)
{
    unsigned long maske;
    maske = 1L << (zaehler -1 );
    while ( maske != 0 )
    {
        if (maske & code) 
            bit_file->akt_byte |= bit_file->maske;
        bit_file->maske >>= 1;
        if (bit_file->maske == 0)
        {
            if ( putc(bit_file->akt_byte, bit_file->file) != bit_file->akt_byte )
                fataler_fehler("Fataler Fehler in OutputBits!\n");          
            bit_file->akt_byte = 0;
            bit_file->maske = 0x80;
        }
    maske >>= 1;
    }
}

/*==========================================================================*/

int InputBit 
(
    BIT_FILE * bit_file
)
{
    int wert;
    if (bit_file->maske == 0x80)
    {
        bit_file->akt_byte = getc(bit_file->file );
        if (bit_file->akt_byte == EOF)
            fataler_fehler("Fataler Fehler in InputBit!\n");
    }
    wert = bit_file->akt_byte & bit_file->maske;
    bit_file->maske >>= 1;
    if (bit_file->maske == 0) bit_file->maske = 0x80;
    return (wert ? 1 : 0 );
}

/*==========================================================================*/

unsigned long InputBits
(
    BIT_FILE * bit_file,
    int bit_zaehler
)
{
    unsigned long maske;
    unsigned long return_wert;
    maske = 1L << (bit_zaehler - 1);
    return_wert = 0;
    while (maske != 0)
    {
        if (bit_file->maske == 0x80)
        {
            bit_file->akt_byte = getc(bit_file->file);
            if (bit_file->akt_byte == EOF)
                fataler_fehler("Fataler Fehler in InputBit!\n");
        }
        if (bit_file->akt_byte & bit_file->maske)
            return_wert |= maske;
        maske >>= 1;
        bit_file->maske >>= 1;
        if (bit_file->maske == 0)
            bit_file->maske = 0x80;
    }
    return (return_wert);
}

/*==========================================================================*/

void FileBinaerAusgeben
(
    FILE *file,
    unsigned int code,
    int bits
)
{
    unsigned int maske;
    maske = 1 << (bits -1);
    while (maske != 0)
    {
        if (code & maske)
            fputc('1', file);
        else
            fputc('0', file);
        maske >>= 1;
    }
}

/*==========================================================================*/

void fataler_fehler (char * fmt, ...)

{
    va_list argptr;

    va_start ( argptr, fmt );
    printf("Fataler Fehler: ");
    vprintf(fmt, argptr);
    va_end(argptr);
    exit(-1);
}
