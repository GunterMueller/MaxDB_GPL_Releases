/*!===========================================================================

  module:       StudioTD_TableDef.cpp

  ----------------------------------------------------------------------------

  responsible:  MarkusS, WolfgangA, Hans-GeorgB

  special area: SQL Studio

  description:  Implementation of the table definition class StudioTD_TableDef.

  see:          http://www.sapdb.org

  version:      7.4

  created:      2000-05-11

  updated:      2000-07-07

  ----------------------------------------------------------------------------

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SQLStudio/TableDefinition/StudioTD_TableDef.h"


/*===========================================================================*
 *     StudioTD_TableDef::StudioTD_TableDef                                  *
 *===========================================================================*/
StudioTD_TableDef::StudioTD_TableDef ()
    : mMode(CreateNew), mSampleValue(20000), mSampleUnit("Rows"),
      mTemporary(false), mIgnoreRollback(false), mCaseSensitive(false),mVariableLength(false)
{
  setType (ClassTableDef);
}

/*===========================================================================*
 *     StudioTD_TableDef::StudioTD_TableDef                                  *
 *===========================================================================*/
StudioTD_TableDef::StudioTD_TableDef (const StudioTD_TableDef & table)
{
  StudioTD_ColumnDef      *col, *oldCol;
  StudioTD_ConstraintDef  *constr, *oldConstr;
  StudioTD_ForeignKeyDef  *forkey, *oldForkey;
  Studio_List               list;

  setType (ClassTableDef);

  /*-------------------------------------------------------------------------*
   * Copy the table attributes.                                              *
   *-------------------------------------------------------------------------*/
  mName               = table.mName;
  mMode               = table.mMode;
  mCreateAs           = table.mCreateAs;
  mSampleValue        = table.mSampleValue;
  mSampleUnit         = table.mSampleUnit;
  mDuplicates         = table.mDuplicates;
  mTemporary          = table.mTemporary;
  mIgnoreRollback     = table.mIgnoreRollback;
  mCaseSensitive      = table.mCaseSensitive;
  mComment            = table.mComment;
  mIgnoreIncomplete   = table.mIgnoreIncomplete;


  /*-------------------------------------------------------------------------*
   * Copy the columns.                                                       *
   *-------------------------------------------------------------------------*/
  for (oldCol = (StudioTD_ColumnDef *)((Studio_List &)table.mColumns).first();
       oldCol;
       oldCol = (StudioTD_ColumnDef *)((Studio_List &)table.mColumns).next())
  {
    col = new StudioTD_ColumnDef (*oldCol);
    mColumns.append (col);
  }

  for (oldCol = (StudioTD_ColumnDef *)((Studio_List &)table.mRemovedColumns).first();
       oldCol;
       oldCol = (StudioTD_ColumnDef *)((Studio_List &)table.mRemovedColumns).next())
  {
    col = new StudioTD_ColumnDef (*(StudioTD_ColumnDef *)oldCol);
    mRemovedColumns.append (col);
  }

  /*-------------------------------------------------------------------------*
   * Copy the constraints.                                                   *
   *-------------------------------------------------------------------------*/
  for (oldConstr = (StudioTD_ConstraintDef *)((Studio_List &)table.mConstraints).first();
       oldConstr;
       oldConstr = (StudioTD_ConstraintDef *)((Studio_List &)table.mConstraints).next())
  {
    constr = new StudioTD_ConstraintDef (*oldConstr);
    mConstraints.append (constr);
  }

  for (oldConstr = (StudioTD_ConstraintDef *)((Studio_List &)table.mRemovedConstraints).first();
       oldConstr;
       oldConstr = (StudioTD_ConstraintDef *)((Studio_List &)table.mRemovedConstraints).next())
  {
    constr = new StudioTD_ConstraintDef (*(StudioTD_ConstraintDef *)oldConstr);
    mRemovedConstraints.append (constr);
  }

  /*-------------------------------------------------------------------------*
   * Copy the foreign keys.                                                  *
   *-------------------------------------------------------------------------*/
  for (oldForkey = (StudioTD_ForeignKeyDef *)((Studio_List &)table.mForeignKeys).first();
       oldForkey;
       oldForkey = (StudioTD_ForeignKeyDef *)((Studio_List &)table.mForeignKeys).next())
  {
    forkey = new StudioTD_ForeignKeyDef (*oldForkey);
    mForeignKeys.append (forkey);
  }

  for (oldForkey = (StudioTD_ForeignKeyDef *)((Studio_List &)table.mRemovedForeignKeys).first();
       oldForkey;
       oldForkey = (StudioTD_ForeignKeyDef *)((Studio_List &)table.mRemovedForeignKeys).next())
  {
    forkey = new StudioTD_ForeignKeyDef (*oldForkey);
    mRemovedForeignKeys.append (forkey);
  }
}

/*===========================================================================*
 *     StudioTD_TableDef::~StudioTD_TableDef                                 *
 *===========================================================================*/
StudioTD_TableDef::~StudioTD_TableDef()
{
    reset();
}


/*===========================================================================*
 *     StudioTD_TableDef::setAlterType                                       *
 *===========================================================================*/
void StudioTD_TableDef::setAlterType (StudioTD_AlterType type)
{
  Studio_ListData  item;

  for (item = mColumns.first(); item; item = mColumns.next())
    ((StudioTD_ColumnDef *)item)->setAlterType (type);

  for (item = mConstraints.first(); item; item = mConstraints.next())
    ((StudioTD_ConstraintDef *)item)->setAlterType (type);

  for (item = mForeignKeys.first(); item; item = mForeignKeys.next())
    ((StudioTD_ForeignKeyDef *)item)->setAlterType (type);
}


/*===========================================================================*
 *     StudioTD_TableDef::addColumn                                          *
 *===========================================================================*/
StudioTD_ColumnDef * StudioTD_TableDef::addColumn ()
{
  StudioTD_ColumnDef *col;

  /*-------------------------------------------------------------------------*
   * Create a new column object.                                             *
   *-------------------------------------------------------------------------*/
  col = new StudioTD_ColumnDef;

  /*-------------------------------------------------------------------------*
   * Append the new column object to the columns list.                       *
   *-------------------------------------------------------------------------*/
  mColumns.append   (col);

  /*-------------------------------------------------------------------------*
   * Initialize the alter type.                                              *
   *-------------------------------------------------------------------------*/
  col->setAlterType (AlterAdd);

  mDirty = true;

  return col;
}


/*===========================================================================*
 *     StudioTD_TableDef::removeColumn                                       *
 *===========================================================================*/
void StudioTD_TableDef::removeColumn (SAPDB_UInt index)
{
  StudioTD_ColumnDef *col;

  /*-------------------------------------------------------------------------*
   * Check if index is a valid list position.                                *
   *-------------------------------------------------------------------------*/
  if (index >= mColumns.size())
    return;

  /*-------------------------------------------------------------------------*
   * Remove the item with the given index from the columns list and append   *
   * it to the removed columns list.                                         *
   *-------------------------------------------------------------------------*/
  col = (StudioTD_ColumnDef *)mColumns.remove (index, 1);
  mRemovedColumns.append (col);

  mDirty = true;
}


/*===========================================================================*
 *     StudioTD_TableDef::column                                             *
 *===========================================================================*/
StudioTD_ColumnDef * StudioTD_TableDef::column (SAPDB_UInt index)
{
    return (StudioTD_ColumnDef *)mColumns.at (index);
}


/*===========================================================================*
 *     StudioTD_TableDef::findColumn                                         *
 *===========================================================================*/
SAPDB_Int StudioTD_TableDef::findColumn(SAPDB_UInt2 pos)
{
    StudioTD_ColumnDef *pCol = NULL;
    SAPDB_Int          i    = 0;

    /*-------------------------------------------------------------------------*
     * Search the list for an item with the given position.                    *
     *-------------------------------------------------------------------------*/
    for (pCol = REINTERPRET_CAST(StudioTD_ColumnDef*, mColumns.first());
         pCol;
         pCol = REINTERPRET_CAST(StudioTD_ColumnDef*, mColumns.next()), ++i)
    {
        if (pos == pCol->ColPos())
        {
            return i;
        }
    }
    return -1;
}


