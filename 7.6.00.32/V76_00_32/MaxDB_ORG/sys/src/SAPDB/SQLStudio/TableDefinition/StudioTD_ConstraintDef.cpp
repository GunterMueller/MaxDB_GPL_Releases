/*!===========================================================================

  module:       StudioTD_ConstraintDef.cpp

  ----------------------------------------------------------------------------

  responsible:  MarkusS,WolfgangA

  special area: SQL Studio

  description:  Implementation of the constraint definition class StudioTD_ConstraintDef.

  see:          www.sapdb.org

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
#include "SQLStudio/TableDefinition/StudioTD_ConstraintDef.h"

static StudioTD_String functions[] = 
          {"ABS", "ACOS", "ADDDATE", "ADDTIME", "ALPHA", "ASCII", "ASIN", "ATAN", "ATAN2", 
           "CEIL", "CHAR", "CHR", "COS", "COSH", "COT", "DATE", "DATEDIFF", "DAY", "DAYNAME", 
           "DAYOFMONTH", "DAYOFWEEK", "DAYOFYEAR", "DECODE", "DEGREES", "EBCDIC", "EXP", 
           "EXPAND", "FIXED", "FLOOR", "GREATEST", "HEX", "HOUR", "INDEX", "INITCAP", 
           "LEAST", "LENGTH", "LFILL", "LN", "LOG", "LOWER", "LPAD", "LTRIM", "MAKEDATE", 
           "MAKETIME", "MAPCHAR", "MICROSECOND", "MINUTE", "MONTH", "MONTHNAME", "NOROUND", 
           "NUM", "PI", "POWER", "RADIANS", "REPLACE", "RFILL", "ROUND", "RPAD", "RTRIM", 
           "SECOND", "SIGN", "SIN", "SINH", "SORT", "SOUNDEX", "SQRT", "SUBDATE", "SUBSTR", 
           "SUBTIME", "TAN", "TANH", "TIME", "TIMEDIFF", "TIMESTAMP", "TRANSLATE", "TRIM", 
           "TRUNC", "UPPER", "VALUE", "WEEKOFYEAR", "YEAR"};


/*===========================================================================*
 *     StudioTD_ConstraintDef::StudioTD_ConstraintDef                        *
 *===========================================================================*/
StudioTD_ConstraintDef::StudioTD_ConstraintDef ()
    : mName("")
{
  setType (ClassConstraintDef);
}


/*===========================================================================*
 *     StudioTD_ConstraintDef::StudioTD_ConstraintDef                        *
 *===========================================================================*/
StudioTD_ConstraintDef::StudioTD_ConstraintDef (const StudioTD_ConstraintDef& constr)
{
  StudioTD_ConditionDef  *cond, *oldCond;

  setType (ClassConstraintDef);

  mName = constr.mName;
  
  for (oldCond = (StudioTD_ConditionDef *)((Studio_List &)constr.mConditions).first(); 
       oldCond; 
       oldCond = (StudioTD_ConditionDef *)((Studio_List &)constr.mConditions).next())
  {
    cond = new StudioTD_ConditionDef (*oldCond);
    mConditions.append (cond);
  }
  
  for (oldCond = (StudioTD_ConditionDef *)((Studio_List &)constr.mRemovedConditions).first(); 
       oldCond; 
       oldCond = (StudioTD_ConditionDef *)((Studio_List &)constr.mRemovedConditions).next())
  {
    cond = new StudioTD_ConditionDef (*(StudioTD_ConditionDef *)oldCond);
    mRemovedConditions.append (cond);
  }
}


/*===========================================================================*
 *     StudioTD_ConstraintDef::~StudioTD_ConstraintDef                       *
 *===========================================================================*/
StudioTD_ConstraintDef::~StudioTD_ConstraintDef()
{
}

  
/*===========================================================================*
 *     StudioTD_ConstraintDef::setAlterType                                  *
 *===========================================================================*/
