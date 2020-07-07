/*!===========================================================================

  module:       StudioTD_Base.h

  ------------------------------------------------------------------------------

  responsible:  MarkusS

  special area: SQL Studio

  description:  Definition of the base class StudioTD_Base.

  created:      2000-06-19

  updated:      2000-06-19

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

#ifndef STUDIOTD_BASE_H
#define STUDIOTD_BASE_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*!===========================================================================
  class: StudioTD_Base
 
  description: 
    Base class for holding a type string of a derived class.

    This is used for checking the type of the class. This is required
    especially for use in a C-DLL (from Visual Basic), because of missing
    type information.
============================================================================*/
class StudioTD_Base
{
public:
  /*!-------------------------------------------------------------------------
    declaration:  StudioTD_ClassType
    description:  Types of derived classes.
  --------------------------------------------------------------------------*/
  typedef enum _studiotd_classtype
  {
    ClassGeneric,
    ClassTableDef,
    ClassTableDefODBC,
    ClassTableDefOI,
    ClassColumnDef,
    ClassConstraintDef,
    ClassConditionDef,
    ClassForeignKeyDef
    
  } StudioTD_ClassType;
  /*!-------------------------------------------------------------------------
    function:     StudioTD_Base
    description:  Constructor of the class.
  --------------------------------------------------------------------------*/
  StudioTD_Base () {}
  /*!-------------------------------------------------------------------------
    function:     ~StudioTD_Base
    description:  Destructor of the class.
  --------------------------------------------------------------------------*/
  ~StudioTD_Base() {}
  /*!-------------------------------------------------------------------------
    function:     typeOf
    description:  Get the type of the class.
    returns:      StudioTD_ClassType
  --------------------------------------------------------------------------*/
  virtual StudioTD_ClassType typeOf () { return mType; }

protected:
  /*!-------------------------------------------------------------------------
    function:     setType
    description:  Set the type of the class.
  --------------------------------------------------------------------------*/
  void setType (StudioTD_ClassType type) { mType = type; }

private:
  StudioTD_ClassType        mType;                 // Type of the class

};  /*! endclass: STUDIOTD_Base */

#endif   /* StudioTD_BASE_H */
