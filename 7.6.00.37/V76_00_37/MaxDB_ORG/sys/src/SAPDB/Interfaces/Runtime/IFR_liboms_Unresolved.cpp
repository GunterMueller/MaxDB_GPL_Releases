/*!
  @file           IFR_liboms_Unresolved.cpp
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Unresolved
  @brief          Resolve unresolved functions
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
#include "gsp03.h" /* nocheck */

static const char *IFR_Unresolved_hostname = "localhost";

#ifdef __cplusplus
extern "C" {
#endif
  char const *RTESys_Hostname()
  {
    return IFR_Unresolved_hostname;
  }

  void	eo60k_Message   ( tsp3_priority_Param msg_prio ,
                          tsp00_Int4            msg_type ,
                          tsp00_Int4            msg_no ,
                          const char         *msg_label ,
                          const char         *msg_txt )
  {
  }

#ifdef __cplusplus
}
#endif
