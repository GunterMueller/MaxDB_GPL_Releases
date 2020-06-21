/*!===========================================================================

  module:       StudioTD_ConditionDef.cpp

  ----------------------------------------------------------------------------

  responsible:  MarkusS

  special area: SQL Studio

  description:  Implementation of the condition definition class StudioTD_ConditionDef.

  created:      2000-05-12

  updated:      2000-05-12

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
#include "SQLStudio/TableDefinition/StudioTD_ConditionDef.h"


/*===========================================================================*
 *     StudioTD_ConditionDef::StudioTD_ConditionDef                                  *
 *===========================================================================*/
StudioTD_ConditionDef::StudioTD_ConditionDef ()
    : mNotOperator(false), mNoExpression(false), mLogOperator("<none>"), 
      mAlterType(AlterNoChange)
{
  setType (ClassConditionDef);
}

/*===========================================================================*
 *     StudioTD_ConditionDef::~StudioTD_ConditionDef                                 *
 *===========================================================================*/
StudioTD_ConditionDef::~StudioTD_ConditionDef()
{
}


/*===========================================================================*
 *     StudioTD_ConditionDef::findIncomplete                                     *
 *===========================================================================*/
StudioTD_String StudioTD_ConditionDef::findIncomplete ()
{
  /*-------------------------------------------------------------------------*
   * If all field have their initial values the definition is totally empty  *
   * --> it will be ignored.                                                 *
   *-------------------------------------------------------------------------*/
  if (   mColumn.Empty() && mPredicate.Empty() && !mNotOperator
      && mExpression.Empty() && !mNoExpression && (mLogOperator == "<none>"))
    return "";

  /*-------------------------------------------------------------------------*
   * If column or predicate or expression is empty its index is returned.    *
   *-------------------------------------------------------------------------*/
  if (mColumn.Empty())
    return "1";

  if (mPredicate.Empty())
    return "2";

  if (mExpression.Empty() && !mNoExpression)
    return "3";

  return "";
}


/*===========================================================================*
 *     StudioTD_ConditionDef::genCreateStatement                                 *
 *===========================================================================*/
StudioTD_String StudioTD_ConditionDef::genCreateStatement (StudioTD_Mode mode, SAPDB_Bool casesensitive)
{
  StudioTD_String  sql;
  StudioTD_String  escapedColumnName;
  SAPDB_UInt       pos;

  /*-------------------------------------------------------------------------*
   * If column or predicate or expression is empty the column is not taken   *
   * into account.                                                           *
   *-------------------------------------------------------------------------*/
  if (mColumn.Empty() || mPredicate.Empty() || (mExpression.Empty() && !mNoExpression))
    return "";

  /*-------------------------------------------------------------------------*
   * Insert the column.                                                      *
   *-------------------------------------------------------------------------*/
  sql += "\"";
  escapedColumnName = mColumn;
  escapeQuoteChar(escapedColumnName,"\"");
  sql += CASE(escapedColumnName);
  sql += "\"";
  
  /*-------------------------------------------------------------------------*
   * Insert the predicate.                                                   *
   *-------------------------------------------------------------------------*/
  if ((mPredicate == "TRUE") || (mPredicate == "FALSE") || (mPredicate == "NULL"))
    sql += " IS";
  
  if (mNotOperator)
    sql += " NOT";

  pos = mExpression.Find ("(SOUNDS)");
  if ((mPredicate == "LIKE") && (pos != StudioTD_String::NPos))
    sql += "SOUNDS";
  
  sql += " ";
  sql += mPredicate;

  /*-------------------------------------------------------------------------*
   * Insert the expression.                                                  *
   *-------------------------------------------------------------------------*/
  if (pos != StudioTD_String::NPos)
  {
    sql += " ";
    sql += mExpression.SubStrBasis (0, pos - 1);
  }
  else
  {
    if (mPredicate == "IN")
    {
      sql += " (";
      sql += mExpression;
      sql += ")";
    }
    else if (!mNoExpression)
    {
      sql += " ";
      sql += mExpression;
    }
  }
  
  /*-------------------------------------------------------------------------*
   * Insert the logical operator.                                            *
   *-------------------------------------------------------------------------*/
  if (mLogOperator == "<none>")
    sql += ",\n";
  else
  {
    sql += "     ";
    sql += mLogOperator;
    sql += "\n\t\t\t\t\t";
  }
  
  return sql;
}

