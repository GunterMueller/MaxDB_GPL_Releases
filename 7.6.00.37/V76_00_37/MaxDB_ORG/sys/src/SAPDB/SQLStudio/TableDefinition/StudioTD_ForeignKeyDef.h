/*!===========================================================================

  module:       StudioTD_ForeignKeyDef.h

  ------------------------------------------------------------------------------

  responsible:  MarkusS,WolfgangA

  special area: SQL Studio

  description:  Definition of the foreign key definition class StudioTD_ForeignKeyDef.

  see:          www.sapdb.org

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

#ifndef STUDIOTD_FOREIGNKEYDEF_H
#define STUDIOTD_FOREIGNKEYDEF_H


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
  class: StudioTD_ForeignKeyDef
 
  description: 
    Class for managing a foreign key definition.

    It contains all the attributes of a foreign key definition and methods for
    creating SQL statements (CREATE or ALTER TABLE).
============================================================================*/
class StudioTD_ForeignKeyDef : public StudioTD_Base
{
public:
  /*!-------------------------------------------------------------------------
    function:     StudioTD_ForeignKeyDef
    description:  Constructor of the class.
  --------------------------------------------------------------------------*/
  StudioTD_ForeignKeyDef ();
  /*!-------------------------------------------------------------------------
    function:     ~StudioTD_ForeignKeyDef
    description:  Destructor of the class.
  --------------------------------------------------------------------------*/
  ~StudioTD_ForeignKeyDef();

  
  /*!-------------------------------------------------------------------------
    function:     name
    description:  Get the value of the foreign key name attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String name () { return mName; }
  /*!-------------------------------------------------------------------------
    function:     setName
    description:  Set the value of the foreign key name attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setName (StudioTD_String name)
  {
    SET_DIRTYFLAG(mName, name);
    SET_ATTRIBUTE(mName, name);
  }
  /*!-------------------------------------------------------------------------
    function:     referencingColumns
    description:  Get the value of the referencing columns attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String referencingColumns () { return mRefColumns; }
  /*!-------------------------------------------------------------------------
    function:     setReferencingColumns
    description:  Set the value of the referencing columns attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setReferencingColumns (StudioTD_String columns)
  {
    SET_DIRTYFLAG(mRefColumns, columns);
    SET_ATTRIBUTE(mRefColumns, columns);
  }
  /*!-------------------------------------------------------------------------
    function:     reference
    description:  Get the value of the reference attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String reference () { return mReference; }
  /*!-------------------------------------------------------------------------
    function:     setReference
    description:  Set the value of the reference attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setReference (StudioTD_String reference)
  {
    SET_DIRTYFLAG(mReference, reference);
    SET_ATTRIBUTE(mReference, reference);
  }
  /*!-------------------------------------------------------------------------
    function:     onDelete
    description:  Get the value of the OnDelete attribute.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  inline StudioTD_String onDelete () { return mOnDelete; }
  /*!-------------------------------------------------------------------------
    function:     setOnDelete
    description:  Set the value of the OnDelete attribute to the given value.
  --------------------------------------------------------------------------*/
  inline void setOnDelete (StudioTD_String ondelete)
  {
    SET_DIRTYFLAG(mOnDelete, ondelete);
    SET_ATTRIBUTE(mOnDelete, ondelete);
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
    description:  Check if the foreign key definition is incomplete
                  (referencing columns or reference is missing).
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String findIncomplete ();
  /*!-------------------------------------------------------------------------
    function:     genCreateStatement
    description:  Generate a part of a CREATE statement for this foreign key
                  with the attributes of the foreign key.
    arguments:    mode - Creation context
                  casesensitive - Flag for case sensitive names
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genCreateStatement (StudioTD_Mode mode, SAPDB_Bool casesensitive);

  /*!-------------------------------------------------------------------------
    function:     genCreateXML
    description:  Generate a Foreign Key XML Element
                  with the attributes of the foreign key.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genCreateXML      (StudioTD_String owner,StudioTD_String table, SAPDB_Bool casesensitive);

  /*!-------------------------------------------------------------------------
    function:     genAlterStatement
    description:  Generate a part of a ALTER statement for this foreign key
                  with the changed attributes of the foreign key.
    arguments:	  owner, Table owner
				  table - Table name
                  casesensitive - Flag for case sensitive names
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genAlterStatement (StudioTD_String owner,StudioTD_String table, SAPDB_Bool casesensitive);
  /*!-------------------------------------------------------------------------
    function:     genDescription
    description:  Generate a description of the foreign key definition that
                  can be stored as a draft.
    returns:      StudioTD_String
  --------------------------------------------------------------------------*/
  StudioTD_String genDescription ();
  /*!-------------------------------------------------------------------------
    function:     parseDescription
    description:  Parse a given description of a foreign key definition that
                  was stored as a draft.
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

  /*!-------------------------------------------------------------------------
    function:     isSep
    description:  Check if the sep flag is set.
    returns:      SAPDB_Bool
   --------------------------------------------------------------------------*/
  inline SAPDB_Bool isSep () { return mSep; }
  /*!-------------------------------------------------------------------------
    function:     resetSep
    description:  Reset the sep flag.
  --------------------------------------------------------------------------*/
  inline void resetSep () { mSep = false; }



private:
  StudioTD_String           mName;                 // Name of the foreign key
  StudioTD_String           mRefColumns;           // Referencing columns of the foreign key
  StudioTD_String           mReference;            // Reference of the foreign key
  StudioTD_String           mOnDelete;             // OnDelete clause of the foreign keys
  StudioTD_String           mComment;              // Comment for the foreign key
  StudioTD_AlterType        mAlterType;            // only for AlterTable
  SAPDB_Bool                mDirty;                // Flag for indicating modified attributes
  SAPDB_Bool                mSep;                  // Flag for indicating separator

};  /*! endclass: StudioTD_ForeignKeyDef */

#endif   /* STUDIOTD_FOREIGNKEYDEF_H */
