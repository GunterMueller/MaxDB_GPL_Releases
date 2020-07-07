/*!
   @defgroup       Catalog SAPDB catalog implementation
*/
/*!
  @file           Catalog_Types.hpp
  @author         ThomasA 
  @ingroup        Catalog
  @brief          defines types used in the catalog area

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

#ifndef CATALOG_TYPES_HPP
#define CATALOG_TYPES_HPP

#include "SQLManager/SQLMan_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

typedef tak_messblock_record      Catalog_MessBlockObjectPascal;  //!< the c++ name for tak_messblock_record
typedef tak_systembuffer          Catalog_Object_Pascal;          //!< the c++ name for tak_systembuffer
typedef tak_baserecord            Catalog_TableObjectPascal;      //!< the c++ name for tak_baserecord
typedef tak00_columninfo          Catalog_ColumnInfo;             //!< the c++ name for tak00_columninfo
typedef tak00_colinfo_ptr         Catalog_ColumnInfoPtr;          //!< the c++ name for tak00_colinfo_ptr
typedef tak_methodrecord          Catalog_MethodObjectPascal;     //!< the c++ name for tak_methodrecord
typedef tak_method_fileinforecord Catalog_MethodFileObjectPascal; //!< the c++ name for tak_methodfileinforecord
typedef tak_method_inforecord     Catalog_MethodInfoObjectPascal; //!< the c++ name for tak_method_inforecord
typedef tak_param_info            Catalog_ParameterInfo;          //!< new c++ name for tak_param_info
typedef tak_viewtextrecord        Catalog_ViewtextObjectPascal;   //!< new c++ name for tak_viewtextrecord
typedef tak_statisticsinfo        Catalog_TableStatisticsObjectPascal; //!< new c++ name for tak_statisticsinfo

//typedef tak_userrecord            Catalog_UserObjectPascal;       //!< new c++ name for tak_userrecord 

class Catalog_Object;
class Catalog_AuthorizationObject;

/*!
   @brief cpp encapsulation of tak_userrecord
 */
class Catalog_UserObjectPascal : public tak_userrecord
{
public :
   /*!
     @brief cast to Catalog_AuthorizationObject
    */
   operator Catalog_AuthorizationObject*()
   {
       return reinterpret_cast<Catalog_AuthorizationObject*>(this);
   }
};

/*!
   @brief defines a catalog object identifier. 
 */
class Catalog_ObjectId : public tgg00_Surrogate
{
public :
    /*!
       @brief constructor, initializes to 0x00
     */
    Catalog_ObjectId()
    {
        memset (this, 0, sizeof(*this));
    }
    /*!
       @brief copy constructor, source is a pascal surrogate
     */
    Catalog_ObjectId(const tgg00_Surrogate& source)
    {
        *this = source;
    }
    /*!
       @brief assignment
     */
    void operator= (const Catalog_ObjectId& source)
    {
        SAPDB_MemCopyNoCheck (this, &source, sizeof(*this));
    }
    /*!
       @brief assignment from pascal surrogate
     */
    void operator= (const tgg00_Surrogate& source)
    {
        SAPDB_MemCopyNoCheck (this, &source, sizeof(*this));
    }
    /*!
       @brief checks for assigned object identification 
     */
    bool IsNull() const 
    {
        for (int ix = 0; ix < sizeof(*this); ++ix) {
            if ((*this)[ix] != 0)
            {
                return false;
            }
        }
        return true;
    }

};

/*!
   @brief defines a timestamp 
*/
class Catalog_Timestamp
{
public :
    /*!
       @brief default constructor
     */
    Catalog_Timestamp();
    /*!
       @brief constructs from date and time given as integers in catalog layout
       @param date in integer representation
       @param time in integer representation
     */
    Catalog_Timestamp(SAPDB_Int4 date, SAPDB_Int4 time);
    /*!
       @brief yields date and time as integers in catalog representation
       @param date date in integer representation
       @param time time in integer representation
     */
    void ToInt (SAPDB_Int4& date, SAPDB_Int4& time);
    /*!
       @brief cast to a 0-terminated string
     */
    operator char* ()
    {
        return &m_timestamp[0];
    }
private :
    char m_timestamp[27]; //<! 0 termininated timestamp in ascii representation
};

/*!
   @brief defines a GUID
 */
class Catalog_GUID : public tsp00_C16
{
public :
    Catalog_GUID() //!< default constructor
    {
    }
    Catalog_GUID(tsp00_C16& SourceGuid) //!< copy constructor
    {
        SAPDB_MemCopyNoCheck (this, &SourceGuid[0], sizeof(*this));
    }
};

/*!
   @brief macro calculating the offset of a class member
 */
#define CATALOG_OFFSET(T,M) (((char*) &((T*) 0)->M) - (char*) 0)

/*!
   @brief macro casting a catalog object to the required type
*/
#define CATALOG_OBJECT_CAST(TYPE,PTR,OFFSET) reinterpret_cast<TYPE*>(reinterpret_cast<unsigned char*>(PTR) + OFFSET)

#endif
