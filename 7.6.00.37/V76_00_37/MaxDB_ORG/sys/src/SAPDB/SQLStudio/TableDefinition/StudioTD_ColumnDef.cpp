/*!===========================================================================

  module:       StudioTD_ColumnDef.cpp

  ----------------------------------------------------------------------------

  responsible:  MarkusS,WolfgangA,Hans-GeorgB

  special area: SQL Studio

  description:  Implementation of the column definition class StudioTD_ColumnDef.

  see:          www.sapdb.org

  version:      7.4  

  created:      2000-05-11

  updated:      2000-05-11
                2003-02-28
                2003-03-31

  ----------------------------------------------------------------------------


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
#include "SQLStudio/TableDefinition/StudioTD_ColumnDef.h"


/*===========================================================================*
 *     StudioTD_ColumnDef::StudioTD_ColumnDef                                *
 *===========================================================================*/
StudioTD_ColumnDef::StudioTD_ColumnDef ()
    : mKey(false), mNotNull(false), mUnique(false), mAlterType(AlterNoChange),mKeyPos(0)
{
  setType (ClassColumnDef);
}

/*===========================================================================*
 *     StudioTD_ColumnDef::~StudioTD_ColumnDef                               *
 *===========================================================================*/
StudioTD_ColumnDef::~StudioTD_ColumnDef()
{
}


/*===========================================================================*
 *     StudioTD_ColumnDef::findIncomplete                                    *
 *===========================================================================*/
StudioTD_String StudioTD_ColumnDef::findIncomplete ()
{
  /*-------------------------------------------------------------------------*
   * If all field have their initial values the definition is totally empty  *
   * --> it will be ignored.                                                 *
   *-------------------------------------------------------------------------*/
  if (   mName.Empty() && mDataType.Empty() && mDimension.Empty()
      && mCodeType.Empty() && !mKey && !mNotNull && mDefault.Empty()
      && mComment.Empty())
    return "";

  /*-------------------------------------------------------------------------*
   * If name or datatype is empty its index is returned.                     *
   *-------------------------------------------------------------------------*/
  if (mName.Empty())
    return "0";

  if (mDataType.Empty())
    return "1";

  return "";
}


/*===========================================================================*
 *     StudioTD_ColumnDef::genCreateStatement                                *
 *===========================================================================*/
