/*!
 * \file    SAPDBAlogo_MD5File.h
 * \author  FerdiF
 * \brief   MD5 message-digest algorithm (due to Ron Rivest).
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#ifndef SAPDBALGO_MD5FILE_H
#define SAPDBALGO_MD5FILE_H

/// MD5 message-digest algorithm applied to a file (due to Ron Rivest).
/*!
** MD5 message-digest algorithm to a file (due to Ron Rivest).
**
** \param filename Name of the file the MD5 digest algorithm should be applied to.
** \param sum  character string of a 32 bytes length conatining the MD5 footprint
** \return 1 = successfull, 0 = failed
**/
#ifdef __cplusplus
externC int SAPDBAlgo_MD5File (char *filename, char *sum);
#else
extern int SAPDBAlgo_MD5File (char *filename, char *sum);
#endif
#endif