void StudioTD_ConstraintDef::setAlterType (StudioTD_AlterType type)
{
  Studio_ListData  cond = mConditions.first();

  while (cond)
  {
    ((StudioTD_ConditionDef *)cond)->setAlterType (type);
    cond = mConditions.next();
  }
}

  
/*===========================================================================*
 *     StudioTD_ConstraintDef::alterType                                     *
 *===========================================================================*/
StudioTD_AlterType StudioTD_ConstraintDef::alterType ()
{
  return ((StudioTD_ConditionDef *)mConditions.first())->alterType();
}

  
/*===========================================================================*
 *     StudioTD_ConstraintDef::addCondition                                  *
 *===========================================================================*/
StudioTD_ConditionDef * StudioTD_ConstraintDef::addCondition ()
{
  StudioTD_ConditionDef *cond;
  
  /*-------------------------------------------------------------------------*
   * Create a new condition object.                                          *
   *-------------------------------------------------------------------------*/
  cond = new StudioTD_ConditionDef;

  /*-------------------------------------------------------------------------*
   * Append the new condition object to the conditions list.                 *
   *-------------------------------------------------------------------------*/
  mConditions.append (cond);

  /*-------------------------------------------------------------------------*
   * Initialize the alter type.                                              *
   *-------------------------------------------------------------------------*/
  cond->setAlterType (AlterAdd);
  
  return cond;
}


/*===========================================================================*
 *     StudioTD_ConstraintDef::removeCondition                               *
 *===========================================================================*/
void StudioTD_ConstraintDef::removeCondition (SAPDB_UInt index)
{
  StudioTD_ConditionDef *cond;
  
  /*-------------------------------------------------------------------------*
   * Check if index is a valid list position.                                *
   *-------------------------------------------------------------------------*/
  if (index >= mConditions.size())
    return;

  /*-------------------------------------------------------------------------*
   * Remove the item with the given index from the conditions list and       *
   * append it to the removed conditions list.                               *
   *-------------------------------------------------------------------------*/
  cond = (StudioTD_ConditionDef *)mConditions.remove (index, 1);
  mRemovedConditions.append (cond);
}


/*===========================================================================*
 *     StudioTD_ConstraintDef::condition                                     *
 *===========================================================================*/
StudioTD_ConditionDef * StudioTD_ConstraintDef::condition (SAPDB_UInt index)
{
  return (StudioTD_ConditionDef *)mConditions.at (index);
}


/*===========================================================================*
 *     StudioTD_ConstraintDef::conditionCount                                *
 *===========================================================================*/
SAPDB_UInt StudioTD_ConstraintDef::conditionCount ()
{
  return mConditions.size();
}


/*===========================================================================*
 *     StudioTD_ConstraintDef::findIncomplete                                *
 *===========================================================================*/
StudioTD_String StudioTD_ConstraintDef::findIncomplete ()
{
  Studio_ListData     cond;
  SAPDB_Int          i;
  StudioTD_String    s;
  
  /*-------------------------------------------------------------------------*
   * For each item in the conditions list check if it is incomplete.         *
   *-------------------------------------------------------------------------*/
  for (cond = mConditions.first(), i = 0; cond; cond = mConditions.next(), i++)
  {
    s = ((StudioTD_ConditionDef *)cond)->findIncomplete();
    
    if (!s.Empty())
      return s;
  }

  return "";
}


/*===========================================================================*
 *     StudioTD_ConstraintDef::genCreateStatement                            *
 *===========================================================================*/
