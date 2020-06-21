/**************************************************************************/
/*!  @defgroup      PrimaryFile */
/**************************************************************************/
/*!

  @file         Data_PrimaryFile.hpp
  @author       UweH
  @ingroup      PrimaryFile
  @brief        This is the main interface for accessing primary data files.
  @since        2003-05-12
  @sa           see also ....

*/
/*!
\if EMIT_LICENCE

  copyright:    Copyright (c) 2001-2005 SAP AG


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
#ifndef DATA_PRIMARY_FILE_HPP
#define DATA_PRIMARY_FILE_HPP

#include "DataAccess/Data_BaseFile.hpp"

struct tgg00_TransContext;

/// Main interface to primary files
class Data_PrimaryFile : public Data_BaseFile
{
public:

    /// standard constructor for normal usage
    Data_PrimaryFile ( tgg00_TransContext &trans,
                       const Data_PageNo   root = Data_PageNo() )
    : Data_BaseFile ( Data_TableFile,
                      Data_PageId(root, Data_PageRecoveryMode(Data_Static,Data_Recoverable)),
                      Data_PageAccessManager(trans, Data_TableFile, 
                                             Data_PageRecoveryMode(Data_Static,Data_Recoverable), root) )
    {}
    virtual bool Create(SAPDBErr_MessageList &errlist)
    {
        return false;
	}
    virtual bool Drop(SAPDBErr_MessageList &errlist)
    {
        return false;
	}
    virtual bool Verify (bool                  isCold,
                         SAPDBErr_MessageList &errlist)
	{
        return false;
	}
};

#endif  /* DATA_PRIMARY_FILE_HPP */

