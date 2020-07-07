/*!
  @file           heo21.h
  @author         JoergM
  @special area   RTE internal XUser Access
  @brief          
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



#ifndef HEO21_H
#define HEO21_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

void 
sql21put_name (tsp00_C18 acToCryptName,
	       tsp00_CryptName alCryptName);

void 
sql21write_crypt (tsp00_Int4 lZahl,
		  tsp00_C20 acErgebnis);

tsp00_Int4 
sql21read_crypt (tsp00_C20 acReCryptName);


#endif			/* HEO21_H */