/*===========================================================================*
 *     StudioTD_TableDef::findColumn                                         *
 *===========================================================================*/
SAPDB_Int StudioTD_TableDef::findColumn (StudioTD_String name)
{
    StudioTD_ColumnDef *pCol;
    SAPDB_Int           i;

    /*-------------------------------------------------------------------------*
     * Search the list for an item with the given name.                        *
     *-------------------------------------------------------------------------*/
    for (pCol = (StudioTD_ColumnDef *)mColumns.first(), i = 0;
         pCol;
         pCol = (StudioTD_ColumnDef *)mColumns.next(), i++)
    {
        if (pCol->name() == name)
        {
            return i;
        }
    }
    return -1;
}


/*===========================================================================*
 *     StudioTD_TableDef::twistColumns                                       *
 *===========================================================================*/
void StudioTD_TableDef::twistColumns (SAPDB_UInt index)
{
    mColumns.twist(index);
}


/*===========================================================================*
 *     StudioTD_TableDef::addConstraint                                      *
 *===========================================================================*/
StudioTD_ConstraintDef * StudioTD_TableDef::addConstraint ()
{
    StudioTD_ConstraintDef *constr;

    /*-------------------------------------------------------------------------*
     * Create a new constraint object.                                         *
     *-------------------------------------------------------------------------*/
    constr = new StudioTD_ConstraintDef;

    /*-------------------------------------------------------------------------*
     * Append the new constraint object to the constraints list.               *
     *-------------------------------------------------------------------------*/
    mConstraints.append (constr);

    /*-------------------------------------------------------------------------*
     * Initialize the alter type.                                              *
     *-------------------------------------------------------------------------*/
    constr->setAlterType (AlterAdd);

    return constr;
}


/*===========================================================================*
 *     StudioTD_TableDef::removeConstraint                                   *
 *===========================================================================*/
void StudioTD_TableDef::removeConstraint (SAPDB_UInt index)
{
  StudioTD_ConstraintDef *constr;

  /*-------------------------------------------------------------------------*
   * Check if index is a valid list position.                                *
   *-------------------------------------------------------------------------*/
  if (index >= mConstraints.size())
    return;

  /*-------------------------------------------------------------------------*
   * Remove the item with the given index from the constraints list and      *
   * append it to the removed constraints list.                              *
   *-------------------------------------------------------------------------*/
  constr = (StudioTD_ConstraintDef *)mConstraints.remove (index, 1);
  mRemovedConstraints.append (constr);
}


/*===========================================================================*
 *     StudioTD_TableDef::constraint                                         *
 *===========================================================================*/
StudioTD_ConstraintDef * StudioTD_TableDef::constraint (SAPDB_UInt index)
{
  return (StudioTD_ConstraintDef *)mConstraints.at (index);
}


/*===========================================================================*
 *     StudioTD_TableDef::findConstraint                                     *
 *===========================================================================*/
SAPDB_Int StudioTD_TableDef::findConstraint (StudioTD_String name)
{
  StudioTD_ConstraintDef *constr;
  SAPDB_Int               i;

  /*-------------------------------------------------------------------------*
   * Search the list for an item with the given name.                        *
   *-------------------------------------------------------------------------*/
  for (constr = (StudioTD_ConstraintDef *)mConstraints.first(), i = 0;
       constr;
       constr = (StudioTD_ConstraintDef *)mConstraints.next(), i++)
  {
    if (constr->name() == name)
      return i;
  }

  return -1;
}


/*===========================================================================*
 *     StudioTD_TableDef::addForeignKey                                      *
 *===========================================================================*/
StudioTD_ForeignKeyDef * StudioTD_TableDef::addForeignKey ()
{
  StudioTD_ForeignKeyDef *forkey;

  /*-------------------------------------------------------------------------*
   * Create a new foreign key object.                                        *
   *-------------------------------------------------------------------------*/
  forkey = new StudioTD_ForeignKeyDef;

  /*-------------------------------------------------------------------------*
   * Append the new foreign key object to the foreign keys list.             *
   *-------------------------------------------------------------------------*/
  mForeignKeys.append (forkey);

  /*-------------------------------------------------------------------------*
   * Initialize the alter type.                                              *
   *-------------------------------------------------------------------------*/
  forkey->setAlterType (AlterAdd);

  return forkey;
}


/*===========================================================================*
 *     StudioTD_TableDef::removeForeignKey                                   *
 *===========================================================================*/
void StudioTD_TableDef::removeForeignKey (SAPDB_UInt index)
{
  StudioTD_ForeignKeyDef *forkey;

  /*-------------------------------------------------------------------------*
   * Check if index is a valid list position.                                *
   *-------------------------------------------------------------------------*/
  if (index >= mForeignKeys.size())
    return;

  /*-------------------------------------------------------------------------*
   * Remove the item with the given index from the foreign keys list and     *
   * append it to the removed foreign keys list.                             *
   *-------------------------------------------------------------------------*/
  forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.remove (index, 1);
  mRemovedForeignKeys.append (forkey);
}


/*===========================================================================*
 *     StudioTD_TableDef::foreignKey                                         *
 *===========================================================================*/
StudioTD_ForeignKeyDef * StudioTD_TableDef::foreignKey (SAPDB_UInt index)
{
  return (StudioTD_ForeignKeyDef *)mForeignKeys.at (index);
}


/*===========================================================================*
 *     StudioTD_TableDef::findForeignKey                                     *
 *===========================================================================*/
SAPDB_Int StudioTD_TableDef::findForeignKey (StudioTD_String name)
{
  StudioTD_ForeignKeyDef *forkey;
  SAPDB_Int               i;

  /*-------------------------------------------------------------------------*
   * Search the list for an item with the given name.                        *
   *-------------------------------------------------------------------------*/
  for (forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.first(), i = 0;
       forkey;
       forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.next(), i++)
  {
    if (forkey->name() == name)
      return i;
  }

  return -1;
}


/*===========================================================================*
 *     StudioTD_TableDef::reset                                              *
 *===========================================================================*/
void StudioTD_TableDef::reset ()
{
    /*-------------------------------------------------------------------------*
     * Reset the table attributes to their initial values.                     *
     *-------------------------------------------------------------------------*/
    mOwner        = "";
    mName           = "";
    mMode           = CreateNew;
    mSampleValue    = 20000;
    mSampleUnit     = "Rows";
    mDuplicates     = "";
    mTemporary      = false;
    mIgnoreRollback = false;
    mCaseSensitive  = false;
    mComment        = "";
    mConstraintName = "";
    mConstraintExpression = "";

    /*-------------------------------------------------------------------------*
     * Remove all columns, constraints, and foreign keys.                      *
     *-------------------------------------------------------------------------*/

    StudioTD_ColumnDef *pCol = NULL;
    if (0 != mColumns.size())
    {
        pCol = REINTERPRET_CAST(StudioTD_ColumnDef*, mColumns.first());
        while (NULL != pCol)
        {
            delete pCol;
            pCol = REINTERPRET_CAST(StudioTD_ColumnDef*, mColumns.next());
        }
        mColumns.clear();
    }

    if (0 != mRemovedColumns.size())
    {
        pCol = REINTERPRET_CAST(StudioTD_ColumnDef*, mRemovedColumns.first());
        while (pCol != NULL)
        {
            delete pCol;
            pCol = REINTERPRET_CAST(StudioTD_ColumnDef*, mRemovedColumns.next());
        }
        mRemovedColumns.clear();
    }

    StudioTD_ConstraintDef *pConstr = NULL;
    if (0 != mConstraints.size())
    {
        pConstr = REINTERPRET_CAST(StudioTD_ConstraintDef*, mConstraints.first());
        while (NULL != pConstr)
        {
            delete pConstr;
            pConstr = REINTERPRET_CAST(StudioTD_ConstraintDef*, mConstraints.next());
        }
        mConstraints.clear();
    }

    if (0 != mRemovedConstraints.size())
    {
        pConstr = REINTERPRET_CAST(StudioTD_ConstraintDef*, mRemovedConstraints.first());
        while (NULL != pConstr)
        {
            delete pConstr;
            pConstr = REINTERPRET_CAST(StudioTD_ConstraintDef*, mRemovedConstraints.next());
        }
        mRemovedConstraints.clear();
    }

    StudioTD_ForeignKeyDef *pForKey = NULL; 
    if (0 != mForeignKeys.size())
    {
        pForKey = REINTERPRET_CAST(StudioTD_ForeignKeyDef*, mForeignKeys.first());
        while (NULL != pForKey)
        {
            delete pForKey;
            pForKey = REINTERPRET_CAST(StudioTD_ForeignKeyDef*, mForeignKeys.next());
        }
        mForeignKeys.clear();
    }

    if (0 != mRemovedForeignKeys.size())
    {
        pForKey = REINTERPRET_CAST(StudioTD_ForeignKeyDef*, mRemovedForeignKeys.first());
        while (NULL != pForKey)
        {
            delete pForKey;
            pForKey = REINTERPRET_CAST(StudioTD_ForeignKeyDef*, mRemovedForeignKeys.next());
        }
        mRemovedForeignKeys.clear();
    }
}
// StudioTD_TableDef::reset()

