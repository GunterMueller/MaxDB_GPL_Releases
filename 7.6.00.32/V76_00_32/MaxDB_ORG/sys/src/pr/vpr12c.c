/*!**************************************************************************

  module      : vpr12c.c

  -------------------------------------------------------------------------

  responsible : BurkhardD

  special area:
  description : Contains functions for execute SQL comming from ODBC.
                replaces most of PASCAL coded in vpr12.

  last changed: 
  see also    :

  change history : 
        
        D025086 08-11-2000 Created for UNICODE Support. New parser etc.

  -------------------------------------------------------------------------





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

#include "gpr00.h"
#include "gpr05.h"
#include "vpr12c.h"
#include "vpr05c.h"
#include "vpr08c.h"
#include "vpr01Con.h"
#include "vpr03Part.h"
#include "vpr03Packet.h"

void pr12cBeforeParse(sqlcatype *sqlca,
                      struct tpr01_ConDesc *ConDesc,
                      sqldatype *sqlda,
                      tsp00_Int2 *cmdfetch)
{
  int i;
  tsp00_Int2 kano;
  tpr00_FetchSpecEnum prno;
  tpr05_Symbol symb = CPR_S_EOF;
  tpr05_Symbol lastsymb = CPR_S_EOF;
  sqlcxatype *sqlcxap = sqlca->sqlcxap;
 
  *cmdfetch = cpr_is_false;
  kano = 0;
  prno = CPR_FE_EMPTY;
  p12putsqlkano (sqlca, sqlda, &kano, &prno);
  if (sqlcxap->xakano == cpr_com_mfetch
      || sqlcxap->xakano == cpr_com_mfetch_describe) {
    *cmdfetch = cpr_is_true;
    prno = sqlcxap->xacfetspec;
    p12putsqlkano (sqlca, sqlda, &kano, &prno);
    if ( prno = CPR_FE_POSC ) {
      /* get pos unsigned_integer into fapos */
      /* get pos signed_integer into fapos in case of
       * the fetch is an ABSOLUTE OR RELATIVE */
      sqlratype *sqlrap = sqlca->sqlrap;
      tpr05_String partString;
      tpr05_StringEncoding PacketEncoding = 
        pr03PacketGetEncoding(ConDesc->ga->gareqptr);
      tsp1_part *partPtr = pr03PartFind(sqlrap, sp1pk_command);
      char *rawPtr = pr03PartGetRawPtr(partPtr);
      tpr05_SymbolIterator SIter;
      partString.allocationType = constant_epr05;
      pr05IfCom_String_InitString(&partString, rawPtr, pr03PartGetPartLength(partPtr), PacketEncoding, constant_epr05);		        
      pr05cSymbolOpen(&partString, &SIter);
      while ((symb = pr05cSymbolEnum(&SIter)) != CPR_S_EOF) {
        if (symb == CPR_S_UNSIGNED_INTEGER) {
          sqlcxap->xacfetposc = p05chtoint4(&rawPtr[SIter.SymbolPos], SIter.SymbolLength);
        }
        if (lastsymb == CPR_S_MINUS) 
          sqlcxap->xacfetposc = -sqlcxap->xacfetposc;
        lastsymb = symb; 
      }
    }    
  }
}
