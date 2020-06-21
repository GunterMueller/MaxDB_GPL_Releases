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
#ifndef STUDIO_ERRORS_H
#define STUDIO_ERRORS_H
/*!**********************************************************************

  module: Studio_Errors.h

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: SQL Studio

  description:  Error codes for SQL  Studio

  see also:

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/


/*----------------------------------------------------------------------------\
// Error Constants
//----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------\
// ST_ERR001
//----------------------------------------------------------------------------*/
#define ST_ERR001 "No Dbc Handle"

/*----------------------------------------------------------------------------\
// ST_ERR002
//----------------------------------------------------------------------------*/
#define ST_ERR002  "No Datatbase Filesystem !"

/*----------------------------------------------------------------------------\
// ST_ERR003
//----------------------------------------------------------------------------*/
#define ST_ERR003 "No ODBC Driver!"

/*----------------------------------------------------------------------------\
// ST_ERR004
//----------------------------------------------------------------------------*/
#define ST_ERR004 "Create Datatbase Filesystem failed !"

/*----------------------------------------------------------------------------\
// ST_ERR013
//----------------------------------------------------------------------------*/
#define ST_ERR013  "No Dbc Pool !"

/*----------------------------------------------------------------------------\
// ST_ERR014
//----------------------------------------------------------------------------*/
#define ST_ERR014  "Connection was down, and is reconnected !"

/*----------------------------------------------------------------------------\
// ST_ERR015
//----------------------------------------------------------------------------*/
#define ST_ERR015  "Connection is down"

/*----------------------------------------------------------------------------\
// ST_ERR016
//----------------------------------------------------------------------------*/
#define ST_ERR016  "No connection object !"



#endif
