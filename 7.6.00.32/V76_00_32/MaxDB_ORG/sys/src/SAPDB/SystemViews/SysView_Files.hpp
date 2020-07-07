/*!
  @file           SysView_Files.hpp
  @author         ElkeZ
  @ingroup        SystemViews
  @brief   This contains a class object representing the FILES view.
  @see
*/
/*

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



*/



#ifndef SYSVIEW_FILES_HPP
#define SYSVIEW_FILES_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_SysViewBase.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_FileTypes.hpp"
#include "DataAccess/Data_Types.hpp"
#include "Container/Container_List.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class FileDir_Iterator;
class FileDir_Generic;
class FileDir_Table;
class SAPDBMem_IRawAllocator;

/*!
   @class    SysView_Files
   @brief    This class is used to handle the system table named
             'FILES'. It mainly provides the Create() and
             Execute() functions for this table.
 */


class SysView_Files : public SysView_SysViewBase
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
    SysView_Files(SQLMan_Context  &context,
                  tak40_show_glob &a41v);

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
       @return  [int] - number of columns
     */
    SAPDB_Int   GetColCount (void);

    /*!
       @brief   returns the estimated number of rows for this table.
       @return  [int] - estimated number of rows
     */
    SAPDB_Int   EstimateRows (void);

    /*!
       @brief   fills this table with information
       @return  none
     */
    void    Execute (void);

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

private:
    SAPDB_UInt4 m_pageSize;

    enum {
        c_tableNameLength = 32 ///< constant for the maximal number
                               ///  of characters in a tablename
    };

    enum AttributeValue {
        avNotPresent,
        avNotSet,
        avSet
    };

    class ResultCollector;
    friend class ResultCollector;
    struct FilesRow;
    friend struct FilesRow;

    struct FilesRow {
        FileDir_FileNo fileNo;
        FileDir_FileNo parentFileNo;
        SAPDB_UInt8    entryCount;
        SAPDB_UInt8    lobPageCount;
        Data_FileType  fileType;
        Data_PageNo    rootPage;
        Data_FileState fileState;
        SAPDB_Int4     indexPageCount;
        SAPDB_Int4     leafPageCount;
        char           comment[c_tableNameLength+1];
        SAPDB_Int4     sessionId;
        SAPDB_Bool     bCountersInitialized;
        SAPDB_Bool     bLobCounterInitialized;
        SAPDB_Bool     bOutputSessionId;
        AttributeValue avClustered;
        AttributeValue avDynamic;
        AttributeValue avPacked;
    };

    class ResultCollector
    {
    public :
        /*!
          @brief constructor
        */
        ResultCollector(
            SAPDBMem_IRawAllocator& allocator,
            SysView_Files&          filesView);

        /*!
          @brief adds one row to the collection
        */
        void Append(
            SQLMan_Context& context,
            const FilesRow& fileEntry);

        /*!
          @brief inserts all collected rows into the result set
        */
        void Flush();

        /*!
          @brief destructor, currently does nothing
        */
        ~ResultCollector();
    private :

        typedef Container_List<FilesRow> FilesRowList;

        SysView_Files&    m_filesView;
        FilesRowList      m_rowList;
    };


    void OutputFdir(
        FileDir_Iterator& it,
        ResultCollector&  resultCollector,
        const SAPDB_Bool  bOutputSessionId,
        const SAPDB_Int4  outputSessionId,
        Msg_List&         errMsg);

    SAPDB_Bool AddRow(FilesRow& row);

    void AddAttributeValue(const AttributeValue av);

    SAPDB_Bool GetLobPageCount(
        FileDir_Table& tableEntry,
        SAPDB_UInt8&   lobPageCount,
        Msg_List&      errMsg);
};



#endif  /* SYSVIEW_FILES_HPP */