/*===========================================================================*
 *     StudioTD_ConditionDef::genCreateXML
 *===========================================================================*/
StudioTD_String StudioTD_ConditionDef::genCreateXML( SAPDB_Bool casesensitive )
{
   StudioTD_String   xml;

   SAPDB_UInt       pos;

  /*-------------------------------------------------------------------------*
   * If column or predicate or expression is empty the column is not taken   *
   * into account.                                                           *
   *-------------------------------------------------------------------------*/
  if (mColumn.Empty() || mPredicate.Empty() || (mExpression.Empty() && !mNoExpression))
    return "";

  /*-------------------------------------------------------------------------*
   * Insert the column.                                                      *
   *-------------------------------------------------------------------------*/
  xml += CASE(mColumn);
    
  /*-------------------------------------------------------------------------*
   * Insert the predicate.                                                   *
   *-------------------------------------------------------------------------*/
  if ((mPredicate == "TRUE") || (mPredicate == "FALSE") || (mPredicate == "NULL"))
    xml += " IS";
  
  if (mNotOperator)
    xml += " NOT";

  pos = mPredicate.Find ("(SOUNDS)");
  if ((mPredicate == "LIKE") && (pos != StudioTD_String::NPos))
       xml += "SOUNDS";
  
  xml += mPredicate;

  /*-------------------------------------------------------------------------*
   * Insert the expression.                                                  *
   *-------------------------------------------------------------------------*/
  if (pos != StudioTD_String::NPos)
  {
    xml += " ";
    xml += mExpression.SubStrBasis (0, pos - 1);
  }
  else
  {
    if (mPredicate == "IN")
        xml += mExpression;
      
    else if (!mNoExpression)
        xml += mExpression;
  }
  
  /*-------------------------------------------------------------------------*
   * Insert the logical operator.                                            *
   *-------------------------------------------------------------------------*/
  if (mLogOperator == "<none>")
      xml += ",\n";
  else
      xml += mLogOperator;
    
  return xml;
}


/*===========================================================================*
 *     StudioTD_ConditionDef::genDescription                                 *
 *===========================================================================*/
StudioTD_String StudioTD_ConditionDef::genDescription ()
{
  StudioTD_String  desc;

  desc += mColumn;
  desc += SEP_LEVEL_5;
  desc += (mNotOperator ? "true" : "false");
  desc += SEP_LEVEL_5;
  desc += mPredicate;
  desc += SEP_LEVEL_5;
  desc += mExpression;
  desc += SEP_LEVEL_5;
  desc += (mNoExpression ? "true" : "false");
  desc += SEP_LEVEL_5;
  desc += mLogOperator;
  desc += SEP_LEVEL_5;

  return desc;
}


/*===========================================================================*
 *     StudioTD_ConditionDef::parseDescription                                   *
 *===========================================================================*/
void StudioTD_ConditionDef::parseDescription (StudioTD_String description)
{
  SAPDB_UInt   pos = 0;
  
  mColumn       = NextToken (description, SEP_LEVEL_5, &pos);
  mNotOperator  = (NextToken (description, SEP_LEVEL_5, &pos) == "true") ? true : false;
  mPredicate    = NextToken (description, SEP_LEVEL_5, &pos);
  mExpression   = NextToken (description, SEP_LEVEL_5, &pos);
  mNoExpression = (NextToken (description, SEP_LEVEL_5, &pos) == "true") ? true : false;
  mLogOperator  = NextToken (description, SEP_LEVEL_5, &pos);
}