/*===========================================================================*
 *     StudioTD_TableDef::readTableDefinition                                *
 *===========================================================================*/
SAPDB_UInt
StudioTD_TableDef::readTableDefinition (StudioTD_String tablename, StudioTD_String owner,SAPDB_Bool ownerIsSchema)
{
    StudioTD_String         sql;
    StudioTD_String         name;
    StudioTD_String         record[11];
    StudioTD_String         s;
    StudioTD_String         whereClause;
    SAPDB_Bool              bRead = false;
    SAPDB_Bool              bFKSchema = false;
    StudioTD_ColumnDef     *col;
    StudioTD_ConstraintDef *constr;
    StudioTD_ForeignKeyDef *forkey;
    SAPDB_Bool              bColIsNull[11];

    StudioTD_String         escaped_tablename = tablename;
    StudioTD_String         escaped_owner = owner;

    // we have to duplicate every singlequote in the incoming names 
    // (e.g. "O'Reilly" -> "O''Reilly" ) , if they are to be converted
    // into SQL string literals .
    escapeQuoteChar(escaped_tablename, "'");
    escapeQuoteChar(escaped_owner, "'");

    /*-------------------------------------------------------------------------*
     * Reset the table definition to its initial values.                       *
     *-------------------------------------------------------------------------*/
    reset();

    mOwner = owner;
    mName = tablename;  

    /*-------------------------------------------------------------------------*
     * Prepare the WHERE clause for the SQL statements.                        *
     *-------------------------------------------------------------------------*/
    whereClause += "WHERE A.tablename = '";
    whereClause += escaped_tablename;
    if(ownerIsSchema)
        whereClause += "' AND A.schemaname = '";
    else
        whereClause += "' AND A.owner = '";

    whereClause += escaped_owner;
    whereClause += "' ";

    /*-------------------------------------------------------------------------*
     * Load the column definitions from the database.                          *
     *-------------------------------------------------------------------------*/
sql  = "SELECT A.columnname, A.datatype, A.len, A.dec, A.codetype, "
         "A.nullable,A.mode,A.keypos,A.default, A.comment, B.type "
         "FROM domain.columns A, ";
    if(ownerIsSchema){
        sql +=  "(select schemaname, tablename, columnname, type,indexname from domain.indexcolumns outertab where outertab.tablename = '";
        sql += escaped_tablename;
        sql += "'AND outertab.schemaname = '";
        sql += escaped_owner;
        sql += "' AND type = 'UNIQUE' and indexname <> 'SYSPRIMARYKEYINDEX' and 1 = ( select count(indexname) from domain.indexcolumns innertab where innertab.tablename = '";
        sql += escaped_tablename;
        sql += "'AND innertab.schemaname = '";
        sql += escaped_owner;
        sql += "' AND innertab.indexname = outertab.indexname)) B ";
        sql += whereClause;
        sql += "AND A.schemaname = B.schemaname (+) AND A.tablename = B.tablename (+) AND A.columnname = B.columnname (+) ORDER BY A.pos";
    }else{        
        sql +=  "(select owner, tablename, columnname, type,indexname from domain.indexcolumns outertab where outertab.tablename = '";
        sql += escaped_tablename;
        sql += "'AND outertab.owner = '";
        sql += escaped_owner;
        sql += "' AND type = 'UNIQUE' and indexname <> 'SYSPRIMARYKEYINDEX' and 1 = ( select count(indexname) from domain.indexcolumns innertab where innertab.tablename = '";
        sql += escaped_tablename;
        sql += "'AND innertab.owner = '";
        sql += escaped_owner;
        sql += "' AND innertab.indexname = outertab.indexname)) B ";
        sql += whereClause;
        sql += "AND A.owner = B.owner (+) AND A.tablename = B.tablename (+) AND A.columnname = B.columnname (+) ORDER BY A.pos";
    }

    if (!executeSQL (sql))
    {
        return StudioTD_WRONG_SQL_STATEMENT;
    }


    while (readResult(record,bColIsNull))
    {
        col = addColumn();
        col->setName     (record[0]);
        col->setOldName  (record[0]);
        col->setDataType (ProperCase (record[1]));

        if ((col->dataType() == "Char") || (col->dataType() == "Varchar") || (col->dataType() == "Float"))
        {
            col->setDimension (record[2]);
        }

        if (col->dataType() == "Fixed")
        {
            s  = record[2];
            s += ",";
            s += record[3];
            col->setDimension (s);
        }

        col->setCodeType (record[4]);

        if (record[5] != "YES")
        {
            col->setNotNull (true);
        }

        if (record[6] == "KEY")
        {
            col->setKey (true);
            if (!bColIsNull[7])
            {
                col->setKeyPos(AtoI(record[7]));
            }
        }   

        if (!bColIsNull[8])
        {
            col->setDefaultValueEx(record[8]);
        }

        if (!bColIsNull[9])
        {
            col->setComment (record[9]);
        }

        if (!bColIsNull[10])
        {
            col->setUnique (true);
        }

        bRead = true;
    }   // end while (readResult(record,bColIsNull))

    if (!bRead)
    {
        return StudioTD_TABLE_NOT_FOUND;
    }

    /*-------------------------------------------------------------------------*
     * Load the constraint definitions from the database.                      *
     *-------------------------------------------------------------------------*/
    sql  = "SELECT A.constraintname, A.definition FROM domain.constraints A ";
    sql += whereClause;

    if (!executeSQL (sql))
    {
        return StudioTD_WRONG_SQL_STATEMENT;
    }

    while (readResult(record,bColIsNull))
    {
        constr = addConstraint();
        constr->setName(record[0]);
        constr->parseConditions(record[1]);
    }

    /*-------------------------------------------------------------------------*
     * check for a column schema name.                                         *
     *-------------------------------------------------------------------------*/
    sql  = "SELECT columnname FROM columns where tablename = 'FOREIGNKEYCOLUMNS' and columnname = 'REFSCHEMANAME'";
    if (executeSQL (sql)){
        while (readResult(record,bColIsNull)){

            if (record[0] == "REFSCHEMANAME"){
                bFKSchema = true;
            }
        }
    }
    /*-------------------------------------------------------------------------*
     * Load the foreign key definitions from the database.                     *
     *-------------------------------------------------------------------------*/
    if(bFKSchema){
        sql  = "SELECT A.fkeyname, A.columnname, A.reftablename, A.refschemaname, A.refcolumnname, A.rule, A.comment ";
    }else{
        sql  = "SELECT A.fkeyname, A.columnname, A.reftablename, A.refowner, A.refcolumnname, A.rule, A.comment "; 
    }
    
    sql += "FROM   foreignkeycolumns A ";
    sql += whereClause;

    if (!executeSQL (sql))
    {
        return StudioTD_WRONG_SQL_STATEMENT;
    }

    while (readResult(record,bColIsNull))
    {
        if (name != record[0])
        {
            forkey = addForeignKey();
            forkey->setName       (record[0]);
            s  = record[3];
            s += ".";
            s += record[2];
            s += " (";
            forkey->setReference (s);

            if (record[5].SubStr (0, 6) == "DELETE")
            {
                forkey->setOnDelete (record[5].SubStr (6));
            }
            else
            {
                forkey->setOnDelete (record[5]);
            }

            if (!bColIsNull[6])
            {
                forkey->setComment(record[6]);
            }

            name = record[0];
        }

        if (!forkey->referencingColumns().Empty())
        {
            s  = forkey->referencingColumns();
            s += ",";
            forkey->setReferencingColumns (s);
            s  = forkey->reference().SubStr (0, forkey->reference().Length() - 1);
            s += ",";
            forkey->setReference (s);
        }

        s  = forkey->referencingColumns();
        s += record[1];
        forkey->setReferencingColumns (s);
        s  = forkey->reference();
        s += record[4];
        s += ")";
        forkey->setReference (s);
    }   // end while (readResult(record,bColIsNull))

    /*-------------------------------------------------------------------------*
     * Load the misc attributes of the table definition.                       *
     *-------------------------------------------------------------------------*/
    sql  = "SELECT A.comment, A.sample_percent, A.sample_rows ";
    sql += "FROM   domain.tables A ";
    sql += whereClause;

    if (!executeSQL(sql))
    {
        return StudioTD_WRONG_SQL_STATEMENT;
    }

    if (readResult(record,bColIsNull))
    {
        if (!bColIsNull[0])
        {
            mComment = record[0];
        }

        if (!bColIsNull[1])
        {
            mSampleValue = AtoI (record[1]);
            mSampleUnit  = "Percent";
        }
        else
        {
            mSampleValue = AtoI (record[2]);
            mSampleUnit  = "Rows";
        }

        if (escaped_owner == "TEMP")
        {
            mTemporary = true;
        }
    }

    setAlterType (AlterNoChange);

    return StudioTD_OK;
}
// StudioTD_TableDef::readTableDefinition


