/*!
  @file           SysView_PseudoLongHandler.hpp
  @author         ThomasA
  @ingroup        SystemViews
  @brief          This contains a class which presents data of system views as columns of datatype long.
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

#ifndef SYSVIEW_PSEUDOLONGHANDLER_HPP
#define SYSVIEW_PSEUDOLONGHANDLER_HPP 

#include "SAPDBCommon/SAPDB_Types.hpp"

class  SQLMan_Context;
struct tgg00_FileId;

/*!
   @brief provides functions for presenting system view data as columns of datatype long
*/
class SysView_PseudoLongHandler
{
public :

    struct LongIdentificator
    {
        SAPDB_Int4 m_fileId;
        SAPDB_Int4 m_rowId;
        SAPDB_Int2 m_rowCount;
    };
    /*!
       @brief constructor
    */
    SysView_PseudoLongHandler(SQLMan_Context& context);
    /*!
       @brief drop a pseudo long container, i.e. the file containing long columns stored as a sequence rows
       @param context the current SQLManager context
       @param fileId identifies the container
     */
     void DestroyPseudoLongContainer(SQLMan_Context& context, SAPDB_Int4& fileId);
    /*!
       @brief inserts a pseudo long values into the statements pseudo long container.
       If this container does not already exist, a container (file) is created implicitely.
       The value is stored as a sequence of rows in the container file. 
       @param context the current SQLManager context
       @param pValue pointer to the value to be stored
       @param size length of the value to be stored
       @param key [out] identifcation of the pseudo long column
    */
    void InsertLong(SQLMan_Context& context, const void* pValue, int size, LongIdentificator& key);
    /*!
      @brief reads a substring of a pseudo lng column
      @param context the current SQLManager context
      @param longId  an integer identifiying the pseudo long column
      @param startPos the required position of the pseudo long column
      @param length the required length
      @param buf the container for the result
      @param the size of the result container
     */
    int ReadLong (SQLMan_Context& context, LongIdentificator& longId, int startPos, int length, void* buf, int bufSize);
private :
    /*!
        @brief create the file identification of a pseudo long container
    */
    inline void CreateFileId(SQLMan_Context& context, int fileId, tgg00_FileId& file);
    static int m_keyLength;
    static int m_infoOffset;
    static int m_maxInfoLength;
    int m_fileIdGenerator;
};

#endif