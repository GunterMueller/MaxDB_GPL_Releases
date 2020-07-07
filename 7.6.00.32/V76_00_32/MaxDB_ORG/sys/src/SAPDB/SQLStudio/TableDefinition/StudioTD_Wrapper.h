/*!===========================================================================

  module:       StudioTD_Wrapper.h

  ------------------------------------------------------------------------------

  responsible:  MarkusS,WolfgangA

  special area: SQL Studio

  description:  Prototypes of the C wrapper functions.

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

#ifndef STUDIOTD_WRAPPER_H
#define STUDIOTD_WRAPPER_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SQLStudio/TableDefinition/StudioTD_WTableDefODBC.h"
#include <assert.h>


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  PROTOTYPES                                                               *
 *===========================================================================*/
void *       __stdcall table_new                        ();
void *       __stdcall table_copy                       (void *_this);
void         __stdcall table_delete                     (void *_this);

SAPDB_Int    __stdcall table_name                       (void *_this, SAPDB_UCS2 *out);
void         __stdcall table_setName                    (void *_this, SAPDB_UCS2 *name);
SAPDB_Int    __stdcall table_owner                      (void *_this, SAPDB_UCS2 *out);
void         __stdcall table_setOwner                   (void *_this, SAPDB_UCS2 *name);
SAPDB_UInt   __stdcall table_mode                       (void *_this);
void         __stdcall table_setMode                    (void *_this, SAPDB_UInt mode);
SAPDB_Int    __stdcall table_basedOn                    (void *_this, SAPDB_UCS2 *out);
void         __stdcall table_setBasedOn                 (void *_this, SAPDB_UCS2 *basedon);
SAPDB_Int    __stdcall table_createAs                   (void *_this, SAPDB_UCS2 *out);
void         __stdcall table_setCreateAs                (void *_this, SAPDB_UCS2 *createas);
SAPDB_UInt   __stdcall table_sampleValue                (void *_this);
void         __stdcall table_setSampleValue             (void *_this, SAPDB_UInt value);
SAPDB_Int    __stdcall table_sampleUnit                 (void *_this, SAPDB_UCS2 *out);
void         __stdcall table_setSampleUnit              (void *_this, SAPDB_UCS2 *unit);
SAPDB_Int    __stdcall table_duplicates                 (void *_this, SAPDB_UCS2 *out);
void         __stdcall table_setDuplicates              (void *_this, SAPDB_UCS2 *duplicates);
SAPDB_Bool   __stdcall table_temporary                  (void *_this);
void         __stdcall table_setTemporary               (void *_this, SAPDB_Bool temporary);
SAPDB_Bool   __stdcall table_ignoreRollback             (void *_this);
void         __stdcall table_setIgnoreRollback          (void *_this, SAPDB_Bool ignorerollback);
SAPDB_Bool   __stdcall table_caseSensitive              (void *_this);
void         __stdcall table_setCaseSensitive           (void *_this, SAPDB_Bool casesensitive);
SAPDB_Int    __stdcall table_comment                    (void *_this, SAPDB_UCS2 *out);
void         __stdcall table_setComment                 (void *_this, SAPDB_UCS2 *comment);
SAPDB_Bool   __stdcall table_ignoreIncomplete           (void *_this);
void         __stdcall table_setIgnoreIncomplete        (void *_this, SAPDB_Bool ignoreincomplete);
void         __stdcall table_setAlterType               (void *_this, SAPDB_UInt type);

void *       __stdcall table_addColumn                  (void *_this);
void         __stdcall table_removeColumn               (void *_this, SAPDB_UInt index);
void *       __stdcall table_column                     (void *_this, SAPDB_UInt index);
SAPDB_Int    __stdcall table_findColumn                 (void *_this, SAPDB_UCS2 *name);
SAPDB_UInt   __stdcall table_columnCount                (void *_this);
void         __stdcall table_twistColumns               (void *_this, SAPDB_UInt index);
void *       __stdcall table_addConstraint              (void *_this);
void         __stdcall table_removeConstraint           (void *_this, SAPDB_UInt index);
void *       __stdcall table_constraint                 (void *_this, SAPDB_UInt index);
SAPDB_Int    __stdcall table_findConstraint             (void *_this, SAPDB_UCS2 *name);
SAPDB_UInt   __stdcall table_constraintCount            (void *_this);
void *       __stdcall table_addForeignKey              (void *_this);
void         __stdcall table_removeForeignKey           (void *_this, SAPDB_UInt index);
void *       __stdcall table_foreignKey                 (void *_this, SAPDB_UInt index);
SAPDB_Int    __stdcall table_findForeignKey             (void *_this, SAPDB_UCS2 *name);
SAPDB_UInt   __stdcall table_foreignKeyCount            (void *_this);
SAPDB_UInt   __stdcall table_keyCount			        (void *_this);

