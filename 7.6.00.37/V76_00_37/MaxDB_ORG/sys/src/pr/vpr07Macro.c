/*!================================================================
 module:    vpr07Macro.c

 responsible: BurkhardD
 
 special area:   Usefull Preprozessor Macros

 description: 
 
 see:  

  -------------------------------------------------------------------------

 



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




 ===================================================================*/

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include <stdlib.h>
#include <string.h>
#include "vpr07Macro.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

/* stolen from veo47.c */
externC void pr07C2P(void *pout, void *cin, int len)
{
  char *pptr ;       /* pascal string ptr */
  char *cptr ;       /* C string ptr */
  
  pptr = (char *) pout;
  cptr = (char *) cin;
  
  while ( len -- > 0 ) {
    if ( * cptr == '\0' ) {
      * pptr ++ = ' ' ;
    }
    else {
      * pptr ++ = * cptr ++ ;
    }
  }  
}

externC void pr07P2C( void *cout , void *pin , int len )
{
  char *pptr ;       /* pascal string ptr */
  char *cptr ;       /* C string ptr */
  
  pptr = (char *)pin;
  pptr += len - 1 ;
  cptr = (char *)cout;
  cptr += len ;
  
  * cptr -- = '\0' ;
  
  while ( pptr != pin && * pptr == ' ' ) {
    * cptr -- = '\0' ;
    pptr -- ;
  }
  
  if ( pptr == pin && * pptr == ' ' ) {
    * cptr -- = '\0' ;
  }
  else {
    while ( pptr != pin ) {
      * cptr -- = * pptr -- ;
    }
    * cptr = * pptr ; /* don't forget last char */
  }
}

void pr07Put2Up( char *cinout )
{
  size_t i = strlen(cinout);
  
  if (cinout) {
    for (i=0; i < strlen(cinout); i++) {
      *(cinout + i) = toupper( *(cinout + i) );
    } 
  }
  return; 
}

void pr07StripChar(char *cin, char *stripc)
{
  char *c = stripc;
  while (*c != 0) {
    char *s = strchr(cin, *c);
    if (s) *s = '\0';
    c++;
  }
}
