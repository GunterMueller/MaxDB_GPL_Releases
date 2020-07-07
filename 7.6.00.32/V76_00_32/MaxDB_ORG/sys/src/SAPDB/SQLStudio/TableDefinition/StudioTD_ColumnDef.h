/*!===========================================================================

  module:       StudioTD_ColumnDef.h

  ------------------------------------------------------------------------------

  responsible:  MarkusS,WolfgangA

  special area: SQL Studio

  description:  Definition of the column definition class StudioTD_ColumnDef.

  version:      7.4

  created:      2000-05-11

  updated:      2000-07-07

  ------------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG
  



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




============================================================================*/

#ifndef STUDIOTD_COLUMNDEF_H
#define STUDIOTD_COLUMNDEF_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SQLStudio/TableDefinition/StudioTD_Global.h"
#include "SQLStudio/TableDefinition/StudioTD_Util.h"
#include "SQLStudio/TableDefinition/StudioTD_Base.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*!===========================================================================
  class: StudioTD_ColumnDef
 
  description: 
    Class for managing a column definition.

    It contains all the attributes of a column definition and methods for
    creating SQL statements (CREATE or ALTER TABLE).
============================================================================*/
class StudioTD_ColumnDef : public StudioTD_Base
{
public:
  /*!-------------------------------------------------------------------------
    function:     StudioTD_ColumnDef
    description:  Constructor of the class.
  --------------------------------------------------------------------------*/
  StudioTD_ColumnDef ();
  /*!-------------------------------------------------------------------------
    function:     ~StudioTD_ColumnDef
    description:  Destructor of the class.
  --------------------------------------------------------------------------*/
  ~StudioTD_ColumnDef();

  /*!-------------------------------------------------------------------------
    function:     name
    description:  Get the value of the column name attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String tableowner () { return mTableOwner ;}
  /*!-------------------------------------------------------------------------
    function:     name
    description:  Get the value of the column name attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String tablename  () { return mTableName  ;}

  /*!-------------------------------------------------------------------------
    function:     setName
    description:  Set the value of the column name attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setTableOwner( StudioTD_String owner )     { mTableOwner = owner    ; }
  /*!-------------------------------------------------------------------------
    function:     setName
    description:  Set the value of the column name attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setTableName ( StudioTD_String tablename ) { mTableName  = tablename; }
  
  /*!-------------------------------------------------------------------------
    function:     name
    description:  Get the value of the column name attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String name () { return mName; }
  /*!-------------------------------------------------------------------------
    function:     setName
    description:  Set the value of the column name attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setName (StudioTD_String name)
  {
    SET_DIRTYFLAG(mName, name);
      
    if (mAlterType == AlterNoChange)
      mOldName = mName;
      
    SET_ATTRIBUTE(mName, name);
  }
  /*!-------------------------------------------------------------------------
    function:     oldName
    description:  Get the value of the old column name attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String oldName () { return mOldName; }
  /*!-------------------------------------------------------------------------
    function:     setOldName
    description:  Set the value of the old column name attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setOldName (StudioTD_String name) { mOldName = name; }
  /*!-------------------------------------------------------------------------
    function:     dataType
    description:  Get the value of the datatype attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String dataType () { return mDataType; }
  /*!-------------------------------------------------------------------------
    function:     setDataType
    description:  Set the value of the datatype attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setDataType (StudioTD_String datatype)
  {
    SET_DIRTYFLAG(mDataType, datatype);
    SET_ATTRIBUTE(mDataType, datatype);
  }
  /*!-------------------------------------------------------------------------
    function:     dimension
    description:  Get the value of the dimension attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String dimension () { return mDimension; }
  /*!-------------------------------------------------------------------------
    function:     setDimension
    description:  Set the value of the dimension attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setDimension (StudioTD_String dimension)
  {
    SET_DIRTYFLAG(mDimension, dimension);
    SET_ATTRIBUTE(mDimension, dimension);
  }
  /*!-------------------------------------------------------------------------
    function:     codeType
    description:  Get the value of the codetype attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String codeType () { return mCodeType; }
  /*!-------------------------------------------------------------------------
    function:     setCodeType
    description:  Set the value of the codetype attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setCodeType (StudioTD_String codetype)
  {
    SET_DIRTYFLAG(mCodeType, codetype);
    SET_ATTRIBUTE(mCodeType, codetype);
  }
  /*!-------------------------------------------------------------------------
    function:     key
    description:  Get the value of the key attribute.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  inline SAPDB_Bool key () { return mKey; }
  /*!-------------------------------------------------------------------------
    function:     setKey
    description:  Set the value of the key attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setKey (SAPDB_Bool key)
  {
    SET_DIRTYFLAG(mKey, key);
    SET_ATTRIBUTE(mKey, key);
  }
  /*!-------------------------------------------------------------------------
    function:     keyPos
    description:  Get the position of the key.
    returns:      SAPDB_Int
  --------------------------------------------------------------------------*/
  inline SAPDB_Int2 keyPos () { return mKeyPos; }
  /*!-------------------------------------------------------------------------
    function:     setKey
    description:  Set the value of the key attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setKeyPos (SAPDB_Int2 keyPos)
  {
    SET_DIRTYFLAG(mKeyPos, keyPos);
    SET_ATTRIBUTE(mKeyPos, keyPos);
  }

  
  /*!-------------------------------------------------------------------------
    function:     ColPos
    description:  Get the position of the original column position.
    returns:      SAPDB_Int
  --------------------------------------------------------------------------*/
  inline SAPDB_Int2 ColPos()
  {
      return mColPos;
  }
  /*!-------------------------------------------------------------------------
    function:     setColPos
    description:  Set the value of the original position to the given value.
  --------------------------------------------------------------------------*/
  inline void setColPos(SAPDB_Int2 lColPos)
  {
      SET_DIRTYFLAG(mColPos, lColPos);
      SET_ATTRIBUTE(mColPos, lColPos);
  }