void         __stdcall table_reset                      (void *_this);
SAPDB_UInt   __stdcall table_readTableDefinition        (void *_this, SAPDB_UCS2 *tablename, SAPDB_UCS2 *owner, SAPDB_Bool ownerIsSchema);
SAPDB_Bool   __stdcall table_hasValidColumns            (void *_this);
SAPDB_Bool   __stdcall table_isDirty                    (void *_this);
void         __stdcall table_resetDirty                 (void *_this);
SAPDB_Int    __stdcall table_findIncompleteDefs         (void *_this, SAPDB_UCS2 *out);
SAPDB_Int    __stdcall table_genCreateStatement         (void *_this, SAPDB_UCS2 *out);
SAPDB_Int    __stdcall table_genAlterStatement          (void *_this, void *oldTable, SAPDB_UCS2 *out);
SAPDB_Int    __stdcall table_genDescription             (void *_this, SAPDB_UCS2 *out);
void         __stdcall table_parseDescription           (void *_this, SAPDB_UCS2 *description);
SAPDB_Bool   __stdcall table_setConnection              (void *_this, void *henv, void *hdbc);



SAPDB_Int    __stdcall column_name                      (void *_this, SAPDB_UCS2 *out);
void         __stdcall column_setName                   (void *_this, SAPDB_UCS2 *name);
SAPDB_Int    __stdcall column_oldName                   (void *_this, SAPDB_UCS2 *out);
void         __stdcall column_setOldName                (void *_this, SAPDB_UCS2 *name);
SAPDB_Int    __stdcall column_dataType                  (void *_this, SAPDB_UCS2 *out);
void         __stdcall column_setDataType               (void *_this, SAPDB_UCS2 *datatype);
SAPDB_Int    __stdcall column_dimension                 (void *_this, SAPDB_UCS2 *out);
void         __stdcall column_setDimension              (void *_this, SAPDB_UCS2 *dimension);
SAPDB_Int    __stdcall column_codeType                  (void *_this, SAPDB_UCS2 *out);
void         __stdcall column_setCodeType               (void *_this, SAPDB_UCS2 *codetype);
SAPDB_Bool   __stdcall column_key                       (void *_this);
void         __stdcall column_setKey                    (void *_this, SAPDB_Bool key);
SAPDB_Int2   __stdcall column_keyPos                  (void *_this);
void         __stdcall column_setKeyPos               (void *_this, SAPDB_Int2 keyNum);
SAPDB_Bool   __stdcall column_notNull                   (void *_this);
void         __stdcall column_setNotNull                (void *_this, SAPDB_Bool notnull);
SAPDB_Int    __stdcall column_defaultValue              (void *_this, SAPDB_UCS2 *out);
void         __stdcall column_setDefaultValue           (void *_this, SAPDB_UCS2 *defaultvalue);
SAPDB_Bool   __stdcall column_unique                    (void *_this);
void         __stdcall column_setUnique                 (void *_this, SAPDB_Bool unique);
SAPDB_Int    __stdcall column_comment                   (void *_this, SAPDB_UCS2 *out);
void         __stdcall column_setComment                (void *_this, SAPDB_UCS2 *comment);
SAPDB_UInt   __stdcall column_alterType                 (void *_this);
void         __stdcall column_setAlterType              (void *_this, SAPDB_UInt type);

SAPDB_Int    __stdcall column_findIncomplete            (void *_this, SAPDB_UCS2 *out);
SAPDB_Int    __stdcall column_genCreateStatement        (void *_this, SAPDB_UInt mode, SAPDB_Bool casesensitive, SAPDB_UCS2 *out);
SAPDB_Int    __stdcall column_genDescription            (void *_this, SAPDB_UCS2 *out);
void         __stdcall column_parseDescription          (void *_this, SAPDB_UCS2 *description);



SAPDB_Int    __stdcall constraint_name                  (void *_this, SAPDB_UCS2 *out);
void         __stdcall constraint_setName               (void *_this, SAPDB_UCS2 *name);
SAPDB_UInt   __stdcall constraint_alterType             (void *_this);
void         __stdcall constraint_setAlterType          (void *_this, SAPDB_UInt type);

void *       __stdcall constraint_addCondition          (void *_this);
void         __stdcall constraint_removeCondition       (void *_this, SAPDB_UInt index);
void *       __stdcall constraint_condition             (void *_this, SAPDB_UInt index);
SAPDB_UInt   __stdcall constraint_conditionCount        (void *_this);

