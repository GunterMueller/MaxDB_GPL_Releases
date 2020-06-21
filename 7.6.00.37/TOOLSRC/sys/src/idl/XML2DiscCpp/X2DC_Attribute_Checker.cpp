/*!*****************************************************************************

  module      : X2DC_Attribute_Checker.cpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of persistent class description code
  description : Perform checks not possible during validation
			
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


#include "idl/XML2DiscCpp/X2DC_Attribute_Checker.hpp"
#include "idl/XML2DiscCpp/X2DC_WarnAndErrHandler.hpp"


/******************************************************************************/
/*     Implementation of class Attribute_Checker                              */
/******************************************************************************/

Attribute_Checker::Attribute_Checker(int * pError)
: m_pError(pError)
, m_isWithin_Import(0)
, m_isWithin_Enum(false)
, m_isWithin_GenEnum(false)
, m_isWithin_Struct(false)
{}

/*----------------------------------------------------------------------------*/

Attribute_Checker::~Attribute_Checker() {

  if ( (!*m_pError) && ( m_isWithin_Import ||
                         m_isWithin_Enum   ||
                         m_isWithin_Struct ) ) {
    X2DC_WarnMsg( "Attribute checker not cleaned up correctly!", false );
  }

}

/*----------------------------------------------------------------------------*/