StudioTD_String
StudioTD_ColumnDef::genCreateStatement (StudioTD_Mode mode, SAPDB_Bool casesensitive)
{
    StudioTD_String  sql;
	StudioTD_String  escapedName;
    StudioTD_String  attributes;
    StudioTD_String  delimiter;

    StudioTD_String  escaped_Default = mDefault;

    escapeQuoteChar(escaped_Default, "'");

    /*-------------------------------------------------------------------------*
     * If name or datatype is empty the column is not taken into account.      *
     *-------------------------------------------------------------------------*/
    if (mName.Empty() || mDataType.Empty())
    {
        return "";
    }

    /*-------------------------------------------------------------------------*
     * Insert a TAB and the name of the column.                                *
     *-------------------------------------------------------------------------*/
    sql += "\t\"";
	escapedName = CASE(mName);
	escapeQuoteChar(escapedName,"\"");
    sql += escapedName;
    sql += "\"";

    /*-------------------------------------------------------------------------*
     * If a query was used as a template, that's all --> return.               *
     *-------------------------------------------------------------------------*/
    if (mode == CreateFromQuery)
    {
        sql += ","StudioTD_NEW_LINE;
        return sql;
    }
  
    /*-------------------------------------------------------------------------*
     * Insert the datatype fo the column.                                      *
     *-------------------------------------------------------------------------*/
    sql += "  ";
    sql += mDataType;

    /*-------------------------------------------------------------------------*
     * Depending on the datatype the dimension and codetype is inserted.       *
     *-------------------------------------------------------------------------*/
    if ((mDataType == "Char") || (mDataType == "Varchar"))
    {
        if (!mDimension.Empty())
        {
            sql += " (";
            sql += mDimension;
            sql += ")";
        }
    
        if (!mCodeType.Empty())
        {
            sql += " ";
            sql += mCodeType;
        }
    }
  
    if ((mDataType == "Fixed") || (mDataType == "Float"))
    {
        if (!mDimension.Empty())
        {
            sql += " (";
            sql += mDimension;
            sql += ")";
        }
    }
  
    if ((mDataType == "Long") || (mDataType == "Longfile"))
    {
        if (!mCodeType.Empty())
        {
            sql += " ";
            sql += mCodeType;
        }
    }

    /*-------------------------------------------------------------------------*
     * Insert the NOT NULL clause.                                             *
     *-------------------------------------------------------------------------*/
    if (mNotNull)
    {
        attributes += "  NOT NULL";
    }

    /*-------------------------------------------------------------------------*
     * Insert the default value.                                               *
     *-------------------------------------------------------------------------*/
    if (!mDefault.Empty())      // be sure to handle only not empty default values (don't do too much)
    {
        if ( (SAPDB_TRUE == mbUserDefinedDefault) &&
             ( (mDataType == "Char")    || 
               (mDataType == "Varchar") ||
               (mDataType == "Long")    ||
               (mDataType == "Date")    ||
               (mDataType == "Time")    ||
               (mDataType == "Timestamp" ) ) )
        {
            delimiter = "'";
        }

        attributes += "  DEFAULT ";
        attributes += ( ( (mCodeType == "BYTE") && (SAPDB_TRUE == mbUserDefinedDefault) ) ? "x" : "" );
        attributes += delimiter;
        attributes += escaped_Default;
        attributes += delimiter;

    }   // end if (!mDefault.Empty())
  
    /*-------------------------------------------------------------------------*
     * Append the column attributes to the statement.                          *
     *-------------------------------------------------------------------------*/
    if (!attributes.Empty())
    {
        sql += attributes;
    }
    sql += ","StudioTD_NEW_LINE;

    /*-------------------------------------------------------------------------*
     * Insert the unique clause.                                               *
     *-------------------------------------------------------------------------*/
    if (mUnique)
    {
        sql += "\tUNIQUE(";
        sql += mName;
        sql += ")";
        sql += ","StudioTD_NEW_LINE;
    }

    return sql;
}

/*===========================================================================*
 *     StudioTD_ColumnDef::genCreateXML
 *===========================================================================*/