SAPDB_Int    __stdcall constraint_findIncomplete        (void *_this, SAPDB_UCS2 *out);
SAPDB_Int    __stdcall constraint_genCreateStatement    (void *_this, SAPDB_UInt mode, SAPDB_Bool casesensitive, SAPDB_UCS2 *out);
SAPDB_Int    __stdcall constraint_genAlterStatement     (void *_this, SAPDB_Bool casesensitive, SAPDB_UCS2 *out);
void         __stdcall constraint_parseConditions       (void *_this, SAPDB_UCS2 *conditions);
SAPDB_Int    __stdcall constraint_genDescription        (void *_this, SAPDB_UCS2 *out);
void         __stdcall constraint_parseDescription      (void *_this, SAPDB_UCS2 *description);
SAPDB_Bool   __stdcall constraint_isFunction            (void *_this, SAPDB_UCS2 *str);



SAPDB_Int    __stdcall condition_column                 (void *_this, SAPDB_UCS2 *out);
void         __stdcall condition_setColumn              (void *_this, SAPDB_UCS2 *column);
SAPDB_Int    __stdcall condition_predicate              (void *_this, SAPDB_UCS2 *out);
void         __stdcall condition_setPredicate           (void *_this, SAPDB_UCS2 *predicate);
SAPDB_Bool   __stdcall condition_notOperator            (void *_this);
void         __stdcall condition_setNotOperator         (void *_this, SAPDB_Bool notoperator);
SAPDB_Int    __stdcall condition_expression             (void *_this, SAPDB_UCS2 *out);
void         __stdcall condition_setExpression          (void *_this, SAPDB_UCS2 *expression);
SAPDB_Bool   __stdcall condition_noExpression           (void *_this);
void         __stdcall condition_setNoExpression        (void *_this, SAPDB_Bool noexpression);
SAPDB_Int    __stdcall condition_logOperator            (void *_this, SAPDB_UCS2 *out);
void         __stdcall condition_setLogOperator         (void *_this, SAPDB_UCS2 *logoperator);
SAPDB_UInt   __stdcall condition_alterType              (void *_this);
void         __stdcall condition_setAlterType           (void *_this, SAPDB_UInt type);

SAPDB_Int    __stdcall condition_findIncomplete         (void *_this, SAPDB_UCS2 *out);
SAPDB_Int    __stdcall condition_genCreateStatement     (void *_this, SAPDB_UInt mode, SAPDB_Bool casesensitive, SAPDB_UCS2 *out);
SAPDB_Int    __stdcall condition_genDescription         (void *_this, SAPDB_UCS2 *out);
void         __stdcall condition_parseDescription       (void *_this, SAPDB_UCS2 *description);



SAPDB_Int    __stdcall foreignkey_name                  (void *_this, SAPDB_UCS2 *out);
void         __stdcall foreignkey_setName               (void *_this, SAPDB_UCS2 *name);
SAPDB_Int    __stdcall foreignkey_referencingColumns    (void *_this, SAPDB_UCS2 *out);
void         __stdcall foreignkey_setReferencingColumns (void *_this, SAPDB_UCS2 *columns);
SAPDB_Int    __stdcall foreignkey_reference             (void *_this, SAPDB_UCS2 *out);
void         __stdcall foreignkey_setReference          (void *_this, SAPDB_UCS2 *reference);
SAPDB_Int    __stdcall foreignkey_onDelete              (void *_this, SAPDB_UCS2 *out);
void         __stdcall foreignkey_setOnDelete           (void *_this, SAPDB_UCS2 *ondelete);
SAPDB_Int    __stdcall foreignkey_comment               (void *_this, SAPDB_UCS2 *out);
void         __stdcall foreignkey_setComment            (void *_this, SAPDB_UCS2 *comment);
SAPDB_UInt   __stdcall foreignkey_alterType             (void *_this);
void         __stdcall foreignkey_setAlterType          (void *_this, SAPDB_UInt type);

SAPDB_Int    __stdcall foreignkey_findIncomplete        (void *_this, SAPDB_UCS2 *out);
SAPDB_Int    __stdcall foreignkey_genCreateStatement    (void *_this, SAPDB_UInt mode, SAPDB_Bool casesensitive, SAPDB_UCS2 *out);
SAPDB_Int    __stdcall foreignkey_genDescription        (void *_this, SAPDB_UCS2 *out);
void         __stdcall foreignkey_parseDescription      (void *_this, SAPDB_UCS2 *description);

#endif   /* STUDIOTD_WRAPPER_H */