StudioTD_String StudioTD_ConstraintDef::genCreateStatement (StudioTD_Mode mode, SAPDB_Bool casesensitive)
{
  StudioTD_String  sql;
  StudioTD_String  escapedColumnName;
  StudioTD_String  conditions;
  Studio_ListData  cond;

  /*-------------------------------------------------------------------------*
   * Generate the intro of the CONSTRAINT clause.                            *
   *-------------------------------------------------------------------------*/
  sql += "\tCONSTRAINT ";
  
  /*-------------------------------------------------------------------------*
   * Insert the constraint name.                                             *
   *-------------------------------------------------------------------------*/
  if (!mName.Empty())
  {
    sql += "\"";
	escapedColumnName = CASE(mName);
    escapeQuoteChar(escapedColumnName,"\"");
    sql += escapedColumnName;
    sql += "\" CHECK\t";
  }
  else
    sql += "\t\t\t";
  
  /*-------------------------------------------------------------------------*
   * For each item in the condition list generate its part of the statement. *
   *-------------------------------------------------------------------------*/
  for (cond = mConditions.first(); cond; cond = mConditions.next())
    conditions += ((StudioTD_ConditionDef *)cond)->genCreateStatement (mode, casesensitive);
  
  if (!conditions.Empty())
  {
    sql += conditions;
    return sql;
  }
  else
    return "";
}

/*===========================================================================*
 *     StudioTD_ConstraintDef::genCreateXML                                  *
 *===========================================================================*/
StudioTD_String StudioTD_ConstraintDef::genCreateXML (StudioTD_Mode mode, SAPDB_Bool casesensitive)
{

   StudioTD_String deployedcolumn;

   deployedcolumn += "oim:label =\"";
   deployedcolumn += string2XML( mName );
   deployedcolumn += "\" oim:name=\"";
   deployedcolumn += string2XML( mName );
   deployedcolumn += "\"";
   deployedcolumn += " bodyLanguage = \"&lt;constraint_definition&gt;::= see SAPDB reference manual \" ";
   deployedcolumn += " body = \" ";
   deployedcolumn += string2XML( mDefinition );
   deployedcolumn += "\">\n";

   return deployedcolumn;
}

/*===========================================================================*
 *     StudioTD_ConstraintDef::genAlterStatement                             *
 *===========================================================================*/
StudioTD_String StudioTD_ConstraintDef::genAlterStatement (SAPDB_Bool casesensitive)
{
  StudioTD_String  sql;
  StudioTD_String  escapedColumnName;
  SAPDB_Bool       bChanged = false;
  SAPDB_UInt       pos;

  StudioTD_ConditionDef   *cond;
  
  /*-------------------------------------------------------------------------*
   * Check, if only one empty condition exists --> return an empty string.   *
   *-------------------------------------------------------------------------*/
  if (mConditions.size() == 1)
  {
    cond = (StudioTD_ConditionDef *)mConditions.first();
    if ((cond->column().Empty()) && (cond->predicate().Empty()) &&
        (cond->expression().Empty() && !cond->noExpression()))
      return "";
  }
  
  /*-------------------------------------------------------------------------*
   * If one of the conditions was changed the complete constraint will be    *
   * inserted into the statement.                                            *
   *-------------------------------------------------------------------------*/
  for (cond = (StudioTD_ConditionDef *)mConditions.first(); 
       cond; 
       cond = (StudioTD_ConditionDef *)mConditions.next())
  {
    /*-----------------------------------------------------------------------*
     * Insert the column name.                                               *
     *-----------------------------------------------------------------------*/
    if (isFunction (cond->column()))
      sql += CASE(cond->column());
    else
    {
      sql += "\"";
	  escapedColumnName = cond->column();
	  escapeQuoteChar(escapedColumnName,"\"");
      sql += CASE(escapedColumnName);
      sql += "\"";
    }
    
    /*-----------------------------------------------------------------------*
     * Insert the predicate.                                                 *
     *-----------------------------------------------------------------------*/
    if (   (cond->predicate() == "TRUE") || (cond->predicate() == "FALSE") 
        || (cond->predicate() == "NULL"))
      sql += " IS";
    
    if (cond->notOperator())
      sql += " NOT";
    
    if ((cond->predicate() == "LIKE") && (cond->expression().Find ("(SOUNDS)") != StudioTD_String::NPos))
      sql += " SOUNDS";
    
    sql += " ";
    sql += cond->predicate();
    
    /*-----------------------------------------------------------------------*
     * Insert the expression.                                                *
     *-----------------------------------------------------------------------*/
    pos = cond->expression().Find (" (SOUNDS)");
    
    if (pos == StudioTD_String::NPos)
    {
      if (cond->predicate() == "IN")
      {
        sql += " (";
        sql += cond->expression();
        sql += ")";
      }
      else
      {
        sql += " ";
        sql += cond->expression();
      }
    }
    else
    {
      sql += " ";
      sql += cond->expression().SubStrBasis (0, pos);
    }
    
    /*-----------------------------------------------------------------------*
     * Insert the logical operator.                                          *
     *-----------------------------------------------------------------------*/
    if (cond->logOperator() != "<none>")
    {
      sql += " ";
      sql += cond->logOperator();
      sql += " ";
    }
  }
  
  return sql;
}


