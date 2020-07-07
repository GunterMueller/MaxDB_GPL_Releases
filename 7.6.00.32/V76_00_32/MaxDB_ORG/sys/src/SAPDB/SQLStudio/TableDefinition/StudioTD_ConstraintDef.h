/*!===========================================================================

  module:       StudioTD_ConstraintDef.h

  ------------------------------------------------------------------------------

  responsible:  MarkusS

  special area: SQL Studio

  description:  Definition of the constraint definition class StudioTD_ConstraintDef.

  version:      7.4

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

#ifndef STUDIOTD_CONSTRAINTDEF_H
#define STUDIOTD_CONSTRAINTDEF_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SQLStudio/Studio_List.hpp"

#include "SQLStudio/TableDefinition/StudioTD_Global.h"
#include "SQLStudio/TableDefinition/StudioTD_Util.h"
#include "SQLStudio/TableDefinition/StudioTD_Base.h"
#include "SQLStudio/TableDefinition/StudioTD_ConditionDef.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*!===========================================================================
  class: StudioTD_ConstraintDef
 
  description: 
    Class for managing a constraint definition.

    It contains all the attributes of a constraint definition and methods for
    creating SQL statements (CREATE or ALTER TABLE).
============================================================================*/
class StudioTD_ConstraintDef : public StudioTD_Base
{
public:
  /*!-------------------------------------------------------------------------
    function:     StudioTD_ConstraintDef
    description:  Constructor of the class.
  --------------------------------------------------------------------------*/
  StudioTD_ConstraintDef ();
  /*!-------------------------------------------------------------------------
    function:     StudioTD_ConstraintDef
    description:  Copy constructor of the class.
  --------------------------------------------------------------------------*/
  StudioTD_ConstraintDef (const StudioTD_ConstraintDef& constr);
  /*!-------------------------------------------------------------------------
    function:     ~StudioTD_ConstraintDef
    description:  Destructor of the class.
  --------------------------------------------------------------------------*/
  ~StudioTD_ConstraintDef();

  
  /*!-------------------------------------------------------------------------
    function:     name
    description:  Get the value of the constraint name attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String name () { return mName; }
  /*!-------------------------------------------------------------------------
    function:     setName
    description:  Set the value of the constraint name attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setName (StudioTD_String name) { SET_DIRTYFLAG(mName, name); mName = name; }

  /*!-------------------------------------------------------------------------
    function:     definition
    description:  Get the value of the constraint definition attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String definition () { return mDefinition; }
  /*!-------------------------------------------------------------------------
    function:     setDefinition
    description:  Set the value of the constraint name attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setDefinition (StudioTD_String definition) {  mDefinition = definition; }

  /*!-------------------------------------------------------------------------
    function:     setAlterType
    description:  Set the value of the alterType attribute to the given value.
  --------------------------------------------------------------------------*/
  void setAlterType (StudioTD_AlterType type);
  /*!-------------------------------------------------------------------------
    function:     alterType
    description:  Get the value of the alterType attribute.
  --------------------------------------------------------------------------*/
  StudioTD_AlterType alterType ();

  
  /*!-------------------------------------------------------------------------
    function:     addCondition
    description:  Create a new empty condition definition and append it to the
                  conditions list.
    returns:      StudioTD_ConditionDef *
  --------------------------------------------------------------------------*/
  StudioTD_ConditionDef * addCondition ();
  /*!-------------------------------------------------------------------------
    function:     removeCondition
    description:  Removee the element with the given index from the list.
    arguments:    index - Index of the element to remove
  --------------------------------------------------------------------------*/
  void removeCondition (SAPDB_UInt index);
  /*!-------------------------------------------------------------------------
    function:     condition
    description:  Get a reference to the element with the given index.
    arguments:    index - Index of the element to get
    returns:      StudioTD_ConditionDef *
  --------------------------------------------------------------------------*/
  StudioTD_ConditionDef * condition (SAPDB_UInt index);
  /*!-------------------------------------------------------------------------
    function:     conditionCount
    description:  Get the number of elements in the conditions list.
    returns:      SAPDB_UInt
  --------------------------------------------------------------------------*/
  SAPDB_UInt conditionCount ();
  
  
  /*!-------------------------------------------------------------------------
    function:     findInomplete
    description:  Check if the constraint definition is incomplete (any 
                  incomplete conditions).
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String findIncomplete ();
  /*!-------------------------------------------------------------------------
    function:     genCreateStatement
    description:  Generate a CREATE statement for this constraint with the
                  attributes of the constraint.
    arguments:    mode - Creation context
                  casesensitive - Flag for case sensitive names
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genCreateStatement (StudioTD_Mode mode, SAPDB_Bool casesensitive);

   StudioTD_String genCreateXML      (StudioTD_Mode mode, SAPDB_Bool casesensitive);


   /*!-------------------------------------------------------------------------
    function:     genAlterStatement
    description:  Generate a part of a ALTER statement for this constraint with
                  the changed attributes of the constraint.
    arguments:    casesensitive - Flag for case sensitive names
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genAlterStatement (SAPDB_Bool casesensitive);
  /*!-------------------------------------------------------------------------
    function:     parceConditions
    description:  Parse the given string which contains one or more condition
                  definitions.
    arguments:    conditions - The conditions string
  --------------------------------------------------------------------------*/
  void parseConditions (StudioTD_String conditions);
  /*!-------------------------------------------------------------------------
    function:     genDescription
    description:  Generate a description of the constraint definition that
                  can be stored as a draft.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genDescription ();
  /*!-------------------------------------------------------------------------
    function:     parseDescription
    description:  Parse a given description of a constraint definition that
                  was stored as a draft.
    arguments:    description - Description to be parsed
  --------------------------------------------------------------------------*/
  void parseDescription (StudioTD_String description);
  /*!-------------------------------------------------------------------------
    function:     hasChanged
    description:  Check if the constraint has changed conditions.
    return:       SAPDB_Bool
  --------------------------------------------------------------------------*/
  SAPDB_Bool hasChanged ();
  /*!-------------------------------------------------------------------------
    function:     hasAdded
    description:  Check if the constraint has adanged conditions.
    return:       SAPDB_Bool
  --------------------------------------------------------------------------*/
  SAPDB_Bool hasAdded ();
  /*!-------------------------------------------------------------------------
    function:     isFunction
    description:  Check if the beginning of the given string is a function
                  name.
    arguments:    str - String to be checked
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  SAPDB_Bool isFunction (StudioTD_String str);
  /*!-------------------------------------------------------------------------
    function:     isDirty
    description:  Check if the dirty flag is set for at least one of the
                  conditions.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  SAPDB_Bool isDirty ();
  /*!-------------------------------------------------------------------------
    function:     resetDirty
    description:  Reset the dirty flag of all conditions.
  --------------------------------------------------------------------------*/
  void resetDirty ();
  
  
private:
  /*!-------------------------------------------------------------------------
    function:     nextLogOperator
    description:  Search for the next logical operator (AND or OR) in the 
                  given string.
    arguments:    str - String to be searched
                  start - Starting point
    returns:      SAPDB_Int
  --------------------------------------------------------------------------*/
  SAPDB_Int nextLogOperator (StudioTD_String str, SAPDB_UInt start);

  /*!-------------------------------------------------------------------------
    function:     extractPredicate
    description:  extract the next predicate out of token stream

    arguments:    [in] token - String to be searched
                  [in] start - Starting point
				  [out] pred - found predicate
				  [out] end - search position in token

    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  SAPDB_Bool extractPredicate(const StudioTD_String& token,
							const SAPDB_UInt &start,
							StudioTD_String &pred,
							SAPDB_UInt &end);

  
  StudioTD_String           mName;                 // Name of the constraint
  StudioTD_String           mDefinition;           // Definition of the constraint
  SAPDB_Bool                mDirty;                // Flag for indicating modified attributes

  Studio_List                mConditions;           // List of conditions of constraint
  Studio_List                mRemovedConditions;    // List of removed conditions
  
};  /*! endclass: StudioTD_ConstraintDef */

#endif   /* STUDIOTD_CONSTRAINTDEF_H */
