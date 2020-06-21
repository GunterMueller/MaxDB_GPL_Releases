/*****************************************************************************/
/*!
 @file    FileDir_OmsFixed.hpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief   Header file for smart point and interface class
          FileDir_OmsFixed.

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

#ifndef FILEDIR_OMSFIXED_HPP
#define FILEDIR_OMSFIXED_HPP

#include "FileDirectory/FileDir_Oms.hpp"
#include "FileDirectory/FileDir_IOmsFixed.hpp"
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

class FileDir_OmsFixedEntry;

/*!
 * @class FileDir_OmsFixed
 * @ingroup FileDirectory_Classes
 * @copydoc FileDir_Generic
 * @author MartinKi
 *
 * @brief SmartPointer class also serving as an interface class to all
 *        file directory OMS entries.
 */
class FileDir_OmsFixed : public FileDir_Oms,
                         public FileDir_IOmsFixed
{
public:

// currently FileDir_OmsFixed is exactly the same as FileDir_Generic...


    /*!
     * @copydoc FileDir_Generic::Assign
     */
    template <class T>
    void Assign(T& Ptr);
/*************************************************************************/

    /*!
     * @brief Default constructor.
     */
    FileDir_OmsFixed();

    /*!
     * @brief Constructor.
     *
     * @param fe [in] Pointer to FileDir_OmsEntry or subclass.
     */
    FileDir_OmsFixed(FileDir_OmsFixedEntry* fe);

    /*!
     * @brief Copy-constructor.
     */
    FileDir_OmsFixed(const FileDir_OmsFixed& fe);
};

/*************************************************************************/

template <class T>
void FileDir_OmsFixed::Assign(T& Ptr)
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

#endif // FILEDIR_OMSFIXED_HPP