StudioTD_String StudioTD_ColumnDef::genCreateXML( SAPDB_Bool casesensitive )
{

  StudioTD_String  attributes;
  StudioTD_String  delimiter;

  StudioTD_String  deployedcolumn;
  
  /*-------------------------------------------------------------------------*
   * Insert the name of the column.                              
   *-------------------------------------------------------------------------*/

   // SQLDescribeCol : column name, type, column size, decimal digits, and nullability
   // SQLColAttribute: all see sysodbctypes

   // dbm:DeployedColumn 
   deployedcolumn += "<dbm:DeployedColumn oim:id=\"_"; 
   deployedcolumn += string2XML(mTableOwner);
   deployedcolumn += ".";
   deployedcolumn += string2XML(mTableName );
   deployedcolumn += ".";
   deployedcolumn += string2XML(CASE(mName));
   deployedcolumn += "\" oim:label=\"";
   deployedcolumn += string2XML(CASE(mName));
   deployedcolumn += "\" oim:name=\"";
   deployedcolumn += string2XML(CASE(mName));
   deployedcolumn += "\" dbm:comments=\""; 
   deployedcolumn += string2XML(mComment);
   deployedcolumn += "\" dbm:IdentityIncrement=\"";
   deployedcolumn += "";
   deployedcolumn += "\" dbm:initialValue=\"";
   deployedcolumn += mDefault;
   deployedcolumn += "\" dbm:initialValueLanguage=\"";
   deployedcolumn += "";
   deployedcolumn += "\" dbm:IsConstant=\"";
   deployedcolumn += "";
   deployedcolumn += "\" dbm:IsNullable=\"";          // SQL_DESC_NULLABLE(ODBC 3.0)     ,SQLColAttribute.
   if ( mNotNull == SAPDB_TRUE  )
   deployedcolumn += "0";
   if ( mNotNull == SAPDB_FALSE )
   deployedcolumn += "1";
   deployedcolumn += "\" dbm:IsReadOnly=\"";
   deployedcolumn += "";
   deployedcolumn += "\" dbm:Length=\"";              // SQL_DESC_LENGTH (ODBC 3.0)      ,SQLColAttribute.
   deployedcolumn += mDimension;
   deployedcolumn += "\" dbm:MinimumLength=\"";
   deployedcolumn += "";
   deployedcolumn += "\" dbm:name=\"";                // SQL_DESC_NAME(ODBC 3.0)         ,SQLColAttribute.
   deployedcolumn += string2XML(CASE(mName));
   deployedcolumn += "\" dbm:NumericPrecision=\"";    // SQL_DESC_PRECISION(ODBC 3.0)    ,SQLColAttribute.
   deployedcolumn += "";
   deployedcolumn += "\" dbm:NumericScale=\"";        // SQL_DESC_SCALE(ODBC 3.0)        ,SQLColAttribute.
   deployedcolumn += "";
   deployedcolumn += "\" dbm:OctetLength=\"";         // SQL_DESC_OCTET_LENGTH(ODBC 3.0) ,SQLColAttribute
   deployedcolumn += "";
   deployedcolumn += "\" dbm:Ordinal=\"";
   deployedcolumn += "";
   deployedcolumn += "\" dbm:TimePrecision=\"";
   deployedcolumn += "";
   deployedcolumn += "\">\n";

   // sdbm:<data_typ>

  /*-------------------------------------------------------------------------*
   * Insert the datatype fo the column.                                      *
   *-------------------------------------------------------------------------*/

   deployedcolumn += "<sdbm:";                         // SQLColAttribute 
   if (( mDataType == "Char" || mDataType == "Varchar") && 
	     mCodeType == "BYTE" ) 
         deployedcolumn += "BINARY";
   else
         deployedcolumn += mDataType.ToUpper();           // SQL_DESC_TYPE(ODBC 3.0), SQL_DESC_TYPE_NAME(ODBC 1.0)
   deployedcolumn += " oim:href=\"#_";
   deployedcolumn += "sdbm:";   
   if ((mDataType == "CHAR" || mDataType == "VARCHAR") ) 
   {
        if (mCodeType == "BYTE" )
		{
            deployedcolumn += "BINARY";
		}
        else 
		if (mCodeType == "ASCII" || mCodeType == "UNICODE" )  
        {
            deployedcolumn += mDataType.ToUpper(); 
	        deployedcolumn += "\" sdbm:Encoding=\"";         
	        deployedcolumn += mCodeType;
		}
   }
   deployedcolumn += "\"/>\n";
   deployedcolumn += "</dbm:DeployedColumn>\n";
  
   return deployedcolumn;
}

/*===========================================================================*
 *     StudioTD_ColumnDef::genAlterStatement                                 *
 *===========================================================================*/
