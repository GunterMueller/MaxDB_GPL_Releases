/*!
  @file           Catalog_DBProcDescriptor.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          descriptor for a db-procedure

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


#ifndef CATALOG_DBPROCDESCRIPTOR_HPP
#define CATALOG_DBPROCDESCRIPTOR_HPP

class Catalog_ObjectId;
class SQLMan_Context;
class Catalog_DBProcObject;
class Catalog_DBProcInfoObject;
class Msg_List;

#include "SQLManager/Catalog/Catalog_ReturnCodes.hpp"

/*!
   @brief describes a db-procedure
  */
class Catalog_DBProcDescriptor
{
public :
    /*!
       @brief constructor
     */
    Catalog_DBProcDescriptor(SQLMan_Context& Context);
    /*!
       @brief destructor
     */
    virtual ~Catalog_DBProcDescriptor();
    /*!
       @brief return codes of Create method
     */
    enum CreateRcEnum {
        createRcOk               = Catalog_ReturnCodes::CatRcOk,
        createRcCancelled        = Catalog_ReturnCodes::CatRcCancelled,
        createRcUnknownProcedure = Catalog_ReturnCodes::CatRcUnknownProcedure,
        createRcSystemError      = Catalog_ReturnCodes::CatRcSystemError
    };

    typedef Catalog_MethodReturnCode<CreateRcEnum> CreateRc;

   /*!
      @brief create the descriptor for the given procedure identification
    */
    CreateRc Create (const Catalog_ObjectId& DbprocId, Msg_List& ErrMsg);
 
    enum GetNameRcEnum {
        gnRcOk            = Catalog_ReturnCodes::CatRcOk,
        gnRcCancelled     = Catalog_ReturnCodes::CatRcCancelled,
        gnRcSystemError   = Catalog_ReturnCodes::CatRcSystemError
    };

    typedef Catalog_MethodReturnCode<GetNameRcEnum> GetNameRc;
     /*!
      @brief returns then name of the schema which contains the db procedure
    */
    GetNameRc GetSchemaName (SQLMan_Identifier& SchemaName, Msg_List& ErrMsg) const;
    /*!
     @brief returns the name of the db-procedure
     */
    GetNameRc GetName (SQLMan_Identifier& Name, Msg_List& ErrMsg) const;
private :
    SQLMan_Context&                   m_context;
    Catalog_DBProcObject*             m_pDBProc;
    mutable Catalog_DBProcInfoObject* m_pDBProcInfo;
};

#endif