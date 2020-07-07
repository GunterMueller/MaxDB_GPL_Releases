/*!
  @file           PIn_SpecialParts.h
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          Sub classes to represent common part types

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

#ifndef PIn_SpecialParts_h
#define PIn_SpecialParts_h

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "SAPDB/PacketInterface/PIn_Part.h"
#include "Messages/Msg_List.hpp"

/*!  @brief Base class for special parts */
class PIn_SpecialPart : public PIn_Part
{
  public:
     /*!  @brief constructor */
    PIn_SpecialPart (PIn_Part & oPart, tsp1_part_kind_Param partKind );
  private:
    /*! brief forbidden copy constructor */
    PIn_SpecialPart(PIn_SpecialPart & oPart){};

}; // end PIn_SpecialPart

/* ======================================================== */

/*!  @brief Error Part Class */
class PIn_ErrorTextPart : public PIn_SpecialPart
{
  public:
     /*!  @brief constructor */
    PIn_ErrorTextPart (PIn_Part  & oPart );

    /*! @brief destructor */
    ~PIn_ErrorTextPart ();

    /*! @brief retrieve errortext */
    const tsp00_Byte * ErrorText() {return this->GetReadData();}
    SAPDB_Int          ErrorLen () {return this->Length();     }

}; // end PIn_ErrorTextPart

/* ======================================================== */

/*!  @brief Message List Part Class */
class PIn_MessageListPart : public PIn_SpecialPart
{
  public:
     /*!  @brief constructor */
    PIn_MessageListPart (PIn_Part  & oPart );

    /*! @brief destructor */
    ~PIn_MessageListPart ();

    /*! @brief retrieve errortext */
    SAPDBErr_MessageList const & MessageList();

  private:
    SAPDBErr_MessageList oMessageList;

}; // end PIn_MessageListPart

/* ======================================================== */

/*!  @brief Session Info Part Class */
class PIn_SessionInfoPart : public PIn_SpecialPart
{
  public:
    /*!  @brief constructor */
    PIn_SessionInfoPart (PIn_Part  & oPart );

    /*! @brief destructor */
    ~PIn_SessionInfoPart ();

    /*! @brief read unicode flag */
    int IsUnicode();

    /*! @brief read page size; in case of error the page size is 0 */
    int getPageSize();

}; // end PIn_SessionInfoPart

/* ======================================================== */

/*!  @brief Session Info Part Class */
#define PIn_UserInfoPart_SYSDBALength 65
#define PIn_UserInfoPart_TypeLength   17
class PIn_UserInfoPart : public PIn_SpecialPart
{
  public:
    /*!  @brief constructor */
    PIn_UserInfoPart (PIn_Part  & oPart );

    /*! @brief destructor */
    ~PIn_UserInfoPart ();

    /*! get the sysdba */
    const char * const GetSYSDBA() const {return szSYSDBA;}

    /*! get the user type */
    const char * const GetType() const {return szType;}

    const char * const GetUserGroup() const {return szUserGroup;}

private:
    char szSYSDBA[PIn_UserInfoPart_SYSDBALength];
    char szUserGroup[PIn_UserInfoPart_SYSDBALength];
    char szType[PIn_UserInfoPart_TypeLength];

    bool GetField
        ( int    nNumber,
          char * szBuffer,
          int  & nSize );
}; // end PIn_UserInfoPart

/* ======================================================== */

/*!  @brief Data Part Class */
class PIn_DataPart : public PIn_SpecialPart
{
  public:
    /*!  @brief unicode flag */
    typedef enum {
      ConvertUnknown,
      ConvertAlways,
      ConvertNever,
      ConvertSometimes
    } ConversionNeeded;

    /*!  @brief constructor */
    PIn_DataPart (PIn_Part  & oPart );

    /*! @brief destructor */
    ~PIn_DataPart ();

    /*! @brief retrieve value adress */
    const tsp00_Byte * GetValueAdress
      ( tsp1_param_info * pInfo );

    /*! @brief check for null */
    bool IsNull
      ( tsp1_param_info * pInfo );

    /*! @brief retrieve value as int */
    bool GetValueAsInt
      ( tsp1_param_info * pInfo,
        SAPDB_Int4      & nValue );

    /*! @brief retrieve value as char */
    bool GetValueAsChar
      ( tsp1_param_info *  pInfo,
        const char      *& pValue,
        tsp00_Int4       & nLength,
        ConversionNeeded & oConversion);
}; // end PIn_DataPart


/* ======================================================== */

/*!  @brief Column Names Part Class */
class PIn_ColumnNamesPart : public PIn_SpecialPart
{
    public:
        /*!  @brief constructor */
        PIn_ColumnNamesPart (PIn_Part  & oPart );

        /*! @brief destructor */
        ~PIn_ColumnNamesPart ();

        char* GetNextColumnName(char *pszBuffer, SAPDB_UInt4& nBufferLength);

        SAPDB_UInt4 GetColumnNamesCount() const;

    private:
        SAPDB_Int4  m_nColumnNamesCount;
        SAPDB_Int4  m_nPartOffset;

}; // end PIn_ColumnNamesPart


/* ======================================================== */

/*!  @brief Key Part Class */
class PIn_KeyPart : public PIn_SpecialPart
{
    public:
        /*!  @brief constructor */
        PIn_KeyPart (PIn_Part  & oPart );

        /*! @brief destructor */
        ~PIn_KeyPart ();

        void GetKey(SAPDB_Byte *pszKeyBuf, SAPDB_UInt4& nKeyLength) const;

    private:
        SAPDB_Int2          m_nKeyLength;
        const SAPDB_Byte*   m_pszKey;

}; // end PIn_KeyPart


/* ======================================================== */

/*!  @brief Surrogate (stamp) Part Class */
class PIn_SurrogatePart : public PIn_SpecialPart
{
    public:
        /*!  @brief constructor */
        PIn_SurrogatePart (PIn_Part  & oPart );

        /*! @brief destructor */
        ~PIn_SurrogatePart ();

        void GetSurrogate(SAPDB_Byte *pszStampBuf, SAPDB_UInt4& nStampLength) const;

    private:
        SAPDB_Int4          m_nSurrogateLength;
        const SAPDB_Byte*   m_pszSurrogate;

}; // end PIn_SurrogatePart


/* ======================================================== */

/*!  @brief BDInfo Part Class */
class PIn_BDInfoPart : public PIn_SpecialPart
{
    public:
        /*!  @brief constructor */
        PIn_BDInfoPart (PIn_Part  & oPart );

        /*! @brief destructor */
        ~PIn_BDInfoPart ();

        /*! @brief BDInfo is always 48 Bytes long */
        void GetBDInfo(SAPDB_Byte *pszBDInfoBuf) const;

    private:
        const SAPDB_Byte*   m_pszBDInfo;
        const SAPDB_UInt4   m_nBDInfoLength;

}; // end PIn_BDInfoPart


#endif // PIn_SpecialParts_h
