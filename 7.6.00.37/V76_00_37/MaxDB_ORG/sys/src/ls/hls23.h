/*! 
  -----------------------------------------------------------------------------
 
  module: hls23.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: Replication Manager
 
  description:  Module that handles the building of records for database pages

  version:      7.5.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1998-2005 SAP AG-2003
 
  -----------------------------------------------------------------------------



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




*/

#ifndef HLS23
#define HLS23

#include "gls00.h"


/* ------------------------------------------------------------------
 * types, constants and some stuff
 * ------------------------------------------------------------------
 */

/*! Declaration: Position constants */

//  These constants are used to build the record for a data page 
#define BUF_START_POS_LS23          ( (tsp00_Int4)      0)  // To memorize the start position in the buffer
#define KEYLEN_OFFSET_LS23          ( (tsp00_Int4)      2)
#define VARCOL_OFFSET_LS23          ( (tsp00_Int4)      4)
#define VARCOL_COUNT_OFFSET_LS23    ( (tsp00_Int4)      6)
#define HEADER_OFFSET_LS23          ( (tsp00_Int4)      8)
#define LENGTH_OFFSET_LS23          ( (tsp00_Int4)      8)

/*! EndDeclaration: Position constants */

/*!
  -----------------------------------------------------------------------------
 
  function:     ls23BuildRecord

  -----------------------------------------------------------------------------

  description:  Build a database record
  
                This function builds one database record and writes it to a buffer.

  arguments:    InputLine   [IN]    - A reference to a char string holding the data read
                                      from a data file.
                RawData     [IN]    - A referenz to a pointer to a structure of string
                                      values which are used within the function to store
                                      column values. The values are preconfigured in the constructor
                                      or initialization routine to:
                                      char string-part: '\0'
                                      length of value:  INSERT_DEFAULT
                                      In case that the value read from data file
                                      has to be converted to fit the column type and value of the
                                      database column the memory for the converted value is
                                      allocated by the conversion functions. In this case the
                                      memory has to be freed after building the record. In case
                                      the value can be inserted into the record without conversion
                                      the string pointer simply points to the value within the
                                      InputLine variable.
                Buffer     [OUT]   - A pointer to a char string. This string is a portion
                                      of a database page that is initialized to start at position Buffer[0]
                                      before calling this function.
                pTransformationCmd     [IN]    - Structure that holds all user information
                                      to process the fastload command.
                pFLRec     [IN]    - Structure that holds all server and db information
                                      to process the fastload command.
                ErrText    [OUT]   - Error text

  returnvalue:  errOK_els00         - in case of NO errors
                some other errors   - in case of errors


  -----------------------------------------------------------------------------
*/
int ls23BuildRecord(tsp00_Addr                Buffer,
                    tls00_TransformationCmd  *pTransformationCmd,
                    tls00_FloadRec           *pFLRec,
                    tsp1_sqlmode_Enum        SQLMode,
                    tsp00_Addr               ErrText);
                          
#endif /*HLS23*/