SAPDB_Bool StudioTD_ConstraintDef::extractPredicate(const StudioTD_String& token,
							SAPDB_UInt const &start,
							StudioTD_String &pred,
							SAPDB_UInt &end)
{
	// first search for <= ; >=  ; <>
	end   = token.Find   ("<=", start);	
	if (end != StudioTD_String::NPos){

		pred = token.SubStrBasis (end,end+2);
		end = end + 2;
		return SAPDB_TRUE;
	}
	end   = token.Find   (">=", start);	
	if (end != StudioTD_String::NPos){

		pred = token.SubStrBasis (end,end+2);
		end = end + 2;
		return SAPDB_TRUE;
	}
	end   = token.Find   ("<>", start);	
	if (end != StudioTD_String::NPos){

		pred = token.SubStrBasis (end,end+2);
		end = end + 2;
		return SAPDB_TRUE;
	}

	//then for = < >
	end   = token.Find   ("=", start);	
	if (end != StudioTD_String::NPos){

		pred = token.SubStrBasis (end,end+1);
		end = end + 1;
		return SAPDB_TRUE;
	}
	end   = token.Find   ("<", start);	
	if (end != StudioTD_String::NPos){

		pred = token.SubStrBasis (end,end+1);
		end = end + 1;
		return SAPDB_TRUE;
	}
	end   = token.Find   (">", start);	
	if (end != StudioTD_String::NPos){

		pred = token.SubStrBasis (end,end+1);
		end = end + 1;
		return SAPDB_TRUE;
	}
	end   = token.Find   ("IS", start);	
	if (end != StudioTD_String::NPos){

		pred = token.SubStrBasis (end,end+2);
		end = end + 2;
		return SAPDB_TRUE;
	}
	end   = token.Find   ("NOT", start);	
	if (end != StudioTD_String::NPos){

		pred = token.SubStrBasis (end,end+3);
		end = end + 3;
		return SAPDB_TRUE;
	}
	end   = token.Find   ("TRUE", start);	
	if (end != StudioTD_String::NPos){

		pred = token.SubStrBasis (end,end+4);
		end = end + 4;
		return SAPDB_TRUE;
	}
	end   = token.Find   ("FALSE", start);	
	if (end != StudioTD_String::NPos){

		pred = token.SubStrBasis (end,end+5);
		end = end + 5;
		return SAPDB_TRUE;
	}
	end   = token.Find   ("IN", start);	
	if (end != StudioTD_String::NPos){

		pred = token.SubStrBasis (end,end+2);
		end = end + 2;
		return SAPDB_TRUE;
	}
	end   = token.Find   ("SOUNDS", start);	
	if (end != StudioTD_String::NPos){

		pred = token.SubStrBasis (end,end+6);
		end = end + 6;
		return SAPDB_TRUE;
	}

	end   = token.Find   ("LIKE", start);	
	if (end != StudioTD_String::NPos){

		pred = token.SubStrBasis (end,end+4);
		end = end + 4;
		return SAPDB_TRUE;
	}
	end   = token.Find   ("BETWEEN", start);	
	if (end != StudioTD_String::NPos){

		pred = token.SubStrBasis (end,end+7);
		end = end + 7;
		return SAPDB_TRUE;
	}

	return SAPDB_FALSE;
}

/*===========================================================================*
 *     StudioTD_ConstraintDef::parseConditions                               *
 *===========================================================================*/
