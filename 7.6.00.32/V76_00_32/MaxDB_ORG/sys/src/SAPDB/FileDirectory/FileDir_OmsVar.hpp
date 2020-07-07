/*****************************************************************************/
/*!
 @file    FileDir_OmsVar.hpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief   Header file for smart point and interface class
          FileDir_OmsVar.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
/*****************************************************************************/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#ifndef FILEDIR_OMSVAR_HPP
#define FILEDIR_OMSVAR_HPP

#include "FileDirectory/FileDir_Oms.hpp"
#include "FileDirectory/FileDir_IOmsVar.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_FileTypes.hpp"

#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class FileDir_OmsVarEntry;

/*!
 * @class FileDir_OmsVar
 * @ingroup FileDirectory_Classes
 * @copydoc FileDir_Generic
 * @author MartinKi
 *
 * @brief SmartPointer class also serving as an interface class to all
 *        file directory OMS entries.
 */
class FileDir_OmsVar : public FileDir_Oms,
                       public FileDir_IOmsVar
{
public:
    /*!
     * @brief FIXME: Returns.
     *
     * @copydoc FileDir_OmsVarEntry::AddContPartition
     */
    FileDir_ReturnCode AddContPartition(
        const Data_PageNo& rootPage,
        SAPDB_UInt2        partitionNo);

    /*!
     * @brief Prepares fileId for partition partitionNo for use in bd
     *        procedures.
     *
     */
    void BuildContFileId(
        SAPDB_Int       partitionNo, 
        tgg00_FileId&   fileId) const;

    /*!
     * @brief FIXME: Returns.
     *
     * @copydoc FileDir_OmsVarEntry::GetContPartition
     */
    Data_PageNo GetContPartition(SAPDB_UInt2 partitionNo) const;

    /*!
     * @copydoc FileDir_Generic::Assign
     */
    template <class T>
    void Assign(T& Ptr);
/*************************************************************************/

    /*!
     * @brief Default constructor.
     */
    FileDir_OmsVar();

    /*!
     * @brief Constructor.
     *
     * @param fe [in] Pointer to FileDir_OmsEntry or subclass.
     */
    FileDir_OmsVar(FileDir_OmsVarEntry* fe);

    /*!
     * @brief Copy-constructor.
     */
    FileDir_OmsVar(const FileDir_OmsVar& fe);
};

/*************************************************************************/

template <class T>
void FileDir_OmsVar::Assign(T& Ptr)
{
    /*
     * the following statement makes sure that down- and upcasts
     * via Assign() are typesafe; if you encounter a compile error
     * this probably means that you tried to Assign() a smart
     * pointer to unrelated entry types
     */
    T* test = static_cast<T*>(this); // check previous comment if you get a compile error here
    if ( Ptr.IsSecondaryPointer() ) {
        AssignSecondary( Ptr );
    } else {
        AssignEntry( Ptr.fileEntry );
    }
}

/*************************************************************************/

#endif // FILEDIR_OMSVAR_HPP
