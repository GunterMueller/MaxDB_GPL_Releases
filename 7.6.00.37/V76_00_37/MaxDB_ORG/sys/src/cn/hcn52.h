/*


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

/*! 
  -----------------------------------------------------------------------------
 
  module: hcn52.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer Parameter History Class
   
  description:  DBMServer Parameter History Class - Specification

  version:      7.2.

  -----------------------------------------------------------------------------
 
                          Copyright (c) 1998-2005 SAP AG
 
  -----------------------------------------------------------------------------
*/

#ifndef _HCN52_H_
#define _HCN52_H_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "gsp00.h"
#include "gcn002.h"

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

typedef CSTRING_TYPE_MSP00      (  tcn52_C1024c, 1024);

/*!
  -----------------------------------------------------------------------------
  Class:        cn52ParamHistoryRecord
  -----------------------------------------------------------------------------

  description:  Parameter hitroy record

                An instance of this class represents one entry in the
                parameter history.
                
  -----------------------------------------------------------------------------
 */
class cn52ParamHistoryRecord
{
  public:

  /*!---------------------------------------------------------------------------
    Declaration:  Public types
    Description:  Public types used by public member functions.
  */
    typedef enum {
      StDeleted,
      StChanged,
      StActive,
      StUnknown
    } HistoryState;


    /*!
      -------------------------------------------------------------------------
      function:     cn52ParamHistoryRecord
      -------------------------------------------------------------------------
      description:  Constructor

                    Reads the value of this to the name 

      arguments:    szDBName   [IN] - name of the database
                    szName     [IN] - of the parameter
                    szNewValue [IN] - new value of the
                    szOldValue [IN] - old value of the parameter
                    szGroup    [IN] - group of the parameter
                    szComment  [IN] - comment for this change of parameter
                    nState     [IN] - state of the parameter
      -------------------------------------------------------------------------
     */
     cn52ParamHistoryRecord ( const tsp00_DbNamec        & szDBName,
                              const tcn002_XpValueName   & szName,
                              const tcn002_XpValueString & szNewValue,
                              const tcn002_XpValueString & szOldValue,
                              const tsp00_C8c            & szGroup,
                              const tsp00_C80c           & szComment,
                              const HistoryState           nState);
     cn52ParamHistoryRecord ( const tsp00_DbNamec        & szDBName);
     
    /*!
      -------------------------------------------------------------------------
      function:     ~cn52ParamHistoryRecord
      -------------------------------------------------------------------------
      description:  Destructor

      -------------------------------------------------------------------------
     */
     ~cn52ParamHistoryRecord();

    /*!
      -------------------------------------------------------------------------
      function:     appendToFile
      -------------------------------------------------------------------------
      description:  append record to file

      arguments:    szError [OUT] - error text 

      return value: true on success otherwise false

      -------------------------------------------------------------------------
     */
     bool appendToFile(DBMSrvMsg_Error     & oError);

    /*!
      -------------------------------------------------------------------------
      function:     readFirst
      -------------------------------------------------------------------------
      description:  read the first matching record

      arguments:    szError [OUT] - error text 
                    bEnd    [OUT] - end of file found

      return value: true on success otherwise false

      -------------------------------------------------------------------------
     */
     bool readFirst(DBMSrvMsg_Error     & oError,
                    bool                & bEnd );

    /*!
      -------------------------------------------------------------------------
      function:     readNext
      -------------------------------------------------------------------------
      description:  read the next matching record

      arguments:    szError [OUT] - error text 
                    bEnd    [OUT] - end of file found

      return value: true on success otherwise false

      -------------------------------------------------------------------------
     */
     bool readNext(DBMSrvMsg_Error     & oError,
                    bool               &  bEnd );

    /*!
      -------------------------------------------------------------------------
      function:     setSelection
      -------------------------------------------------------------------------
      description:  read the next matching record

      arguments:    szSelection [IN] - selection data 

      return value: true on success otherwise false

      -------------------------------------------------------------------------
     */
     bool setSelection(const tcn52_C1024c & szSelection);

    /*!
      -------------------------------------------------------------------------
      function:     get*
      -------------------------------------------------------------------------
      description:  data of parameters

      return value: data

      -------------------------------------------------------------------------
     */
     const tcn002_XpValueName    & getName     () const {return m_szName;}
     const tcn002_XpValueString  & getNewValue () const {return m_szNewValue;}
     const tcn002_XpValueString  & getOldValue () const {return m_szOldValue;}
     const tsp00_Datec           & getDate     () const {return m_szDate;}
     const tsp00_Timec           & getTime     () const {return m_szTime;}
     const HistoryState            getState    () const {return m_nState;}
     const tsp00_C1c             & getStateChar() const {return m_szState;}
     const tsp00_C8c             & getGroup    () const {return m_szGroup;}
     const tsp00_C80c            & getComment  () const {return m_szComment;}
     const tcn52_C1024c          & getRecord   ();
     const tcn52_C1024c          & getHeader   ();

    protected:

      void initMembers     ( );
      bool analyzeRecord   ( tcn52_C1024c            & szRecord ); 
      bool checkNameArray  (const tcn002_XpValueName & szName);
      bool readFields      ( char                    * pValue,
                             tsp00_Int4              & nFields  ) const;
      bool readStrings     ( char                    * pValue,
                             tcn52_C1024c            & szString ) const;
      bool migrateFile     ( DBMSrvMsg_Error         & oError);
      void analyzeRecV01   ( tcn52_C1024c            & szRecord ); 
      void analyzeRecV02   ( tcn52_C1024c            & szRecord ); 

      tcn002_XpValueName   m_szName;
      tcn002_XpValueString m_szNewValue;
      tcn002_XpValueString m_szOldValue;
      HistoryState         m_nState;
      tsp00_Datec          m_szDate;
      tsp00_Timec          m_szTime;
      tsp00_C1c            m_szState;
      tsp00_C8c            m_szGroup;
      tsp00_C80c           m_szComment;
      tsp00_DbNamec        m_szDBName;
      tcn52_C1024c         m_szRecord;
                     
      bool                 m_bFileOpen;
      tsp00_Int4           m_hFile;

      tcn002_XpValueName   m_szSelName;
      tcn52_C1024c         m_szSelGroup;
      tcn52_C1024c         m_szSelState;
      tsp00_Datec          m_szSelDate;
      tsp00_Int4           m_nFields;
      
#define MAX_KEYS_CN52 500
      tcn002_XpValueName   m_NameArray[MAX_KEYS_CN52];
 
};

/*! EndClass: cn52DBMConfigValue */

#endif /* _HCN52_H_ */

