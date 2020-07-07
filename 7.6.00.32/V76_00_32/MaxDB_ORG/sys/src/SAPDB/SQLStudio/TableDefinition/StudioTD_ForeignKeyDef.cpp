/*!===========================================================================

  module:       StudioTD_ForeignKeyDef.cpp

  ----------------------------------------------------------------------------

  responsible:  MarkusS,WolfgangA,Hans-GeorgB

  special area: SQL Studio

  description:  Implementation of the foreign key definition class StudioTD_ForeignKeyDef.

  see:          www.sapdb.org

  version:      7.4

  created:      2000-05-12

  updated:      2001-02-12

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
#include "SQLStudio/TableDefinition/StudioTD_ForeignKeyDef.h"


/*===========================================================================*
 *     StudioTD_ForeignKeyDef::StudioTD_ForeignKeyDef                                *
 *===========================================================================*/
StudioTD_ForeignKeyDef::StudioTD_ForeignKeyDef ()
    : mName(""), mOnDelete("RESTRICT"), mAlterType(AlterNoChange),mSep(true)
{
  setType (ClassForeignKeyDef);
}

/*===========================================================================*
 *     StudioTD_ForeignKeyDef::~StudioTD_ForeignKeyDef                               *
 *===========================================================================*/
StudioTD_ForeignKeyDef::~StudioTD_ForeignKeyDef()
{
}


/*===========================================================================*
 *     StudioTD_ForeignKeyDef::findIncomplete                                    *
 *===========================================================================*/
StudioTD_String StudioTD_ForeignKeyDef::findIncomplete ()
{
  /*-------------------------------------------------------------------------*
   * If all field have their initial values the definition is totally empty  *
   * --> it will be ignored.                                                 *
   *-------------------------------------------------------------------------*/
  if (   (mName.Empty()) && mRefColumns.Empty() && mReference.Empty()
      && (mOnDelete == "RESTRICT") && mComment.Empty())
    return "";

  /*-------------------------------------------------------------------------*
   * If referencing columns or reference is empty its index is returned.     *
   *-------------------------------------------------------------------------*/
  if (mRefColumns.Empty())
    return "1";

  if (mReference.Empty())
    return "2";

  return "";
}


/*===========================================================================*
 *     StudioTD_ForeignKeyDef::genCreateStatement                                *
 *===========================================================================*/
StudioTD_String StudioTD_ForeignKeyDef::genCreateStatement (StudioTD_Mode mode, SAPDB_Bool casesensitive)
{
  StudioTD_String  sql;
  StudioTD_String  escapedName,escapedColumnName,escapedReference;
  SAPDB_Int        start, end;
  
  /*-------------------------------------------------------------------------*
   * If referencing columns or reference is empty the foreign key is not     *
   * taken into account.                                                     *
   *-------------------------------------------------------------------------*/
  if (mRefColumns.Empty() || mReference.Empty())
    return "";

  /*-------------------------------------------------------------------------*
   * Generate the intro of the FOREIGN KEY clause.                           *
   *-------------------------------------------------------------------------*/
  sql += "\tFOREIGN KEY ";
  
  /*-------------------------------------------------------------------------*
   * Insert the foreign key name.                                            *
   *-------------------------------------------------------------------------*/
  if (!mName.Empty())
  {
    sql += "\"";
	escapedName = CASE(mName);
	escapeQuoteChar(escapedName,"\"");
    sql += escapedName;
    sql += "\"\t";
  }
  else
    sql += "\t\t";

  /*-------------------------------------------------------------------------*
   * Insert the referencing columns.                                         *
   *-------------------------------------------------------------------------*/
  sql += "(";
  
  start = 0;
  end   = mRefColumns.Find (",", start);
  
  while (end != StudioTD_String::NPos)
  {
    sql  += "\"";
	escapedColumnName = CASE(mRefColumns.SubStrBasis (start, end));
	escapeQuoteChar(escapedColumnName,"\"");
    sql  += escapedColumnName;
    sql  += "\",";
    start = end + 1;
    end   = mRefColumns.Find (",", start);
  }
  
  sql += "\"";
  escapedColumnName = CASE(mRefColumns.SubStr (start));
  escapeQuoteChar(escapedColumnName,"\"");
  sql  += escapedColumnName;
  sql += "\")";

  /*-------------------------------------------------------------------------*
   * Insert the reference.                                                   *
   *-------------------------------------------------------------------------*/
  sql += " REFERENCES ";
  
  start = mReference.Find (".");
  end   = mReference.Find ("(");
  
  if (StudioTD_String::NPos != start)       // no owner name in reference
  {
    sql += "\"";
	escapedReference = mReference.SubStrBasis (0, start);
    escapeQuoteChar(escapedReference,"\"");
    sql  += escapedReference;
	sql += "\".";
  }
  else
  {
    start = -1;
  }
  sql += "\"";
  escapedReference = mReference.SubStrBasis (start + 1, end - 1);
  escapeQuoteChar(escapedReference,"\"");
  sql  += escapedReference;
  sql += "\" (";
  
  start = end + 1;
  end   = mReference.Find (",", start);
  
  while (end != StudioTD_String::NPos)
  {
    sql  += "\"";
	escapedColumnName = mReference.SubStrBasis (start, end);
	escapeQuoteChar(escapedColumnName,"\"");
    sql  += escapedColumnName;
    sql  += "\",";
    start = end + 1;
    end   = mReference.Find (",", start);
  }

  sql += "\"";
  escapedColumnName = mReference.SubStrBasis (start, mReference.Length() - 1);
  escapeQuoteChar(escapedColumnName,"\"");
  sql  += escapedColumnName;
  sql += "\")";

  /*-------------------------------------------------------------------------*
   * Insert the ON DELETE clause.                                            *
   *-------------------------------------------------------------------------*/
  if (!mOnDelete.Empty())
  {
    sql += " ON DELETE ";
    sql += mOnDelete;
  }
  
  sql += ","StudioTD_NEW_LINE;

  return sql;
}
/*===========================================================================*
 *     StudioTD_ForeignKeyDef::genCreateXML
 *===========================================================================*/