void StudioTD_ConstraintDef::parseConditions (StudioTD_String conditions)
{
  Studio_List            tokens;
  Studio_ListData        t;
  StudioTD_String        token;
  StudioTD_String        sep;
  StudioTD_String        s;
  StudioTD_String::BasisElementIndex	start = 0;
  StudioTD_String::BasisElementIndex	end = 0;
  SAPDB_Bool             bColumn = true;
  SAPDB_Bool             bNew = true;
  StudioTD_ConditionDef *cond = NULL;
  
  /*-------------------------------------------------------------------------*
   * Split the given string into the single conditions.                      *
   *-------------------------------------------------------------------------*/
  while ((end = nextLogOperator (conditions, start)) != StudioTD_String::NPos)
  {
    tokens.append (new StudioTD_String (conditions.SubStr (start, end - start).Trim()));
    start = end + 1;
  }

  tokens.append (new StudioTD_String (conditions.SubStr (start).Trim()));
  
  /*-------------------------------------------------------------------------*
   * Create the condition with the tokens.                                   *
   *-------------------------------------------------------------------------*/
  for (t = tokens.first(); t; t = tokens.next())
  {
    token = *(StudioTD_String *)t;

    cond = addCondition();
    
    /*-----------------------------------------------------------------------*
     * Extract the column name.                                              *
     *-----------------------------------------------------------------------*/
    if (token[0] == (SAPDB_UTF8)'"')
    {
	  end = token.Find ("\"", 1);
      cond->setColumn (token.SubStrBasis (1, end));

  	  start = end+1;
	  if(extractPredicate(token,start,s,end) == SAPDB_FALSE){
			return;
	  }	
    }
    else
    {
      if (isFunction (token)) //e.g. SubStr
      {
        end = token.Find (")") + 1;
        cond->setColumn  (token.SubStrBasis (0 , end));

		//set new start value to new end value
        start = end;
		/*if there are no " then find predicate first and extract columname*/
		if(extractPredicate(token,start,s,end) == SAPDB_FALSE){
			return;
		}		
      }
      else
      {
		/*if there are no " then find predicate first and extract columname*/
		if(extractPredicate(token,start,s,end) == SAPDB_FALSE){
			return;
		}		
        cond->setColumn  (token.SubStrBasis (0, end-s.Length()));        		
      }
    }

	//set new start value to end value		
	start = end;        
	if (token[start] == (SAPDB_UTF8)' '){
		//skip next space
		start ++;
	}

    
    /*-----------------------------------------------------------------------*
     * Extract the predicate and the expression.                             *
     *-----------------------------------------------------------------------*/   
    
    if ((s == "=") || (s == "<")  || (s == "<=") || (s == ">") || (s == ">=") || (s == "<>"))
    {
      cond->setPredicate    (s);
      end = nextLogOperator (token, start);
      if (end == StudioTD_String::NPos)
        cond->setExpression (token.SubStr (start).Trim());
      else
      {
        end = token.RFind    (" ") + 1;
        cond->setExpression  (token.SubStrBasis (start, end - 1));
        cond->setLogOperator (token.SubStr (end));
      }
      continue;
    }
    
    if (s == "IS")
    {
      end   = token.Find   (" ", start);
	  //if there is no space more. take up to the end
	  if (end == StudioTD_String::NPos){
		s = token.SubStr (start);
		s.Trim();
	  }
	  else
		s = token.SubStrBasis (start,end);

      start = end + 1;
    }
    
    if (s == "NOT")
    {
      cond->setNotOperator (true);

      end   = token.Find   (" ", start);
      //if there is no space more. take up to the end
	  if (end == StudioTD_String::NPos){
		 s = token.SubStr (start);
		 s.Trim();
	  }
	  else
		s = token.SubStrBasis (start,end);

      start = end + 1;
    }

    if ((s == "TRUE") || (s == "FALSE") || (s == "NULL"))
    {
      cond->setPredicate    (s);
      cond->setNoExpression (true);
      end = nextLogOperator (token, start + s.Length());
      if (end != StudioTD_String::NPos)
      {
        end = token.RFind    (" ") + 1;
        cond->setLogOperator (token.SubStr (end).Trim());
      }
	  start = end + 1;

      continue;
    }

    if (s == "IN")
    {
      cond->setPredicate (s);
      end = nextLogOperator (token, start);
      if (end == StudioTD_String::NPos){
			
        StudioTD_String sTemp = token.SubStr(start + 1);
		sTemp.Trim();
        cond->setExpression (sTemp.SubStr(0,sTemp.Length()-1));
	  }
      else
      {
        end = token.RFind    (" ") + 1;
        cond->setExpression  (token.SubStrBasis (start + 1, end - 3));
        cond->setLogOperator (token.SubStr (end));
      }
	  start = end + 1;

      continue;
    }

    if (s == "BETWEEN")
    {
      cond->setPredicate    (s);
      end = token.Find      (" AND", start) - 1;
      end = nextLogOperator (token, end + 5);
      if (end == StudioTD_String::NPos)
        cond->setExpression (token.SubStr (start));
      else
      {
        end = token.RFind    (" ") + 1;
        cond->setExpression  (token.SubStrBasis (start, end - 1));
        cond->setLogOperator (token.SubStr (end));
      }
      continue;
    }

	if (s == "SOUNDS")
    {
      cond->setPredicate    ("LIKE");
	  end   = token.Find   ("LIKE", start);		  
	  if (end != StudioTD_String::NPos){
	  
		  s = token.SubStrBasis (end,end+4);
		  start = end + s.Length();			
		  end = nextLogOperator (token, start +s.Length());
		  if (end == StudioTD_String::NPos)
		  {		
			s  = token.SubStr (start);			
			s += " (SOUNDS)";
			cond->setExpression (s);
		  }
		  else
		  {
			end = token.RFind (" ") + 1;
			s  = token.SubStrBasis (start, end - 2);
			s += "(SOUNDS)";
			cond->setExpression  (s);
			cond->setLogOperator (token.SubStr (end));
		  }
	  }
    }

    if (s == "LIKE")
    {
      cond->setPredicate    (s);
      end = nextLogOperator (token, start);
      if (end == StudioTD_String::NPos)
        cond->setExpression (token.SubStr (start));
      else
      {
        end = token.RFind    (" ") + 1;
        cond->setExpression  (token.SubStrBasis (start, end - 1));
        cond->setLogOperator (token.SubStr (end));
      }
      continue;
    }    
  }
}



