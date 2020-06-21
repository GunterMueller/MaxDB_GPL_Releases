/*!
  @file    Msg_ProtFileInfo.hpp
  @author  Robin
  @ingroup Common
  @brief   Message file tag class
  
  handling of file tags

  -------------------------------------------------------------------------

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

#ifndef MSG_PROTFILEINFO_HPP
#define MSG_PROTFILEINFO_HPP

#define MSG_PROTFILEINFO_KNLMSG           (*(tsp00_Pathc *) "_KNLMSG"       )
#define MSG_PROTFILEINFO_KNLMSGOLD        (*(tsp00_Pathc *) "_KNLMSGOLD"    )
#define MSG_PROTFILEINFO_KNLMSGARCHIVE    (*(tsp00_Pathc *) "_KNLMSGARCHIVE")


#include "Messages/Msg_IOutput.hpp"

class Msg_ProtFileInfoData;

/*!
  @class Msg_ProtFileInfo
  @brief single protocol file info

 */
class Msg_ProtFileInfo
{
public:
    /*!
        @brief  constructor. Create protocol file info object. An object created by this constructor is empty -
                calling one of the member methods below does not cause any harm, but will return 0.
                The only way to get a usable protocol file info object
                is to call one of the static methods ByTagName() and ByFileName() (see below).

    */
    Msg_ProtFileInfo()
        :m_pProtFileInfoData(NULL)
    {};

    /*!
        @brief  Get a Msg_ProtFileInfo for a file tag
                
        @param  tagName                 [in]    file tag (symbolic name e.g. "KNLDIAG")
    */
    static Msg_ProtFileInfo  ByTagName(SAPDB_Char *tagName);

    /*!
        @brief  Get a Msg_ProtFileInfo for a given file name
                
        @param  tagName                 [in]    file name
    */
    static Msg_ProtFileInfo  ByFileName(SAPDB_Char *fileName);

    /*!
        @brief  member access method
    */
    const SAPDB_UTF8 *GetXParam();
    
    /*!
        @brief  member access method
    */
    const char   * GetTypeString();

    /*!
        @brief  member access method
    */
    long           GetShow();

    /*!
        @brief  member access method
    */
    long           GetType();

    /*!
        @brief  member access method
    */
    long           GetOrgType();

    /*!
        @brief  member access method
    */
    bool           GetRemoveFlag();

    /*!
        @brief  member access method
    */
    const char   * GetName();

    /*!
        @brief  member access method
    */
    const char   * GetExt();

    /*!
        @brief  member access method
    */
    long           GetLocation();

    /*!
        @brief  member access method
    */
    const char   * GetBaName();

    /*!
        @brief  member access method
    */
    const char   * GetBaExt();

    /*!
        @brief  member access method
    */
    long           GetBaLocation();

    /*!
        @brief  member access method
    */
    const char   * GetClass();

    /*!
        @brief  member access method
    */
    const char   * GetComment();

    /*!
        @brief  member access method
    */
    long           GetHeader();

    /*!
        @brief  member access method
    */
    long           GetTimestampPos();

    /*!
        @brief  member access method
    */
    const char   * GetTimestampFmt();

    /*!
        @brief  member access method
    */
    bool           RemoveEmptyLinesFlag();

    /*!
        @brief  member access method
    */
    bool           OperationFlag();

    /*!
        @brief  Get the complete file name of the protocol file
                This method decides whether the name is hard-coded or must be retrieved from the parameter file
        @param  fileName                [out]   on successful completion: the complete file name
        @param  dbName                  [in]    the name of the database
    */
    void           GetCompleteFileName(RTE_Filename &fileName,RTE_DBName &dbName);

    /*!
        @brief  Get the complete file name of the protocol file
                This method decides whether the name is hard-coded or must be retrieved from the parameter file
        @param  fileName                [out]   on successful completion: the complete file name
        @param  path                    [in]    given base path for the file
    */
    void           GetCompleteFileName(RTE_Filename &fileName,RTE_Path &path);

    Msg_IOutput::OutputStyle    GetStyle(void);

    bool           LongColumns();

private:
    Msg_ProtFileInfo(Msg_ProtFileInfoData *pProtFileInfoData)
        :m_pProtFileInfoData(pProtFileInfoData)
    {};
    Msg_ProtFileInfoData *m_pProtFileInfoData;
 
    static         Msg_ProtFileInfoData protFileInfoData[];
};



#endif /* MSG_PROTFILEINFO_HPP */