StudioTD_String StudioTD_ForeignKeyDef::genCreateXML( StudioTD_String owner,StudioTD_String table, SAPDB_Bool casesensitive)
{
   StudioTD_String   foreignKey,sql;
   StudioTD_String   owner_referenced , tablename_referenced , id_referenced;
   StudioTD_String   owner_referencing, tablename_referencing, id_referencing;

   SAPDB_Int        start_referencing, end_referencing;
   SAPDB_Int        start_referenced , end_referenced;

   // comment();

   // <referential_constraint_definition> ::=
   // FOREIGN KEY [<referential_constraint_name>] (<referencing_column>,...)
   // REFERENCES <referenced_table> [(<referenced_column>,...)] [<delete_rule>]

   // <referencing_column>  mRefColumns
   // <referenced_column>   mReference

  /*-------------------------------------------------------------------------*
   * If referencing columns or referenced columns are empty the foreign key  *
   * is not taken into account.                                              *
   *-------------------------------------------------------------------------*/
  if ( mRefColumns.Empty() || mReference.Empty()  )
       return "";

  /*-------------------------------------------------------------------------*
   * Insert the referencing column.                                          *
   *-------------------------------------------------------------------------*/
    
  start_referencing = 0;
  end_referencing   = mRefColumns.Find (",", start_referencing);

  // id of referencing table
  owner_referencing      = owner;
  tablename_referencing  = table;
  
  id_referencing         = owner;
  id_referencing        += ".";
  id_referencing        += table;

 /*-------------------------------------------------------------------------*
  * Insert the referenced column                                            *
  *-------------------------------------------------------------------------*/
 
  start_referenced = mReference.Find (".");
  end_referenced   = mReference.Find ("(");

  // id of referenced table

  // owner name in reference
  if (StudioTD_String::NPos != start_referenced)   
  {
      owner_referenced = mReference.SubStrBasis (0, start_referenced);
	  id_referenced    = owner_referenced;
      id_referenced   += ".";
  }
  else
      start_referenced = -1;

  tablename_referenced  = mReference.SubStrBasis (start_referenced + 1, end_referenced - 1);

  id_referenced        += tablename_referenced;

  start_referenced = end_referenced + 1;
  end_referenced   = mReference.Find (",", start_referenced);

  foreignKey       = "<dbm:KeyColumns>\n";
  
  while (end_referencing != StudioTD_String::NPos )
  {
       /*-------------------------------------------------------------------------*
        * Insert the referencing column.                                          *
        *-------------------------------------------------------------------------*/
        foreignKey       += "<dbm:DeployedColumn oim:id=\"_";
        foreignKey       += id_referencing;
        foreignKey       += ".";
        foreignKey       += mName;
        foreignKey       += ".";
        foreignKey       += CASE(mRefColumns.SubStrBasis (start_referencing, end_referencing));

        start_referencing = end_referencing + 1;
        end_referencing   = mRefColumns.Find (",", start_referencing);

       /*-------------------------------------------------------------------------*
        * Insert the referenced column                                            *
        *-------------------------------------------------------------------------*/
        foreignKey       += "\"  oim:href=\"#_";
        foreignKey       += id_referenced;
        foreignKey       += ".";
        foreignKey       += mName;
        foreignKey       += ".";
        foreignKey       += mReference.SubStrBasis (start_referenced, end_referenced);
        foreignKey       += "\"/>\n";

        start_referenced  = end_referenced  + 1;
        end_referenced    = mReference.Find  (",", start_referenced );
  }

  /*-------------------------------------------------------------------------*
   * Insert the referencing column.                                          *
   *-------------------------------------------------------------------------*/
   foreignKey += "<dbm:DeployedColumn oim:id=\"_";
   foreignKey += id_referencing;
   foreignKey += ".";
   foreignKey += mName;
   foreignKey += ".";
   foreignKey += CASE(mRefColumns.SubStr (start_referencing));

  /*-------------------------------------------------------------------------*
   * Insert the referenced columns                                           *
   *-------------------------------------------------------------------------*/
   foreignKey += "\"  oim:href=\"#_";
   foreignKey += id_referenced;
   foreignKey += ".";
   foreignKey += mName;
   foreignKey += ".";
   foreignKey += mReference.SubStrBasis (start_referenced, mReference.Length() - 1);
   foreignKey += "\"/>\n";

   foreignKey += "</dbm:KeyColumns>\n";

  /*-------------------------------------------------------------------------*
   * Insert the referential constraint                                       *
   *-------------------------------------------------------------------------*/

   // dbm:ReferentialConstraint
   // dbm:ForeignKeyRole
   if (!mOnDelete.Empty())
   {
       foreignKey += "<dbm:ForeignKeyRole oim:id=\"_SAPDBREFCONSTRAINT.";
       foreignKey += id_referencing;
       foreignKey += ".";
       foreignKey += mName;
       foreignKey += "\" DeleteRule = \"ON DELETE ";
       foreignKey += mOnDelete;
       foreignKey += "\" isSpecification =\"1\"";
       foreignKey += "/>\n";
   }
   return foreignKey;
}
/*===========================================================================*
 *     StudioTD_ForeignKeyDef::genAlterStatement                                 *
 *===========================================================================*/