/*===========================================================================*
 *     StudioTD_ConstraintDef::isFunction                                    *
 *===========================================================================*/
SAPDB_Bool StudioTD_ConstraintDef::isFunction (StudioTD_String str)
{
  SAPDB_UInt  i, pos;
  
  pos = str.FindFirstOf (" \t(");
  if (pos != StudioTD_String::NPos)
    str = str.SubStrBasis (0, pos);
  
  for (i = 0; i < sizeof(functions)/sizeof(functions[0]); i++)
  {
    if (str == functions[i])
      return true;
  }
  
  return false;
}


/*===========================================================================*
 *     StudioTD_ConstraintDef::nextLogOperator                               *
 *===========================================================================*/
SAPDB_Int StudioTD_ConstraintDef::nextLogOperator (StudioTD_String str, SAPDB_UInt start)
{
  SAPDB_UInt   orPos, andPos, betweenPos, p;
  
  orPos      = str.Find (" OR", start);
  andPos     = str.Find (" AND", start);
  betweenPos = str.Find (" BETWEEN", start);

  if (betweenPos != StudioTD_String::NPos)
  {
    p = str.Find (" AND", betweenPos);

    if (p == andPos)
    {
      andPos = str.Find (" AND", p + 1);
    }
  }
  
  if ((orPos == StudioTD_String::NPos) && (andPos == StudioTD_String::NPos))
    return StudioTD_String::NPos;
  
  if (orPos == StudioTD_String::NPos)
    return andPos + 4;
  
  if (andPos == StudioTD_String::NPos)
    return orPos + 3;
  
  if (orPos < andPos)
    return orPos + 3;
  else
    return andPos + 4;
}


