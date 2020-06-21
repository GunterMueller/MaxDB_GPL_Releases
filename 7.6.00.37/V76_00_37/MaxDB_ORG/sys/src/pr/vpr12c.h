/*!**************************************************************************

  module      : vpr12c.h

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

#ifndef __VPR12C_H__
#define __VPR12C_H__
/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

/*==================================================================*
 *  PASCAL PROTOTYPES (vpr12)                                       *
 *==================================================================*/

void p12putsqlkano(sqlcatype *sqlca, sqldatype *sqlda, tsp00_Int2 *kano, tpr00_FetchSpecEnum *prno);

#endif