/*===========================================================================*
 *     StudioTD_TableDef::hasValidColumns                                    *
 *===========================================================================*/
SAPDB_Bool StudioTD_TableDef::hasValidColumns ()
{
  SAPDB_Int           i;
  StudioTD_ColumnDef *col;

  /*-------------------------------------------------------------------------*
   * For each item in the columns list check if it is a valid definition.    *
   *-------------------------------------------------------------------------*/
  for (col = (StudioTD_ColumnDef *)mColumns.first(), i = 0;
       col;
       col = (StudioTD_ColumnDef *)mColumns.next(), i++)
  {
    if ((!col->name().Empty()) && (!col->dataType().Empty()))
      return true;
  }

  return false;
}


/*===========================================================================*
 *     StudioTD_TableDef::isDirty                                            *
 *===========================================================================*/
SAPDB_Bool StudioTD_TableDef::isDirty ()
{
  StudioTD_ColumnDef      *col;
  StudioTD_ConstraintDef  *constr;
  StudioTD_ForeignKeyDef  *forkey;

  /*-------------------------------------------------------------------------*
   * Check the dirty flag of the table.                                      *
   *-------------------------------------------------------------------------*/
  if (mDirty)
    return true;

  /*-------------------------------------------------------------------------*
   * Check the dirty flag of the columns.                                    *
   *-------------------------------------------------------------------------*/
  for (col = (StudioTD_ColumnDef *)mColumns.first();
       col;
       col = (StudioTD_ColumnDef *)mColumns.next())
  {
    if (col->isDirty())
      return true;
  }

  /*-------------------------------------------------------------------------*
   * Check the dirty flag of the constraints.                                *
   *-------------------------------------------------------------------------*/
  for (constr = (StudioTD_ConstraintDef *)mConstraints.first();
       constr;
       constr = (StudioTD_ConstraintDef *)mConstraints.next())
  {
    if (constr->isDirty())
      return true;
  }

  /*-------------------------------------------------------------------------*
   * Check the dirty flag of the foreign keys.                               *
   *-------------------------------------------------------------------------*/
  for (forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.first();
       forkey;
       forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.next())
  {
    if (forkey->isDirty())
      return true;
  }

  return false;
}


/*===========================================================================*
 *     StudioTD_TableDef::resetDirty                                         *
 *===========================================================================*/
void StudioTD_TableDef::resetDirty ()
{
  StudioTD_ColumnDef      *col;
  StudioTD_ConstraintDef  *constr;
  StudioTD_ForeignKeyDef  *forkey;

  /*-------------------------------------------------------------------------*
   * Reset the dirty flag of the table.                                      *
   *-------------------------------------------------------------------------*/
  mDirty = false;

  /*-------------------------------------------------------------------------*
   * Reset the dirty flag of the columns.                                    *
   *-------------------------------------------------------------------------*/
  for (col = (StudioTD_ColumnDef *)mColumns.first();
       col;
       col = (StudioTD_ColumnDef *)mColumns.next())
    col->resetDirty();

  /*-------------------------------------------------------------------------*
   * Reset the dirty flag of the constraints.                                *
   *-------------------------------------------------------------------------*/
  for (constr = (StudioTD_ConstraintDef *)mConstraints.first();
       constr;
       constr = (StudioTD_ConstraintDef *)mConstraints.next())
    constr->resetDirty();

  /*-------------------------------------------------------------------------*
   * Reset the dirty flag of the foreign keys.                               *
   *-------------------------------------------------------------------------*/
  for (forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.first();
       forkey;
       forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.next())
    forkey->resetDirty();
}


/*===========================================================================*
 *     StudioTD_TableDef::findIncompleteDefs                                 *
 *===========================================================================*/
StudioTD_String StudioTD_TableDef::findIncompleteDefs ()
{
  SAPDB_Int                i;
  StudioTD_String          s;
  char                     buf[16];
  StudioTD_ColumnDef      *col;
  StudioTD_ConstraintDef  *constr;
  StudioTD_ForeignKeyDef  *forkey;

  /*-------------------------------------------------------------------------*
   * For each item in the columns list check if it is incomplete.            *
   *-------------------------------------------------------------------------*/
  for (col = (StudioTD_ColumnDef *)mColumns.first(), i = 0;
       col;
       col = (StudioTD_ColumnDef *)mColumns.next(), i++)
  {
    s = col->findIncomplete();

    if (!s.Empty())
    {
      sprintf (buf, "0 %d %s", i, s.StrPtr());
      return buf;
    }
  }

  /*-------------------------------------------------------------------------*
   * For each item in the constraints list check if it is incomplete.        *
   *-------------------------------------------------------------------------*/
  for (constr = (StudioTD_ConstraintDef *)mConstraints.first(), i = 0;
       constr;
       constr = (StudioTD_ConstraintDef *)mConstraints.next(), i++)
  {
    s = constr->findIncomplete();

    if (!s.Empty())
    {
      sprintf (buf, "1 %d %s", i, s.StrPtr());
      return buf;
    }
  }

  /*-------------------------------------------------------------------------*
   * For each item in the foreign keys list check if it is incomplete.       *
   *-------------------------------------------------------------------------*/
  for (forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.first(), i = 0;
       forkey;
       forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.next(), i++)
  {
    s = forkey->findIncomplete();

    if (!s.Empty())
    {
      sprintf (buf, "2 %d %s", i, s.StrPtr());
      return buf;
    }
  }

  return "";
}


/*===========================================================================*
 *     StudioTD_TableDef::genCreateStatement                                 *
 *===========================================================================*/