/*===========================================================================*
 *     StudioTD_ConstraintDef::genDescription                                *
 *===========================================================================*/
StudioTD_String StudioTD_ConstraintDef::genDescription ()
{
  StudioTD_String        desc;
  StudioTD_ConditionDef *cond;

  desc  = mName;
  desc += SEP_LEVEL_3;

  /*-------------------------------------------------------------------------*
   * For each item in the conditions list add its part of the description.   *
   *-------------------------------------------------------------------------*/
  desc += "conditions:::";
  for (cond = (StudioTD_ConditionDef *)mConditions.first(); 
       cond; 
       cond = (StudioTD_ConditionDef *)mConditions.next())
  {
    desc += cond->genDescription();
    desc += SEP_LEVEL_4;
  }

  desc += SEP_LEVEL_3;
  
  return desc;
}


/*===========================================================================*
 *     StudioTD_ConstraintDef::parseDescription                              *
 *===========================================================================*/
void StudioTD_ConstraintDef::parseDescription (StudioTD_String description)
{
  SAPDB_UInt             start = 0, end;
  StudioTD_String        conditions;
  StudioTD_ConditionDef *cond;

  mName      = NextToken (description, SEP_LEVEL_3, &start);
  conditions = NextToken (description, SEP_LEVEL_3, &start);

  /*-------------------------------------------------------------------------*
   * Split the foreign key description into its parts (sep. SEP_LEVEL_4).    *
   *-------------------------------------------------------------------------*/
  start = 0;
  while ((end = conditions.Find (SEP_LEVEL_4, start)) != StudioTD_String::NPos)
  {
    if (conditions.SubStr (start, 13) == "conditions:::")
      start += 13;
    
    cond = addCondition();
    cond->parseDescription (conditions.SubStrBasis (start, end));
    start = end + 1;
  }
}


/*===========================================================================*
 *     StudioTD_ConstraintDef::hasChanged                                    *
 *===========================================================================*/
SAPDB_Bool StudioTD_ConstraintDef::hasChanged ()
{
  Studio_ListData  cond;

  /*-------------------------------------------------------------------------*
   * For each item in the conditions list check if it was changed or added.  *
   *-------------------------------------------------------------------------*/
  for (cond = mConditions.first(); cond; cond = mConditions.next())
  {
    if (   (((StudioTD_ConditionDef *)cond)->alterType() == AlterChange) 
        || (((StudioTD_ConditionDef *)cond)->alterType() == AlterAdd))
      return true;
  }
  
  return false;
}


/*===========================================================================*
 *     StudioTD_ConstraintDef::isDirty                                       *
 *===========================================================================*/
SAPDB_Bool StudioTD_ConstraintDef::isDirty ()
{
  Studio_ListData  cond;

  /*-------------------------------------------------------------------------*
   * Check the dirty flag of the constraint.                                 *
   *-------------------------------------------------------------------------*/
  if (mDirty)
    return true;
  
  /*-------------------------------------------------------------------------*
   * Check the dirty flags of the conditions.                                *
   *-------------------------------------------------------------------------*/
  for (cond = mConditions.first(); cond; cond = mConditions.next())
  {
    if (((StudioTD_ConditionDef *)cond)->isDirty())
      return true;
  }
  
  return false;
}


/*===========================================================================*
 *     StudioTD_ConstraintDef::resetDirty                                    *
 *===========================================================================*/
void StudioTD_ConstraintDef::resetDirty ()
{
  Studio_ListData  cond;

  /*-------------------------------------------------------------------------*
   * Reset the dirty flag of the constraint.                                 *
   *-------------------------------------------------------------------------*/
  mDirty = false;
  
  /*-------------------------------------------------------------------------*
   * Reset the dirty flags of the conditions.                                *
   *-------------------------------------------------------------------------*/
  for (cond = mConditions.first(); cond; cond = mConditions.next())
    ((StudioTD_ConditionDef *)cond)->resetDirty();
}