  /*!-------------------------------------------------------------------------
    function:     notNull
    description:  Get the value of the notnull attribute.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  inline SAPDB_Bool notNull () { return mNotNull; }
  /*!-------------------------------------------------------------------------
    function:     setNotNull
    description:  Set the value of the not null attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setNotNull (SAPDB_Bool notnull)
  {
    SET_DIRTYFLAG(mNotNull, notnull);
    SET_ATTRIBUTE(mNotNull, notnull);
  }
  
  /*!-------------------------------------------------------------------------
    function:     defaultValue
    description:  Get the value of the default attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String defaultValue () { return mDefault; }
  
  /*!-------------------------------------------------------------------------
    function:     IsDefaultUserDefinedValue
    description:  Get information if the default is a DB function or a user
                  defined value.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  inline SAPDB_Bool
  IsDefaultUserDefinedValue()
  {
      return mbUserDefinedDefault;
  }
  
  /*!-------------------------------------------------------------------------
    function:     setDefaultValue
    description:  Set the value of the default attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setDefaultValue (StudioTD_String defaultvalue)
  {
    SET_DIRTYFLAG(mDefault, defaultvalue);
    SET_ATTRIBUTE(mDefault, defaultvalue);
  }

  void setDefaultValueEx(const StudioTD_String &defaultvalue);

  
  /*!-------------------------------------------------------------------------
    function:     setDefaultValue - overloaded
    description:  Set the value of the default attribute to the given value
                  and additionally if the given value is a DB function or
                  a user defined value.
  --------------------------------------------------------------------------*/
  inline void
  setDefaultValue(StudioTD_String defaultvalue, SAPDB_Bool bUserDefined)
  {
      SET_DIRTYFLAG(mDefault, defaultvalue);
      SET_ATTRIBUTE(mDefault, defaultvalue);

      SET_DIRTYFLAG(mbUserDefinedDefault, bUserDefined);
      SET_ATTRIBUTE(mbUserDefinedDefault, bUserDefined);
  }