StudioTD_String StudioTD_TableDef::genCreateStatement ()
{
  StudioTD_String          sql,s,k,keyTemp;
  StudioTD_String          escapedTableName,escapedOwner,escapedColumnName,escapedConstraint,escapedForeignKey;
  SAPDB_Bool               casesensitive = mCaseSensitive;
  SAPDB_Bool               bKey = false;
  SAPDB_Bool               bFirstKey = false;
  StudioTD_String*         primaryKeys;
  char                     buf[256];

  StudioTD_ColumnDef      *col;
  StudioTD_ConstraintDef  *constr;
  StudioTD_ForeignKeyDef  *forkey;

  StudioTD_String::ConstIterator  iter;

  /*-------------------------------------------------------------------------*
   * Check if a table name was given. If not --> return an empty string.     *
   *-------------------------------------------------------------------------*/
  if (mName.Empty())
    return "";

  /*-------------------------------------------------------------------------*
   * Generate the intro of the CREATE TABLE statement.                       *
   *-------------------------------------------------------------------------*/
  sql += "CREATE TABLE \"";

  if (mTemporary)
    sql += "TEMP\".\"";

  escapedOwner = mOwner;
  escapeQuoteChar(escapedOwner,"\"");
  if(!mTemporary && escapedOwner != ""){
      sql += escapedOwner;
      sql += "\".\"";
  }

  escapedTableName = CASE(mName);
  escapeQuoteChar(escapedTableName,"\"");
  sql += escapedTableName;
  sql += "\""StudioTD_NEW_LINE"("StudioTD_NEW_LINE;

  /*-------------------------------------------------------------------------*
   * For each item in the columns list generate its part of the statement.   *
   * In a second run the PRIMARY KEY clause is generated.                    *
   *-------------------------------------------------------------------------*/
  for (col = (StudioTD_ColumnDef *)mColumns.first(); col; col = (StudioTD_ColumnDef *)mColumns.next())
    sql += col->genCreateStatement (mMode, mCaseSensitive);

  /*-------------------------------------------------------------------------*
   * A primary key, constraints and foreign keys are only inserted if the    *
   * mode is not CreateFromQuery (query as a template).                      *
   *-------------------------------------------------------------------------*/
  if (mMode != CreateFromQuery)
  {
    SAPDB_UInt keyCounter = keyCount();
    primaryKeys = new StudioTD_String[keyCounter];

    for (col = (StudioTD_ColumnDef *)mColumns.first(); col; col = (StudioTD_ColumnDef *)mColumns.next()){
      
      if (col->key()){

        //the fist column will set the the key flag
        if(col->keyPos() == 1){
            bKey = true;
            bFirstKey = true;
        }

        keyTemp = (bFirstKey ? "\tPRIMARY KEY (" : "");
        keyTemp  += "\"";

		escapedColumnName = CASE(col->name());
		escapeQuoteChar(escapedColumnName,"\"");

		keyTemp  += escapedColumnName;
        keyTemp  += "\", ";    

        bFirstKey = false;
    
        primaryKeys[col->keyPos()-1] = keyTemp;
          
      }             
    }

    for(SAPDB_UInt i = 0; i < keyCounter;i++){
        sql += primaryKeys[i];
    }

    //delete all elements
    if(primaryKeys != NULL)
       delete[] primaryKeys;

    if (bKey)
    {
      iter = sql.End();
      iter-= 2;
      sql.Erase (iter, sql.End());
      sql += "),"StudioTD_NEW_LINE;
    }

    /*-----------------------------------------------------------------------*
     * For each item in the constraint list generate its part of the         *
     * statement.                                                            *
     *-----------------------------------------------------------------------*/
    for (constr = (StudioTD_ConstraintDef *)mConstraints.first();
         constr;
         constr = (StudioTD_ConstraintDef *)mConstraints.next())
      sql += constr->genCreateStatement (mMode, mCaseSensitive);

    /*-----------------------------------------------------------------------*
     * For each item in the foreignkey list generate its part of the         *
     * statement.                                                            *
     *-----------------------------------------------------------------------*/
    for (forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.first();
         forkey;
         forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.next())
      sql += forkey->genCreateStatement (mMode, mCaseSensitive);
  }

  iter = sql.End();
  iter -= 2;
  if (*iter == '\r')
  {
    iter -= sizeof(char);
  }
  sql.Erase (iter, sql.End());
  sql += StudioTD_NEW_LINE")";

  /*-------------------------------------------------------------------------*
   * Insert misc table attributes.                                           *
   *-------------------------------------------------------------------------*/
  if (mTemporary && mIgnoreRollback)
  {
    sql += StudioTD_NEW_LINE"IGNORE ROLLBACK";
  }

  if ((mSampleValue != 20000) || (mSampleUnit != "Rows"))
  {
    sprintf (buf, StudioTD_NEW_LINE"SAMPLE %d %s", mSampleValue, UpperCase(mSampleUnit).StrPtr());
    sql += buf;
  }

  /*-------------------------------------------------------------------------*
   * If the table is based on a query template the SELECT statement is       *
   * appended in the AS clause.                                              *
   *-------------------------------------------------------------------------*/
  if (mMode == CreateFromQuery)
  {
    sql += StudioTD_NEW_LINE"AS ";
    sql += mCreateAs;

    if (!mDuplicates.Empty())
    {
      sql += " ";
      sql += UpperCase(mDuplicates);
      sql += " DUPLICATES";
    }
  }

  /*-------------------------------------------------------------------------*
   * Finally, append the comments.                                           *
   *-------------------------------------------------------------------------*/
  if (!mComment.Empty())
  {
    StudioTD_String  escaped_Comment = mComment;
    escapeQuoteChar(escaped_Comment, "'");

    sql += SEP;
    sql += "COMMENT ON TABLE \"";

	sql += escapedTableName;

    sql += "\" IS '";
    sql += escaped_Comment;          
    sql += "'";
  }

  for (col = (StudioTD_ColumnDef *)mColumns.first();
       col;
       col = (StudioTD_ColumnDef *)mColumns.next())
  {
    if (!col->comment().Empty())
    {
      StudioTD_String escaped_col_comment = col->comment();

      escapeQuoteChar(escaped_col_comment, "'");

      sql += SEP;
      sql += "COMMENT ON COLUMN \"";

	  sql += escapedTableName;
      
      sql += "\".\"";

      escapedColumnName = CASE(col->name());
	  escapeQuoteChar(escapedColumnName,"\"");
	  sql += escapedColumnName;

      sql += "\" IS '";
      sql += escaped_col_comment;           
      sql += "'";
    }
  }

  if (mMode != CreateFromQuery)
  {
    for (forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.first();
         forkey;
         forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.next())
    {
      if (!forkey->comment().Empty())
      {
        StudioTD_String         escaped_forkey_comment = forkey->comment();

        escapeQuoteChar(escaped_forkey_comment, "'");

        sql += SEP;
        sql += "COMMENT ON FOREIGN KEY \"";
        sql += escapedTableName;
        sql += "\".\"";
		escapedForeignKey = CASE(forkey->name());
		escapeQuoteChar(escapedForeignKey,"\"");
        sql += escapedForeignKey;
        sql += "\" IS '";
        sql += escaped_forkey_comment;  
        sql += "'";
      }
    }
  }

  return sql;
}
/*===========================================================================*
 *     StudioTD_TableDef::genCreateXML
 *===========================================================================*/
