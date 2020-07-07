/*!===========================================================================

  module:       StudioTD_TableDef.h

  ------------------------------------------------------------------------------

  responsible:  MarkusS, WolfgangA

  special area: SQL Studio

  description:  Definition of the table definition class StudioTD_TableDef.

  version:      7.6

  created:      2000-05-09

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

#ifndef STUDIOTD_TABLEDEF_H
#define STUDIOTD_TABLEDEF_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include <stdio.h>

#include "SQLStudio/Studio_List.hpp"

#include "SQLStudio/TableDefinition/StudioTD_Global.h"
#include "SQLStudio/TableDefinition/StudioTD_Util.h"
#include "SQLStudio/TableDefinition/StudioTD_Base.h"
#include "SQLStudio/TableDefinition/StudioTD_ColumnDef.h"
#include "SQLStudio/TableDefinition/StudioTD_ConstraintDef.h"
#include "SQLStudio/TableDefinition/StudioTD_ForeignKeyDef.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*!===========================================================================
  declaration: StudioTD_RETURN_CODES
 
  description: 
    Return code signaling the error state of a function call.

  possible values:
    StudioTD_OK - No error occured
    StudioTD_TABLE_NOT_FOUND - The requested table is not defined
============================================================================*/
typedef enum _sttd_return_codes
{
  StudioTD_OK,
  StudioTD_TABLE_NOT_FOUND,
  StudioTD_WRONG_SQL_STATEMENT
  
} StudioTD_RETURN_CODES;


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
#define TYPE(a)          (a *)0


/*!===========================================================================
  class: StudioTD_TableDef
 
  description: 
    Class for managing a table definition.

    It contains all the attributes of a table definition and methods for
    creating SQL statements (CREATE or ALTER TABLE) or reading an existing
    table definition from the database.
============================================================================*/
class StudioTD_TableDef : public StudioTD_Base
{
public:
  /*!-------------------------------------------------------------------------
    function:     StudioTD_TableDef
    description:  Constructor of the class.
  --------------------------------------------------------------------------*/
  StudioTD_TableDef ();
  /*!-------------------------------------------------------------------------
    function:     StudioTD_TableDef
    description:  Copy constructor of the class.
  --------------------------------------------------------------------------*/
  StudioTD_TableDef (const StudioTD_TableDef& table);
  /*!-------------------------------------------------------------------------
    function:     ~StudioTD_TableDef
    description:  Destructor of the class.
  --------------------------------------------------------------------------*/
  ~StudioTD_TableDef();

  

