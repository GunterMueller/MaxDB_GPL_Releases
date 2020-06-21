/*



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





*/

#include <stdio.h>

#include "hpa112FileCompr.h"

#define BUF_LEN 1024

int main( int argc, char *argv[] )
{
    tpa112_FileReader reader;
    tsp00_Int4  read;
    tsp00_Int4  written;
    tsp00_Bool  moreData = true;
    char        buf[BUF_LEN];

    if (argc < 3) {
        printf( "Usage: decompr <infile> <outfile>\n" );
        return 1;
    } else {
        FILE *fout = NULL;

        if (!reader.Open( argv[1] )) {
            printf( "Input file %s could not be opened.\n", argv[1] );

            return 3;
        };

        fout = fopen( argv[2], "wb" );
        if (fout == NULL) {
            printf( "Output file %s could not be opened.\n", argv[2] );
            reader.Close();
            
            return 2;
        };
        while (moreData) {            
            if (!reader.Read( (tpa110_DataPtr) buf, BUF_LEN, read, moreData )) {
                printf( "Read failed!\n" );
                reader.Close();
                fclose( fout );
                
                return 4;
            };
            written = fwrite( buf, 1, read, fout );
            if (written < read) {
                printf( "Write failed!\n" );
                reader.Close();
                fclose( fout );
                
                return 5;
            };
        }; // while

        reader.Close();
        fclose( fout );
    }; // if

    return 0;
}; // main
