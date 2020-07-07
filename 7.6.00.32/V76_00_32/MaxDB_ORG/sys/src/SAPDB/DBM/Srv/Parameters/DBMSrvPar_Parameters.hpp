/*!
  @file           DBMSrvPar_Parameters.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer database parameters handling - Specification

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

#ifndef _DBMSrvPar_Parameters_HPP_
#define _DBMSrvPar_Parameters_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "DBM/Srv/Parameters/DBMSrvPar_Parameter.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "ToolsCommon/Tools_Array.hpp"

/*! @brief parameter pointer object */
class DBMSrvPar_Parameter_Pointer {
public:
  DBMSrvPar_Parameter_Pointer() : pPointer(0) {}
private:
  DBMSrvPar_Parameter * pPointer;
  friend class DBMSrvPar_Parameters;
  friend class DBMSrvPar_Iterator;
  friend class DBMSrvPar_ClassIterator;
}; // end class DBMSrvPar_Parameter_Pointer

typedef Tools_Array<DBMSrvPar_Parameter_Pointer> DBMSrvPar_Parameter_Array;

/*! @brief parameters object */
class DBMSrvPar_Parameters : protected DBMSrvPar_Parameter_Array {

public:
  /*! @brief  Constructor */
  DBMSrvPar_Parameters
                  ( const char * szDBNameIN );
  /*! @brief  Destructor */
  ~DBMSrvPar_Parameters();
  /*! @brief  Save a parameter object in container.
      This function save the provided parameter object in the container.
      @param  pParameter [IN] - pointer to parameter object */
  void                  Put
                  ( DBMSrvPar_Parameter * pParameter  );
  /*! @brief  Get a parameter object from container by name.
      This function return the requested parameter object from the container.
      @param  szParameter [IN] - null terminated name of parameter
      @return Pointer to parameter object */
  DBMSrvPar_Parameter * Get
                  ( const char          * szParameter ) const;
  DBMSrvPar_Parameter * Get
    ( const SAPDB_UTF8    * szParameter ) const {return Get((const char *) szParameter);}

  /*! @brief  Deletes a parameter object from container by name.
      This function deletes and fress the meory the requested parameter object 
      from the container.
      @param  szParameter [IN] - null terminated name of parameter
      @param  pParameter  [IN] - pointer to parameter structure */
  void Delete
                  ( const char          * szParameter );
  void Delete
                  ( const DBMSrvPar_Parameter * pParameter  );


  /*! @brief  Creates a instance of the parameter object
      @param  szDBName   [IN]  - name of database
      @param  bReadOnly  [IN]  - does not migrate or modify the database parameters
      @param  oError      [OUT] - a message list which contains information in case of errros
      @return Pointer to parameters object. In case of errors the function returns a null pointer
              and oError will be filled. */
  static DBMSrvPar_Parameters * GetInstance
     ( const char      * szDBName,
       bool              bReadOnly,
       DBMSrvMsg_Error & oError );

  /*! @brief  Set a parameter value with check
      @param  szParameter     [IN]  - name of parameter 
      @param  szValue         [IN] - value for parameter
      @param  bIgnoreReadOnly [IN] - ignore the read only check
      @param  oWriteMode      [IN] - were to write permanent online or both
      @return the well known DBMServer error */
 bool SetValueWithCheck
     ( const char                       * szParameter,
       const char                       * szValue,
       bool                               bIgnoreReadOnly,
       DBMSrvPar_Parameter::XPWriteMode   oWriteMode,
       DBMSrvMsg_Error                  & oError );

  /*! @brief  Check all parameters
      @param  oError      [OUT] - a message list which contains information in case of errros
      @return the well known DBMServer error */
  bool CheckAll
     ( DBMSrvMsg_Error & oError );

  /*! @brief  Write parameters to file and/or kernel
      @param  oError      [OUT] - a message list which contains information in case of errros
      @return the well known DBMServer error */
  bool Commit
     ( DBMSrvMsg_Error & oError );

private:
  /*! @brief  Compte hash value */
  int                   GetHash
                  ( const char          * szParameter ) const;

  /*! DBMSrvPar_Parameters */
  static const size_type m_HashBase;

  Tools_DynamicUTF8String                 szDBName;

public:
  DBMSrvPar_Parameter      * pFirst;             /* first parameter (by help file order)              */
  bool                       bParamFileExist;    /* exists at least one parameter in paramfile        */
  bool                       bKernelDirty;       /* Dirty-Flag                                        */
  bool                       bDevspaceDirty;     /* Dirty-Flag                                        */

  friend class DBMSrvPar_Iterator;
  friend class DBMSrvPar_ClassIterator;
}; // end class DBMSrvPar_Parameters



#endif /* _DBMSrvPar_Parameters_HPP_ */