StudioTD_String StudioTD_ColumnDef::genAlterStatement (StudioTD_ColumnDef *oldColumn,
                                                       StudioTD_String owner,
                                                       StudioTD_String table,
                                                       SAPDB_Bool casesensitive)
{
    StudioTD_String  sql;
	StudioTD_String  escapedOwner,escapedTable;
	StudioTD_String  escapedName,escapedOldName;
    StudioTD_String  sql_default_attr;    // we cannot both add a new col and set its default, null, or
                                          // key attributes.
    StudioTD_String  sql_null_attr;
    StudioTD_String  sql_key_attr;
    StudioTD_String  attributes;
    StudioTD_String  delimiter;
    StudioTD_String  intro;

    StudioTD_String  escaped_Default = mDefault;
    StudioTD_String  escaped_Comment = mComment;

    escapeQuoteChar(escaped_Default, "'");
    escapeQuoteChar(escaped_Comment, "'");

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

	escapedName = CASE(mName);
	escapeQuoteChar(escapedName,"\"");

	if(oldColumn != NULL){
		escapedOldName = CASE(oldColumn->name());
		escapeQuoteChar(escapedOldName,"\"");
	}

    if (mAlterType == AlterChange)
    {
        /*-----------------------------------------------------------------------*
         * Compare the datatypes, dimensions and code types.                     *
         *-----------------------------------------------------------------------*/
        if ( (mDataType != oldColumn->dataType())   ||
             (mDimension != oldColumn->dimension()) ||
             (mCodeType != oldColumn->codeType())    )
        {
            sql += intro;
            sql += "MODIFY (\"";
            sql += escapedOldName;
            sql += "\" ";
            sql += mDataType;

            if ((mDataType == "Char") || (mDataType == "Varchar"))
            {
                if (!mDimension.Empty())
                {
                    sql += " (";
                    sql += mDimension;
                    sql += ")";
                }

                if (!mCodeType.Empty())
                {
                    sql += " ";
                    sql += mCodeType;
                }
            }
        
            if ((mDataType == "Fixed") || (mDataType == "Float"))
            {
                if (!mDimension.Empty())
                {
                    sql += " (";
                    sql += mDimension;
                    sql += ")";
                }
            }

            sql += ")";
            sql += SEP;
        }

        /*-----------------------------------------------------------------------*
         * Compare the Not Null values.                                          *
         *-----------------------------------------------------------------------*/
        if (mNotNull != oldColumn->notNull())
        {
            sql += intro;
            sql += "COLUMN \"";
            sql += escapedOldName;
            sql += "\" ";
            sql += (mNotNull ? "NOT" : "DEFAULT");
            sql += " NULL";
            sql += SEP;
        }

        /*-----------------------------------------------------------------------*
         * Compare the default values.                                           *
         *-----------------------------------------------------------------------*/
        if (mDefault != oldColumn->defaultValue())
        {
            if ( (SAPDB_TRUE == mbUserDefinedDefault) &&
                 ( (mDataType == "Char")    || 
                   (mDataType == "Varchar") || 
                   (mDataType == "Long")    ||
                   (mDataType == "Date")    ||
                   (mDataType == "Time")    ||
                   (mDataType == "Timestamp") ) )
            {
                delimiter = "'";
            }

            sql += intro;
            sql += "COLUMN \"";
            sql += escapedOldName;
            sql += "\"";

            if (mDefault == "")
            {
                sql += " DROP DEFAULT";
                sql += SEP;
            }

            if (oldColumn->defaultValue() == "")
            {
                sql += " ADD DEFAULT ";
            }
            else
            {
                sql += " ALTER DEFAULT ";
            }

            sql += ( ( (mCodeType == "BYTE") && (SAPDB_TRUE == mbUserDefinedDefault) ) ? "x" : "" );
            sql += delimiter;
            sql += escaped_Default;              
            sql += delimiter;
            sql += SEP;
        }

        /*-----------------------------------------------------------------------*
         * Compare the comments.                                                 *
         *-----------------------------------------------------------------------*/
        if (mComment != oldColumn->comment())
        {
            sql += "COMMENT ON COLUMN \"";
            sql += escapedTable;

            sql += "\".\"";
            sql += escapedOldName;
            sql += "\" IS '";
            sql += escaped_Comment;  
            sql += "'";
            sql += SEP;
        }

        /*-----------------------------------------------------------------------*
         * Compare the column names.                                             *
         *-----------------------------------------------------------------------*/
        if (mName != oldColumn->name())
        {
            sql += "RENAME COLUMN \"";
            sql += escapedOwner;
            sql += "\".\"";
            sql += escapedTable;
            sql += "\".\"";
            sql += escapedOldName;
            sql += "\" ";
            sql += "TO \"";
            sql += escapedName;
            sql += "\"";
            sql += SEP;
        }
    }
    else  /*  AlterAdd  */
    {
        /*-----------------------------------------------------------------------*
         * If name or datatype is empty the column is not taken into account.    *
         *-----------------------------------------------------------------------*/
        if (mName.Empty() || mDataType.Empty())
            return "";

        /*-----------------------------------------------------------------------*
         * Insert the column name and the datatype of the added column.          *
         *-----------------------------------------------------------------------*/
        sql += intro;
        sql += "ADD (\"";
        sql += escapedName;
        sql += "\" ";
        sql += mDataType;

        /*-----------------------------------------------------------------------*
         * Depending on the datatype, insert a dimension and/or a code           *
         * specification into the statement.                                     *
         *-----------------------------------------------------------------------*/
        if ((mDataType == "Char") || (mDataType == "Varchar") || (mDataType == "Long"))
        {
            if (!mDimension.Empty() && mDataType != "Long")
            {
                sql += " (";
                sql += mDimension;
                sql += ")";
            }

            if (!mCodeType.Empty())
            {
                sql += " ";
                sql += mCodeType;
            }
        }
      
        if ((mDataType == "Fixed") || (mDataType == "Float"))
        {
            if (!mDimension.Empty())
            {
                sql += " (";
                sql += mDimension;
                sql += ")";
            }
        }

        /*-----------------------------------------------------------------------*
         * Insert the NOT NULL clause.                                           *
         *-----------------------------------------------------------------------*/
        if (mNotNull)
        {
            sql_null_attr += intro;
            sql_null_attr += "COLUMN \"";
            sql_null_attr += escapedName;
            sql_null_attr += "\" ";
            sql_null_attr += "NOT NULL";
            sql_null_attr += SEP;
        }

        /*-----------------------------------------------------------------------*
         * Insert the default value.                                             *
         *-----------------------------------------------------------------------*/

        if (!mDefault.Empty())
        {
            delimiter = "";

            if ( (SAPDB_TRUE == mbUserDefinedDefault) &&
                 ( (mDataType == "Char")    || 
                   (mDataType == "Varchar") ||
                   (mDataType == "Long")    ||
                   (mDataType == "Date")    ||
                   (mDataType == "Time")    ||
                   (mDataType == "Timestamp" ) ) )
            {
                delimiter = "'";
            }

            sql_default_attr += intro;
            sql_default_attr += "COLUMN \"";
            sql_default_attr += escapedName;
            sql_default_attr += "\"";
            sql_default_attr += " ADD DEFAULT ";
            sql_default_attr += ( ( (mCodeType == "BYTE") && (SAPDB_TRUE == mbUserDefinedDefault) ) ? "x" : "" );
            sql_default_attr += delimiter;
            sql_default_attr += escaped_Default;              
            sql_default_attr += delimiter;
            sql_default_attr += SEP;
        }
    
        /*-----------------------------------------------------------------------*
         * Insert the unique clause.                                             *
         *-----------------------------------------------------------------------*/
        if (mUnique)
            attributes += " UNIQUE";
  
        /*-----------------------------------------------------------------------*
         * Append the column attributes to the statement.                        *
         *-----------------------------------------------------------------------*/
        if (!attributes.Empty())
            sql += attributes;
  
        sql += ")";
        sql += SEP;
  
        /*-----------------------------------------------------------------------*
         * Append the comment statement.                                         *
         *-----------------------------------------------------------------------*/
        if (!mComment.Empty())
        {
            sql += "COMMENT ON COLUMN \"";

			sql += escapedTable;
            sql += "\".\"";
			sql += escapedName;
            sql += "\" IS '";
            sql += escaped_Comment;  
            sql += "'";
            sql += SEP;
        }
        /*-----------------------------------------------------------------------*
         * Append separate stmts to change attributes of the newly created col                                         *
         *-----------------------------------------------------------------------*/
        sql += sql_null_attr;
        sql += sql_default_attr;
    }

    return sql;
}