StudioTD_String StudioTD_TableDef::genCreateXML()
{
  SAPDB_Bool               bKey = SAPDB_FALSE;
  StudioTD_String          s;
  StudioTD_String          k;
  char                     buf[256];

  StudioTD_ColumnDef      *col;
  StudioTD_ConstraintDef  *constr;
  StudioTD_ForeignKeyDef  *forkey;

  StudioTD_String::ConstIterator  iter;

  StudioTD_String deployedTable;
  StudioTD_String deployedTableUniqueKeys;
  StudioTD_String deployedTableForeignKeys;
  StudioTD_String deployedTableColumnConstraints;

  /*-------------------------------------------------------------------------*
   * Check if a owner and table name was given.                              *
   *-------------------------------------------------------------------------*/
  if (mName.Empty())
    return "";

  // dbm:DeployedTable
  deployedTable += "<sdbm:DeployedTable oim:id=\"_";
  deployedTable += string2XML(mOwner);
  deployedTable += ".";
  deployedTable += string2XML(mName);
  deployedTable += "\" oim:label=\"";
  deployedTable += string2XML(mName);
  deployedTable += "\" oim:name=\"";
  deployedTable += string2XML(mName);
  deployedTable += "\" dbm:comments=\"";
  deployedTable += string2XML(mComment);
  deployedTable += "\" sdbm:samplevalue=\"";
  if ((mSampleValue != 20000) || (mSampleUnit != "Rows"))
  {
      sprintf (buf, "%d ", mSampleValue );
      StudioTD_String sampleValue( buf );

      deployedTable += sampleValue;
      deployedTable += "\" sdbm:sampleunit=\"";
      deployedTable += UpperCase(mSampleUnit);
  }
  deployedTable += "\">\n";

  // dbm:ColumnSetColumns
  deployedTable += "<dbm:ColumnSetColumns>\n";
  deployedTable += "<uml:ClassifierFeature>\n";

  for (col = (StudioTD_ColumnDef *)mColumns.first(); col; col = (StudioTD_ColumnDef *)mColumns.next())
       deployedTable += col->genCreateXML ( caseSensitive ());

  deployedTable += "</uml:ClassifierFeature>\n";
  deployedTable += "</dbm:ColumnSetColumns>\n";

  // dbm:DeployedTableUniqueKeys
  for (col = (StudioTD_ColumnDef *)mColumns.first();col; col = (StudioTD_ColumnDef *)mColumns.next())
  {
      if (col->key())
      {
          if ( !bKey )
          {
              deployedTableUniqueKeys  = "<dbm:TableUniqueKeys>\n";
              deployedTableUniqueKeys += "<dbm:UniqueKey oim:id=\"_";
              deployedTableUniqueKeys += string2XML ( owner() );
              deployedTableUniqueKeys += ".";
              deployedTableUniqueKeys += string2XML ( name () );
              deployedTableUniqueKeys += ".";
              deployedTableUniqueKeys += string2XML ( col->name () );
              deployedTableUniqueKeys += "\" >";
              deployedTableUniqueKeys += "<dbm:KeyColumns>\n";
              deployedTableUniqueKeys += col->genCreateXML( caseSensitive ());
              bKey = SAPDB_TRUE;
          }
          else
              deployedTableUniqueKeys += col->genCreateXML( caseSensitive ());
      }
  }
  // dbm:ReferentialConstraint
  // dbm:UniqueKeyRole
  if ( bKey )
  {
       //deployedTableUniqueKeys += "</dbm:UniqueKeyRole>\n";  //!!
       deployedTableUniqueKeys += "</dbm:KeyColumns>\n";
       deployedTableUniqueKeys += "</dbm:UniqueKey>\n";
       deployedTableUniqueKeys += "</dbm:TableUniqueKeys>\n";
  }

  bKey = SAPDB_FALSE;

  // dbm:DeployedTableForeignKeys
  for (forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.first(); forkey; forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.next())
  {
      if ( !bKey )
      {
              deployedTableForeignKeys  = "<dbm:TableForeignKeys>\n";
              deployedTableForeignKeys += "<dbm:ForeignKey oim:id=\"_";
              deployedTableForeignKeys += string2XML ( owner() );
              deployedTableForeignKeys += ".";
              deployedTableForeignKeys += string2XML ( name () );
              deployedTableForeignKeys += ".";
              deployedTableForeignKeys += string2XML ( forkey->name () );
              deployedTableForeignKeys += "\" >";
              deployedTableForeignKeys += forkey->genCreateXML ( mOwner, mName, caseSensitive () );
              deployedTableForeignKeys += "</dbm:ForeignKey>\n";
              bKey = SAPDB_TRUE;
      }
      else
      {
              deployedTableForeignKeys += "<dbm:ForeignKey oim:id=\"_";
              deployedTableForeignKeys += owner();
              deployedTableForeignKeys += ".";
              deployedTableForeignKeys += name ();
              deployedTableForeignKeys += ".";
              deployedTableForeignKeys += forkey->name ();
              deployedTableForeignKeys += "\" >";
              deployedTableForeignKeys += forkey->genCreateXML ( mOwner, mName, caseSensitive () );
              deployedTableForeignKeys += "</dbm:ForeignKey>\n";
      }
  }
  // dbm:ReferentialConstraint
  // dbm:ForeignKeyRole
  if ( bKey )
       deployedTableForeignKeys += "</dbm:TableForeignKeys>\n";

  // dbm:DeployedTableColumnConstraints
  if ( NULL != (StudioTD_ConstraintDef *)mConstraints.first() )
  {
      deployedTableColumnConstraints  = "<dbm:TableColumnConstraints>\n";
      for (constr = (StudioTD_ConstraintDef *)mConstraints.first(); constr; constr = (StudioTD_ConstraintDef *)mConstraints.next())
      {
          deployedTableColumnConstraints += "<dbm:ColumnConstraint oim:id=\"_";
          deployedTableColumnConstraints += string2XML ( owner() );
          deployedTableColumnConstraints += ".";
          deployedTableColumnConstraints += string2XML ( name () );
          deployedTableColumnConstraints += ".";
          deployedTableColumnConstraints += string2XML ( constr->name () );
          deployedTableColumnConstraints += ".";
          deployedTableColumnConstraints += string2XML ( constr->name () );
          deployedTableColumnConstraints += "\" ";
          deployedTableColumnConstraints += constr->genCreateXML ( mMode, mCaseSensitive );
          deployedTableColumnConstraints += "</dbm:ColumnConstraint>";
      }
      deployedTableColumnConstraints += "</dbm:TableColumnConstraints>\n";
  }

  // dbm:DeployedTableConstraint
/*  if ( !constraintexpression().Empty() )
  {
       deployedTableConstraint  = "<dbm:TableConstraint oim:id=\"_";
       deployedTableConstraint += string2XML( owner() );
       deployedTableConstraint += ".";
       deployedTableConstraint += string2XML( name () );
       deployedTableConstraint += ".";
       deployedTableConstraint += string2XML( constraintname() );
       deployedTableConstraint += "\"";
       deployedTableConstraint += " bodyLanguage = \"&lt;constraint_definition&gt;::= see SAPDB reference manual \" ";
       deployedTableConstraint += " body = \" ";
       deployedTableConstraint += string2XML( constraintexpression() );
       deployedTableConstraint += "\">\n";
       deployedTableConstraint += "</dbm:TableConstraint>\n";
  }
*/
  deployedTable += deployedTableUniqueKeys;
  deployedTable += deployedTableForeignKeys;
  deployedTable += deployedTableColumnConstraints;
  //deployedTable += "</sdbm:DeployedTable>\n";       DeployedTableIndices
  //deployedTable += "</uml:NamespaceOwnedElement>\n";  DeployedTableIndices

  return deployedTable;

}
/*===========================================================================*
 *     StudioTD_TableDef::genAlterStatement                                  *
 *===========================================================================*/
