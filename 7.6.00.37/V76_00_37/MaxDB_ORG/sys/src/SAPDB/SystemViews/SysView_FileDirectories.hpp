/*!
  @file           SysView_FileDirectories.hpp
  @author         MartinKi
  @ingroup        SystemViews
  @brief          This file contains a class object representing the
                  "FILEDIRECTORIES" view.
  @see

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



#ifndef SYSVIEW_FILEDIRECTORIES_HPP
#define SYSVIEW_FILEDIRECTORIES_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_SysViewBase.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "Container/Container_List.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

 /*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
class FileDir_IDirectory;
class SAPDBMem_IRawAllocator;

/*!
   @class    SysView_FileDirectories
   @brief    This class is used to handle the system table named
             'FILEDIRECTORIES'. It mainly provides the Create() and
             Execute() functions for this table.
 */


class SysView_FileDirectories : public SysView_SysViewBase
{

public:

    /*-----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   constructs an instance and calls the base ISysView constructor.
       @return  none
     */
    SysView_FileDirectories(SQLMan_Context  &context,
                            tak40_show_glob &a41v)
        : SysView_SysViewBase (context, a41v)
    {}

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*/
    /*! @name Table creation */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   fills an empty table object with the column definitions.
       @return  none
     */
    void    Create (void);

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*/
    /*! @name Information access */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   returns the number of columns in this table.
       @return  [SAPDB_Int] - number of columns
     */
    SAPDB_Int       GetColCount (void);

    /*!
       @brief   returns the estimated number of rows for this table.
       @return  [SAPDB_Int] - estimated number of rows
     */
    SAPDB_Int       EstimateRows (void);

    /*!
       @brief   fills this table with information
       @return  none
     */
    void    Execute (void);

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/
private:
    class ResultCollector;
    friend class ResultCollector;

    struct FDirRow {
        SAPDB_Int4     sessionId;
        RTE_TaskId     taskId;
        FileDir_FileNo maxFileNo;
        SAPDB_UInt4    entryCount;
        SAPDB_UInt4    cacheEntryCount;
        SAPDB_UInt4    hashTableSlotCount;
        SAPDB_UInt4    usedHashTableSlotCount;
        SAPDB_UInt4    maxHashTableChainEntryCount;
        SAPDB_UInt4    resizeHashTableCount;
    };

    class ResultCollector
    {
    public :
        /*!
          @brief constructor
        */
        ResultCollector(
            SAPDBMem_IRawAllocator&  allocator,
            SysView_FileDirectories& fDirView);

        /*!
          @brief adds one row to the collection
        */
        void Append(
            SQLMan_Context& context,
            const FDirRow& fileEntry);

        /*!
          @brief inserts all collected rows into the result set
        */
        void Flush();

        /*!
          @brief destructor, currently does nothing
        */
        ~ResultCollector();
    private :

        typedef Container_List<FDirRow> FDirRowList;

        SysView_FileDirectories& m_fDirView;
        FDirRowList              m_rowList;
    };

    void QuerySharedFileDirectory();

    void QuerySessionFileDirectories();

    SAPDB_Bool AddRow(FDirRow& row);
};



#endif  /* SYSVIEW_FILEDIRECTORIES_HPP */