void Attribute_Checker::StartElement( int & error, const char * name, const char ** atts ) {

  switch (X2DC_getElementType(name)) {

    /*---------- IMPORT ------------------*/

    case elemImport_X2DC : {
      /* Sequence: FILE (r), WRAPPER (o) */
      /* Basically already checked in XMLMerge. */
      m_isWithin_Import++;
      break;
    }

    /*---------- STRUCT ------------------*/

    case elemStruct_X2DC : {
      /* Sequence: NAME (r), UUID (o), GENDEFCONSTR (o), GENTABHANDLE (o) */
      /* NAME: position */
      if (0 != IndexOfNameAtt(atts)) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", "STRUCT" );
        error = 1;
        return;
      }
      /* NAME: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* UUID, GENDEFCONSTR, GENTABHANDLE: position */
      int idIdx = 0, dcIdx = 0, thIdx = 0;
      for ( int i = 2; atts[i]; i = i+2 ) {
        switch (X2DC_getAttributeType(atts[i])) {
          case attUuid_X2DC : {
            if (2 != i) {
              X2DC_ErrMsg( M__INVALID_ATTRPOS, "UUID", "STRUCT" );
              error = 1;
              return;
            }
            idIdx = i;
            break;
          }
          case attGendefconstr_X2DC : {
            if (idIdx) {
              if (4 != i) {
                X2DC_ErrMsg( M__INVALID_ATTRPOS, "GENDEFCONSTR", "STRUCT" );
                error = 1;
                return;
              }
            }
            else {
              if (2 != i) {
                X2DC_ErrMsg( M__INVALID_ATTRPOS, "GENDEFCONSTR", "STRUCT" );
                error = 1;
                return;
              }
            }
            dcIdx = i;
            break;
          }
          case attGentabhandle_X2DC : {
            thIdx = i;
            break;
          }
        }
      }
      /* GENDEFCONSTR, GENTABHANDLE: value */
      if (dcIdx) {
        if ( (0 != strcmp( atts[dcIdx+1], "YES" )) && (0 != strcmp( atts[dcIdx+1], "NO" )) ) {
          X2DC_ErrMsg( M__INVALID_ATTRVAL, "GENDEFCONSTR", "STRUCT" );
          error = 1;
          return;
        }
      }
      if (thIdx) {
        if ( (0 != strcmp( atts[thIdx+1], "YES" )) && (0 != strcmp( atts[thIdx+1], "NO" )) ) {
          X2DC_ErrMsg( M__INVALID_ATTRVAL, "GENTABHANDLE", "STRUCT" );
          error = 1;
          return;
        }
      }
      /* GENTABHANDLE: Only within import mode */
      if ( thIdx && (0 == m_isWithin_Import) ) {
        X2DC_ErrMsg( M__ATTR_ONLY_WITHIN_IMPORT, "GENTABHANDLE", "STRUCT" );
        error = 1;
        return;
      }
      /* Ok */
      m_isWithin_Struct = true;
      break;
    }

    /*---------- UNION -------------------*/

    case elemUnion_X2DC : {
      /* Sequence: NAME (r) */
      /* NAME: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    case elemAlignmentmember_X2DC :
    case elemIntrinsicmember_X2DC : {
      /* Sequence: NAME (r), TYPE (r) */
      /* NAME: position */
      if (0 != IndexOfNameAtt(atts)) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", name );
        error = 1;
        return;
      }
      /* NAME, TYPE: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      else if (!LengthValid(atts[3])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[3] );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    /*---------- CLASS -------------------*/

    case elemClass_X2DC : {
      /* Sequence: NAME (r), EXTENSION (o), GENDEFCONSTR (o) */
      /* NAME: position */
      if (0 != IndexOfNameAtt(atts)) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", "CLASS" );
        error = 1;
        return;
      }
      /* NAME: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* EXTENSION, GENDEFCONSTR: position */
      int dfIdx = 0;
      if (atts[2]) {
        if (atts[4]) {
          if (attExtension_X2DC != X2DC_getAttributeType(atts[2])) {
            X2DC_ErrMsg( M__INVALID_ATTRPOS, "EXTENSION", "CLASS" );
            error = 1;
            return;
          }
          else {
            dfIdx = 4;
          }
        }
        else if (attGendefconstr_X2DC == X2DC_getAttributeType(atts[2])) {
          dfIdx = 2;
        }
      }
      /* GENDEFCONSTR: value */
      if (dfIdx) {
        if ( (0 != strcmp( atts[dfIdx+1], "public" ))    &&
             (0 != strcmp( atts[dfIdx+1], "private" ))   &&
             (0 != strcmp( atts[dfIdx+1], "protected" )) &&
             (0 != strcmp( atts[dfIdx+1], "NO" )) ) {
          X2DC_ErrMsg( M__INVALID_ATTRVAL, "GENDEFCONSTR", "CLASS" );
          error = 1;
          return;
        }
      }
      /* Ok */
      break;
    }

    /*---------- PERSCLASS ---------------*/

    case elemPersclass_X2DC : {
      /* Sequence: NAME (r), ID (r), IDENUM (o), EXTENSION (o), PERSBASECLASS (o) */
      /* NAME: position */
      if (0 != IndexOfNameAtt(atts)) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", "PERSCLASS" );
        error = 1;
        return;
      }
      /* NAME: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* ID: position */
      /* PERSBASECLASS: length */
      /* GENDEFCONSTR: may not exist */
      for ( int i = 2; atts[i]; i = i+2 ) {
        switch (X2DC_getAttributeType(atts[i])) {
          case attId_X2DC : {
            if (2 != i) {
              X2DC_ErrMsg( M__INVALID_ATTRPOS, "ID", "PERSCLASS" );
              error = 1;
              return;
            }
            else {
              break;
            }
          }
          case attPersbaseclass_X2DC : {
            if (!LengthValid(atts[i+1])) {
              X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[i+1] );
              error = 1;
              return;
            }
            else {
              break;
            }
          }
          case attGendefconstr_X2DC : {
            X2DC_ErrMsg( M__INVALID_ATTR, "GENDEFCONSTR", "PERSCLASS" );
            error = 1;
            return;
          }
        }
      }
      /* Ok */
      break;
    }

    case elemKey_X2DC : {
      /* Ok */
      break;
    }

    case elemIndex_X2DC : {
      /* Sequence: ID (r) */
      /* ID: value */
      if (!( (0 < atoi(atts[1])) && (atoi(atts[1]) < 65535+1) )) {
        X2DC_ErrMsg( M__INVALID_IDVALUE, "INDEX" );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    /*---------- PERSKEYEDCLASS ----------*/

    case elemPerskeyedclass_X2DC : {
      /* Sequence: NAME (r), ID (r), IDENUM (o), KEYCLASS (r), EXTENSION (o) */
      /* NAME: position */
      if (0 != IndexOfNameAtt(atts)) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", "PERSKEYEDCLASS" );
        error = 1;
        return;
      }
      /* NAME: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* ID: position */
      /* KEYCLASS: length */
      /* GENDEFCONSTR: may not exist */
      for ( int i = 2; atts[i]; i = i+2 ) {
        switch (X2DC_getAttributeType(atts[i])) {
          case attId_X2DC : {
            if (2 != i) {
              X2DC_ErrMsg( M__INVALID_ATTRPOS, "ID", "PERSKEYEDCLASS" );
              error = 1;
              return;
            }
            else {
              break;
            }
          }
          case attKeyclass_X2DC : {
            if (!LengthValid(atts[i+1])) {
              X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[i+1] );
              error = 1;
              return;
            }
            else {
              break;
            }
          }
          case attGendefconstr_X2DC : {
            X2DC_ErrMsg( M__INVALID_ATTR, "GENDEFCONSTR", "PERSKEYEDCLASS" );
            error = 1;
            return;
          }
        }
      }
      /* Ok */
      break;
    }

    /*---------- TEMPLCLASS --------------*/

    case elemTemplclass_X2DC : {
      /* Sequence: NAME (r), EXTENSION (o) */
      /* NAME: position */
      if (0 != IndexOfNameAtt(atts)) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", "TEMPLCLASS" );
        error = 1;
        return;
      }
      /* NAME: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    case elemTemplparams_X2DC : {
      break;
    }

    case elemClassparam_X2DC : {
      /* Sequence: IDENTIFIER (r) */
      /* IDENTIFIER: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    case elemIntparam_X2DC : {
      /* Sequence: INTSYNONYM (r), IDENTIFIER (r) */
      /* INTSYNONYM, IDENTIFIER: position */
      if (attIntsynonym_X2DC != X2DC_getAttributeType(atts[0])) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS_IN_ELEMENT, "INTSYNONYM", "INTPARAM", "TEMPLPARAMS" );
        error = 1;
        return;
      }
      /* INTSYNONYM, IDENTIFIER: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      else if (!LengthValid(atts[3])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[3] );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    /*---------- PERSTEMPLCLASS ----------*/

    case elemPerstemplclass_X2DC : {
      /* Sequence: NAME (r), ID (r), IDENUM (o), EXTENSION (o), PERSBASECLASS (o) */
      /* NAME: position */
      if (0 != IndexOfNameAtt(atts))  {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", "PERSTEMPLCLASS" );
        error = 1;
        return;
      }
      /* NAME: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* ID: position */
      /* PERSBASECLASS: length */
      for ( int i = 2; atts[i]; i = i+2 ) {
        switch (X2DC_getAttributeType(atts[i])) {
          case attId_X2DC : {
            if (2 != i) {
              X2DC_ErrMsg( M__INVALID_ATTRPOS, "ID", "PERSTEMPLCLASS" );
              error = 1;
              return;
            }
            else {
              break;
            }
          }
          case attPersbaseclass_X2DC : {
            if (!LengthValid(atts[i+1])) {
              X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[i+1] );
              error = 1;
              return;
            }
            else {
              break;
            }
          }
        }
      }
      /* Ok */
      break;
    }

    /*---------- INSTANTIATE -------------*/

    case elemInstantiate_X2DC : {
      /* Sequence: TEMPLATE (r), TYPE (r) */
      /* TEMPLATE, TYPE: position */
      if (attTemplate_X2DC != X2DC_getAttributeType(atts[0])) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "TEMPLATE", "INSTANTIATE" );
        error = 1;
        return;
      }
      /* TEMPLATE, TYPE: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      else if (!LengthValid(atts[3])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[3] );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    case elemClassinstance_X2DC : {
      /* Sequence: TYPE (r) */
      /* TYPE: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    case elemIntinstance_X2DC : {
      /* Sequence: VALUE (r) */
      /* VALUE: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    /*---------- PERSINTERFACE -----------*/

    case elemPersinterface_X2DC : {
      /* Sequence: NAME (r), ID (r), EXTENSION (o) */
      /* NAME: position */
      if (0 != IndexOfNameAtt(atts)) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", "PERSINTERFACE" );
        error = 1;
        return;
      }
      /* NAME: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* ID: position */
      if (attId_X2DC != X2DC_getAttributeType(atts[2])) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "ID", "PERSINTERFACE" );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    /*---------- DERIVATION --------------*/

    case elemDerivefrom_X2DC : {
      /* Sequence: BASECLASS (r), TYPE (r) */
      /* BASECLASS, TYPE: position */
      if (attBaseclass_X2DC != X2DC_getAttributeType(atts[0])) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "BASECLASS", "DERIVEFROM" );
        error = 1;
        return;
      }
      /* BASECLASS: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* TYPE: value */
      if ( (0 != strcmp( atts[3], "public" ))  &&
           (0 != strcmp( atts[3], "private" )) &&
           (0 != strcmp( atts[3], "protected" )) ) {
        X2DC_ErrMsg( M__INVALID_ATTRVAL, "TYPE", "DERIVEFROM" );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    /*---------- FRIEND ------------------*/

    case elemFriend_X2DC : {
      /* Sequence: NAME (r) */
      /* NAME: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    /*---------- SECTION -----------------*/

    case elemSection_X2DC : {
      /* Sequence: ACCESSTYPE (r) */
      /* ACCESSTYPE: value */
      if ( (0 != strcmp( atts[1], "public" ))  &&
           (0 != strcmp( atts[1], "private" )) &&
           (0 != strcmp( atts[1], "protected" )) ) {
        X2DC_ErrMsg( M__INVALID_ATTRVAL, "ACCESSTYPE", "SECTION" );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    /*---------- CONSTANTS ---------------*/

    case elemEnum_X2DC : {
      /* Sequence: NAME (r) */
      /* NAME: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* Ok */
      m_isWithin_Enum = true;
      break;
    }

    case elemGeneralenum_X2DC : {
      /* Sequence: NAME (o) */
      /* NAME: length */
      if ( atts[0] && (!LengthValid(atts[1])) ) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* Ok */
      m_isWithin_GenEnum = true;
      break;
    }

    case elemConst_X2DC : {
      /* Sequence: NAME (r), TYPE (r), STRING/CHAR/NUMBER/EXPRESSION (o) */
      /* NAME, TYPE: position */
      if (0 != IndexOfNameAtt(atts)) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", "CONST" );
        error = 1;
        return;
      }
      if (attType_X2DC != X2DC_getAttributeType(atts[2])) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "TYPE", "CONST" );
        error = 1;
        return;
      }
      /* NAME, TYPE: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      else if (!LengthValid(atts[3])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[3] );
        error = 1;
        return;
      }
      /* STRING/CHAR/NUMBER/EXPRESSION: one and only one */
      if (!atts[4]) {
        X2DC_ErrMsg( M__ATTR_MISSING, "CONST", "third" );
        error = 1;
        return;
      }
      if (atts[6]) {
        X2DC_ErrMsg( M__TOO_MANY_ATTR, "CONST" );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    case elemExpr_X2DC :
    case elemAdd_X2DC :
    case elemSub_X2DC :
    case elemFixed_X2DC :
    case elemIdent_X2DC : {
      break;
    }

    /*---------- MEMBERS -----------------*/

    case elemBitfields_X2DC : {
      /* Sequence: TYPE (r), SIZE (r) */
      /* TYPE, SIZE: position */
      if (attType_X2DC != X2DC_getAttributeType(atts[0])) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "TYPE", "BITFIELDS" );
        error = 1;
        return;
      }
      /* TYPE: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    case elemField_X2DC : {
      /* Sequence: NAME (r), BITCNT (r) */
      /* NAME, BITCNT: position */
      if (0 != IndexOfNameAtt(atts)) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS_IN_ELEMENT, "NAME", "FIELD", "BITFIELDS" );
        error = 1;
        return;
      }
      /* NAME: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    case elemClassmember_X2DC : {
      /* Sequence: NAME (r), TYPE (r), DIMENSION (o), CONST (o), STATIC (o) */
      /* NAME: position */
      if (0 != IndexOfNameAtt(atts)) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", "CLASSMEMBER" );
        error = 1;
        return;
      }
      /* TYPE: position */
      if (attType_X2DC != X2DC_getAttributeType(atts[2])) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "TYPE", "CLASSMEMBER" );
        error = 1;
        return;
      }
      /* NAME, TYPE: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      else if (!LengthValid(atts[3])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[3] );
        error = 1;
        return;
      }
      /* DIMENSION, CONST, STATIC: position */
      int dimIdx = 0, conIdx = 0, staIdx = 0;
      for ( int i = 4; atts[i]; i = i+2 ) {
        switch (X2DC_getAttributeType(atts[i])) {
          case attDimension_X2DC : {
            if (i != 4) {
              X2DC_ErrMsg( M__INVALID_ATTRPOS, "DIMENSION", "CLASSMEMBER" );
              error = 1;
              return;
            }
            dimIdx = i;
            break;
          }
          case attConst_X2DC : {
            if (dimIdx) {
              if (6 != i) {
                X2DC_ErrMsg( M__INVALID_ATTRPOS, "CONST", "CLASSMEMBER" );
                error = 1;
                return;
              }
            }
            else {
              if (4 != i) {
                X2DC_ErrMsg( M__INVALID_ATTRPOS, "CONST", "CLASSMEMBER" );
                error = 1;
                return;
              }
            }
            conIdx = i;
            break;
          }
          case attStatic_X2DC : {
            staIdx = i;
            break;
          }
        }
      }
      /* CONST, STATIC: value */
      if (conIdx) {
        if ( (0 != strcmp( atts[conIdx+1], "YES" )) && (0 != strcmp( atts[conIdx+1], "NO" )) ) {
          X2DC_ErrMsg( M__INVALID_ATTRVAL, "CONST", "CLASSMEMBER" );
          error = 1;
          return;
        }
      }
      if (staIdx) {
        if ( (0 != strcmp( atts[staIdx+1], "YES" )) && (0 != strcmp( atts[staIdx+1], "NO" )) ) {
          X2DC_ErrMsg( M__INVALID_ATTRVAL, "STATIC", "CLASSMEMBER" );
          error = 1;
          return;
        }
      }
      /* Ok */
      break;
    }

    case elemMember_X2DC : {
      if (m_isWithin_Struct) {
        /* Sequence: NAME (r), TYPE (r), DIMENSION (o) */
        /* NAME: position */
        if (0 != IndexOfNameAtt(atts)) {
          X2DC_ErrMsg( M__INVALID_ATTRPOS_IN_ELEMENT, "NAME", "MEMBER", "STRUCT" );
          error = 1;
          return;
        }
        /* TYPE: position */
        if (attType_X2DC != X2DC_getAttributeType(atts[2])) {
          X2DC_ErrMsg( M__INVALID_ATTRPOS_IN_ELEMENT, "TYPE", "MEMBER", "STRUCT" );
          error = 1;
          return;
        }
        /* NAME, TYPE: length */
        if (!LengthValid(atts[1])) {
          X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
          error = 1;
          return;
        }
        else if (!LengthValid(atts[3])) {
          X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[3] );
          error = 1;
          return;
        }
      }
      else if (m_isWithin_Enum) {
        /* Sequence: NAME (r), VALUE (r) */
        /* NAME, VALUE: position */
        if (0 != IndexOfNameAtt(atts)) {
          X2DC_ErrMsg( M__INVALID_ATTRPOS_IN_ELEMENT, "NAME", "MEMBER", "ENUM" );
          error = 1;
          return;
        }
        /* NAME: length */
        if (!LengthValid(atts[1])) {
          X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
          error = 1;
          return;
        }
      }
      else if (m_isWithin_GenEnum) {
        /* Sequence: NAME (r), VALUE (o), EXPRESSION (o) */
        /* NAME: position */
        if (0 != IndexOfNameAtt(atts)) {
          X2DC_ErrMsg( M__INVALID_ATTRPOS_IN_ELEMENT, "NAME", "MEMBER", "GENERALENUM" );
          error = 1;
          return;
        }
        /* NAME: length */
        if (!LengthValid(atts[1])) {
          X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
          error = 1;
          return;
        }
        /* VALUE/EXPRESSION: either or */
        if (!atts[2]) {
          X2DC_ErrMsg( M__ATTR_MISSING_IN_ELEMENT, "MEMBER", "GENERALENUM", "second" );
          error = 1;
          return;
        }
        if (atts[4]) {
          X2DC_ErrMsg( M__EITHER_OR_ATTR_IN_ELEMENT, "MEMBER", "GENERALENUM", "VALUE", "EXPRESSION" );
          error = 1;
          return;
        }
      }
      /* Ok */
      break;
    }

    case elemOidmember_X2DC :
    case elemExtoidmember_X2DC :
    case elemVaroidmember_X2DC : {
      /* Sequence: NAME (r), DIMENSION (o) */
      /* NAME: position */
      if (0 != IndexOfNameAtt(atts)) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", name );
        error = 1;
        return;
      }
      /* NAME: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    case elemPtrmember_X2DC : {
      /* Sequence: NAME (r), TYPE (r), CONSTVAL (o), CONSTPTR (o) */
      /* NAME: position */
      if (0 != IndexOfNameAtt(atts)) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", "PTRMEMBER" );
        error = 1;
        return;
      }
      /* TYPE: position */
      if (attType_X2DC != X2DC_getAttributeType(atts[2])) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "TYPE", "PTRMEMBER" );
        error = 1;
        return;
      }
      /* NAME, TYPE: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      else if (!LengthValid(atts[3])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[3] );
        error = 1;
        return;
      }
      /* CONSTVAL, CONSTPTR: position */
      if (atts[4]) {
        if ( atts[6] && (attConstval_X2DC != X2DC_getAttributeType(atts[4])) ) {
          X2DC_ErrMsg( M__INVALID_ATTRPOS, "CONSTVAL", "PTRMEMBER" );
          error = 1;
          return;
        }
      }
      /* CONSTVAL, CONSTPTR: value */
      for ( int i = 4; atts[i]; i = i+2 ) {
        if ( (0 != strcmp( atts[i+1], "YES" )) && (0 != strcmp( atts[i+1], "NO" )) ) {
          X2DC_ErrMsg( M__INVALID_ATTRVAL, atts[i], "PTRMEMBER" );
          error = 1;
          return;
        }
      }
      /* Ok */
      break;
    }

    case elemTemplmember_X2DC : {
      /* Sequence: NAME (r), TEMPLATE (r), DIMENSION (o) */
      /* NAME: position */
      if (0 != IndexOfNameAtt(atts)) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", "TEMPLMEMBER" );
        error = 1;
        return;
      }
      /* TEMPLATE: position */
      if (attTemplate_X2DC != X2DC_getAttributeType(atts[2])) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "TEMPLATE", "TEMPLMEMBER" );
        error = 1;
        return;
      }
      /* NAME, TEMPLATE: length */
      if (!LengthValid(atts[1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[1] );
        error = 1;
        return;
      }
      else if (!LengthValid(atts[3])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[3] );
        error = 1;
        return;
      }
      /* Ok */
      break;
    }

    /*---------- TYPEDEF -----------------*/

    case elemTypedef_X2DC : {
      /* Sequence: UUID (o), NAME (r), TYPE (r), DIMENSION (o), GENTABHANDLE (o) */
      /* NAME: position */
      int nameIdx = IndexOfNameAtt(atts);
      for ( int i = nameIdx+2; atts[i]; i = i+2 ) {
        if (attUuid_X2DC == X2DC_getAttributeType(atts[i])) {
          X2DC_ErrMsg( M__INVALID_ATTRPOS, "UUID", "TYPEDEF" );
          error = 1;
          return;
        }
      }
      switch (nameIdx) {
        case 0 : {
          // ok
          break;
        }
        case 2 : {
          if (attUuid_X2DC != X2DC_getAttributeType(atts[0])) {
            X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", "TYPEDEF" );
            error = 1;
            return;
          }
          break;
        }
        default : {
          X2DC_ErrMsg( M__INVALID_ATTRPOS, "NAME", "TYPEDEF" );
          error = 1;
          return;
        }
      }
      /* NAME: length */
      if (!LengthValid(atts[nameIdx+1])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[nameIdx+1] );
        error = 1;
        return;
      }
      /* TYPE: position */
      if (attType_X2DC != X2DC_getAttributeType(atts[nameIdx+2])) {
        X2DC_ErrMsg( M__INVALID_ATTRPOS, "TYPE", "TYPEDEF" );
        error = 1;
        return;
      }
      /* TYPE: length */
      if (!LengthValid(atts[nameIdx+3])) {
        X2DC_ErrMsg( M__INVALID_STRING_LENGTH, atts[nameIdx+3] );
        error = 1;
        return;
      }
      /* DIMENSION: only for OmsVersionId  */
      /* GENTABHANDLE: only in import mode */
      /* DIMENSION, GENTABHANDLE: not both */
      if (atts[nameIdx+4]) {
        if ( (attDimension_X2DC == X2DC_getAttributeType(atts[nameIdx+4])) &&
             (0 != strcmp( "OmsVersionId", atts[nameIdx+1] )) ) {
          X2DC_ErrMsg("Attribute DIMENSION for TYPEDEF only for \"OmsVersionId\" allowed!");
          error = 1;
          return;
        }
        if ( (attGentabhandle_X2DC == X2DC_getAttributeType(atts[nameIdx+4])) &&
             (!m_isWithin_Import) ) {
          X2DC_ErrMsg( M__ATTR_ONLY_WITHIN_IMPORT, "GENTABHANDLE", "TYPEDEF" );
          error = 1;
          return;
        }
        if (atts[nameIdx+6]) {
          X2DC_ErrMsg( M__TOO_MANY_ATTR, "TYPEDEF" );
          error = 1;
          return;
        }
      }
      /* Ok */
      break;
    }

  }

}

/*----------------------------------------------------------------------------*/

void Attribute_Checker::EndElement(const char * name) {

  switch (X2DC_getElementType(name)) {

    case elemImport_X2DC : {
      m_isWithin_Import--;
      break;
    }

    case elemStruct_X2DC : {
      m_isWithin_Struct = false;
      break;
    }

    case elemEnum_X2DC : {
      m_isWithin_Enum = false;
      break;
    }

    case elemGeneralenum_X2DC : {
      m_isWithin_GenEnum = false;
      break;
    }

  }

}

