/*!===========================================================================

  module:       StudioTD_Wrapper.cpp

  ------------------------------------------------------------------------------

  responsible:  MarkusS,WolfgangA

  special area: SQL Studio

  description:  Implementation of the C wrapper functions.

  see:          http://www.sapdb.org

  created:      2000-06-07

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SQLStudio/TableDefinition/StudioTD_Wrapper.h"


/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::StudioTD_TableDef().             *
 *===========================================================================*/
void * __stdcall table_new()
{
  return new StudioTD_WTableDefODBC;
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::StudioTD_TableDef().             *
 *===========================================================================*/
void * __stdcall table_copy(void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return new StudioTD_WTableDefODBC (*me);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::~StudioTD_TableDef().            *
 *===========================================================================*/
void __stdcall table_delete (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  if (me)
    delete me;
}
/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::owner().                          *
 *===========================================================================*/
SAPDB_Int __stdcall table_owner (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  SAPDB_UCS2              *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  assert (out);

  StudioTD_String sbuf = me->owner();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);
  
  return (sbuf.Size());
}
/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setOwner().                       *
 *===========================================================================*/
void __stdcall table_setOwner (void *_this, SAPDB_UCS2 *name)
{
  StudioTD_String         s;
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);

  if (name)
    s.ConvertFromUCS2 (name, name + wcslen(name));
  else
    s = "";
  
  me->setOwner (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::name().                          *
 *===========================================================================*/
SAPDB_Int __stdcall table_name (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  assert (out);

  StudioTD_String sbuf = me->name();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);
  
  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setName().                       *
 *===========================================================================*/
void __stdcall table_setName (void *_this, SAPDB_UCS2 *name)
{
  StudioTD_String         s;
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);

  if (name)
    s.ConvertFromUCS2 (name, name + wcslen(name));
  else
    s = "";
  
  me->setName (s);
}


/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::mode().                          *
 *===========================================================================*/
SAPDB_UInt __stdcall table_mode (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return (SAPDB_UInt)me->mode();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setMode().                       *
 *===========================================================================*/
void __stdcall table_setMode (void *_this, SAPDB_UInt mode)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  me->setMode ((StudioTD_Mode)mode);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::basedOn().                       *
 *===========================================================================*/
SAPDB_Int __stdcall table_basedOn (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  assert (out);
  
  StudioTD_String sbuf = me->basedOn();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setBasedOn().                    *
 *===========================================================================*/
void __stdcall table_setBasedOn (void *_this, SAPDB_UCS2 *basedon)
{
  StudioTD_String         s;
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);

  if (basedon)
    s.ConvertFromUCS2 (basedon, basedon + wcslen(basedon));
  else
    s = "";

  me->setBasedOn (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::createAs().                      *
 *===========================================================================*/
SAPDB_Int __stdcall table_createAs (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  assert (out);
  
  StudioTD_String sbuf = me->createAs();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setCreateAs().                   *
 *===========================================================================*/
void __stdcall table_setCreateAs (void *_this, SAPDB_UCS2 *createas)
{
  StudioTD_String         s;
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);

  if (createas)
    s.ConvertFromUCS2 (createas, createas + wcslen(createas));
  else
    s = "";

  me->setCreateAs (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::sampleValue().                   *
 *===========================================================================*/
SAPDB_UInt __stdcall table_sampleValue (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->sampleValue();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setSampleValue().                *
 *===========================================================================*/
void __stdcall table_setSampleValue (void *_this, SAPDB_UInt value)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  me->setSampleValue (value);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::sampleUnit().                    *
 *===========================================================================*/
SAPDB_Int __stdcall table_sampleUnit (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  assert (out);
  
  StudioTD_String sbuf = me->sampleUnit();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setSampleUnit().                 *
 *===========================================================================*/
void __stdcall table_setSampleUnit (void *_this, SAPDB_UCS2 *unit)
{
  StudioTD_String         s;
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  if (unit)
    s.ConvertFromUCS2 (unit, unit + wcslen(unit));
  else
    s = "";

  me->setSampleUnit (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::duplicates().                    *
 *===========================================================================*/
SAPDB_Int __stdcall table_duplicates (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  assert (out);
  
  StudioTD_String sbuf = me->duplicates();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setDuplicates().                 *
 *===========================================================================*/
void __stdcall table_setDuplicates (void *_this, SAPDB_UCS2 *duplicates)
{
  StudioTD_String         s;
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  if (duplicates)
    s.ConvertFromUCS2 (duplicates, duplicates + wcslen(duplicates));
  else
    s = "";

  me->setDuplicates (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::temporary().                     *
 *===========================================================================*/
SAPDB_Bool __stdcall table_temporary (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->temporary();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setTemporary().                  *
 *===========================================================================*/
void __stdcall table_setTemporary (void *_this, SAPDB_Bool temporary)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  me->setTemporary (temporary);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::ignoreRollback().                *
 *===========================================================================*/
SAPDB_Bool __stdcall table_ignoreRollback (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->ignoreRollback();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setIgnoreRollback().             *
 *===========================================================================*/
void __stdcall table_setIgnoreRollback (void *_this, SAPDB_Bool ignorerollback)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  me->setIgnoreRollback (ignorerollback);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::caseSensitive().                 *
 *===========================================================================*/
SAPDB_Bool __stdcall table_caseSensitive (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->caseSensitive();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setCaseSensitive().              *
 *===========================================================================*/
void __stdcall table_setCaseSensitive (void *_this, SAPDB_Bool casesensitive)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  me->setCaseSensitive (casesensitive);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::comment().                       *
 *===========================================================================*/
SAPDB_Int __stdcall table_comment (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  assert (out);
  
  StudioTD_String sbuf = me->comment();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setComment().                    *
 *===========================================================================*/
void __stdcall table_setComment (void *_this, SAPDB_UCS2 *comment)
{
  StudioTD_String         s;
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  if (comment)
    s.ConvertFromUCS2 (comment, comment + wcslen(comment));
  else
    s = "";

  me->setComment (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::ignoreIncomplete().              *
 *===========================================================================*/
SAPDB_Bool __stdcall table_ignoreIncomplete (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->ignoreIncomplete();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setIgnoreIncomplete().           *
 *===========================================================================*/
void __stdcall table_setIgnoreIncomplete (void *_this, SAPDB_Bool ignoreincomplete)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  me->setIgnoreIncomplete (ignoreincomplete);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setAlterType().                  *
 *===========================================================================*/
void __stdcall table_setAlterType (void *_this, SAPDB_UInt type)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  me->setAlterType ((StudioTD_AlterType)type);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::addColumn().                     *
 *===========================================================================*/
void * __stdcall table_addColumn (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->addColumn();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::removeColumn().                  *
 *===========================================================================*/
void __stdcall table_removeColumn (void *_this, SAPDB_UInt index)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  me->removeColumn (index);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::column().                        *
 *===========================================================================*/
void * __stdcall table_column (void *_this, SAPDB_UInt index)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->column (index);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::findColumn().                    *
 *===========================================================================*/
SAPDB_Int __stdcall table_findColumn (void *_this, SAPDB_UCS2 *name)
{
  StudioTD_String         s;
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  if (name)
    s.ConvertFromUCS2 (name, name + wcslen(name));
  else
    s = "";

  return me->findColumn (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::columnCount().                   *
 *===========================================================================*/
SAPDB_UInt __stdcall table_columnCount (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->columnCount();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::twistColumns().                  *
 *===========================================================================*/
void __stdcall table_twistColumns (void *_this, SAPDB_UInt index)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  me->twistColumns (index);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::addConstraint().                 *
 *===========================================================================*/
void * __stdcall table_addConstraint (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->addConstraint();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::removeConstraint().              *
 *===========================================================================*/
void __stdcall table_removeConstraint (void *_this, SAPDB_UInt index)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  me->removeConstraint (index);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::constraint().                    *
 *===========================================================================*/
void * __stdcall table_constraint (void *_this, SAPDB_UInt index)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->constraint (index);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::findConstraint().                *
 *===========================================================================*/
SAPDB_Int __stdcall table_findConstraint (void *_this, SAPDB_UCS2 *name)
{
  StudioTD_String         s;
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  if (name)
    s.ConvertFromUCS2 (name, name + wcslen(name));
  else
    s = "";

  return me->findConstraint (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::constraintCount().               *
 *===========================================================================*/
SAPDB_UInt __stdcall table_constraintCount (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->constraintCount();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::addForeignKey().                 *
 *===========================================================================*/
void * __stdcall table_addForeignKey (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->addForeignKey();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::removeForeignKey().              *
 *===========================================================================*/
void __stdcall table_removeForeignKey (void *_this, SAPDB_UInt index)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  me->removeForeignKey (index);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::foreignKey().                    *
 *===========================================================================*/
void * __stdcall table_foreignKey (void *_this, SAPDB_UInt index)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->foreignKey (index);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::findForeignKey().                *
 *===========================================================================*/
SAPDB_Int __stdcall table_findForeignKey (void *_this, SAPDB_UCS2 *name)
{
  StudioTD_String         s;
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  if (name)
    s.ConvertFromUCS2 (name, name + wcslen(name));
  else
    s = "";

  return me->findForeignKey (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::foreignKeyCount().               *
 *===========================================================================*/
SAPDB_UInt __stdcall table_foreignKeyCount (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->foreignKeyCount();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::foreignKeyCount().               *
 *===========================================================================*/
SAPDB_UInt __stdcall table_keyCount (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->keyCount();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::reset().                         *
 *===========================================================================*/
void __stdcall table_reset (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  me->reset();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::readTableDefinition().           *
 *===========================================================================*/
SAPDB_UInt __stdcall table_readTableDefinition (void *_this, SAPDB_UCS2 *tablename, SAPDB_UCS2 *owner,SAPDB_Bool ownerIsSchema)

{
  StudioTD_String         sTable;
  StudioTD_String         sOwner;
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  if (tablename)
    sTable.ConvertFromUCS2 (tablename, tablename + wcslen(tablename));
  else
    sTable = "";

  if (owner)
    sOwner.ConvertFromUCS2 (owner, owner + wcslen(owner));
  else
    sOwner = "";

  return me->readTableDefinition (sTable, sOwner,ownerIsSchema);

}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::hasValidColumns().               *
 *===========================================================================*/
SAPDB_Bool __stdcall table_hasValidColumns (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->hasValidColumns();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::isDirty().                       *
 *===========================================================================*/
SAPDB_Bool __stdcall table_isDirty (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->isDirty();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::resetDirty().                    *
 *===========================================================================*/
void __stdcall table_resetDirty (void *_this)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  me->resetDirty();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::findIncompleteDefs().            *
 *===========================================================================*/
SAPDB_Int __stdcall table_findIncompleteDefs (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  assert (out);
  
  StudioTD_String sbuf = me->findIncompleteDefs();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::genCreateStatement().            *
 *===========================================================================*/
SAPDB_Int __stdcall table_genCreateStatement (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  assert (out);
  
  StudioTD_String sbuf = me->genCreateStatement();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::genAlterStatement().             *
 *===========================================================================*/
SAPDB_Int __stdcall table_genAlterStatement (void *_this, void *oldTable, SAPDB_UCS2 *out)
{
  StudioTD_WTableDefODBC  *me  = (StudioTD_WTableDefODBC *)_this;
  StudioTD_WTableDefODBC  *old = (StudioTD_WTableDefODBC *)oldTable;
  SAPDB_UCS2             *end;

  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  assert (old);
  assert (old->typeOf() == StudioTD_Base::ClassTableDefODBC);
  assert (out);
  
  StudioTD_String sbuf = me->genAlterStatement(old);
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);
  

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::genDescription().                *
 *===========================================================================*/
SAPDB_Int __stdcall table_genDescription (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  assert (out);
  
  StudioTD_String sbuf = me->genDescription();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);


  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::parseDescription().              *
 *===========================================================================*/
void __stdcall table_parseDescription (void *_this, SAPDB_UCS2 *description)
{
  StudioTD_String         s;
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  if (description)
    s.ConvertFromUCS2 (description, description + wcslen(description));
  else
    s = "";

  me->parseDescription (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_TableDef::setConnection().                 *
 *===========================================================================*/
SAPDB_Bool __stdcall table_setConnection (void *_this, void *hdbc, void *henv)
{
  StudioTD_WTableDefODBC  *me = (StudioTD_WTableDefODBC *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassTableDefODBC);
  
  return me->setConnection (hdbc, henv);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::name().                         *
 *===========================================================================*/
SAPDB_Int __stdcall column_name (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  SAPDB_UCS2          *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  assert (out);
  
  StudioTD_String sbuf = me->name();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::setName().                      *
 *===========================================================================*/
void __stdcall column_setName (void *_this, SAPDB_UCS2 *name)
{
  StudioTD_String      s;
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  if (name)
    s.ConvertFromUCS2 (name, name + wcslen(name));
  else
    s = "";
  
  me->setName (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::oldName().                      *
 *===========================================================================*/
SAPDB_Int __stdcall column_oldName (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  SAPDB_UCS2          *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  assert (out);
  
  StudioTD_String sbuf = me->oldName();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::setOldName().                   *
 *===========================================================================*/
void __stdcall column_setOldName (void *_this, SAPDB_UCS2 *name)
{
  StudioTD_String      s;
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  if (name)
    s.ConvertFromUCS2 (name, name + wcslen(name));
  else
    s = "";

  me->setOldName (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::dataType().                     *
 *===========================================================================*/
SAPDB_Int __stdcall column_dataType (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  SAPDB_UCS2          *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  assert (out);
  
  StudioTD_String sbuf = me->dataType();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::setDataType().                  *
 *===========================================================================*/
void __stdcall column_setDataType (void *_this, SAPDB_UCS2 *datatype)
{
  StudioTD_String      s;
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  if (datatype)
    s.ConvertFromUCS2 (datatype, datatype + wcslen(datatype));
  else
    s = "";

  me->setDataType (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::dimension().                    *
 *===========================================================================*/
SAPDB_Int __stdcall column_dimension (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  SAPDB_UCS2          *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  assert (out);
  
  StudioTD_String sbuf = me->dimension();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::setDimension().                 *
 *===========================================================================*/
void __stdcall column_setDimension (void *_this, SAPDB_UCS2 *dimension)
{
  StudioTD_String      s;
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  if (dimension)
    s.ConvertFromUCS2 (dimension, dimension + wcslen(dimension));
  else
    s = "";

  me->setDimension (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::codeType().                     *
 *===========================================================================*/
SAPDB_Int __stdcall column_codeType (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  SAPDB_UCS2          *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  assert (out);
  
  StudioTD_String sbuf = me->codeType();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::setCodeType().                  *
 *===========================================================================*/
void __stdcall column_setCodeType (void *_this, SAPDB_UCS2 *codetype)
{
  StudioTD_String      s;
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  if (codetype)
    s.ConvertFromUCS2 (codetype, codetype + wcslen(codetype));
  else
    s = "";

  me->setCodeType (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::key().                          *
 *===========================================================================*/
SAPDB_Bool __stdcall column_key (void *_this)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  return me->key();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::setKey().                       *
 *===========================================================================*/
void __stdcall column_setKey (void *_this, SAPDB_Bool key)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  me->setKey (key);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::keyPos().                          *
 *===========================================================================*/
SAPDB_Int2 __stdcall column_keyPos (void *_this)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  return me->keyPos();
}


/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::setKeyPso().                       *
 *===========================================================================*/
void __stdcall column_setKeyPos (void *_this, SAPDB_Int2 keyPos)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  me->setKeyPos (keyPos);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::notNull().                      *
 *===========================================================================*/
SAPDB_Bool __stdcall column_notNull (void *_this)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  return me->notNull();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::setNotNull().                   *
 *===========================================================================*/
void __stdcall column_setNotNull (void *_this, SAPDB_Bool notnull)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  me->setNotNull (notnull);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::defaultValue().                 *
 *===========================================================================*/
SAPDB_Int __stdcall column_defaultValue (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  SAPDB_UCS2          *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  assert (out);
  
  StudioTD_String sbuf = me->defaultValue();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::setDefaultValue().              *
 *===========================================================================*/
void __stdcall column_setDefaultValue (void *_this, SAPDB_UCS2 *defaultvalue)
{
  StudioTD_String      s;
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  if (defaultvalue)
    s.ConvertFromUCS2 (defaultvalue, defaultvalue + wcslen(defaultvalue));
  else
    s = "";

  me->setDefaultValueEx (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::unique().                       *
 *===========================================================================*/
SAPDB_Bool __stdcall column_unique (void *_this)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  return me->unique();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::setUnique().                    *
 *===========================================================================*/
void __stdcall column_setUnique (void *_this, SAPDB_Bool unique)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  me->setUnique (unique);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::comment().                      *
 *===========================================================================*/
SAPDB_Int __stdcall column_comment (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  SAPDB_UCS2          *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  assert (out);
  
  StudioTD_String sbuf = me->comment();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::setComment().                   *
 *===========================================================================*/
void __stdcall column_setComment (void *_this, SAPDB_UCS2 *comment)
{
  StudioTD_String      s;
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  if (comment)
    s.ConvertFromUCS2 (comment, comment + wcslen(comment));
  else
    s = "";

  me->setComment (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::alterType().                    *
 *===========================================================================*/
SAPDB_UInt __stdcall column_alterType (void *_this)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  return me->alterType();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::setAlterType().                 *
 *===========================================================================*/
void __stdcall column_setAlterType (void *_this, SAPDB_UInt type)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  me->setAlterType ((StudioTD_AlterType)type);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::findIncomplete().               *
 *===========================================================================*/
SAPDB_Int __stdcall column_findIncomplete (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  SAPDB_UCS2          *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  assert (out);
  
  StudioTD_String sbuf = me->findIncomplete();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::genCreateStatement().           *
 *===========================================================================*/
SAPDB_Int __stdcall column_genCreateStatement (void *_this, SAPDB_UInt mode, 
                                               SAPDB_Bool casesensitive, SAPDB_UCS2 *out)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  SAPDB_UCS2          *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  assert (out);
  
  StudioTD_String sbuf = me->genCreateStatement((StudioTD_Mode)mode, casesensitive);
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}


/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::genDescription().               *
 *===========================================================================*/
SAPDB_Int __stdcall column_genDescription (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  SAPDB_UCS2          *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  assert (out);
  
  StudioTD_String sbuf = me->genDescription();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ColumnDef::parseDescription().             *
 *===========================================================================*/
void __stdcall column_parseDescription (void *_this, SAPDB_UCS2 *description)
{
  StudioTD_String      s;
  StudioTD_ColumnDef  *me = (StudioTD_ColumnDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassColumnDef);
  
  if (description)
    s.ConvertFromUCS2 (description, description + wcslen(description));
  else
    s = "";

  me->parseDescription (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::name().                     *
 *===========================================================================*/
SAPDB_Int __stdcall constraint_name (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  SAPDB_UCS2              *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  assert (out);
  
  StudioTD_String sbuf = me->name();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setName().                  *
 *===========================================================================*/
void __stdcall constraint_setName (void *_this, SAPDB_UCS2 *name)
{
  StudioTD_String          s;
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  
  if (name)
    s.ConvertFromUCS2 (name, name + wcslen(name));
  else
    s = "";

  me->setName (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::alterType().                *
 *===========================================================================*/
SAPDB_UInt __stdcall constraint_alterType (void *_this)
{
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  
  return me->alterType();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setAlterType().             *
 *===========================================================================*/
void __stdcall constraint_setAlterType (void *_this, SAPDB_UInt type)
{
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  
  me->setAlterType ((StudioTD_AlterType)type);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::addCondition().             *
 *===========================================================================*/
void * __stdcall constraint_addCondition (void *_this)
{
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  
  return me->addCondition();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::removeCondition().          *
 *===========================================================================*/
void __stdcall constraint_removeCondition (void *_this, SAPDB_UInt index)
{
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  
  me->removeCondition (index);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::condition().                *
 *===========================================================================*/
void * __stdcall constraint_condition (void *_this, SAPDB_UInt index)
{
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  
  return me->condition (index);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::conditionCount().           *
 *===========================================================================*/
SAPDB_UInt __stdcall constraint_conditionCount (void *_this)
{
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  
  return me->conditionCount();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::findIncomplete().           *
 *===========================================================================*/
SAPDB_Int __stdcall constraint_findIncomplete (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  SAPDB_UCS2              *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  assert (out);
  
  StudioTD_String sbuf = me->findIncomplete();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::genCreateStatement().       *
 *===========================================================================*/
SAPDB_Int __stdcall constraint_genCreateStatement (void *_this, SAPDB_UInt mode, 
                                                   SAPDB_Bool casesensitive, SAPDB_UCS2 *out)
{
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  SAPDB_UCS2              *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  assert (out);
  
  StudioTD_String sbuf = me->genCreateStatement ((StudioTD_Mode)mode, casesensitive);
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::genAlterStatement().        *
 *===========================================================================*/
SAPDB_Int __stdcall constraint_genAlterStatement (void *_this, SAPDB_Bool casesensitive, SAPDB_UCS2 *out)
{
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  SAPDB_UCS2              *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  assert (out);
  
  StudioTD_String sbuf = me->genAlterStatement (casesensitive);
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::parseConditions().          *
 *===========================================================================*/
void __stdcall constraint_parseConditions (void *_this, SAPDB_UCS2 *conditions)
{
  StudioTD_String          s;
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  
  if (conditions)
    s.ConvertFromUCS2 (conditions, conditions + wcslen(conditions));
  else
    s = "";

  me->parseConditions (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::genDescription().           *
 *===========================================================================*/
SAPDB_Int __stdcall constraint_genDescription (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  SAPDB_UCS2              *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  assert (out);
  
  StudioTD_String sbuf = me->genDescription();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::parseDescription().         *
 *===========================================================================*/
void __stdcall constraint_parseDescription (void *_this, SAPDB_UCS2 *description)
{
  StudioTD_String          s;
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  
  if (description)
    s.ConvertFromUCS2 (description, description + wcslen(description));
  else
    s = "";

  me->parseDescription (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::parseDescription().         *
 *===========================================================================*/
SAPDB_Bool __stdcall constraint_isFunction (void *_this, SAPDB_UCS2 *str)
{
  StudioTD_String          s;
  StudioTD_ConstraintDef  *me = (StudioTD_ConstraintDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConstraintDef);
  
  if (str)
    s.ConvertFromUCS2 (str, str + wcslen(str));
  else
    return false;

  return me->isFunction (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::column().                   *
 *===========================================================================*/
SAPDB_Int __stdcall condition_column (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  assert (out);
  
  StudioTD_String sbuf = me->column();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setColumn().                *
 *===========================================================================*/
void __stdcall condition_setColumn (void *_this, SAPDB_UCS2 *column)
{
  StudioTD_String         s;
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  
  if (column)
    s.ConvertFromUCS2 (column, column + wcslen(column));
  else
    s = "";

  me->setColumn (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::predicate().                *
 *===========================================================================*/
SAPDB_Int __stdcall condition_predicate (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  assert (out);
  
  StudioTD_String sbuf = me->predicate();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setPredicate().             *
 *===========================================================================*/
void __stdcall condition_setPredicate (void *_this, SAPDB_UCS2 *predicate)
{
  StudioTD_String         s;
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  
  if (predicate)
    s.ConvertFromUCS2 (predicate, predicate + wcslen(predicate));
  else
    s = "";

  me->setPredicate (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::notOperator().              *
 *===========================================================================*/
SAPDB_Bool __stdcall condition_notOperator (void *_this)
{
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  
  return me->notOperator();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setNotOperator().           *
 *===========================================================================*/
void __stdcall condition_setNotOperator (void *_this, SAPDB_Bool notoperator)
{
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  
  me->setNotOperator (notoperator);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::expression().               *
 *===========================================================================*/
SAPDB_Int __stdcall condition_expression (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  assert (out);
  
  StudioTD_String sbuf = me->expression();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setExpression().            *
 *===========================================================================*/
void __stdcall condition_setExpression (void *_this, SAPDB_UCS2 *expression)
{
  StudioTD_String         s;
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  
  if (expression)
    s.ConvertFromUCS2 (expression, expression + wcslen(expression));
  else
    s = "";

  me->setExpression (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::noExpression().             *
 *===========================================================================*/
SAPDB_Bool __stdcall condition_noExpression (void *_this)
{
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  
  return me->noExpression();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setNoExpression().          *
 *===========================================================================*/
void __stdcall condition_setNoExpression (void *_this, SAPDB_Bool noexpression)
{
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  
  me->setNoExpression (noexpression);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::logOperator().              *
 *===========================================================================*/
SAPDB_Int __stdcall condition_logOperator (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  assert (out);
  
  StudioTD_String sbuf = me->logOperator();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setLogOperator().           *
 *===========================================================================*/
void __stdcall condition_setLogOperator (void *_this, SAPDB_UCS2 *logoperator)
{
  StudioTD_String         s;
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  
  if (logoperator)
    s.ConvertFromUCS2 (logoperator, logoperator + wcslen(logoperator));
  else
    s = "";

  me->setLogOperator (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::alterType().                *
 *===========================================================================*/
SAPDB_UInt __stdcall condition_alterType (void *_this)
{
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  
  return me->alterType();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setAlterType().             *
 *===========================================================================*/
void __stdcall condition_setAlterType (void *_this, SAPDB_UInt type)
{
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  
  me->setAlterType ((StudioTD_AlterType)type);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::findIncomplete().           *
 *===========================================================================*/
SAPDB_Int __stdcall condition_findIncomplete (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  assert (out);
  
  StudioTD_String sbuf = me->findIncomplete();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::genCreateStatement().       *
 *===========================================================================*/
SAPDB_Int __stdcall condition_genCreateStatement (void *_this, SAPDB_UInt mode, 
                                                  SAPDB_Bool casesensitive, SAPDB_UCS2 *out)
{
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  assert (out);
  
  StudioTD_String sbuf = me->genCreateStatement((StudioTD_Mode)mode, casesensitive);
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::genDescription().           *
 *===========================================================================*/
SAPDB_Int __stdcall condition_genDescription (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  SAPDB_UCS2             *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  assert (out);
  
  StudioTD_String sbuf = me->genDescription();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::parseDescription().         *
 *===========================================================================*/
void __stdcall condition_parseDescription (void *_this, SAPDB_UCS2 *description)
{
  StudioTD_String         s;
  StudioTD_ConditionDef  *me = (StudioTD_ConditionDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassConditionDef);
  
  if (description)
    s.ConvertFromUCS2 (description, description + wcslen(description));
  else
    s = "";

  me->parseDescription (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::name().                     *
 *===========================================================================*/
SAPDB_Int __stdcall foreignkey_name (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  SAPDB_UCS2              *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  assert (out);
  
  StudioTD_String sbuf = me->name();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setName().                  *
 *===========================================================================*/
void __stdcall foreignkey_setName (void *_this, SAPDB_UCS2 *name)
{
  StudioTD_String          s;
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  
  if (name)
    s.ConvertFromUCS2 (name, name + wcslen(name));
  else
    s = "";

  me->setName (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::referencingColumns().       *
 *===========================================================================*/
SAPDB_Int __stdcall foreignkey_referencingColumns (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  SAPDB_UCS2              *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  assert (out);
  
  StudioTD_String sbuf = me->referencingColumns();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setReferencingColumns().    *
 *===========================================================================*/
void __stdcall foreignkey_setReferencingColumns (void *_this, SAPDB_UCS2 *columns)
{
  StudioTD_String          s;
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  
  if (columns)
    s.ConvertFromUCS2 (columns, columns + wcslen(columns));
  else
    s = "";

  me->setReferencingColumns (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::reference().                *
 *===========================================================================*/
SAPDB_Int __stdcall foreignkey_reference (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  SAPDB_UCS2              *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  assert (out);
  
  StudioTD_String sbuf = me->reference();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setReference().             *
 *===========================================================================*/
void __stdcall foreignkey_setReference (void *_this, SAPDB_UCS2 *reference)
{
  StudioTD_String          s;
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  
  if (reference)
    s.ConvertFromUCS2 (reference, reference + wcslen(reference));
  else
    s = "";

  me->setReference (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::onDelete().                 *
 *===========================================================================*/
SAPDB_Int __stdcall foreignkey_onDelete (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  SAPDB_UCS2              *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  assert (out);
  
  StudioTD_String sbuf = me->onDelete();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setOnDelete().              *
 *===========================================================================*/
void __stdcall foreignkey_setOnDelete (void *_this, SAPDB_UCS2 *ondelete)
{
  StudioTD_String          s;
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  
  if (ondelete)
    s.ConvertFromUCS2 (ondelete, ondelete + wcslen(ondelete));
  else
    s = "";

  me->setOnDelete (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::comment().                  *
 *===========================================================================*/
SAPDB_Int __stdcall foreignkey_comment (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  SAPDB_UCS2              *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  assert (out);
  
  StudioTD_String sbuf = me->comment();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);  

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setComment().               *
 *===========================================================================*/
void __stdcall foreignkey_setComment (void *_this, SAPDB_UCS2 *comment)
{
  StudioTD_String          s;
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  
  if (comment)
    s.ConvertFromUCS2 (comment, comment + wcslen(comment));
  else
    s = "";

  me->setComment (s);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::alterType().                *
 *===========================================================================*/
SAPDB_UInt __stdcall foreignkey_alterType (void *_this)
{
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  
  return me->alterType();
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::setAlterType().             *
 *===========================================================================*/
void __stdcall foreignkey_setAlterType (void *_this, SAPDB_UInt type)
{
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  
  me->setAlterType ((StudioTD_AlterType)type);
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::findIncomplete().           *
 *===========================================================================*/
SAPDB_Int __stdcall foreignkey_findIncomplete (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  SAPDB_UCS2              *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  assert (out);
  
  StudioTD_String sbuf = me->findIncomplete();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::genCreateStatement().       *
 *===========================================================================*/
SAPDB_Int __stdcall foreignkey_genCreateStatement (void *_this, SAPDB_UInt mode, 
                                                   SAPDB_Bool casesensitive, SAPDB_UCS2 *out)
{
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  SAPDB_UCS2              *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  assert (out);
  
  StudioTD_String sbuf = me->genCreateStatement ((StudioTD_Mode)mode, casesensitive);
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}



/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::genDescription().           *
 *===========================================================================*/
SAPDB_Int __stdcall foreignkey_genDescription (void *_this, SAPDB_UCS2 *out)
{
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  SAPDB_UCS2              *end;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  assert (out);
  
  StudioTD_String sbuf = me->genDescription();
  sbuf.ConvertToUCS2 (out, out + wcslen(out), end);

  return (sbuf.Size());
}

/*===========================================================================*
 *  Wrapper function for StudioTD_ConstraintDef::parseDescription().         *
 *===========================================================================*/
void __stdcall foreignkey_parseDescription (void *_this, SAPDB_UCS2 *description)
{
  StudioTD_String          s;
  StudioTD_ForeignKeyDef  *me = (StudioTD_ForeignKeyDef *)_this;
  
  assert (me);
  assert (me->typeOf() == StudioTD_Base::ClassForeignKeyDef);
  
  if (description)
    s.ConvertFromUCS2 (description, description + wcslen(description));
  else
    s = "";

  me->parseDescription (s);
}
