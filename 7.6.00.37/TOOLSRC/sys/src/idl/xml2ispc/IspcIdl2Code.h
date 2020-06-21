/*!*****************************************************************************

  module      : IspcIdl2Code.h

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of interface description code
  description : Main program

  ----------------------------------------------------------------------------

    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
    ========== licence end

*******************************************************************************/


#ifndef ISPCIDL2CODE_H
#define ISPCIDL2CODE_H


typedef enum {
  dbo_method_ispc,
  dbo_proc_ispc
} DboType_Enum_ispc;


typedef enum {
  attChar_ispc,
  attDimension_ispc,
  attDirection_ispc,
  attExpression_ispc,
  attGenDefConstr_ispc, /* PTS 1131325, PG */
  attGenTabHandle_ispc, /* PTS 1131325, PG */
  attName_ispc,
  attNumber_ispc,
  attPack_ispc,
  attString_ispc,
  attType_ispc,
  attUnknown_ispc,
  attUuid_ispc,
  attValue_ispc         /* PTS 1131325, PG */
} XmlAttributeType_Enum_ispc;

XmlAttributeType_Enum_ispc ispc_getAttributeType(const char * attribute);


typedef enum {
  elemAdd_ispc,
  elemAlignmentmember_ispc, /* PTS 1141233, PG */
  elemCoClass_ispc,
  elemConst_ispc,
  elemCpp_ispc,
  elemCppsqldbc_ispc, /* PTS 1138438, PG */
  elemDefine_ispc, /* PTS 1141233, PG */
  elemDiv_ispc,
  elemEnum_ispc,
  elemExpr_ispc,
  elemFixed_ispc,
  elemFloat_ispc,
  elemIdent_ispc,
  elemIdl_ispc,
  elemIdl2Xml_ispc,
  elemImport_ispc,
  elemInclude_ispc,
  elemInterface_ispc,
  elemIntrinsicmember_ispc, /* PTS 1141233, PG */
  elemLibrary_ispc,
  elemMember_ispc,
  elemMod_ispc,
  elemMul_ispc,
  elemParam_ispc,
  elemPragma_ispc,
  elemStruct_ispc,
  elemSub_ispc,
  elemTypedef_ispc,
  elemUnion_ispc, /* PTS 1141233, PG */
  elemUnknown_ispc
} XmlElementType_Enum_ispc;

XmlElementType_Enum_ispc ispc_getElementType(const char * element);


int ispc_getWarnLevel();    /* PTS 1109175 */

int ispc_getMultiOffsets(); /* PTS 1115374 */


#endif /* ISPCIDL2CODE_H */

