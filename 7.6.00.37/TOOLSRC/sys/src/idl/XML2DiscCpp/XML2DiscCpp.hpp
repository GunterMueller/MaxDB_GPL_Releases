/*!*****************************************************************************

  module      : XML2DiscCpp.hpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of persistent class description code
  description : Main program
				
  ----------------------------------------------------------------------------

    ========== licence begin GPL
    Copyright (C) 2000-2005 SAP AG

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


#ifndef XML2DISCCPP_HPP
#define XML2DISCCPP_HPP


const int nameLen = 128;
const int defaultLen = 16;


/******************************************************************************/
/*     XML attributes                                                         */
/******************************************************************************/

typedef enum {
  attAccesstype_X2DC,
  attBaseclass_X2DC,
  attBitcount_X2DC,
  attChar_X2DC,
  attConst_X2DC,
  attConstptr_X2DC,
  attConstval_X2DC,
  attDimension_X2DC,
  attExpression_X2DC,
  attExtension_X2DC,
  attFile_X2DC,
  attGendefconstr_X2DC,
  attGentabhandle_X2DC,
  attId_X2DC,
  attIdentifier_X2DC,
  attIdenum_X2DC,
  attIntsynonym_X2DC,
  attKeyclass_X2DC,
  attName_X2DC,
  attNumber_X2DC,
  attPack_X2DC,
  attPersbaseclass_X2DC,
  attTempl_X2DC,
  attTemplate_X2DC,
  attType_X2DC,
  attSize_X2DC,
  attStatic_X2DC,
  attString_X2DC,
  attUuid_X2DC,
  attUnknown_X2DC,
  attValue_X2DC
} XmlAttributeType_X2DC;

XmlAttributeType_X2DC X2DC_getAttributeType(const char * attribute);

static inline int IndexOfNameAtt(const char ** atts) {
  int idx = -1;
  for ( int i = 0; atts[i]; i = i+2 ) {
    if (attName_X2DC == X2DC_getAttributeType(atts[i])) {
      idx = i;
      break;
    }
  }
  return idx;
}


/******************************************************************************/
/*     XML elements                                                           */
/******************************************************************************/

typedef enum {
  elemAdd_X2DC,
  elemAlignmentmember_X2DC,
  elemBitfields_X2DC,
  elemClass_X2DC,
  elemClasses_X2DC,
  elemClassinstance_X2DC,
  elemClassmember_X2DC,
  elemClassparam_X2DC,
  elemConst_X2DC,
  elemCpp_X2DC,
  elemDefine_X2DC,
  elemDerivefrom_X2DC,
  elemDiv_X2DC,
  elemEnum_X2DC,
  elemExpr_X2DC,
  elemExtoidmember_X2DC,
  elemField_X2DC,
  elemFixed_X2DC,
  elemFriend_X2DC,
  elemGeneralenum_X2DC,
  elemIdent_X2DC,
  elemImport_X2DC,
  elemIndex_X2DC,
  elemInstantiate_X2DC,
  elemIntinstance_X2DC,
  elemIntparam_X2DC,
  elemIntrinsicmember_X2DC,
  elemKey_X2DC,
  elemMember_X2DC,
  elemMul_X2DC,
  elemOidmember_X2DC,
  elemPersclass_X2DC,
  elemPersinterface_X2DC,
  elemPerskeyedclass_X2DC,
  elemPerstemplclass_X2DC,
  elemPragma_X2DC,
  elemPtrmember_X2DC,
  elemSection_X2DC,
  elemSize_X2DC,
  elemSizeof_X2DC,
  elemStruct_X2DC,
  elemSub_X2DC,
  elemTemplclass_X2DC,
  elemTemplmember_X2DC,
  elemTemplparams_X2DC,
  elemTypedef_X2DC,
  elemUnknown_X2DC,
  elemUnion_X2DC,
  elemVaroidmember_X2DC
} XmlElementType_X2DC;

XmlElementType_X2DC X2DC_getElementType(const char * element);


/******************************************************************************/
/*     Determine pad size for class/struct members                            */
/******************************************************************************/

static inline int PadSize( const int size, const int alignment ) {
  /* Let (a - 1) = (n + m) where 0 <= n, m < a and (s + n) % a = 0.
     Then: pad size = n = (n + m) - m
                        = (n + m) - ((s + (n + m)) % a) */
  return ( (alignment - 1) - ((size + (alignment - 1)) % alignment) );
}


/******************************************************************************/
/*     Size of a pointer                                                      */
/******************************************************************************/

static inline int SizeOfPointer() {
  return (sizeof(void*));
}


/******************************************************************************/
/*     Alignment of a pointer                                                 */
/******************************************************************************/

static inline int AlignmentOfPointer() {
# if defined(_WIN32) && !defined(BIT64)
    return __alignof(void*);
# else
    return 8;
# endif
}


/******************************************************************************/
/*     Size of OID                                                            */
/******************************************************************************/

static inline int SizeOfOid() {
  return 8;
}


/******************************************************************************/
/*     Alignment of OID                                                       */
/******************************************************************************/

int AlignmentOfOid();


/******************************************************************************/
/*     Size check on Windows32                                                */
/******************************************************************************/

static inline bool PersSizesMatch( int CalcSize, int SpecSize ) {
# if defined(_WIN32) && !defined(BIT64)
    return (CalcSize == SpecSize);
# else
    return true;
# endif
}


/******************************************************************************/
/*     Translate string to upper case                                         */
/******************************************************************************/

static inline void ToUpper( const char * lower, char * upper, int transLen ) {
  int len = (transLen > 0) ? transLen : strlen(lower);
  for ( int i = 0; i < len; i++ ) {
    if (lower[i] == '_') {
      upper[i] = '_';
    }
    else {
      upper[i] = toupper(lower[i]);
    }
  }
  upper[len] = 0;
}


#endif