  /*!-------------------------------------------------------------------------
    function:     owner
    description:  Get the value of the owner attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String owner () { return mOwner; }
  /*!-------------------------------------------------------------------------
    function:     setOwner
    description:  Set the value of the owner attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setOwner (StudioTD_String owner )
  {
      mOwner = owner;
  }
   /*!-------------------------------------------------------------------------
    function:     schema
    description:  Get the value of the owner attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String schemaname () { return m_schemaname; }
  /*!-------------------------------------------------------------------------
    function:     setOwner
    description:  Set the value of the owner attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setSchemaName (StudioTD_String schemaname )
  {
      m_schemaname = schemaname;
  }
  /*!-------------------------------------------------------------------------
    function:     name
    description:  Get the value of the tablename attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String name () { return mName; }
  /*!-------------------------------------------------------------------------
    function:     setName
    description:  Set the value of the tablename attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setName (StudioTD_String name)
  {
    SET_DIRTYFLAG(mName, name);
    mName = name;
  }

  /*!-------------------------------------------------------------------------
   function:     setConstraintName
   description:  Set the value of the tablename attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void  setConstraintName      ( StudioTD_String name )       { mConstraintName =  name; }
  /*!-------------------------------------------------------------------------
   function:     setConstraintExpression
   description:  Set the value of the tablename attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void  setConstraintExpression( StudioTD_String expression ) { mConstraintExpression =  expression; }

 /*!-------------------------------------------------------------------------
    function:     constraintname
    description:  Get the value of the tablename attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String constraintname()       { return mConstraintName ;}

 /*!-------------------------------------------------------------------------
    function:     constraintexpression
    description:  Get the value of the tablename attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String constraintexpression() { return mConstraintExpression;}

  /*!-------------------------------------------------------------------------
    function:     mode
    description:  Get the value of the mode attribute.
    returns:      StudioTD_Mode
  --------------------------------------------------------------------------*/
  inline StudioTD_Mode mode () { return mMode; }
  /*!-------------------------------------------------------------------------
    function:     setMode
    description:  Set the value of the mode attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setMode (StudioTD_Mode mode) { mMode = mode; }
  /*!-------------------------------------------------------------------------
    function:     basedOn
    description:  Get the value of the basedOn attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String basedOn () { return mBasedOn; }
  /*!-------------------------------------------------------------------------
    function:     setBasedOn
    description:  Set the value of the basedOn attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setBasedOn (StudioTD_String basedon)
  {
    SET_DIRTYFLAG(mBasedOn,basedon);
    mBasedOn = basedon;
  }
  /*!-------------------------------------------------------------------------
    function:     createAs
    description:  Get the value of the createAs attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String createAs () { return mCreateAs; }
  /*!-------------------------------------------------------------------------
    function:     setCreateAs
    description:  Set the value of the createAs attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setCreateAs (StudioTD_String createas)
  {
    SET_DIRTYFLAG(mCreateAs,createas);
    mCreateAs = createas;
  }
  /*!-------------------------------------------------------------------------
    function:     sampleValue
    description:  Get the value of the sample value attribute.
    returns:      SAPDB_UInt
  --------------------------------------------------------------------------*/
  inline SAPDB_UInt sampleValue () { return mSampleValue; }
  /*!-------------------------------------------------------------------------
    function:     setSampleValue
    description:  Set the value of the sample value attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setSampleValue (SAPDB_UInt value)
  {
    SET_DIRTYFLAG(mSampleValue,value);
    mSampleValue = value;
  }
  /*!-------------------------------------------------------------------------
    function:     sampleUnit
    description:  Get the value of the sample unit attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String sampleUnit () { return mSampleUnit; }
  /*!-------------------------------------------------------------------------
    function:     setSampleUnit
    description:  Set the value of the sample unit attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setSampleUnit (StudioTD_String unit)
  {
    SET_DIRTYFLAG(mSampleUnit,unit);
    mSampleUnit = unit;
  }
  /*!-------------------------------------------------------------------------
    function:     duplicates
    description:  Get the value of the duplicates attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String duplicates () { return mDuplicates; }
  /*!-------------------------------------------------------------------------
    function:     setDuplicates
    description:  Set the value of the duplicates attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setDuplicates (StudioTD_String duplicates)
  {
    SET_DIRTYFLAG(mDuplicates,duplicates);
    mDuplicates = duplicates;
  }
  /*!-------------------------------------------------------------------------
    function:     temporary
    description:  Get the value of the temporary attribute.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  inline SAPDB_Bool temporary () { return mTemporary; }
  /*!-------------------------------------------------------------------------
    function:     setTemporary
    description:  Set the value of the temporary attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setTemporary (SAPDB_Bool temporary)
  {
    SET_DIRTYFLAG(mTemporary,temporary);
    mTemporary = temporary;
  }
  /*!-------------------------------------------------------------------------
    function:     ignoreRollback
    description:  Get the value of the ignoreRollback attribute.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  inline SAPDB_Bool ignoreRollback () { return mIgnoreRollback; }
  /*!-------------------------------------------------------------------------
    function:     setIgnoreRollback
    description:  Set the value of the ignoreRollback attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setIgnoreRollback (SAPDB_Bool ignorerollback)
  {
    SET_DIRTYFLAG(mIgnoreRollback,ignorerollback);
    mIgnoreRollback = ignorerollback;
  }
  /*!-------------------------------------------------------------------------
    function:     caseSensitive
    description:  Get the value of the caseSensitive attribute.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  inline SAPDB_Bool caseSensitive () { return mCaseSensitive; }
  /*!-------------------------------------------------------------------------
    function:     setCaseSensitive
    description:  Set the value of the caseSensitive attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setCaseSensitive (SAPDB_Bool casesensitive)
  {
    SET_DIRTYFLAG(mCaseSensitive,casesensitive);
    mCaseSensitive = casesensitive;
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
    SET_DIRTYFLAG(mComment,comment);
    mComment = comment;
  }
  /*!-------------------------------------------------------------------------
    function:     ignoreIncomplete
    description:  Get the value of the ignoreIncomplete attribute.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  inline SAPDB_Bool ignoreIncomplete () { return mIgnoreIncomplete; }
  /*!-------------------------------------------------------------------------
    function:     setIgnoreIncomplete
    description:  Set the value of the ignoreIncomplete attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setIgnoreIncomplete (SAPDB_Bool ignoreincomplete) { mIgnoreIncomplete = ignoreincomplete; }
  /*!-------------------------------------------------------------------------
    function:     setAlterType
    description:  Set the AlterType attribute of all its components to the
                  given value.
  --------------------------------------------------------------------------*/
  void setAlterType (StudioTD_AlterType type);

  
  /*!-------------------------------------------------------------------------
    function:     addColumn
    description:  Create a new empty column definition and append it to the
                  columns list.
    returns:      StudioTD_ColumnDef *
  --------------------------------------------------------------------------*/
  StudioTD_ColumnDef * addColumn ();
  /*!-------------------------------------------------------------------------
    function:     removeColumn
    description:  Removes the element with the given index from the list.
    arguments:    index - Index of the element to remove
  --------------------------------------------------------------------------*/
  void removeColumn (SAPDB_UInt index);
  /*!-------------------------------------------------------------------------
    function:     column
    description:  Get a reference to the element with the given index.
    arguments:    index - Index of the element to get
    returns:      StudioTD_ColumnDef *
  --------------------------------------------------------------------------*/
  StudioTD_ColumnDef * column (SAPDB_UInt index);
  /*!-------------------------------------------------------------------------
    function:     findColumn
    description:  Get the index of the element with the given name.
    arguments:    name - Name of the requested column
    returns:      SAPDB_Int
  --------------------------------------------------------------------------*/
  SAPDB_Int findColumn (StudioTD_String name);
  /*!-------------------------------------------------------------------------
    function:     findColumn
    description:  Get the index of the element with the given position
    arguments:    pos - position of column in table definition
    returns:      SAPDB_UInt
  --------------------------------------------------------------------------*/
  SAPDB_Int findColumn(SAPDB_UInt2 pos);