StudioTD_String StudioTD_TableDef::genAlterStatement (StudioTD_TableDef *oldTable)
{
  StudioTD_String         sql;
  StudioTD_String         escapedTableName,escapedOwner,escapedColumnName,escapedConstraint,escapedForeignKey;
  SAPDB_Bool              casesensitive = mCaseSensitive;
  SAPDB_Bool              bFirst = true, bKeyChanged = false;
  SAPDB_UInt              nNewKeys, nOldKeys;
  StudioTD_String         intro, s ,keyTemp;
  StudioTD_String*        primaryKeys;
  StudioTD_ColumnDef      *oldCol;

  StudioTD_ColumnDef     *col;
  StudioTD_ConstraintDef *constr;
  StudioTD_ForeignKeyDef *forkey;

  StudioTD_String::ConstIterator  iter;

  char                     buf[256];

  /*-------------------------------------------------------------------------*
   * Check if a table name was given. If not --> return an empty string.     *
   *-------------------------------------------------------------------------*/
  if (mName.Empty())
    return "";

  /*-------------------------------------------------------------------------*
   * Generate the intro of the ALTER TABLE statement.                        *
   *-------------------------------------------------------------------------*/
  intro  = "ALTER TABLE \"";

  escapedOwner = mOwner;
  escapeQuoteChar(escapedOwner,"\"");
  if(mOwner != ""){
      intro += escapedOwner;
      intro += "\".\"";
  }

  escapedTableName = mName;
  escapeQuoteChar(escapedTableName,"\"");
  intro += escapedTableName;
  intro += "\" ";

  /*-------------------------------------------------------------------------*
   * For each item in the columns list that was changed, added or removed    *
   * add its part of the statement and update the primary key.               *
   *-------------------------------------------------------------------------*/
  for (col = (StudioTD_ColumnDef *)mColumns.first();
       col;
       col = (StudioTD_ColumnDef *)mColumns.next())
  {
    if (col->alterType() == AlterChange)
    {
      oldCol = oldTable->column (oldTable->findColumn (col->oldName()));
      sql += col->genAlterStatement (oldCol, mOwner,mName, mCaseSensitive);

      if (col->key() != oldCol->key())
        bKeyChanged = true;
    }
  }

  for (col = (StudioTD_ColumnDef *)mRemovedColumns.first();
       col;
       col = (StudioTD_ColumnDef *)mRemovedColumns.next())
  {
    if (oldTable->findColumn (col->oldName()) > -1)
    {
      if (bFirst)
      {
        sql += intro;
        sql += "DROP (";
      }

      sql += "\"";
	  escapedColumnName = CASE(col->name());
	  escapeQuoteChar(escapedColumnName,"\"");
      sql += escapedColumnName;
      sql += "\", ";
      bFirst = false;
    }
  }

  if (!bFirst)
  {
    iter = sql.End();
	iter -= 2;
    sql.Erase (iter, sql.End());
    sql += ") CASCADE";
    sql += SEP;
  }

  for (col = (StudioTD_ColumnDef *)mColumns.first();
       col;
       col = (StudioTD_ColumnDef *)mColumns.next())
  {
    if (col->alterType() == AlterAdd)
    {
      sql += col->genAlterStatement (NULL,mOwner, mName, mCaseSensitive);

      if (col->key())
        bKeyChanged = true;
    }
  }

  nNewKeys = keyCount();
  nOldKeys = oldTable->keyCount();

  if (bKeyChanged){

    if (nNewKeys == 0){

      sql += intro;
      sql += "DROP PRIMARY KEY";
      sql += SEP;
    }
    else{
      if (nOldKeys == 0){
        sql += intro;
        sql += "ADD PRIMARY KEY (";
      }
      else{
        sql += intro;
        sql += "ALTER PRIMARY KEY (";
      }

      primaryKeys = new StudioTD_String[nNewKeys];
      for (col = (StudioTD_ColumnDef *)mColumns.first();
           col;
           col = (StudioTD_ColumnDef *)mColumns.next())
      {
        if (col->key()){

          keyTemp = "\"";
          escapedColumnName = CASE(col->name());
		  escapeQuoteChar(escapedColumnName,"\"");
		  keyTemp += escapedColumnName;
          keyTemp += "\", ";
          primaryKeys[col->keyPos()-1] = keyTemp;
          
        }               
      }

      for(SAPDB_UInt i = 0; i < nNewKeys;i++){
          sql += primaryKeys[i];
      }

      //delete all elements
      if(primaryKeys != NULL)
        delete[] primaryKeys;
      
      iter = sql.End();
      iter -= 2;
      sql.Erase (iter, sql.End());
      sql += ")";
      sql += SEP;
    }
  }

  /*-------------------------------------------------------------------------*
   * For each item in the constraints list that was changed, added or        *
   * removed add its part of the statement.                                  *
   *-------------------------------------------------------------------------*/
  for (constr = (StudioTD_ConstraintDef *)mConstraints.first();
       constr;
       constr = (StudioTD_ConstraintDef *)mConstraints.next())
  {
    if (constr->alterType() == AlterChange)
    {
      sql += intro;
      sql += "ALTER CONSTRAINT \"";
      escapedConstraint = CASE(constr->name());
	  escapeQuoteChar(escapedConstraint,"\"");
	  sql += escapedConstraint;
      sql += "\" CHECK ";
      sql += constr->genAlterStatement (mCaseSensitive);
      sql += SEP;
    }
  }

  for (constr = (StudioTD_ConstraintDef *)mConstraints.first();
       constr;
       constr = (StudioTD_ConstraintDef *)mConstraints.next())
  {
    if (constr->alterType() == AlterAdd)
    {
      s = constr->genAlterStatement (mCaseSensitive);

      if (!s.Empty())
      {
        if (!constr->name().Empty())
        {
          sql += intro;
          sql += "ADD CONSTRAINT \"";
		  escapedConstraint = CASE(constr->name());
	      escapeQuoteChar(escapedConstraint,"\"");
          sql += escapedConstraint;
          sql += "\" CHECK ";
        }
        else
        {
          sql += intro;
          sql += "ADD CONSTRAINT ";
        }
        sql += s;
        sql += SEP;
      }
    }
  }

  for (constr = (StudioTD_ConstraintDef *)mRemovedConstraints.first();
       constr;
       constr = (StudioTD_ConstraintDef *)mRemovedConstraints.next())
  {
    if (oldTable->findConstraint (constr->name()) > -1)
    {
      sql += intro;
      sql += "DROP CONSTRAINT \"";
	  
	  escapedConstraint = CASE(constr->name());
	  escapeQuoteChar(escapedConstraint,"\"");
      sql += escapedConstraint;
      sql += "\"";
      sql += SEP;
    }
  }

  /*-------------------------------------------------------------------------*
   * For each item in the foreign keys list that was changed, added or       *
   * removed add its part of the statement.                                  *
   *-------------------------------------------------------------------------*/
  for (forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.first();
       forkey;
       forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.next())
  {
    if ((forkey->alterType() == AlterChange) || (forkey->alterType() == AlterAdd))
      sql += forkey->genAlterStatement (mOwner,mName, mCaseSensitive);
  }

  for (forkey = (StudioTD_ForeignKeyDef *)mRemovedForeignKeys.first();
       forkey;
       forkey = (StudioTD_ForeignKeyDef *)mRemovedForeignKeys.next())
  {
    if (oldTable->findForeignKey (forkey->name()) > -1)
    {
      sql += intro;
      sql += "DROP FOREIGN KEY \"";

	  escapedForeignKey = CASE(forkey->name());
	  escapeQuoteChar(escapedForeignKey,"\"");
	  sql += escapedForeignKey;
      sql += "\"";
      sql += SEP;
    }
  }

  /*-------------------------------------------------------------------------*
   * Alter misc table attributes.                                           *
   *-------------------------------------------------------------------------*/
  if ((mSampleValue != oldTable->sampleValue()) || (mSampleUnit != oldTable->sampleUnit()))
  {
    sprintf (buf, StudioTD_NEW_LINE"SAMPLE %d %s", mSampleValue, UpperCase(mSampleUnit).StrPtr());
    sql += intro;
    sql += buf;
    sql += SEP;
  }

  /*-------------------------------------------------------------------------*
   * Finally, append the comments.                                           *
   *-------------------------------------------------------------------------*/
  if (mComment != oldTable->comment())
  {
    StudioTD_String  escaped_Comment = mComment;

    escapeQuoteChar(escaped_Comment, "'");

    sql += "COMMENT ON TABLE \"";
    if(escapedOwner != ""){
      sql += escapedOwner;
      sql += "\".\"";
    }
    sql += escapedTableName;
    sql += "\" IS '";
    sql += escaped_Comment; 
    sql += "'";
    sql += SEP;
  }


  if (sql.Length() > 0)
  {
    iter = sql.End();
    iter -= (2 + 2*StudioTD_NEW_LINE_LEN);
    sql.Erase (iter, sql.End());
  }

  return sql;
}


