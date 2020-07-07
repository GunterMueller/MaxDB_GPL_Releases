/*!
  @file           SQLMan_Types.hpp
  @author         ThomasA
  @brief          types used in sql manager
 
\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef SQLMan_TYPES_HPP
#define SQLMan_TYPES_HPP

#include "vak001.h"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

typedef tgg00_DateTimeFormat SQLMan_DateTimeFormat;  //!< new name for tgg00_DateTimeFormat
typedef tgg00_Surrogate      SQLMan_Surrogate;       //!< new name for SQLMan_Surrogate
typedef tgg00_FileId         SQLMan_FileId;          //!< new name for tgg00_FileId
typedef pasbool              SQLMan_PascalBool;      //!< new name for pasbool  
typedef tgg00_MessBufStruct  SQLMan_MessStruct;      //!< new name for tgg00_MessBufStruct
typedef tgg00_MessType       SQLMan_MessType;        //!< new name for tgg00_MessType
typedef tgg00_MessType2      SQLMan_MessType2;       //!< new name for tgg00_MessType2
typedef tsp00_SqlMode        SQLMan_SqlMode;         //!< new name for tsp00_SqlMode
typedef tgg00_StackEntry     SQLMan_StackEntry;      //!< new name for tgg00_StackEntry
typedef tgg00_TransContext   SQLMan_TransContext;    //!< new name for tgg00_TransContext
typedef tsp00_DataType       SQLMan_DataType;        //!< new name for tsp00_DataType
typedef tak_queryrewrite_mode_Enum SQLMan_QueryRewriteMode; //!< new name for tak_queryrewrite_mode_Enum
typedef tak_appl_type        SQLMan_ApplType;        //!< new name for tak_appl_type
typedef tsp00_NodeId         SQLMan_NodeId;          //!< new name for tsp00_NodeId
typedef tak_nls_param_rec    SQLMan_NlsParams;       //!< new name for tak_nls_param_rec
/*!
   @brief defines layout of an identifier
*/
class SQLMan_Identifier : public tsp00_KnlIdentifier
{
public :
    /*!
       @brief constructor, initializes an blank identifier
    */
    SQLMan_Identifier();
    /*!
       @brief constructor, ATTENTION!!! initializes to blank identifier only if initialize=TRUE
    */
    SQLMan_Identifier(bool initialize);
    /*!
       @brief contructor, initializes with a null terminated string
       @param source identifier to be copied, must be ascii and null terminated
    */
    SQLMan_Identifier(const char* Source, bool AsciiSource);
    /*!
       @brief contructor, initializes with a pascal string
       @param source identifier to be copied, must be ascii 
       @param length length of the identifier
    */
    SQLMan_Identifier(const void* Source, const SAPDB_UInt2 Length, bool AsciiSource);
    /*!
       @brief contructor, initializes with a kernel identifier
       @param source identifier to be copied
    */
    inline SQLMan_Identifier(const tsp00_KnlIdentifier& Source);
    /*!
      @brief blanks the identifier
    */
    void SetBlank();
    /*!
      @brief returns true if the identifier is blank
    */
    SAPDB_Bool IsBlank() const;
    /*!
      @brief returns the length of the identifier text
    */
    SAPDB_UInt GetLength() const;
    /*!
      @brief write a utf8 representation to buffer
    */
    void ToUtf8(SAPDB_UTF8* buffer, SAPDB_UInt bufferSize) const;
private :
    /*!
      @brief avoids identifier to be iterpreted as a char*. Note : operator char* is not implemented, i.e. the kernel cannot be linked 
             if someone tries to call this function.
     */
    operator char* () const;
};

inline SQLMan_Identifier::SQLMan_Identifier(const tsp00_KnlIdentifier& Source)
{
    SAPDB_MemCopyNoCheck (this, &Source, sizeof(Source));
}

class SharedSQL_ParseID;

/*!
   @brief identifier of a sql execution plan 
*/
class SQLMan_ParseId : public tak_parsid
{
public :
    /*!
       @brief copy constructor
       @param source the plan identifier to be copied
    */
    SQLMan_ParseId(const tak_parsid& source)
    {
        SAPDB_MemCopyNoCheck(this, &source, sizeof(*this));
    }
    /*!
       @brief SharedSQL_ParseID and SQLMan_ParseId are the same
    */
    operator SharedSQL_ParseID&()
    {
        return *REINTERPRET_CAST(SharedSQL_ParseID*, this);
    }
};

#endif