  /*!-------------------------------------------------------------------------
    function:     columnCount
    description:  Get the number of elements in the columns list.
    returns:      SAPDB_UInt
  --------------------------------------------------------------------------*/
  inline SAPDB_UInt columnCount () { return mColumns.size(); }
  /*!-------------------------------------------------------------------------
    function:     twistColumns
    description:  Twist the column of index with the column of index+1.
    arguments:    index - Index of the first column
  --------------------------------------------------------------------------*/
  void twistColumns (SAPDB_UInt index);
  /*!-------------------------------------------------------------------------
    function:     addConstraint
    description:  Create a new empty constraint definition and append it to the
                  constraints list.
    returns:      StudioTD_ConstraintDef *
  --------------------------------------------------------------------------*/
  StudioTD_ConstraintDef * addConstraint ();
  /*!-------------------------------------------------------------------------
    function:     removeConstraint
    description:  Removee the element with the given index from the list.
    arguments:    index - Index of the element to remove
  --------------------------------------------------------------------------*/
  void removeConstraint (SAPDB_UInt index);
  /*!-------------------------------------------------------------------------
    function:     constraint
    description:  Get a reference to the element with the given index.
    arguments:    index - Index of the element to get
    returns:      StudioTD_ConstraintDef *
  --------------------------------------------------------------------------*/
  StudioTD_ConstraintDef * constraint (SAPDB_UInt index);
  /*!-------------------------------------------------------------------------
    function:     findConstraint
    description:  Get the index of the element with the given name.
    arguments:    name - Name of the requested constraint
    returns:      SAPDB_Int
  --------------------------------------------------------------------------*/
  SAPDB_Int findConstraint (StudioTD_String name);
  /*!-------------------------------------------------------------------------
    function:     constraintCount
    description:  Get the number of elements in the constraints list.
    returns:      SAPDB_UInt
  --------------------------------------------------------------------------*/
  inline SAPDB_UInt constraintCount () { return mConstraints.size(); }
  /*!-------------------------------------------------------------------------
    function:     addForeignKey
    description:  Create a new empty foreign key definition and append it to
                  the foreign keys list.
    returns:      StudioTD_ForeignKeyDef *
  --------------------------------------------------------------------------*/
  StudioTD_ForeignKeyDef * addForeignKey ();
  /*!-------------------------------------------------------------------------
    function:     removeForeignKey
    description:  Removee the element with the given index from the list.
    arguments:    index - Index of the element to remove
  --------------------------------------------------------------------------*/
  void removeForeignKey (SAPDB_UInt index);
  /*!-------------------------------------------------------------------------
    function:     foreignKey
    description:  Get a reference to the element with the given index.
    arguments:    index - Index of the element to get
    returns:      StudioTD_ForeignKeyDef *
  --------------------------------------------------------------------------*/
  StudioTD_ForeignKeyDef * foreignKey (SAPDB_UInt index);
  /*!-------------------------------------------------------------------------
    function:     findForeignKey
    description:  Get the index of the element with the given name.
    arguments:    name - Name of the requested foreign key
    returns:      SAPDB_Int
  --------------------------------------------------------------------------*/
  SAPDB_Int findForeignKey (StudioTD_String name);
  /*!-------------------------------------------------------------------------
    function:     foreignKeyCount
    description:  Get the number of elements in the foreign keys list.
    returns:      SAPDB_UInt
  --------------------------------------------------------------------------*/
  inline SAPDB_UInt foreignKeyCount () { return mForeignKeys.size(); }
  
  
  /*!-------------------------------------------------------------------------
    function:     reset
    description:  Reset the table definition to its initial state.
  --------------------------------------------------------------------------*/
  void reset ();
  /*!-------------------------------------------------------------------------
    function:     readTableDefinition
    description:  Read the definition of the given table from the database.
    arguments:    tablename - Name of the table to be loaded
                  owner - Owner of the table to be loaded
                  ownerIsSchema - the owner if the table is a schema
    returns:      SAPDB_UInt
  --------------------------------------------------------------------------*/
  SAPDB_UInt readTableDefinition (StudioTD_String tablename, StudioTD_String owner,SAPDB_Bool ownerIsSchema);
  /*!-------------------------------------------------------------------------
    function:     hasValidColumns
    description:  Check if the table definition contains valid column
                  definitions (with a valid name and datatype).
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  SAPDB_Bool hasValidColumns ();
  /*!-------------------------------------------------------------------------
    function:     isDirty
    description:  Check if the dirty flag is set for at least one of the
                  table components.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  SAPDB_Bool isDirty ();
  /*!-------------------------------------------------------------------------
    function:     resetDirty
    description:  Reset the dirty flags of all table components.
  --------------------------------------------------------------------------*/
  void resetDirty ();
  /*!-------------------------------------------------------------------------
    function:     findIncompleteDefs
    description:  Go through the table definition and check if there are
                  incomplete definition parts. The returned string indicates
                  the location of the missing data in the format
                  "<page> <row> <column>".
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String findIncompleteDefs ();
  /*!-------------------------------------------------------------------------
    function:     genCreateStatement
    description:  Generate a CREATE statement with the attributes of the table.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genCreateStatement ();


  /*!-------------------------------------------------------------------------
    function:     genCreateXML
    description:  Generate a XML string with the attributes of the table.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genCreateXML();
  /*!-------------------------------------------------------------------------
    function:     genAlterStatement
    description:  Generate ALTER statements for the differences between the
                  own objekt and the given reference object.
    arguments:    oldTable - Old table as reference for changes
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genAlterStatement (StudioTD_TableDef *oldTable);
  /*!-------------------------------------------------------------------------
    function:     genDescription
    description:  Generate a description of the table definition that can be
                  stored as a draft.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genDescription ();
  /*!-------------------------------------------------------------------------
    function:     parseDescription
    description:  Parse a given description of a table definition that was
                  stored as a draft.
    arguments:    description - Description to be parsed
  --------------------------------------------------------------------------*/
  void parseDescription (StudioTD_String description);

  
  /*!-------------------------------------------------------------------------
    function:     keyCount
    description:  Determine the number of key columns in the columns list.
    returns:      SAPDB_UInt
  --------------------------------------------------------------------------*/
  SAPDB_UInt keyCount ();

