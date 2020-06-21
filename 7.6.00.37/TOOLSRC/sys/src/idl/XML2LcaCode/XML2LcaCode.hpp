/*!*****************************************************************************

  module      : XML2LcaCode.hpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of application code ("COM routine wrappers")
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


#ifndef XML2LCACODE_HPP
#define XML2LCACODE_HPP


#include "idl/expat/expat.h"


const int maxFileNameLength = 255;
typedef char FileName[maxFileNameLength+1];

const int maxNameLen = 127; /* PTS 1140184 */

/*----------------------------------------------------------------------------*/

typedef enum {
  attDirection_X2AC
, attFunctor_X2AC       /* PTS 1140184 */
, attFunctorheader_X2AC /* PTS 1140184 */
, attGenerate_X2AC
, attGlobals_X2AC       /* PTS 1140184 */
, attLibrary_X2AC       /* PTS 1140184 */
, attName_X2AC
, attPrecompheader_X2AC /* PTS 1140184 */
, attSchematype_X2AC    /* PTS 1140184 */
, attTranstype_X2AC     /* PTS 1140184 */
, attType_X2AC
, attUnknown_X2AC
} XmlAttributeType_X2AC;

XmlAttributeType_X2AC X2AC_getAttributeType(const char * attribute);

/*----------------------------------------------------------------------------*/

typedef enum {
  elemCpp_X2AC
, elemIdl_X2AC
, elemInccontent_X2AC
, elemParam_X2AC
, elemUnknown_X2AC
} XmlElementType_X2AC;

XmlElementType_X2AC X2AC_getElementType(const char * element);

/*----- PTS 1140184 ----------------------------------------------------------*/

class Part
{
public:
  inline Part(const char * pName);
  inline ~Part();
  const char * getName() {return m_pName;}
  void         setNext(Part * pPart) {m_pNext = pPart;}
  Part *       getNext() {return m_pNext;}
private:
  char * m_pName;
  Part * m_pNext;
};

inline Part::Part(const char * pName) {
  m_pName = new char[strlen(pName)+1];
  strcpy( m_pName, pName );
  m_pNext = NULL;
}

inline Part::~Part() {
  if (m_pName) {
    delete [] m_pName;
    m_pName = NULL;
  }
  m_pNext = NULL;
}

class List
{
public:
  List() : m_pFirst(NULL) {};
  inline ~List();
  Part * First() {return m_pFirst;}
  void Unshift(const char * pName);
private:
  Part * m_pFirst;
};

inline List::~List() {
  while (m_pFirst) {
    Part * pPart = m_pFirst->getNext();
    delete m_pFirst;
    m_pFirst = pPart;
  }
}

inline void List::Unshift(const char * pName) {
  Part * pPart = new Part(pName);
  pPart->setNext(m_pFirst);
  m_pFirst = pPart;
}

/*----------------------------------------------------------------------------*/

int currLineNo(XML_Parser parser);


#endif /* XML2LCACODE_HPP */

