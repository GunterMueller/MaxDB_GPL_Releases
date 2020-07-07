/*!===========================================================================

  module:       StudioTD_ConditionDef.h

  ------------------------------------------------------------------------------

  responsible:  MarkusS

  special area: SQL Studio

  description:  Definition of the condition definition class StudioTD_ConditionDef.

  created:      2000-05-12

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

#ifndef STUDIOTD_CONDITIONDEF_H
#define STUDIOTD_CONDITIONDEF_H


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
  class: StudioTD_ConditionDef
 
  description: 
    Class for managing a condition definition.

    It contains all the attributes of a condition definition and methods for
    creating SQL statements (CREATE or ALTER TABLE).
============================================================================*/
class StudioTD_ConditionDef : public StudioTD_Base
{
public:
  /*!-------------------------------------------------------------------------
    function:     StudioTD_ConditionDef
    description:  Constructor of the class.
  --------------------------------------------------------------------------*/
  StudioTD_ConditionDef ();
  /*!-------------------------------------------------------------------------
    function:     ~StudioTD_ConditionDef
    description:  Destructor of the class.
  --------------------------------------------------------------------------*/
  ~StudioTD_ConditionDef();

  
  /*!-------------------------------------------------------------------------
    function:     column
    description:  Get the value of the column attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String column () { return mColumn; }
  /*!-------------------------------------------------------------------------
    function:     setColumn
    description:  Set the value of the column attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setColumn (StudioTD_String column)
  {
    SET_DIRTYFLAG(mColumn, column);
    SET_ATTRIBUTE(mColumn, column);
  }
  /*!-------------------------------------------------------------------------
    function:     predicate
    description:  Get the value of the predicate attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String predicate () { return mPredicate; }
  /*!-------------------------------------------------------------------------
    function:     setPredicate
    description:  Set the value of the predicate attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setPredicate (StudioTD_String predicate)
  {
    SET_DIRTYFLAG(mPredicate, predicate);
    SET_ATTRIBUTE(mPredicate, predicate);
  }
  /*!-------------------------------------------------------------------------
    function:     notOperator
    description:  Get the value of the notOperator attribute.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  inline SAPDB_Bool notOperator () { return mNotOperator; }
  /*!-------------------------------------------------------------------------
    function:     setNotOperator
    description:  Set the value of the notOperator attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setNotOperator (SAPDB_Bool  notoperator)
  {
    SET_DIRTYFLAG(mNotOperator, notoperator);
    SET_ATTRIBUTE(mNotOperator, notoperator);
  }
  /*!-------------------------------------------------------------------------
    function:     expression
    description:  Get the value of the expression attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String expression () { return mExpression; }
  /*!-------------------------------------------------------------------------
    function:     setExpression
    description:  Set the value of the expression attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setExpression (StudioTD_String expression)
  {
    SET_DIRTYFLAG(mExpression, expression);
    SET_ATTRIBUTE(mExpression, expression);
  }
  /*!-------------------------------------------------------------------------
    function:     noExpression
    description:  Get the value of the noExpression attribute.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  inline SAPDB_Bool noExpression () { return mNoExpression; }
  /*!-------------------------------------------------------------------------
    function:     setNoExpression
    description:  Set the value of the noExpression attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setNoExpression (SAPDB_Bool noexpression)
  {
    SET_DIRTYFLAG(mNoExpression, noexpression);
    SET_ATTRIBUTE(mNoExpression, noexpression);
  }
  /*!-------------------------------------------------------------------------
    function:     logOperator
    description:  Get the value of the logical operator attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String logOperator () { return mLogOperator; }
  /*!-------------------------------------------------------------------------
    function:     setLogOperator
    description:  Set the value of the logical operator attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setLogOperator (StudioTD_String  logoperator)
  {
    SET_DIRTYFLAG(mLogOperator, logoperator);
    SET_ATTRIBUTE(mLogOperator, logoperator);
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
    description:  Check if the condition definition is incomplete (column,
                  predicate or expression is missing).
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String findIncomplete ();
  /*!-------------------------------------------------------------------------
    function:     genCreateStatement
    description:  Generate a part of a CREATE statement for this condition with
                  the attributes of the condition.
    arguments:    mode - Creation context
                  casesensitive - Flag for case sensitive names
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genCreateStatement (StudioTD_Mode mode, SAPDB_Bool casesensitive);

  /*!-------------------------------------------------------------------------
    function:     genCreateXML
    description:  Generate a xml Element for this condition with
                  the attributes of the condition.
    arguments:    casesensitive - Flag for case sensitive names
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genCreateXML ( SAPDB_Bool casesensitive );
  /*!-------------------------------------------------------------------------
    function:     genDescription
    description:  Generate a description of the condition definition that can be
                  stored as a draft.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genDescription ();
  /*!-------------------------------------------------------------------------
    function:     parseDescription
    description:  Parse a given description of a condition definition that was
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
  StudioTD_String           mColumn;               // Name of the column
  StudioTD_String           mPredicate;            // Predicate for the column
  SAPDB_Bool                mNotOperator;          // Flag for the NOT operator of the predicate
  StudioTD_String           mExpression;           // Expression of the conditions
  SAPDB_Bool                mNoExpression;         // Flag if no expression is required
  StudioTD_String           mLogOperator;          // Logical operator between this and next condition
  StudioTD_AlterType        mAlterType;            // only for AlterTable
  SAPDB_Bool                mDirty;                // Flag for indicating modified attributes

};  /*! endclass: StudioTD_ConditionDef */

#endif   /* STUDIOTD_CONDITIONDEF_H */