/*===========================================================================*
 *     StudioTD_ColumnDef::setDefaultValueEx                                 *
 *===========================================================================*/

// prerequisite: data type must have been set; if not char is assumed
void
StudioTD_ColumnDef::setDefaultValueEx(const StudioTD_String &defaultvalue)
{
    // necessary to copy because we might change the content if default is the empty string
    StudioTD_String my_Default              = defaultvalue;
    SAPDB_Bool      my_bUserDefinedDefault = SAPDB_FALSE;
    SAPDB_Int4      my_SubstrIndex         = 8;

     if (my_Default.SubStr(0, 7) == "DEFAULT")
    {

        my_bUserDefinedDefault = SAPDB_FALSE;       // Initialize to DB function

        // Attention:
        // For char type columns it is not yet possible to decide if a default value 
        // 'DEFAULT USER' or 'DEFAULT USERGROUP' or 'DEFAULT SYSDBA' is a string literal or a DB function
        // We'll assume here IN ANY CASE that's a DB function.
        if ( (my_Default.Size() >= 12 && my_Default.SubStr(8, 4) == "USER") ||
             (my_Default.Size() >= 17 && my_Default.SubStr(8, 9) == "USERGROUP") ||
             (my_Default.Size() >= 14 && my_Default.SubStr(8, 6) == "SYSDBA") )
        {
            ;
        }
        else if (my_Default.Size() >= 12 && my_Default.SubStr(8, 4) == "DATE")
        {
            // This kind of default may be assigned to columns of data type 
            // - DATE: here it is the sql function DATE
            // - CHAR: here this has to be handled as string 'DEFAULT DATE';
            //         function DATE is not allowed for columns of data type CHAR
            //         example: CREATE TABLE <name> (<col name> CHAR(N) DEFAULT 'DEFAULT DATE')
            if  (mDataType != "Date")
            {
                my_bUserDefinedDefault = SAPDB_TRUE;
                my_SubstrIndex         = 0;
            }
            // else - initial value SAPDB_FALSE applies
        }
        else if (my_Default.Size() >= 13 && my_Default.SubStr(8, 5) == "STAMP")
        {
            // This kind of default may be assigned to columns of data type 
            // - CHAR|LONG BYTE:          here it is the sql function STAMP
            // - CHAR|LONG ASCII|UNICODE: here this has to be handled as string 'DEFAULT STAMP';
            //                            example: CREATE TABLE <name> (<col name> CHAR(N) DEFAULT 'DEFAULT STAMP')
            if (mCodeType != "BYTE")
            {
                my_bUserDefinedDefault = SAPDB_TRUE;
                my_SubstrIndex         = 0;
            }
        }
        else if (my_Default.Size() >= 17 && my_Default.SubStr(8, 9) == "TIMESTAMP")
        {
            // This kind of default may be assigned to columns of data type 
            // - TIMESTAMP: here it is the sql function TIMESTAMP
            // - CHAR:      here this has to be handled as string 'DEFAULT DATE';
            //              function TIMESTAMP is not allowed for columns of data type CHAR
            //              example: CREATE TABLE <name> (<col name> CHAR(N) DEFAULT 'DEFAULT TIMESTAMP')
            if  (mDataType != "Timestamp")
            {
                my_bUserDefinedDefault = SAPDB_TRUE;
                my_SubstrIndex         = 0;
            }
            // else - initial value SAPDB_FALSE applies
        }
        else if (my_Default.Size() >= 16 && my_Default.SubStr(8, 8) == "TIMEZONE")
        {
            // This kind of default may be assigned only to columns of data type 
            // - FLOAT(p) where p >= 6    : here it is the sql function UTCDIFF
            // - FIXED(p,s) (s>=0, p>=s+6): same as for FLOAT 
            // - CHAR: here this has to be andled as string 'DEFAULT TIMEZONE';
            //         function TIME is not allowed for columns of data type CHAR
            //         example: CREATE TABLE <name> (<col name> CHAR(N) DEFAULT 'DEFAULT TIMEZONE')
            if  ( (mDataType != "Float") && (mDataType != "Fixed") )
            {
                my_bUserDefinedDefault = SAPDB_TRUE;
                my_SubstrIndex         = 0;
            }
            // else - initial value SAPDB_FALSE applies
        }
        else if (my_Default.Size() >= 12 && my_Default.SubStr(8, 4) == "TIME")
        {
            // This kind of default may be assigned to columns of data type 
            // - TIME: here it is the sql function TIME
            // - CHAR: here this has to be andled as string 'DEFAULT TIME';
            //         function TIME is not allowed for columns of data type CHAR
            //         example: CREATE TABLE <name> (<col name> CHAR(N) DEFAULT 'DEFAULT TIME')
            if  (mDataType != "Time")
            {
                my_bUserDefinedDefault = SAPDB_TRUE;
                my_SubstrIndex         = 0;
            }
            // else - initial value SAPDB_FALSE applies
        }
        else if (my_Default.Size() >= 15 && my_Default.SubStr(8, 7) == "UTCDATE")
        {
            // This kind of default may be assigned only to columns of data type 
            // - TIMESTAMP: here it is the sql function TIME
            // - CHAR: here this has to be andled as string 'DEFAULT UTCDATE';
            //         function TIME is not allowed for columns of data type CHAR
            //         example: CREATE TABLE <name> (<col name> CHAR(N) DEFAULT 'DEFAULT UTCDATE')
            if  (mDataType != "Timestamp")
            {
                my_bUserDefinedDefault = SAPDB_TRUE;
                my_SubstrIndex         = 0;
            }
            // else - initial value SAPDB_FALSE applies
        }
        else if (my_Default.Size() >= 15 && my_Default.SubStr(8, 7) == "UTCDIFF")
        {
            // This kind of default may be assigned only to columns of data type 
            // - FLOAT(p) where p >= 4    : here it is the sql function UTCDIFF
            // - FIXED(p,s) (s>=2, p>=s+2): same as for FLOAT 
            // - CHAR: here this has to be andled as string 'DEFAULT UTCDIFF';
            //         function TIME is not allowed for columns of data type CHAR
            //         example: CREATE TABLE <name> (<col name> CHAR(N) DEFAULT 'DEFAULT UTCDIFF')
            if  ( (mDataType != "Float") && (mDataType != "Fixed") )
            {
                my_bUserDefinedDefault = SAPDB_TRUE;
                my_SubstrIndex         = 0;
            }
            // else - initial value SAPDB_FALSE applies
        }
        else if (my_Default.Size() >= 14 && my_Default.SubStr(8, 6) == "SERIAL")
        {
            // This kind of default may be assigned to columns of data type
            // - TIME: here it is the sql function TIME
            // - CHAR: here this has to be andled as string 'DEFAULT TIME';
            //         function TIME is not allowed for columns of data type CHAR
            //         example: CREATE TABLE <name> (<col name> CHAR(N) DEFAULT 'DEFAULT TIME')
            if  ( (mDataType != "Integer") && (mDataType != "Smallint") && (mDataType != "Fixed"))
            {
                my_bUserDefinedDefault = SAPDB_TRUE;
                my_SubstrIndex         = 0;
            }
            // else - initial value SAPDB_FALSE applies
        }
        else if ( (my_Default.Size() >= 12 && my_Default.SubStr(8, 4) == "TRUE") 
                    ||
                  (my_Default.Size() >= 13 && my_Default.SubStr(8, 5) == "FALSE"))
        {
            // The functions TRUE and FALSE for boolean columns are NOT preceeded by the
            // keyword DEFAULT in column DEFAULT in the system table COLUMNS; thus
            // if here comes a value like 'DEFAULT TRUE' it is in any case meant the
            // string 'DEFAULT TRUE'
            my_bUserDefinedDefault = SAPDB_TRUE;
            my_SubstrIndex         = 0;
        }
        else
        {
            my_bUserDefinedDefault = SAPDB_TRUE;
            my_SubstrIndex         = 0;
        }
    }
    else
    {
        if ( ( (my_Default.SubStr(0, 4) == "TRUE") || (my_Default.SubStr(0, 5) == "FALSE") ) &&
             (mDataType == "Boolean") )
        {
            my_bUserDefinedDefault = SAPDB_FALSE;       // Default is DB function
        }
        else
        {
            my_bUserDefinedDefault = SAPDB_TRUE;        // Default is user defined value

            //if the value length is 0 and the column is not NULL set a {SPACE}
            //SAP DB treats empty String ('') or String with SPACES the same
            //with space the VB Surface will distinguish an empty String or a null String
            if(my_Default.Size() == 0)
            {
                my_Default = " ";
            }
        }

        my_SubstrIndex = 0;
    }   // end else if (my_Default.SubStr(0, 7) == "DEFAULT")

    //*
    //*     Now change the flag indicating the use of a DB function
    //*
    SET_DIRTYFLAG(mbUserDefinedDefault, my_bUserDefinedDefault);
    SET_ATTRIBUTE(mbUserDefinedDefault, my_bUserDefinedDefault);

    //*
    //*     Now change the default, finally
    //*
    if (0 == my_SubstrIndex)
    {
        SET_DIRTYFLAG(mDefault, my_Default);
        SET_ATTRIBUTE(mDefault, my_Default);
    }
    else
    {
        SET_DIRTYFLAG(mDefault, my_Default.SubStr(my_SubstrIndex));
        SET_ATTRIBUTE(mDefault, my_Default.SubStr(my_SubstrIndex));
    }
}
// StudioTD_ColumnDef::setDefaultValueEx()


