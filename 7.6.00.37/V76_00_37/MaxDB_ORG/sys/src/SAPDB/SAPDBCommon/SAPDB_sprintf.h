/*!***************************************************************************

  module:       SAPDB_sprintf.h

  responsible : JoergM

  special area: SAPDB sprintf function

  description:  Contains SAPDB own sprintf function and all other definitions found in hsp77.h

  Currently this is only a name wrapper...

  last changed: 2000-08-10  12:27
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/

#ifndef SAPDB_SPRINTF_H
#define SAPDB_SPRINTF_H

#include "hsp77.h"

/* Should be otherway around, but this is much easier to start with... */

/* redefined type definitions */
#define SAPDB_CharConst             tsp77charConst
#define SAPDB_CharTable             tsp77charTable
#define SAPDB_Encoding              tsp77encoding

/* redefined globals all of type "const SAPDB_Encoding * const" */
#define SAPDB_EncodingAscii         sp77encodingAscii
#define SAPDB_EncodingUCS2Swapped   sp77encodingUCS2Swapped
#define SAPDB_EncodingUCS2          sp77encodingUCS2
#define SAPDB_EncodingUCS2Native    sp77encodingUCS2Native
#define SAPDB_EncodingUTF8          sp77encodingUTF8

/* redefined functions */
#define SAPDB_charIsEqual           sp77charIsEqual
#define SAPDB_charMove              sp77charMove
#define SAPDB_sprintf               sp77sprintf
#define SAPDB_vsprintf              sp77vsprintf
#define SAPDB_sprintfUnicode        sp77sprintfUnicode
#define SAPDB_vsprintfUnicode       sp77vsprintfUnicode
#define SAPDB_nativeUnicodeEncoding sp77nativeUnicodeEncoding
#define SAPDB_isValidEncoding       sp77isValidEncoding

#endif /* SAPDB_SPRINTF_H */