StudioTD_String StudioTD_ForeignKeyDef::genAlterStatement (StudioTD_String owner,StudioTD_String table, SAPDB_Bool casesensitive)
{
  StudioTD_String   sql;
  StudioTD_String   escapedTable,escapedOwner,escapedName,escapedColumnName,escapeReference;
  SAPDB_UInt        start, end;
  StudioTD_String   intro;


  StudioTD_String  escaped_Comment = mComment;

  escapeQuoteChar(escaped_Comment,"'");

  intro  = "ALTER TABLE \""; 
  if(owner != "")
  {
      escapedOwner = owner;
	  escapeQuoteChar(escapedOwner,"\"");
	  intro += escapedOwner;
      intro += "\".\"";
  }

  escapedTable = CASE(table);
  escapeQuoteChar(escapedTable,"\"");
  intro += escapedTable;
  intro += "\" ";

  
  /*-------------------------------------------------------------------------*
   * If referencing columns or reference is empty the foreign key is not     *
   * taken into account.                                                     *
   *-------------------------------------------------------------------------*/
  if (mRefColumns.Empty() || mReference.Empty())
    return "";

  /*-------------------------------------------------------------------------*
   * Generate the intro of the FOREIGN KEY clause.                           *
   *-------------------------------------------------------------------------*/
  sql  = intro;
  sql += "FOREIGN KEY ";
  
  /*-------------------------------------------------------------------------*
   * Insert the foreign key name.                                            *
   *-------------------------------------------------------------------------*/
  if (!mName.Empty())
  {
    sql += "\"";
	escapedName = CASE(mName);
	escapeQuoteChar(escapedName,"\"");
    sql += escapedName;
    sql += "\" ";
  }

  /*-------------------------------------------------------------------------*
   * Insert the referencing columns.                                         *
   *-------------------------------------------------------------------------*/
  sql += "(";
  
  start = 0;
  end   = mRefColumns.Find (",", start);
  
  while (end != StudioTD_String::NPos)
  {
    sql  += "\"";    
	escapedColumnName = CASE(mRefColumns.SubStrBasis (start, end));
	escapeQuoteChar(escapedColumnName,"\"");
	sql  += escapedColumnName;
    sql  += "\",";
    start = end + 1;
    end   = mRefColumns.Find (",", start);
  }
  
  sql += "\"";
  escapedColumnName = CASE(mRefColumns.SubStr (start));
  escapeQuoteChar(escapedColumnName,"\"");
  sql += escapedColumnName;
  sql += "\")";

  /*-------------------------------------------------------------------------*
   * Insert the reference.                                                   *
   *-------------------------------------------------------------------------*/
  sql += " REFERENCES ";
  
  start = mReference.Find (".");
  end   = mReference.Find ("(");
  
  if ( start != StudioTD_String::NPos)
  {
	   sql += "\"";
	   escapeReference = mReference.SubStrBasis (0, start);
	   escapeQuoteChar(escapeReference,"\"");
	   sql += escapeReference;
	   sql += "\".\"";
	   escapeReference = mReference.SubStrBasis (start + 1, end - 1);
	   escapeQuoteChar(escapeReference,"\"");
	   sql += escapeReference;
	   sql += "\" (";
  }
  else 
  {
	   sql += "\"";
	   escapeReference = mReference.SubStrBasis (0, end - 1);
	   escapeQuoteChar(escapeReference,"\"");
	   sql += escapeReference;
	   sql  += "\" (";
  }
  
  start = end + 1;
  end   = mReference.Find (",", start);
  
  while (end != StudioTD_String::NPos)
  {
    sql  += "\"";
    escapedColumnName = mReference.SubStrBasis (start, end);
	escapeQuoteChar(escapedColumnName,"\"");
	sql  += escapedColumnName;
    sql  += "\",";
    start = end + 1;
    end   = mReference.Find (",", start);
  }

  sql += "\"";
  escapedColumnName = mReference.SubStrBasis (start, mReference.Length() - 1);
  escapeQuoteChar(escapedColumnName,"\"");
  sql += escapedColumnName;
  sql += "\")";

  /*-------------------------------------------------------------------------*
   * Insert the ON DELETE clause.                                            *
   *-------------------------------------------------------------------------*/
  if (!mOnDelete.Empty())
  {
    sql += " ON DELETE ";
    sql += mOnDelete;
  }
  
  if (isSep() == true)
      sql += SEP; 
  
  /*-------------------------------------------------------------------------*
   * Insert the foreign key name.                                            *
   *-------------------------------------------------------------------------*/
  if ((!mName.Empty()) && (!mComment.Empty()))
  {
    sql += "COMMENT ON FOREIGN KEY \"";
    sql += escapedTable;
    sql += "\".\"";
    sql += CASE(escapedName);
    sql += "\" IS '";
    sql += escaped_Comment; 
    sql += "'";
    sql += SEP;
  }
  
  return sql;
  
}


/*===========================================================================*
 *     StudioTD_ForeignKeyDef::genDescription                                    *
 *===========================================================================*/
StudioTD_String StudioTD_ForeignKeyDef::genDescription ()
{
  StudioTD_String  desc;

  desc  = mName;
  desc += SEP_LEVEL_3;
  desc += mRefColumns;
  desc += SEP_LEVEL_3;
  desc += mReference;
  desc += SEP_LEVEL_3;
  desc += mOnDelete;
  desc += SEP_LEVEL_3;
  desc += mComment;
  desc += SEP_LEVEL_3;

  return desc;
}


/*===========================================================================*
 *     StudioTD_ForeignKeyDef::parseDescription                                  *
 *===========================================================================*/
void StudioTD_ForeignKeyDef::parseDescription (StudioTD_String description)
{
  SAPDB_UInt  pos = 0;
  
  mName         = NextToken (description, SEP_LEVEL_3, &pos);
  mRefColumns   = NextToken (description, SEP_LEVEL_3, &pos);
  mReference    = NextToken (description, SEP_LEVEL_3, &pos);
  mOnDelete     = NextToken (description, SEP_LEVEL_3, &pos);
  mComment      = NextToken (description, SEP_LEVEL_3, &pos);
}