/*===========================================================================*
 *     StudioTD_ColumnDef::genDescription                                    *
 *===========================================================================*/
StudioTD_String StudioTD_ColumnDef::genDescription ()
{
  StudioTD_String  temp;
  StudioTD_String  desc;

  desc  = mName;
  desc += SEP_LEVEL_3;
  desc += mDataType;
  desc += SEP_LEVEL_3;
  desc += mDimension;
  desc += SEP_LEVEL_3;
  desc += mCodeType;
  desc += SEP_LEVEL_3;
  desc += (mKey     ? "true" : "false");
  desc += SEP_LEVEL_3;
  desc += (mNotNull ? "true" : "false");
  desc += SEP_LEVEL_3;
  desc += mDefault;
  desc += SEP_LEVEL_3;
  desc += (mUnique  ? "true" : "false");
  desc += SEP_LEVEL_3;
  desc += mComment;
  desc += SEP_LEVEL_3;
  temp.ConvertFromInt(mKeyPos);
  desc += temp;
  desc += SEP_LEVEL_3;
  
  return desc;
}


/*===========================================================================*
 *     StudioTD_ColumnDef::parseDescription                                  *
 *===========================================================================*/
void StudioTD_ColumnDef::parseDescription (StudioTD_String description)
{
  SAPDB_UInt   pos = 0;
  
  mName      = NextToken (description, SEP_LEVEL_3, &pos);
  mDataType  = NextToken (description, SEP_LEVEL_3, &pos);
  mDimension = NextToken (description, SEP_LEVEL_3, &pos);
  mCodeType  = NextToken (description, SEP_LEVEL_3, &pos);
  mKey       = (NextToken (description, SEP_LEVEL_3, &pos) == "true") ? true : false;
  mNotNull   = (NextToken (description, SEP_LEVEL_3, &pos) == "true") ? true : false;
  mDefault   = NextToken (description, SEP_LEVEL_3, &pos);
  mUnique    = (NextToken (description, SEP_LEVEL_3, &pos) == "true") ? true : false;
  mComment   = NextToken (description, SEP_LEVEL_3, &pos);
  mKeyPos    = AtoI(NextToken (description, SEP_LEVEL_3, &pos));    
}