  /*!-------------------------------------------------------------------------
    function:     defaultCount
    description:  Returns count of columns with default in the columns list
    returns:      SAPDB_Uint4
  --------------------------------------------------------------------------*/
  SAPDB_UInt4 defaultCount();

protected:
  /*!-------------------------------------------------------------------------
    function:     executeSQL
    description:  Execute the given SQL statement.
    arguments:    statement - SQL statement to be executed
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  virtual SAPDB_Bool executeSQL (StudioTD_String& statement) = 0;
  /*!-------------------------------------------------------------------------
    function:     readResult
    description:  Reads the result of an executed SQL statement record by record.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  virtual SAPDB_Bool readResult (StudioTD_String *record,SAPDB_Bool* bIsNull) = 0;



  
  StudioTD_String           mOwner;                // Owner of the table  
  StudioTD_String           m_schemaname;          // Schema of the table  
  StudioTD_String           mName;                 // Name of the table
  StudioTD_Mode             mMode;                 // Mode (usage context)
  StudioTD_String           mBasedOn;              // Template on which the table is based
  StudioTD_String           mCreateAs;             // SQL statement for CREATE AS statement
  SAPDB_UInt                mSampleValue;          // Sample value (for statistics)
  StudioTD_String           mSampleUnit;           // Sample unit (for statistics)
  StudioTD_String           mDuplicates;           // Duplicates clause of the table
  SAPDB_Bool                mTemporary;            // Flag for temporary tables
  SAPDB_Bool                mIgnoreRollback;       // Flag for ignoring rollback
  SAPDB_Bool                mCaseSensitive;        // Flag for case sensitive names
  StudioTD_String           mComment;              // Comment for the misc
  SAPDB_Bool                mVariableLength;       // Flag for no fixed length
  SAPDB_Bool                mIgnoreIncomplete;     // Flag for ignoring incomplete definitions
  SAPDB_Bool                mDirty;                // Flag for indicating modified attributes

  StudioTD_String           mConstraintName;
  StudioTD_String           mConstraintExpression;
  
  Studio_List               mColumns;              // List of columns of table
  Studio_List               mRemovedColumns;       // List of removed columns (only for mode AlterTable)
  Studio_List               mConstraints;          // List of constraints of table
  Studio_List               mRemovedConstraints;   // List of removed constraints (only for mode AlterTable)
  Studio_List               mForeignKeys;          // List of foreign keys of table
  Studio_List               mRemovedForeignKeys;   // List of removed foreign keys (only for mode AlterTable)
  
};  /*! endclass: StudioTD_TableDef */

#endif   /* STUDIOTD_TABLEDEF_H */