/*===========================================================================*
 *     StudioTD_TableDef::genDescription                                     *
 *===========================================================================*/
StudioTD_String StudioTD_TableDef::genDescription ()
{
  StudioTD_String        temp;
  StudioTD_String         desc;
  StudioTD_ColumnDef     *col;
  StudioTD_ConstraintDef *constr;
  StudioTD_ForeignKeyDef *forkey;
  
  /*-------------------------------------------------------------------------*
   * Add the table attributs to the description.                             *
   *-------------------------------------------------------------------------*/
  desc  = "table:::";
  desc += mName;
  desc += SEP_LEVEL_2;
  desc += mBasedOn;
  desc += SEP_LEVEL_2;
  desc += mCreateAs;
  desc += SEP_LEVEL_2;
  desc += mComment;
  desc += SEP_LEVEL_2;
  temp.ConvertFromInt(mSampleValue);
  desc += temp;
  desc += SEP_LEVEL_2;
  desc += mSampleUnit;
  desc += SEP_LEVEL_2;
  desc += mDuplicates;
  desc += SEP_LEVEL_2;
  desc += (mTemporary      ? "true" : "false");
  desc += SEP_LEVEL_2;
  desc += (mIgnoreRollback ? "true" : "false");
  desc += SEP_LEVEL_2;
  desc += (mCaseSensitive  ? "true" : "false");
  desc += SEP_LEVEL_2;  
  desc += mOwner;
  desc += SEP_LEVEL_2;
  desc += SEP_LEVEL_1;

  /*-------------------------------------------------------------------------*
   * For each item in the columns list add its part of the description.      *
   *-------------------------------------------------------------------------*/
  desc += "columns:::";
  for (col = (StudioTD_ColumnDef *)mColumns.first();
       col;
       col = (StudioTD_ColumnDef *)mColumns.next())
  {
    desc += col->genDescription();
    desc += SEP_LEVEL_2;
  }

  desc += SEP_LEVEL_1;

  /*-------------------------------------------------------------------------*
   * For each item in the constraints list add its part of the description.  *
   *-------------------------------------------------------------------------*/
  desc += "constraints:::";
  for (constr = (StudioTD_ConstraintDef *)mConstraints.first();
       constr;
       constr = (StudioTD_ConstraintDef *)mConstraints.next())
  {
    desc += constr->genDescription();
    desc += SEP_LEVEL_2;
  }

  desc += SEP_LEVEL_1;

  /*-------------------------------------------------------------------------*
   * For each item in the foreign keys list add its part of the description. *
   *-------------------------------------------------------------------------*/
  desc += "foreignkeys:::";
  for (forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.first();
       forkey;
       forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.next())
  {
    desc += forkey->genDescription();
    desc += SEP_LEVEL_2;
  }

  desc += SEP_LEVEL_1;

  desc += "mode:::";
  temp.ConvertFromInt(mMode);
  desc += temp;
  desc += SEP_LEVEL_2;
  desc += SEP_LEVEL_1;

  return desc;
}


/*===========================================================================*
 *     StudioTD_TableDef::parseDescription                                   *
 *===========================================================================*/
void StudioTD_TableDef::parseDescription (StudioTD_String description)
{
  StudioTD_String            str;
  SAPDB_UInt                 start = 0, end, s, e;
  StudioTD_ColumnDef        *col;
  StudioTD_ConstraintDef    *constr;
  StudioTD_ForeignKeyDef    *forkey;

  /*-------------------------------------------------------------------------*
   * Reset the table definition to its initial values.                       *
   *-------------------------------------------------------------------------*/
  reset();

  /*-------------------------------------------------------------------------*
   * Split the description into its parts (separator SEP_LEVEL_1).           *
   *-------------------------------------------------------------------------*/
  while ((end = description.Find (SEP_LEVEL_1, start)) != StudioTD_String::NPos)
  {
    str = description.SubStrBasis (start, end);

    start = end + 1;

    /*-----------------------------------------------------------------------*
     * Split the table description into its parts (separator SEP_LEVEL_2).   *
     *-----------------------------------------------------------------------*/
    if (str.SubStr (0, 8) == "table:::")
    {
      s = 8;
      mName           = NextToken (str, SEP_LEVEL_2, &s);
      mBasedOn        = NextToken (str, SEP_LEVEL_2, &s);
      mCreateAs       = NextToken (str, SEP_LEVEL_2, &s);
      mComment        = NextToken (str, SEP_LEVEL_2, &s);
      mSampleValue    = AtoI (NextToken (str, SEP_LEVEL_2, &s));
      mSampleUnit     = NextToken (str, SEP_LEVEL_2, &s);
      mDuplicates     = NextToken (str, SEP_LEVEL_2, &s);
      mTemporary      = (NextToken (str, SEP_LEVEL_2, &s) == "true") ? true : false;
      mIgnoreRollback = (NextToken (str, SEP_LEVEL_2, &s) == "true") ? true : false;
      mCaseSensitive  = (NextToken (str, SEP_LEVEL_2, &s) == "true") ? true : false;
      mOwner          = NextToken (str, SEP_LEVEL_2, &s);

      continue;
    }

    /*-----------------------------------------------------------------------*
     * Split the column description into its parts (separator SEP_LEVEL_2).  *
     *-----------------------------------------------------------------------*/
    if (str.SubStr (0, 10) == "columns:::")
    {
      s = 10;
      while ((e = str.Find (SEP_LEVEL_2, s)) != StudioTD_String::NPos)
      {
        col = addColumn();
        col->parseDescription (str.SubStrBasis (s, e));
        s = e + 1;
      }

      continue;
    }

    /*-----------------------------------------------------------------------*
     * Split the constraint description into its parts (sep. SEP_LEVEL_2).   *
     *-----------------------------------------------------------------------*/
    if (str.SubStr (0, 14) == "constraints:::")
    {
      s = 14;
      while ((e = str.Find (SEP_LEVEL_2, s)) != StudioTD_String::NPos)
      {
        constr = addConstraint();
        constr->parseDescription (str.SubStrBasis (s, e));
        s = e + 1;
      }

      continue;
    }

    /*-----------------------------------------------------------------------*
     * Split the foreign key description into its parts (sep. SEP_LEVEL_2).  *
     *-----------------------------------------------------------------------*/
    if (str.SubStr (0, 14) == "foreignkeys:::")
    {
      s = 14;
      while ((e = str.Find (SEP_LEVEL_2, s)) != StudioTD_String::NPos)
      {
        forkey = addForeignKey();
        forkey->parseDescription (str.SubStrBasis(s, e));
        s = e + 1;
      }

      continue;
    }
    /*-----------------------------------------------------------------------*
     * Split the mode of the table definition dialog                         *  
     *-----------------------------------------------------------------------*/
    if (str.SubStr (0, 7) == "mode:::")
    {
      s = 7;
      mMode = (_studiotd_mode)AtoI (NextToken (str, SEP_LEVEL_2, &s));
    
      continue; 
    }

  }

}


/*===========================================================================*
 *     StudioTD_TableDef::keyCount                                           *
 *===========================================================================*/
SAPDB_UInt StudioTD_TableDef::keyCount ()
{
  StudioTD_ColumnDef  *col;
  SAPDB_UInt           count = 0;

  for (col = (StudioTD_ColumnDef *)mColumns.first(); col; col = (StudioTD_ColumnDef *)mColumns.next())
  {
    if (col->key())
      count++;
  }

  return count;
}


/*===========================================================================*
 *     StudioTD_TableDef::defaultCount                                       *
 *===========================================================================*/
SAPDB_UInt4 StudioTD_TableDef::defaultCount()
{
    StudioTD_ColumnDef  *pCol;
    SAPDB_UInt4          count = 0;

    for (pCol = (StudioTD_ColumnDef *)mColumns.first();
         pCol;
         pCol = (StudioTD_ColumnDef *)mColumns.next())
    {
        if (SAPDB_FALSE == pCol->defaultValue().Empty())
        {
            ++count;
        }
    }
    return count;
}
// defaultCount()