  /*!-------------------------------------------------------------------------
    function:     unique
    description:  Get the value of the unique attribute.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  inline SAPDB_Bool unique () { return mUnique; }
  /*!-------------------------------------------------------------------------
    function:     setUnique
    description:  Set the value of the unique attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setUnique (SAPDB_Bool unique)
  {
    SET_DIRTYFLAG(mUnique, unique);
    SET_ATTRIBUTE(mUnique, unique);
  }
  /*!-------------------------------------------------------------------------
    function:     comment
    description:  Get the value of the comment attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String comment () { return mComment; }
  /*!-------------------------------------------------------------------------
    function:     setComment
    description:  Set the value of the comment attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setComment (StudioTD_String comment)
  {
    SET_DIRTYFLAG(mComment, comment);
    SET_ATTRIBUTE(mComment, comment);
  }
  /*!-------------------------------------------------------------------------
    function:     alterType
    description:  Get the value of the altertype attribute.
    returns:      StudioTD_AlterType
  --------------------------------------------------------------------------*/
  inline StudioTD_AlterType alterType () { return mAlterType; }
  /*!-------------------------------------------------------------------------
    function:     setAlterType
    description:  Set the value of the altertype attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setAlterType (StudioTD_AlterType altertype)
  {
    if (altertype == AlterNoChange)
    {
      mAlterType = altertype;
      return;
    }

    switch (mAlterType)
    {
      case AlterNoChange:
      case AlterChange:
        mAlterType = altertype;
      case AlterAdd:
        if (altertype == AlterRemove)
          mAlterType = altertype;
    }
  }

  
  /*!-------------------------------------------------------------------------
    function:     findInomplete
    description:  Check if the column definition is incomplete (name or
                  datatype is missing).
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String findIncomplete ();
  /*!-------------------------------------------------------------------------
    function:     genCreateStatement
    description:  Generate a part of a CREATE statement for this column with
                  the attributes of the column.
    arguments:    mode - Creation context
                  casesensitive - Flag for case sensitive names
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genCreateStatement (StudioTD_Mode mode, SAPDB_Bool casesensitive);

  /*!-------------------------------------------------------------------------
    function:     genCreateXML
    description:  Generate a Column XML Element
                  with the attributes of the column.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genCreateXML ( SAPDB_Bool casesensitive );

  /*!-------------------------------------------------------------------------
    function:     genAlterStatement
    description:  Generate a part of a ALTER statement for this column with
                  the changed attributes of the column.
    arguments:    oldColumn - Pointer to the original column
                  owner - Table owner
                  table - Table name
                  casesensitive - Flag for case sensitive names
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genAlterStatement (StudioTD_ColumnDef *oldColumn, 
                                     StudioTD_String owner,
                                     StudioTD_String table,
                                     SAPDB_Bool casesensitive);
  /*!-------------------------------------------------------------------------
    function:     genDescription
    description:  Generate a description of the column definition that can be
                  stored as a draft.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genDescription ();
  /*!-------------------------------------------------------------------------
    function:     parseDescription
    description:  Parse a given description of a column definition that was
                  stored as a draft.
    arguments:    description - Description to be parsed
  --------------------------------------------------------------------------*/
  void parseDescription (StudioTD_String description);
  /*!-------------------------------------------------------------------------
    function:     isDirty
    description:  Check if the dirty flag is set.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  inline SAPDB_Bool isDirty () { return mDirty; }
  /*!-------------------------------------------------------------------------
    function:     resetDirty
    description:  Reset the dirty flag.
  --------------------------------------------------------------------------*/
  inline void resetDirty () { mDirty = false; }
  

private:
  StudioTD_String      mTableOwner;           // Name of the Table Owner
  StudioTD_String      mTableName ;           // Name of the Table
  StudioTD_String      mName;                 // Name of the column
  StudioTD_String      mOldName;              // Old name of the column
  StudioTD_String      mDataType;             // Datatype of the column
  StudioTD_String      mDimension;            // Dimension of char columns
  StudioTD_String      mCodeType;             // Codetype of char columns
  SAPDB_Bool           mKey;                  // Flag for key columns
  SAPDB_Bool           mNotNull;              // Flag for NotNull columns  
  StudioTD_String      mDefault;              // Default value of column
  SAPDB_Bool           mbUserDefinedDefault;  // Indicating if default value is user defined or DB function (like DATE)
  SAPDB_Bool           mUnique;               // Flag for unique columns
  StudioTD_String      mComment;              // Comment for the column
  StudioTD_AlterType   mAlterType;            // only for AlterTable
  SAPDB_Bool           mDirty;                // Flag for indicating modified attributes
  SAPDB_Int2           mKeyPos;               // Index of the colum in a composite key
  SAPDB_Int2           mColPos;               // Original position of the column in the table (see column POS
                                              // in system table COLUMNS)

};  /*! endclass: StudioTD_ColumnDef */

#endif   /* STUDIOTD_COLUMNDEF_H */
