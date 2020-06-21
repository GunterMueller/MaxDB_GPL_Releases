/*!*****************************************************************************

  module      : X2DC_Cpp_Type_Registry.hpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of persistent class description code
  description : Type registry
			
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


#ifndef X2DC_CPPTYPEREGISTRY_HPP
#define X2DC_CPPTYPEREGISTRY_HPP


#include <string>
#include <stdio.h>

#include "idl/xml2ispc/Ispc_vsp001.hpp"
#include "idl/XML2DiscCpp/XML2DiscCpp.hpp"
#include "idl/XML2DiscCpp/X2DC_WarnAndErrHandler.hpp"


/******************************************************************************/
/*     Forward declaration: Currently processed type                          */
/******************************************************************************/

static inline const char * CurrTypeName();

static inline bool CurrTypeIsTemplate();


/******************************************************************************/
/*    Cpp_Name                                                                */
/******************************************************************************/

class Cpp_Name
{
public:
  /* Constructor */
  inline Cpp_Name(const char * pName);
  /* Destructor */
  inline ~Cpp_Name();
  /* Accessor */
  const char * getName() const {return m_pName;}
private:
  char * m_pName;
};

inline Cpp_Name::Cpp_Name(const char * pName) {
  if (pName) {
    m_pName = new char[strlen(pName)+1];
    strcpy( m_pName, pName );
  }
  else {
    m_pName = NULL;
  }
}

inline Cpp_Name::~Cpp_Name() {
  delete [] m_pName;
  m_pName = NULL;
}


/******************************************************************************/
/*    Single chained list                                                     */
/******************************************************************************/

template<class T>
class Element
{
public:
  /* Constructor */
  Element(const T * pT) : m_pItem(pT), m_pNext(NULL) {};
  /* Destructor */
  ~Element() { m_pItem = NULL; m_pNext = NULL; }
  /* Accessors */
  const T *    operator()() const {return m_pItem;}
  void         setItem(const T * pT) {m_pItem = pT;}
  void         setNext(Element<T> * pE) {m_pNext = pE;}
  Element<T> * getNext() const {return m_pNext;}
private:
  const T *    m_pItem;
  Element<T> * m_pNext;
};

/*----------------------------------------------------------------------------*/

template <class T> class List;

template<class T>
class Iterator
{
public:
  /* Constructor */
  Iterator( Element<T> * pStart, const List<T> * pList )
   : m_pStart(pStart), m_pCurr(pStart), m_pList(pList) {};
  /* Destructor */
  ~Iterator() { m_pStart = NULL; m_pCurr = NULL; m_pList = NULL; }
  /* Increment operators */
  void operator++() { // prefix
    m_pCurr = m_pCurr->getNext();
  }
  void operator++(int) { // postfix
    m_pCurr = m_pCurr->getNext();
  }
  /* Accessors */
  inline const T * operator()() const;
  inline void      setCurrItem(const T * pT) const;
  inline void      reset() {m_pCurr = m_pStart;}
private:
  Element<T> *    m_pStart;
  Element<T> *    m_pCurr;
  const List<T> * m_pList;
};

template<class T>
inline const T * Iterator<T>::operator()() const {
  if (m_pCurr) {
    return (*m_pCurr)();
  }
  else {
    return NULL;
  }
}

template<class T>
inline void Iterator<T>::setCurrItem(const T * pT) const {
  if (m_pCurr) {
    m_pCurr->setItem(pT);
  }
}

/*----------------------------------------------------------------------------*/

template<class T>
class List
{
public:
  /* Constructors */
  List() : m_pFirst(NULL), m_pLast(NULL) {};
  List(const T * pT) {
    Element<T> * pE = new Element<T>(pT);
    m_pFirst = m_pLast = pE;
  }
  /* Destructor: deletes only elements, no items */
  inline ~List();
  /* Clear list: deletes only items, no elements */
  inline void Clear() const;
  /* Iterator */
  Iterator<T> Begin() const {
    Iterator<T> iter( m_pFirst, this );
    return iter;
  }
  /* Accessors */
  inline const T * getFirstItem() const;
  inline const T * getLastItem() const;
  /* Others */
  bool             IsEmpty() {return (NULL == m_pFirst);}
  inline void      Append(const T * pT);
private:
  Element<T> * m_pFirst;
  Element<T> * m_pLast;
};

template<class T>
inline List<T>::~List() {
  while (m_pFirst) {
    const Element<T> * pE = m_pFirst;
    m_pFirst = pE->getNext();
    delete pE;
  }
}

template<class T>
inline void List<T>::Clear() const {
  Iterator<T> iter = Begin();
  while (iter()) {
    const T * pT = iter();
    delete pT;
    iter.setCurrItem(NULL);
    ++iter;
  }
}

template<class T>
inline const T * List<T>::getFirstItem() const {
  if (m_pFirst) {
    return (*m_pFirst)();
  }
  else {
    return NULL;
  }
}

template<class T>
inline const T * List<T>::getLastItem() const {
  if (m_pLast) {
    return (*m_pLast)();
  }
  else {
    return NULL;
  }
}

template<class T>
inline void List<T>::Append(const T * pT) {
  Element<T> * pE = new Element<T>(pT);
  if (NULL == m_pFirst) {
    m_pFirst = pE;
  }
  else {
    m_pLast->setNext(pE);
  }
  m_pLast = pE;
}


/******************************************************************************/
/*     Bit fields                                                             */
/******************************************************************************/

class Field
{
public:
  /* Constructor */
  Field( const char * pName, const int bitCount ) : m_pName(pName), m_bitCount(bitCount) {};
  /* Destructor */
  ~Field() {};
  /* Accessors */
  const char * getName() const {return m_pName.getName();}
  int          getBitCount() const { return m_bitCount;}
private:
  Cpp_Name m_pName;
  int      m_bitCount;
};

/*----------------------------------------------------------------------------*/

class Bit_Fields
{
public:
  /* Constructor */
  inline Bit_Fields( const char * pType, const int size );
  /* Destructor */
  inline ~Bit_Fields();
  /* Accessors */
  const char *  getType() const {return m_type.getName();}
  int           getSize() const {return m_size;}
  int           getBitCount() const {return m_bitCounter;}
  List<Field> * getFieldList() const {return m_pFieldList;}
  /* Others */
  inline void   addField( int & error, const Field * pField );
  inline bool   sizeConsistent();
private:
  Cpp_Name      m_type;
  int           m_size;
  int           m_bitCounter;
  List<Field> * m_pFieldList;
};

inline Bit_Fields::Bit_Fields( const char * pType, const int size )
: m_type(pType)
, m_size(size)
, m_bitCounter(0)
{
  m_pFieldList = new List<Field>();
}

inline Bit_Fields::~Bit_Fields() {
  m_pFieldList->Clear();
  delete m_pFieldList;
  m_pFieldList = NULL;
}

inline void Bit_Fields::addField( int & error, const Field * pField ) {
  /* Check uniqueness */
  Iterator<Field> iter = m_pFieldList->Begin();
  while (iter()) {
    if (0 == strcmp( iter()->getName(), pField->getName() )) {
      X2DC_ErrMsg( M__DUPLICATE, "type", CurrTypeName(), "bitfield", pField->getName() );
      error = 1;
      return;
    }
    else {
      iter++;
    }
  }
  /* Add */
  m_pFieldList->Append(pField);
  m_bitCounter += pField->getBitCount();
}

inline bool Bit_Fields::sizeConsistent() {
  if (m_size*8 == m_bitCounter) {
    return true;
  }
  else {
    return false;
  }
}


/******************************************************************************/
/*     Templ_Param                                                            */
/******************************************************************************/

typedef enum {
  ClassParam,
  IntParam
} Templ_ParamKind;

typedef enum {
  Unspecified,
  FullType,
  PersClass // only for OmsOid-member
} ClassparamUsage;

/*----------------------------------------------------------------------------*/

class Templ_Param
{
public:
  /* Constructor */
  Templ_Param( const char * pIdentifier, Templ_ParamKind kind )
   : m_identifier(pIdentifier), m_kind(kind), m_number(0), m_usage(Unspecified) {};
  /* Destructor */
  ~Templ_Param() {};
  /* Accessors */
  const char *        getIdentifier() const {return m_identifier.getName();}
  Templ_ParamKind     getKind() const {return m_kind;}
  void                setNumber(int number) {m_number = number;}
  int                 getNumber() const {return m_number;}
  inline void         setClassparamUsage( int & error, ClassparamUsage usage );
  ClassparamUsage     getClassparamUsage() const {return m_usage;}
private:
  Cpp_Name        m_identifier;
  Templ_ParamKind m_kind;
  int             m_number;
  ClassparamUsage m_usage;
};

inline void Templ_Param::setClassparamUsage( int & error, ClassparamUsage usage ) {

  if (ClassParam != m_kind) {
    X2DC_ErrMsg( M__TEMPLPARAM_KIND_MISMATCH, "Int", m_identifier.getName(), CurrTypeName(), "class" );
    error = 1;
    return;
  }

  switch (usage) {
    case Unspecified : {
      X2DC_ErrMsg( M__BAD_TEMPLCLASSPARAM_USAGE, m_identifier.getName(), CurrTypeName() );
      error = 1;
      break;
    }
    case FullType : {
      switch (m_usage) {
        case Unspecified : {
          m_usage = FullType;
          break;
        }
        case FullType : {
          break;
        }
        case PersClass : {
          X2DC_ErrMsg( M__TEMPLCLASSPARAM_USAGE_MISMATCH, m_identifier.getName(), CurrTypeName(), "persistent class" );
          error = 1;
          break;
        }
      }
      break;
    }
    case PersClass : {
      switch (m_usage) {
        case Unspecified : {
          m_usage = PersClass;
          break;
        }
        case FullType : {
          X2DC_ErrMsg( M__TEMPLCLASSPARAM_USAGE_MISMATCH, m_identifier.getName(), CurrTypeName(), "non-persistent type" );
          error = 1;
          break;
        }
        case PersClass : {
          break;
        }
      }
      break;
    }
  }

}

/*----------------------------------------------------------------------------*/

typedef enum {
  Undefined,
  InstanceForClass,
  InstanceForPersclass,
  ParamForClass,
  InstanceForInt
} Templ_ParamInstanceKind;

/*----------------------------------------------------------------------------*/

class Cpp_Type;
class Pers_Class;

class Templ_ParamInstance
{
public:
  /* Constructor */
  inline Templ_ParamInstance( int & error, Templ_ParamInstanceKind kind, int value );
  inline Templ_ParamInstance( int & error, const Cpp_Type * pType );
  inline Templ_ParamInstance( int & error, const Pers_Class * pClass );
  /* Destructor */
  inline ~Templ_ParamInstance();
  /* Accessors */
  void                      setNumber(int number) {m_number = number;}
  int                       getNumber() const {return m_number;}
  Templ_ParamInstanceKind   getKind() const {return m_kind;}
  inline const int *        getIntPtr() const;
  inline const int *        getParamnoPtr() const;
  inline const Cpp_Type *   getCpptypePtr() const;
  inline const Pers_Class * getPersclassPtr() const;
private:
  int                     m_number;
  Templ_ParamInstanceKind m_kind;
  const void *            m_ptr;
};

inline Templ_ParamInstance::Templ_ParamInstance( int & error, Templ_ParamInstanceKind kind, int value ) {
  if ( (InstanceForInt == kind) || (ParamForClass == kind) ) {
      m_kind   = kind;
      int * pI = new int;
      *pI      = value;
      m_ptr    = pI;
  }
  else {
    m_kind = Undefined;
    m_ptr  = NULL;
    X2DC_ErrMsg( M__BAD_TEMPIN_CONSTRUCTION, "int value" );
    error  = 1;
  }
}

inline Templ_ParamInstance::Templ_ParamInstance( int & error, const Cpp_Type * pType ) {
  if (pType) {
    m_kind = InstanceForClass;
    m_ptr  = pType;
  }
  else {
    m_kind = Undefined;
    m_ptr  = NULL;
    X2DC_ErrMsg( M__BAD_TEMPIN_CONSTRUCTION, "const Cpp_Type * pType" );
    error  = 1;
  }
}

inline Templ_ParamInstance::Templ_ParamInstance( int & error, const Pers_Class * pClass ) {
  if (pClass) {
    m_kind = InstanceForPersclass;
    m_ptr  = pClass;
  }
  else {
    m_kind = Undefined;
    m_ptr  = NULL;
    X2DC_ErrMsg( M__BAD_TEMPIN_CONSTRUCTION, "const Pers_Class * pClass" );
    error  = 1;
  }
}

inline Templ_ParamInstance::~Templ_ParamInstance() {
  if ( (m_ptr) && ( (ParamForClass == m_kind) || (InstanceForInt == m_kind) ) ) {
    void * pV = const_cast<void *>(m_ptr);
    int *  pI = reinterpret_cast<int *>(pV);
    delete pI;
  }
  m_ptr = NULL;
}

inline const int * Templ_ParamInstance::getIntPtr() const {
  if ( (m_ptr) && (InstanceForInt == m_kind) ) {
    return reinterpret_cast<const int *>(m_ptr);
  }
  else {
    return NULL;
  }
}

inline const int * Templ_ParamInstance::getParamnoPtr() const {
  if ( (m_ptr) && (ParamForClass == m_kind) ) {
    return reinterpret_cast<const int *>(m_ptr);
  }
  else {
    return NULL;
  }
}

inline const Cpp_Type * Templ_ParamInstance::getCpptypePtr() const {
  if ( (m_ptr) && (InstanceForClass == m_kind) ) {
    return reinterpret_cast<const Cpp_Type *>(m_ptr);
  }
  else {
    return NULL;
  }
}

inline const Pers_Class * Templ_ParamInstance::getPersclassPtr() const {
  if ( (m_ptr) && (InstanceForPersclass == m_kind) ) {
    return reinterpret_cast<const Pers_Class *>(m_ptr);
  }
  else {
    return NULL;
  }
}


/******************************************************************************/
/*     Class/struct member                                                    */
/******************************************************************************/

typedef enum {
  Initial,
  CppType,
  TemparCppType,  // type of member is a template parameter: Ti mem;
  TempinCppType,  // type of member is a template instance: Templ< T1, T2, ... > mem;
  BitFields,
  Oid,
  ExtOid,
  VarOid,
  Pointer 
} Member_Kind;

/*----------------------------------------------------------------------------*/

class Member_Type
{
friend class Member;
  /* Constructor */
  Member_Type() : m_kind(Initial), m_ptr(NULL), m_pTempinList(NULL) {};
  /* Destructor */
  inline ~Member_Type();
  /* Accessors */
  inline void                 setKindNotPtr( int & error, Member_Kind kind );
  inline void                 setKindAndPtr( int & error, Member_Kind kind, const void * p );
  inline void                 setKindAndPtr( int & error, Member_Kind oldKind,
                                             Member_Kind newKind, const void * p );
  Member_Kind                 getKind() const {return m_kind;}
  inline const Cpp_Type *     getCpptypePtr() const;
  inline const Bit_Fields *   getBitfieldsPtr() const;
  inline const char *         getTemplParamIdentifierPtr() const;
  inline void                 addTemplParamInstance( int & error, Templ_ParamInstance * pTempin );
  List<Templ_ParamInstance> * getTemplParamInstanceList() const {return m_pTempinList;}
  inline const Pers_Class *   getPersclassPtr() const;
  /* Members */
  Member_Kind                 m_kind;
  const void *                m_ptr;
  List<Templ_ParamInstance> * m_pTempinList;
};

inline Member_Type::~Member_Type() {
  const Bit_Fields * pBF = getBitfieldsPtr();
  if (pBF) {
    delete pBF;
  }
  m_ptr = NULL;
  if (m_pTempinList) {
    m_pTempinList->Clear();
    delete m_pTempinList;
    m_pTempinList = NULL;
  }
}

inline void Member_Type::setKindNotPtr( int & error, Member_Kind kind ) {
  switch (kind) {
    case Oid : {
      m_kind = kind;
      m_pTempinList = new List<Templ_ParamInstance>();
      break;
    }
    case ExtOid :
    case VarOid :
    case Pointer : {
      m_kind = kind;
      break;
    }
    default : {
      X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S_D, "Member_Type::setKindNotPtr", "BAD kind", kind );
      error = 1;
      break;
    }
  }
}

inline void Member_Type::setKindAndPtr( int & error, Member_Kind kind, const void * p ) {
  if (!p) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S,
                 "Member_Type::setKindAndPtr( int & error, Member_Kind kind, const void * p )", "p == NULL" );
    error = 1;
    return;
  }
  switch (kind) {
    case CppType :
    case TemparCppType :
    case BitFields : {
      m_kind = kind;
      m_ptr  = p;
      break;
    }
    case TempinCppType: {
      m_kind = kind;
      m_ptr  = p;
      m_pTempinList = new List<Templ_ParamInstance>();
      break;
    }
    default : {
      X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S_D, 
                   "Member_Type::setKindAndPtr( int & error, Member_Kind kind, const void * p )",
                   "BAD kind", kind );
      error = 1;
      break;
    }
  }
}

inline void Member_Type::setKindAndPtr( int & error, Member_Kind oldKind, Member_Kind newKind, const void * p ) {
  if ( (TempinCppType == oldKind) && (CppType == newKind) && (TempinCppType == m_kind) && p ) {
    m_pTempinList->Clear();
    delete m_pTempinList;
    m_pTempinList = NULL;
    m_kind = newKind;
    m_ptr = p;
  }
  else {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN,
                 "Member_Type::setKindAndPtr( int & error, Member_Kind oldKind, Member_Kind newKind, const void * p )" );
    error = 1;
  }
}

inline const Cpp_Type * Member_Type::getCpptypePtr() const {
  if ( (CppType == m_kind) || (TempinCppType == m_kind) ) {
    return reinterpret_cast<const Cpp_Type *>(m_ptr);
  }
  else {
    return NULL;
  }
}

inline const Bit_Fields * Member_Type::getBitfieldsPtr() const {
  if (BitFields == m_kind) {
    return reinterpret_cast<const Bit_Fields *>(m_ptr);
  }
  else {
    return NULL;
  }
}

inline const char * Member_Type::getTemplParamIdentifierPtr() const {
  if (TemparCppType == m_kind) {
    return reinterpret_cast<const char *>(m_ptr);
  }
  else {
    return NULL;
  }
}

inline void Member_Type::addTemplParamInstance( int & error, Templ_ParamInstance * pTempin ) {
  if (TempinCppType == m_kind) {
    if (m_pTempinList->IsEmpty()) {
      pTempin->setNumber(1);
    }
    else {
      pTempin->setNumber(m_pTempinList->getLastItem()->getNumber()+1);
    }
    m_pTempinList->Append(pTempin);
  }
  else if (Oid == m_kind) {
    if (m_pTempinList->IsEmpty()) {
      pTempin->setNumber(1);
      m_pTempinList->Append(pTempin);
    }
    else {
      X2DC_ErrMsg(M__TOO_MANY_OIDMEMBER_PARAMS);
      error = 1;
    }
  }
  else {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S_D, "Member_Type::addTemplParamInstance", "BAD kind", m_kind );
    error = 1;
  }
}

inline const Pers_Class * Member_Type::getPersclassPtr() const {
  if ( (Oid == m_kind) && (!m_pTempinList->IsEmpty()) ) {
    return m_pTempinList->getFirstItem()->getPersclassPtr();
  }
  else {
    return NULL;
  }
}

/*----------------------------------------------------------------------------*/

class Member
{
public:
  /* Constructor */
  inline Member(const char * pName);
  /* Destructor */
  inline ~Member();
  /* Accessors */
  const char *                       getName() const {return m_name.getName();}
  inline void                        setDimension( int & error, int dimension );
  inline void                        linkDimensionToTemplParam( int & error, int negTemparNo );
  int                                getDimension() const {return *m_pDim;}
  void                               setOffset(const int offset) {m_offset = offset;}
  int                                getOffset() const {return m_offset;}
  inline void                        setType( int & error, Member_Kind kind, const void * p = NULL );
  void                               promoteTempinCppTypeToCppType( int & error, const Cpp_Type * p ) {
                                       m_type.setKindAndPtr( error, TempinCppType, CppType, p );}
  Member_Kind                        getKind() const {return m_type.getKind();}
  inline List<Templ_ParamInstance> * getTemplParamInstanceList() const;
  inline int                         getTemplParamInstanceCount() const;
  inline void                        addTemplParamInstance( int & error, Templ_ParamInstance * pTemin );
  const Cpp_Type *                   getCpptypePtr() const {return m_type.getCpptypePtr();}
  const Bit_Fields *                 getBitfieldsPtr() const {return m_type.getBitfieldsPtr();}
  const char *                       getTemplParamIdentifierPtr() const {
                                       return m_type.getTemplParamIdentifierPtr();}
  const Pers_Class *                 getOidPersclassPtr() const {return m_type.getPersclassPtr();}
private:
  Cpp_Name    m_name;
  Member_Type m_type;
  int *       m_pDim;
  int         m_offset;
};

inline Member::Member(const char * pName)
: m_name(pName)
, m_offset(0)
{
  m_pDim  = new int;
  *m_pDim = 1;
};

inline Member::~Member() {
  delete m_pDim;
  m_pDim = NULL;
}

inline void Member::setDimension( int & error, int dimension ) {
  if (dimension < 1) {
    X2DC_ErrMsg( M__BAD_MEMBER_DIMENSION, dimension, getName(), CurrTypeName() );
    error = 1;
  }
  else {
    *m_pDim = dimension;
  }
}

inline void Member::linkDimensionToTemplParam( int & error, int negTemparNo ) {
  if (negTemparNo > -1) {
    X2DC_ErrMsg( M__BAD_NUMBER_FOR_DIMPARAM, -negTemparNo, getName() );
    error = 1;
  }
  else {
    *m_pDim = negTemparNo;
  }
}

inline void Member::setType( int & error, Member_Kind kind, const void * p ) {
  switch (kind) {
    case Initial : {
      X2DC_ErrMsg( M__MEMBER_KIND_UNDEF, getName(), CurrTypeName() );
      error = 1;
      break;
    }
    case CppType :
    case TemparCppType :
    case TempinCppType :
    case BitFields : {
      if (p) {
        m_type.setKindAndPtr( error, kind, p );
      }
      else {
        X2DC_ErrMsg( M__NULLPTR_FOR_MEMBER_TYPE, getName() );
        error = 1;
      }
      break;
    }
    case Oid :
    case ExtOid :
    case VarOid :
    case Pointer : {
      if (!p) {
        m_type.setKindNotPtr( error, kind );
      }
      else {
        X2DC_ErrMsg( M__PTR_FOR_MEMBER_TYPE, getName() );
        error = 1;
      }
      break;
    }
  }
}

inline List<Templ_ParamInstance> * Member::getTemplParamInstanceList() const {
  if ( TempinCppType == getKind() || Oid == getKind() ) {
    return m_type.getTemplParamInstanceList();
  }
  else {
    return NULL;
  }
}

inline int Member::getTemplParamInstanceCount() const {
  if ( ( (TempinCppType == getKind()) || (Oid == getKind()) ) && 
       (m_type.getTemplParamInstanceList()) && (!m_type.getTemplParamInstanceList()->IsEmpty()) ) {
      return m_type.getTemplParamInstanceList()->getLastItem()->getNumber();
  }
  else {
    return 0;
  }
}

inline void Member::addTemplParamInstance( int & error, Templ_ParamInstance * pTempin ) {
  if ( (TempinCppType == getKind()) || (Oid == getKind()) ) {
    m_type.addTemplParamInstance( error, pTempin );
  }
  else {
    X2DC_ErrMsg( M__NON_TEMPL_MEMBER, getName() );
    error = 1;
  }
}


/******************************************************************************/
/*     Base_Class                                                             */
/******************************************************************************/

class Base_Class
{
public:
  /* Constructor */
  Base_Class(const Cpp_Type * pType) : m_pType(pType), m_offset(0) {};
  /* Destructor */
  ~Base_Class() {m_pType = NULL;}
  /* Accessors */
  const Cpp_Type * getType() const {return m_pType;}
  void             setOffset(int offset) {m_offset = offset;}
  int              getOffset() const {return m_offset;}
private:
  const Cpp_Type * m_pType;
  int              m_offset;
};


/******************************************************************************/
/*     Cpp_Type                                                               */
/******************************************************************************/

class Cpp_Type
{
public:
  /* Constructor */
  inline Cpp_Type( const char * pName, bool fullType = true, bool templ = false );
  /* Destructor */
  inline ~Cpp_Type();
  /* Accessors */
  inline const char *        getName() const {return m_pNameList->getFirstItem()->getName();}
  inline void                addSynonym( int & error, const char * pSynonym );
  inline bool                isSynonym(const char * pSynonym) const;
  bool                       isFullType() const {return m_fullType;}
  void                       setHidden() {m_hidden = true;}
  bool                       isHidden() const {return m_hidden;}
  bool                       isTemplate() const {return m_template;}
  List<Templ_Param> *        getTemplParamList() const {return m_pTemparList;}
  inline int                 getTemplParamCount() const;
  inline void                addTemplParam( int & error, Templ_Param * pTempar );
  inline const Templ_Param * getNthTemplParam(int N) const;
  inline bool                isClassTemplParam(const char * pIdent);
  inline bool                isIntTemplParam(const char * pIdent);
  inline void                addBaseClass( int & error, Base_Class * pClass, bool isPersClass = false );
  bool                       isDerived() const {return (!m_pBaseclassList->IsEmpty());}
  inline const Base_Class *  findBaseClass(const char * pName) const;
  inline const Base_Class *  getLastBaseClass() const;
  List<Base_Class> *         getBaseClassList() const {return m_pBaseclassList;}
  void                       setSize(const int size) {m_size = size;}
  int                        getSize() const {return m_size;}
  void                       setAlignment(const int alignment) {m_alignment = alignment;}
  int                        getAlignment() const {return m_alignment;}
  inline void                setDefault(const char * pDefault);
  const char *               getDefault() const {return m_pDefault;};
  void                       setVtType(const short vtType) {m_vtType = vtType;}
  short                      getVtType() const {return m_vtType;}
  inline bool                hasMember() const {return (!m_pMemberList->IsEmpty());}
  inline bool                hasMember(const char * pName) const;
  inline void                addMember( int & error, Member * pMember );
  List<Member> *             getMemberList() const {return m_pMemberList;}
  inline const Member *      findCpptypeMember( int & error, const char * pName ) const;
  inline int                 getMemberAlignment( int & error, const Member * pMember );
  inline int                 getMemberSize( int & error, const Member * pMember ) const;
  inline short               getMemberVtType(const Member * pMember) const;
  void                       incBitfieldCount() {m_bitfieldCount++;}
  int                        getBitfieldCount() {return m_bitfieldCount;}
private:
  List<Cpp_Name> *    m_pNameList;
  bool                m_hidden;
  bool                m_fullType;
  bool                m_template;
  List<Templ_Param> * m_pTemparList;
  List<Base_Class> *  m_pBaseclassList;
  int                 m_size;
  int                 m_alignment;
  char *              m_pDefault;
  short               m_vtType;
  List<Member> *      m_pMemberList;
  int                 m_bitfieldCount;
};

inline Cpp_Type::Cpp_Type( const char * pName, bool fullType, bool templ )
: m_fullType(fullType)
, m_hidden(false)
, m_template(templ)
, m_pTemparList(NULL)
, m_size(0)
, m_alignment(0)
, m_pDefault(NULL)
, m_vtType(csp1_vt_empty)
, m_bitfieldCount(0)
{
  Cpp_Name * pN = new Cpp_Name(pName);
  m_pNameList = new List<Cpp_Name>(pN);
  if (templ) {
    m_pTemparList = new List<Templ_Param>();
  }
  m_pMemberList = new List<Member>();
  m_pBaseclassList = new List<Base_Class>();
}

inline Cpp_Type::~Cpp_Type() {
  // Name list
  m_pNameList->Clear();
  delete m_pNameList;
  m_pNameList = NULL;
  // Tempalate parameter list
  if (m_template) {
    m_pTemparList->Clear();
    delete m_pTemparList;
    m_pTemparList = NULL;
  }
  // Base class list
  m_pBaseclassList->Clear();
  delete m_pBaseclassList;
  m_pBaseclassList = NULL;
  // Default
  if (m_pDefault) {
    delete [] m_pDefault;
    m_pDefault = NULL;
  }
  // Member list
  m_pMemberList->Clear();
  delete m_pMemberList;
  m_pMemberList = NULL;
}

inline void Cpp_Type::addSynonym( int & error, const char * pSynonym ) {
  /* Check uniqueness */
  Iterator<Cpp_Name> iter = m_pNameList->Begin();
  while (iter()) {
    if (0 == strcmp( iter()->getName(), pSynonym )) {
      X2DC_ErrMsg( M__DUPLICATE, "type", getName(), "synonym", pSynonym );
      error = 1;
      return;
    }
    else {
      iter++;
    }
  }
  /* Add */
  Cpp_Name * pN = new Cpp_Name(pSynonym);
  m_pNameList->Append(pN);
}

inline bool Cpp_Type::isSynonym(const char * pSynonym) const {
  Iterator<Cpp_Name> iter = m_pNameList->Begin();
  if (iter()) {
    iter++; // first element holds the name not a synonym
  }
  while (iter()) {
    if (0 == strcmp( iter()->getName(), pSynonym )) {
      return true;
    }
    else {
      iter++;
    }
  }
  return false;
}

inline int Cpp_Type::getTemplParamCount() const {
  if ( (m_template) && (m_pTemparList->getLastItem()) ) {
    return m_pTemparList->getLastItem()->getNumber();
  }
  else {
    return 0;
  }
}

inline void Cpp_Type::addTemplParam( int & error, Templ_Param * pTempar ) {
  if (!m_template) {
    X2DC_ErrMsg( M__NON_TEMPL_TYPE, getName() );
    error = 1;
    return;
  }
  else {
    /* Check uniqueness */
    Iterator<Templ_Param> iter = m_pTemparList->Begin();
    while (iter()) {
      if (0 == strcmp( iter()->getIdentifier(), pTempar->getIdentifier() )) {
        X2DC_ErrMsg( M__DUPLICATE, "template", getName(), "parameter", pTempar->getIdentifier() );
        error = 1;
        return;
      }
      else {
        iter++;
      }
    }
    /* Add parameter to list */
    if (m_pTemparList->IsEmpty()) {
      pTempar->setNumber(1);
    }
    else {
      pTempar->setNumber(m_pTemparList->getLastItem()->getNumber()+1);
    }
    m_pTemparList->Append(pTempar);
  }
}

inline const Templ_Param * Cpp_Type::getNthTemplParam(int N) const {
  if ( (m_template) && (!m_pTemparList->IsEmpty()) ) {
    Iterator<Templ_Param> iter = m_pTemparList->Begin();
    while (iter()) {
      if (iter()->getNumber() == N) {
        return iter();
      }
      else {
        iter++;
      }
    }
  }
  return NULL;
}

inline bool Cpp_Type::isClassTemplParam(const char * pIdent) {
  bool isTempar = false;
  if (m_template) {
    Iterator<Templ_Param> iter = m_pTemparList->Begin();
    while (iter()) {
      if ( (iter()->getKind() == ClassParam) &&
           (0 == strcmp( iter()->getIdentifier(), pIdent )) ) {
        isTempar = true;
        break;
      }
      else {
        iter++;
      }
    }
  }
  return isTempar;
}

inline bool Cpp_Type::isIntTemplParam(const char * pIdent) {
  bool isTempar = false;
  if (m_template) {
    Iterator<Templ_Param> iter = m_pTemparList->Begin();
    while (iter()) {
      if ( (iter()->getKind() == IntParam) &&
           (0 == strcmp( iter()->getIdentifier(), pIdent )) ) {
        isTempar = true;
        break;
      }
      else {
        iter++;
      }
    }
  }
  return isTempar;
}

inline void Cpp_Type::addBaseClass( int & error, Base_Class * pClass, bool isPersClass ) {
  /* Check uniqueness */
  const Base_Class * pBC = findBaseClass(pClass->getType()->getName());
  if (pBC) {
    X2DC_ErrMsg( M__DUPLICATE, "type", getName(), "base class", pBC->getType()->getName() );
    error = 1;
    return;
  }
  /* Check uniqueness of member names */
  if (!m_pBaseclassList->IsEmpty()) {
    Iterator<Member> iter = pClass->getType()->getMemberList()->Begin();
    while (iter()) {
      if (hasMember(iter()->getName())) {
        X2DC_ErrMsg( M__DUPLICATE, "type", getName(), "member", iter()->getName() );
        error = 1;
        return;
      }
      iter++;
    }
  }
  /* Add base class */
  if (m_template) {
    m_pBaseclassList->Append(pClass);
  }
  else {
    if (m_pBaseclassList->IsEmpty()) {
      if ( ( (0 != m_size) && ( (m_size != SizeOfPointer()) || (!isPersClass) ) ) ||
           ( (0 == m_size) && (isPersClass) ) ) {
          X2DC_ErrMsg( M__BAD_FIRST_BASE_CLASS, getName() );
          error = 1;
          return;
      }
      else {
        pClass->setOffset(m_size + PadSize( m_size, pClass->getType()->getAlignment() ));
      }
    }
    else {
      pClass->setOffset( m_pBaseclassList->getLastItem()->getOffset() +
                         m_pBaseclassList->getLastItem()->getType()->getSize() +
                         PadSize( m_pBaseclassList->getLastItem()->getOffset() +
                                  m_pBaseclassList->getLastItem()->getType()->getSize(),
                                  pClass->getType()->getAlignment() ) );
    }
    m_pBaseclassList->Append(pClass);
    // Adjust size
    m_size = pClass->getOffset() + pClass->getType()->getSize();
    // Adjust alignment
    if (pClass->getType()->getAlignment() > m_alignment) {
      m_alignment = pClass->getType()->getAlignment();
    }
  }
}

inline const Base_Class * Cpp_Type::findBaseClass(const char * pName) const {
  Iterator<Base_Class> iter = m_pBaseclassList->Begin();
  while (iter()) {
    if (0 == strcmp( iter()->getType()->getName(), pName ) ) {
      return iter();
    }
    else {
      iter++;
    }
  }
  return NULL;
}

inline const Base_Class * Cpp_Type::getLastBaseClass() const {
  if (!m_pBaseclassList->IsEmpty()) {
    return m_pBaseclassList->getLastItem();
  }
  else {
    return NULL;
  }
}

inline void Cpp_Type::setDefault(const char * pDefault) {
  m_pDefault = new char[strlen(pDefault)+1];
  strcpy( m_pDefault, pDefault );
}

inline void Cpp_Type::addMember( int & error, Member * pMember ) {
  /* Check uniqueness */
  if (hasMember(pMember->getName())) {
    X2DC_ErrMsg( M__DUPLICATE, "type", getName(), "member", pMember->getName() );
    error = 1;
    return;
  }
  /* Add member to list */
  m_pMemberList->Append(pMember);
  /* For a non-tempalte, adjust alignment and size, and set offset of member */
  if (!m_template) {
    int memAli  = getMemberAlignment( error, pMember );
    if (error) {
      return;
    }
    int padSize = PadSize( m_size, memAli );
    int memSize = getMemberSize( error, pMember );
    if (error) {
      return;
    }
    // Adjust alignment
    if (memAli > m_alignment) {
      m_alignment = memAli;
    }
    // Set member offset
    pMember->setOffset(m_size + padSize);
    // Adjust size
    m_size = m_size + padSize + memSize;
  }
}

inline bool Cpp_Type::hasMember(const char * pName) const {
  if (isDerived()) {
    Iterator<Base_Class> bcIter = m_pBaseclassList->Begin();
    while (bcIter()) {
      if (bcIter()->getType()->hasMember(pName)) {
        return true;
      }
      else {
        bcIter++;
      }
    }
  }
  Iterator<Member> mIter = m_pMemberList->Begin();
  while (mIter()) {
    if (0 == strcmp( mIter()->getName(), pName )) {
      return true;
    }
    else {
      mIter++;
    }
  }
  return false;
}

inline const Member * Cpp_Type::findCpptypeMember( int & error, const char * pName ) const {
  Iterator<Member> iter = m_pMemberList->Begin();
  while (iter()) {
    if (0 == strcmp( iter()->getName(), pName )) {
      if (iter()->getKind() != CppType) {
        X2DC_ErrMsg( M__BAD_TYPE_MEMBER, "Classification", pName, getName(), "not a C++ type" );
        error = 1;
        return NULL;
      }
      else {
        return iter();
      }
    }
    else {
      iter++;
    }
  }
  return NULL;
}

inline int Cpp_Type::getMemberAlignment( int & error, const Member * pMember ) {
  const Cpp_Type * pType = NULL;
  int ali  = 0;
  switch (pMember->getKind()) {
    case Initial : {
      X2DC_ErrMsg( M__NO_MEMBER_INFO, "alignment", pMember->getName(), "classification UNDEFINED" );
      error = 1;
      break;
    }
    case CppType : {
      pType  = pMember->getCpptypePtr();
      if (!pType) {
        X2DC_ErrMsg( M__NO_MEMBER_INFO, "alignment", pMember->getName(), "type not found" );
        error = 1;
      }
      else {
        ali = pType->getAlignment();
      }
      break;
    }
    case BitFields : {
      ali = 4;
      break;
    }
    case Oid :
    case ExtOid :
    case VarOid : {
      ali = AlignmentOfOid();
      break;
    }
    case Pointer : {
      ali = AlignmentOfPointer();
      break;
    }
  }
  return ali;
}

inline int Cpp_Type::getMemberSize( int & error, const Member * pMember ) const {
  const Cpp_Type * pType = NULL;
  const Bit_Fields * pBF = NULL;
  int size = 0;
  switch (pMember->getKind()) {
    case Initial : {
      X2DC_ErrMsg( M__NO_MEMBER_INFO, "size", pMember->getName(), "classification UNDEFINED" );
      error = 1;
      break;
    }
    case CppType : {
      pType  = pMember->getCpptypePtr();
      if (!pType) {
        X2DC_ErrMsg( M__NO_MEMBER_INFO, "size", pMember->getName(), "type not found" );
        error = 1;
      }
      else {
        size = pMember->getDimension() * pType->getSize();
      }
      break;
    }
    case BitFields : {
      pBF  = pMember->getBitfieldsPtr();
      if (!pBF) {
        X2DC_ErrMsg( M__NO_MEMBER_INFO, "size", pMember->getName(), "bit field list not found" );
        error = 1;
      }
      else {
        size = pBF->getSize();
      }
      break;
    }
    case Oid :
    case ExtOid :
    case VarOid : {
      size = pMember->getDimension() * SizeOfOid();
      break;
    }
    case Pointer : {
      size = SizeOfPointer();
      break;
    }
  }
  return size;
}

inline short Cpp_Type::getMemberVtType(const Member * pMember) const {
  switch (pMember->getKind()) {
    case CppType : {
      return pMember->getCpptypePtr()->getVtType();
    }
    case BitFields : {
      return csp1_vt_lc_bitfields;
    }
    case Oid : {
      return csp1_vt_lc_oid;
    }
    case ExtOid : {
      return csp1_vt_lc_extoid;
    }
    case VarOid : {
      return csp1_vt_lc_varoid;
    }
    default : {
      return csp1_vt_empty;
    }
  }
}


/******************************************************************************/
/*     Pers_Interface                                                         */
/******************************************************************************/

class Pers_Interface
{
public:
  /* Constructor */
  Pers_Interface( const char * pName, const int ID ) : m_name(pName), m_ID(ID) {};
  /* Destructor */
  ~Pers_Interface() {};
  /* Accessors */
  const char * getName() const {return m_name.getName();}
  int          getID() const {return m_ID;}
private:
  Cpp_Name m_name;
  int      m_ID;
};


/******************************************************************************/
/*     Pers_Class                                                             */
/******************************************************************************/

typedef enum { Nop, Add, Sub, Mul, Div } X2DC_Operation;
typedef enum { Value, Identifier, TemplParam, SizeOf, Expression } X2DC_OperandType;

class PTS_Operand;

class PTS_Expression
{
public:
  /* Constructor */
  PTS_Expression() : m_oper(Nop), m_pOp1(NULL), m_pOp2(NULL), m_pWrapper(NULL) {};
  /* Destructor */
  inline ~PTS_Expression();
  /* Accessors */
  inline void      setOperation( int & error, X2DC_Operation oper );
  X2DC_Operation   getOperation() const {return m_oper;}
  inline void      setNextOperand( int & error, PTS_Operand * pOp );
  PTS_Operand *    getOperand1() const {return m_pOp1;}
  PTS_Operand *    getOperand2() const {return m_pOp2;}
  inline void      setWrapper( int & error, PTS_Expression * pPtsExpr );
  PTS_Expression * getWrapper() const {return m_pWrapper;}
private:
  X2DC_Operation   m_oper;
  PTS_Operand *    m_pOp1;
  PTS_Operand *    m_pOp2;
  PTS_Expression * m_pWrapper;
};

inline PTS_Expression::~PTS_Expression() {
  m_pOp1 = NULL;
  m_pOp2 = NULL;
  m_pWrapper = NULL;
}

inline void PTS_Expression::setOperation( int & error, X2DC_Operation oper ) {
  if (Nop == oper) {
    X2DC_ErrMsg(M__NO_OPERATION);
    error = 1;
    return;
  }
  else if (Nop != m_oper) {
    X2DC_ErrMsg( M__DUPLICATE_OPERATION, "pts" );
    error = 1;
    return;
  }
  if ( m_pOp1 || m_pOp2 ) {
    X2DC_ErrMsg(M__OPERATION_FIRST);
    error = 1;
    return;
  }
  else {
    m_oper = oper;
  }
}

inline void PTS_Expression::setNextOperand( int & error, PTS_Operand * pOp ) {
  if (m_pOp2) {
    X2DC_ErrMsg( M__TOO_MANY_OPERANDS, "pts" );
    error = 1;
  }
  else {
    if (!m_pOp1) {
      m_pOp1 = pOp;
    }
    else {
      m_pOp2 = pOp;
    }
  }
}

inline void PTS_Expression::setWrapper( int & error, PTS_Expression * pPtsExpr ) {
  if (m_pWrapper) {
    X2DC_ErrMsg(M__EXPRESSION_ALREADY_EMBEDDED);
    error = 1;
  }
  else {
    m_pWrapper = pPtsExpr;
  }
}

class PTS_Operand
{
public:
  /* Constructor */
  PTS_Operand(X2DC_OperandType type) : m_type(type), m_ptr(NULL) {};
  /* Destructor */
  inline ~PTS_Operand();
  /* Accessors */
  inline void      switchTypeToTemplParam(int & error);
  X2DC_OperandType getType() const {return m_type;}
  void             setPointer(const void * pVoid) {m_ptr = pVoid;}
  const void *     getPointer() const {return m_ptr;}
private:
  X2DC_OperandType m_type;
  const void *     m_ptr;
};

inline PTS_Operand::~PTS_Operand() {
  if (!m_ptr) {
    return;
  }
  switch (m_type) {
    case Value :
    case Identifier :
    case TemplParam : {
      const int * pInt = reinterpret_cast<const int *>(m_ptr);
      delete pInt;
      break;
    }
    case SizeOf :
    case Expression : {
      break;
    }
  }
  m_ptr = NULL;
}

inline void PTS_Operand::switchTypeToTemplParam(int & error) {
  if (Identifier == m_type) {
    m_type = TemplParam;
  }
  else {
    X2DC_ErrMsg(M__BAD_OPERAND_SWITCH);
    error = 1;
  }
}

/*----------------------------------------------------------------------------*/

class Index_Member
{
public:
  /* Constructor */
  inline Index_Member( int & error, const char * pName, const Cpp_Type * pType );
  /* Destructor */
  ~Index_Member() {m_pType = NULL;}
  /* Accessors */
  const char *     getName() const {return m_name.getName();}
  int              getOffset() const {return m_offset;}
  const Cpp_Type * getType() const {return m_pType;}
  bool             isOid() const {return m_isOid;}
  int              getDimension() const {return m_dimension;}
  int              getLevel() const {return m_level;}
private:
  Cpp_Name         m_name;
  int              m_offset;
  const Cpp_Type * m_pType;
  bool             m_isOid;
  int              m_dimension;
  int              m_level;
};

inline Index_Member::Index_Member( int & error, const char * pName, const Cpp_Type * pType )
: m_name(pName)
, m_offset(0)
, m_pType(NULL)
, m_isOid(false)
, m_dimension(1)
, m_level(0)
{

  /* Local variables */

  char * name         = NULL;
  char * indStr       = NULL;
  int    indVal       = -1;
  const Member * pM   = NULL;
  const Cpp_Type * pT = pType;
  unsigned int startPos = 0, dotPos = 0, osbPos = 0;

  char classType[25+1]; // 25 == strlen("persistent template class")
  if (pType->isTemplate()) {
    strcpy( &classType[0], "persistent template class" );
  }
  else {
    strcpy( &classType[0], "persistent class" );
  }

  /* Process member name */

  while (startPos < strlen(pName)) {
    if (strchr( pName+startPos, '.' )) {
      dotPos = strcspn( pName+startPos, "." );
    }
    else {
      dotPos = strlen(pName+startPos);
    }
    name = new char[dotPos+1];
    strncpy( name, pName+startPos, dotPos );
    name[dotPos] = 0;
    ++m_level;
    startPos += dotPos + 1;
    // Check if array element
    if (name[dotPos-1] == ']') {
      if (strchr( name, '[' )) {
        osbPos = strcspn( name, "[" );
      }
      else {
        X2DC_ErrMsg( M__BAD_INDEX_MEMBER, pName, classType, pType->getName() );
        error = 1;
        return;
      }
      if (osbPos == dotPos-2) {
        X2DC_ErrMsg( M__BAD_INDEX_MEMBER, pName, classType, pType->getName() );
        error = 1;
        return;
      }
      else {
        ++m_level;
        if ( (osbPos == dotPos-3) && (name[dotPos-2] == '0') ) {
          indVal = 0;
        }
        else {
          indStr = new char[(dotPos-1)-osbPos];
          strncpy( indStr, name+osbPos+1, (dotPos-1)-osbPos-1 );
          indStr[(dotPos-1)-osbPos-1] = 0;
          indVal = atoi(indStr);
          delete [] indStr;
          indStr = NULL;
          if (indVal < 1) {
            X2DC_ErrMsg( M__BAD_INDEX_MEMBER, pName, classType, pType->getName() );
            error = 1;
            return;
          }
        }
      }
      name[osbPos] = 0;
    }
    // Check for derivation
    if (strchr( name, ':' )) {
      char * bcName          = NULL;
      const Base_Class * pBC = NULL;
      unsigned int isPos = 0, colPos = 0;
      while (isPos < strlen(name)) {
        if (!strchr( name+isPos, ':' )) {
          char * memName = new char[strlen(name)-isPos+1];
          strncpy( memName, &name[isPos], strlen(name)-isPos+1 );
          delete [] name;
          name = new char[strlen(memName)+1];
          strcpy( name, memName );
          delete [] memName;
          break;
        }
        else {
          colPos = strcspn( name+isPos, ":" );
          if ( (colPos == 0) || (colPos > strlen(name)-3) || (name[colPos+1] != ':') ) {
            X2DC_ErrMsg( M__BAD_INDEX_MEMBER, pName, classType, pType->getName() );
            error = 1;
            return;
          }
          bcName = new char[colPos+1];
          strncpy( bcName, name+isPos, colPos );
          bcName[colPos] = 0;
          isPos += colPos + 2;
          pBC = pT->findBaseClass(bcName);
          delete [] bcName;
          if (pBC) {
            m_offset += pBC->getOffset();
            pT = pBC->getType();
          }
          else {
            X2DC_ErrMsg( M__BAD_INDEX_MEMBER, pName, classType, pType->getName() );
            error = 1;
            return;
          }
        }
      }
    }
    // Find member
    pM = pT->findCpptypeMember( error, name );
    if (!pM) {
      X2DC_ErrMsg( M__MEMBER_NOT_FOUND, name, classType, pType->getName() );
      error = 1;
      return;
    }
    if (error) {
      return;
    }
    // Check dimension
    if (pM->getDimension() == 1) {
      if (indVal > -1) {
        X2DC_ErrMsg( M__MEMBER_NOT_FOUND, name, classType, pType->getName() );
        error = 1;
        return;
      }
    }
    else {
      if (indVal == -1) {
        if (startPos > strlen(pName)) {
          m_dimension = pM->getDimension();
        }
        else {
          X2DC_ErrMsg( M__BAD_INDEX_MEMBER, pName, classType, pType->getName() );
          error = 1;
          return;
        }
      }
      else {
        if (indVal > pM->getDimension()-1) {
          X2DC_ErrMsg( M__BAD_INDEX_MEMBER, pName, classType, pType->getName() );
          error = 1;
          return;
        }
      }
    }
    // Get member type
//    pT = pM->getCpptypePtr();
//    if (NULL == pT) {
//      if ( (Oid == pM->getKind()) && || (startPos >= strlen(pName)) ) {
//        m_isOid = true;
//      }
//      else {
//        X2DC_ErrMsg("CTRH_ERROR_39");
//        error = 1;
//        return;
//      }
//    }
    if ( (CppType == pM->getKind()) &&
         (0 != strncmp( pM->getCpptypePtr()->getName(),
                        "X2DC__Gen__Type__Name__", strlen("X2DC__Gen__Type__Name__") )) ) {
      pT = pM->getCpptypePtr();
    }
    else if ( (Oid == pM->getKind()) && (startPos >= strlen(pName)) ) {
      m_isOid = true;
      pT = NULL;
    }
    else {
      X2DC_ErrMsg( M__BAD_INDEX_MEMBER, pName, classType, pType->getName() );
      error = 1;
      return;
    }
    // Adjust offset
    m_offset += pM->getOffset();
    if (indVal > -1) {
      if (pT) {
        m_offset += indVal * pT->getSize();
      }
      else { // Oid
        m_offset += indVal * SizeOfOid();
      }
    }
    // Reset local variables
    delete [] name;
    name = NULL;
    indVal = -1;
  }
  m_pType = pT;

  /* For templates, register only name and type */

  if (pType->isTemplate()) {
    m_offset    = 0;
    m_dimension = 0;
    m_level     = 0;
  }

}

/*----------------------------------------------------------------------------*/

class Index
{
public:
  /* Constructor */
  inline Index(int ID);
  /* Destructor */
  inline ~Index();
  /* Accessors */
  int                  getID() const {return m_ID;}
  inline void          addMember( int & error, const Index_Member * pMember );
  List<Index_Member> * getMemberList() const {return m_pMemberList;}
  int                  getMaxMemberNameLen() const {return m_maxMemNameLen;}
private:
  int                  m_ID;
  List<Index_Member> * m_pMemberList;
  unsigned int         m_maxMemNameLen;
};

inline Index::Index(int ID)
: m_ID(ID)
, m_maxMemNameLen(0)
{
  m_pMemberList = new List<Index_Member>();
}

inline Index::~Index() {
  m_pMemberList->Clear();
  delete m_pMemberList;
  m_pMemberList = NULL;
}

inline void Index::addMember( int & error, const Index_Member * pMember ) {
  /* Check uniqueness */
  Iterator<Index_Member> iter = m_pMemberList->Begin();
  while (iter()) {
    if (0 == strcmp( iter()->getName(), pMember->getName() )) {
      char classType[25+1]; // 25 == strlen("persistent template class")
      if (CurrTypeIsTemplate()) {
        strcpy( classType, "persistent template class" );
      }
      else {
        strcpy( classType, "persistent class" );
      }
      if (0 == m_ID) {
        X2DC_ErrMsg( M__DUPLICATE_KEY_MEMBER, classType, CurrTypeName(), pMember->getName() );
      }
      else {
        X2DC_ErrMsg( M__DUPLICATE_INDEX_MEMBER, classType, CurrTypeName(), pMember->getName(), m_ID );
      }
      error = 1;
      return;
    }
    else {
      iter++;
    }
  }
  /* Add member to list */
  m_pMemberList->Append(pMember);
  if (m_maxMemNameLen < strlen(pMember->getName())) {
    m_maxMemNameLen = strlen(pMember->getName());
  }
}

/*----------------------------------------------------------------------------*/

class Pers_Class
{
public:
  /* Constructors */
  inline Pers_Class( int & error, const int ID, const Cpp_Type * pType );
  inline Pers_Class( int & error, const Cpp_Type * pT );
  /* Destructor */
  inline ~Pers_Class();
  /* Accessors */
  inline void            linkIDToTemplParam( int & error, int negTemparNo );
  int                    getID() const {return m_ID;}
  inline const char *    getName() const;
  const Cpp_Type *       getClass() const {return m_pClass;}
  inline bool            isTemplate() const;
  inline void            setPtsExpression( int & error, const PTS_Expression * pPtsExpr );
  const PTS_Expression * getPtsExpression() const {return m_pPtsExpr;}
  inline void            delPtsExpression(const PTS_Expression * pPtsExpr);
  inline int             getAlignment() const;
  inline int             getSize() const;
  void                   setKey(const Index * pIndex) {m_pKey = pIndex;}
  const Index *          getKey() const {return m_pKey;}
  inline void            addIndex( int & error, const Index * pIndex );
  List<Index> *          getIndexList() const {return m_pIndexList;}
private:
  int                    m_ID;
  const Cpp_Type *       m_pClass;
  const PTS_Expression * m_pPtsExpr;
  const Index *          m_pKey;
  List<Index> *          m_pIndexList;
};

inline Pers_Class::Pers_Class( int & error, const int ID, const Cpp_Type * pType )
: m_ID(ID)
, m_pClass(pType)
, m_pPtsExpr(NULL)
, m_pKey(NULL)
{
  m_pIndexList = new List<Index>();
  if (!pType) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S,
                 "Pers_Class::Pers_Class( int & error, const int ID, const Cpp_Type * pType )",
                 "pType == NULL" );
    error = 1;
  }
  else if (!pType->isFullType()) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S_S,
                 "Pers_Class::Pers_Class( int & error, const int ID, const Cpp_Type * pType )",
                 pType->getName(), "is not a full type" );
    error = 1;
  }
  else if (pType->isTemplate()) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S_S,
                 "Pers_Class::Pers_Class( int & error, const int ID, const Cpp_Type * pType )",
                 pType->getName(), "is a template" );
    error = 1;
  }
}

inline Pers_Class::Pers_Class( int & error, const Cpp_Type * pType )
: m_ID(0)
, m_pClass(pType)
, m_pPtsExpr(NULL)
, m_pKey(NULL)
{
  m_pIndexList = new List<Index>();
  if (!pType) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S,
                 "Pers_Class::Pers_Class( int & error, const Cpp_Type * pType )",
                 "pType == NULL" );
    error = 1;
    return;
  }
  else if (!pType->isFullType()) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S_S,
                 "Pers_Class::Pers_Class( int & error, const Cpp_Type * pType )",
                 pType->getName(), "is not a full type" );
    error = 1;
    return;
  }
  else if (!pType->isTemplate()) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S_S,
                 "Pers_Class::Pers_Class( int & error, const Cpp_Type * pType )",
                 pType->getName(), "is not a template" );
    error = 1;
    return;
  }
}

inline Pers_Class::~Pers_Class() {
  // Class
  delete m_pClass;
  m_pClass = NULL;
  // PTS expression
  delPtsExpression(m_pPtsExpr);
  delete m_pPtsExpr;
  m_pPtsExpr = NULL;
  // Key
  delete m_pKey;
  m_pKey = NULL;
  // Index list
  m_pIndexList->Clear();
  delete m_pIndexList;
  m_pIndexList = NULL;
}

inline void Pers_Class::linkIDToTemplParam( int & error, int negTemparNo ) {
  if (!m_pClass->isTemplate()) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S_S, "Pers_Class::linkIDToTemplParam",
                 m_pClass->getName(), "is not a template" );
    error = 1;
  }
  else if (negTemparNo > -1) {
    X2DC_ErrMsg( M__BAD_NUMBER_FOR_IDPARAM, -negTemparNo, m_pClass->getName() );
    error = 1;
  }
  else {
    m_ID = negTemparNo;
  }
}

inline const char * Pers_Class::getName() const {
  if (m_pClass) {
    return m_pClass->getName();
  }
  else {
    return NULL;
  }
}

inline bool Pers_Class::isTemplate() const {
  bool templ = true;
  if ( (!m_pClass) || (!m_pClass->isTemplate()) ) {
    templ = false;
  }
  return templ;
}

inline void Pers_Class::setPtsExpression( int & error, const PTS_Expression * pPtsExpr ) {
  if (!pPtsExpr) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "Pers_Class::setPtsExpression", "pPtsExpr == NULL" );
    error = 1;
    return;
  }
  else if (!isTemplate()) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S_S, "Pers_Class::setPtsExpression",
                 getName(), "is not a template" );
    error = 1;
    return;
  }
  else {
    m_pPtsExpr = pPtsExpr;
  }
}

inline void Pers_Class::delPtsExpression(const PTS_Expression * pPtsExpr) {
  const PTS_Operand * pPtsOper = NULL;
  if (!pPtsExpr) {
    return;
  }
  else {
    pPtsOper = pPtsExpr->getOperand1();
    switch (pPtsOper->getType()) {
      case Value :
      case Identifier :
      case TemplParam :
      case SizeOf : {
        delete pPtsOper;
        break;
      }
      case Expression : {
        const PTS_Expression * pExpr = reinterpret_cast<const PTS_Expression *>(pPtsOper->getPointer());
        delPtsExpression(pExpr);
        delete pExpr;
        delete pPtsOper;
        break;
      }
    }
    pPtsOper = pPtsExpr->getOperand2();
    switch (pPtsOper->getType()) {
      case Value :
      case Identifier :
      case TemplParam :
      case SizeOf : {
        delete pPtsOper;
        break;
      }
      case Expression : {
        const PTS_Expression * pExpr = reinterpret_cast<const PTS_Expression *>(pPtsOper->getPointer());
        delPtsExpression(pExpr);
        delete pExpr;
        delete pPtsOper;
        break;
      }
    }
  }
}

inline int Pers_Class::getAlignment() const {
  if (m_pClass) {
    return m_pClass->getAlignment();
  }
  else {
    return 0;
  }
}

inline int Pers_Class::getSize() const {
  if (m_pClass) {
    return m_pClass->getSize();
  }
  else {
    return 0;
  }
}

inline void Pers_Class::addIndex( int & error, const Index * pIndex ) {
  /* Check uniqueness */
  Iterator<Index> iter = m_pIndexList->Begin();
  while (iter()) {
    if (iter()->getID() == pIndex->getID()) {
      char classType[25+1]; // 25 == strlen("persistent template class")
      if (isTemplate()) {
        strcpy( &classType[0], "persistent template class" );
      }
      else {
        strcpy( &classType[0], "persistent class" );
      }
      char ID[11];
      sprintf( ID, "%d", pIndex->getID() );
      X2DC_ErrMsg( M__DUPLICATE, classType, getName(), "index", ID );
      error = 1;
      return;
    }
    else {
      iter++;
    }
  }
  /* Add index to list */
  m_pIndexList->Append(pIndex);
}


/******************************************************************************/
/*     Const_Expr                                                             */
/******************************************************************************/

typedef enum { Plus, Minus } X2DC_Operator;
typedef enum { Fixed, Ident } X2DC_Operand;

class Const_Expr
{
public:
  /* Constructor */
  inline Const_Expr();
  /* Destructor */
  inline ~Const_Expr();
  /* Accessors */
  inline void          setOperator( int & error, X2DC_Operator Operator );
  inline void          setOperand( int & error, X2DC_Operand Operand );
  inline void          resetOperand();
  const X2DC_Operand * getOperand() const {return m_pOperand;}
  inline void          setValue( int & error, int value );
  const int *          getValue() const {return m_pValue;}
private:
  X2DC_Operator * m_pOperator;
  X2DC_Operand  * m_pOperand;
  int *           m_pValue;
  int             m_operandCount;
};

inline Const_Expr::Const_Expr()
: m_pOperator(NULL)
, m_pOperand(NULL)
, m_pValue(NULL)
, m_operandCount(0)
{}

inline Const_Expr::~Const_Expr() {
  delete m_pOperator;
  m_pOperator = NULL;
  delete m_pOperand;
  m_pOperand = NULL;
  delete m_pValue;
  m_pValue = NULL;
}

inline void Const_Expr::setOperator( int & error, X2DC_Operator Operator ) {
  if (NULL != m_pOperator) {
    X2DC_ErrMsg( M__DUPLICATE_OPERATION, "const" );
    error = 1;
  }
  else {
    m_pOperator = new X2DC_Operator;
    *m_pOperator = Operator;
  }
}

inline void Const_Expr::setOperand( int & error, X2DC_Operand Operand ) {
  /* Operator must be set before second operand may be specified. */
  if ( (1 == m_operandCount) && (NULL == m_pOperator) ) {
    X2DC_ErrMsg(M__OPERATION_BEFORE_SECOND_OPERAND);
    error = 1;
    return;
  }
  /* Only two operands may be set. */
  if (2 == m_operandCount) {
    X2DC_ErrMsg( M__TOO_MANY_OPERANDS, "const" );
    error = 1;
    return;
  }
  /* Operand may not be set. */
  if (NULL != m_pOperand) {
    X2DC_ErrMsg(M__DUPLICATE_OPERAND);
    error = 1;
    return;
  }
  /* OK */
  m_pOperand  = new X2DC_Operand;
  *m_pOperand = Operand;
  m_operandCount++;
}

inline void Const_Expr::resetOperand() {
  if (NULL != m_pOperand) {
    delete m_pOperand;
    m_pOperand = NULL;
  }
}

inline void Const_Expr::setValue( int & error, int value ) {
  /* At least one operand must be set, at most two operands may be set. */
  if ( (1 != m_operandCount) && (2 != m_operandCount) ) {
    X2DC_ErrMsg( M__INVALID_OPERAND_COUNT, m_operandCount );
    error = 1;
    return;
  }
  /* If one operand is set, value may not be set yet. */
  if (1 == m_operandCount) {
    if (NULL != m_pValue) {
      X2DC_ErrMsg(M__VALUE_SET_OPERAND_CNT1);
      error = 1;
    }
    else {
      m_pValue = new int;
      *m_pValue = value;
    }
    return;
  }
  /* If two operands are set, value must be set already. */
  if (2 == m_operandCount) {
    if (NULL == m_pValue) {
      X2DC_ErrMsg(M__VALUE_NOTSET_OPERAND_CNT2);
      error = 1;
    }
    else {
      /* Operator is set, that's checked in setOperand. */
      switch(*m_pOperator) {
        case Plus : {
          *m_pValue = *m_pValue + value;
          break;
        }
        case Minus : {
          *m_pValue = *m_pValue - value;
          break;
        }
      }
    }
  }
}


/******************************************************************************/
/*     Cpp_Const                                                              */
/******************************************************************************/

class Cpp_Const
{
public:
  /* Constructors */
  inline Cpp_Const(const char * pName);
  inline Cpp_Const( const char * pName, const int value );
  /* Destructor */
  inline ~Cpp_Const();
  /* Accessors */
  inline const char * getName() const {return m_pNameList->getFirstItem()->getName();}
//  inline void         addSynonym( int & error, const char * pSynonym );
  inline bool         isSynonym(const char * pSynonym) const;
  inline void         setValue(const int value);
  const int *         getValue() const {return m_pValue;}
private:
  List<Cpp_Name>  * m_pNameList;
  int *             m_pValue;
};

inline Cpp_Const::Cpp_Const(const char * pName)
: m_pValue(NULL)
{
  Cpp_Name * pN = new Cpp_Name(pName);
  m_pNameList = new List<Cpp_Name>(pN);
}

inline Cpp_Const::Cpp_Const( const char * pName, const int value ) {
  Cpp_Name * pN = new Cpp_Name(pName);
  m_pNameList = new List<Cpp_Name>(pN);
  m_pValue = new int;
  *m_pValue = value;
}

inline Cpp_Const::~Cpp_Const() {
  m_pNameList->Clear();
  delete m_pNameList;
  m_pNameList = NULL;
  delete m_pValue;
  m_pValue = NULL;
}

//inline void Cpp_Const::addSynonym( int & error, const char * pSynonym ) {
//  /* Check uniqueness */
//  Iterator<Cpp_Name> iter = m_pNameList->Begin();
//  while (iter()) {
//    if (0 == strcmp( iter()->getName(), pSynonym )) {
//      X2DC_ErrMsg("CTRH_ERROR_54");
//      error = 1;
//      return;
//    }
//    else {
//      iter++;
//    }
//  }
//  /* Add */
//  Cpp_Name * pN = new Cpp_Name(pSynonym);
//  m_pNameList->Append(pN);
//}

inline bool Cpp_Const::isSynonym(const char * pSynonym) const {
  Iterator<Cpp_Name> iter = m_pNameList->Begin();
  if (iter()) {
    iter++;     // first element holds the name not a synonym
  }
  while (iter()) {
    if (0 == strcmp( iter()->getName(), pSynonym )) {
      return true;
    }
    else {
      iter++;
    }
  }
  return false;
}

inline void Cpp_Const::setValue(const int value) {
  if (m_pValue) {
    *m_pValue = value;
  }
  else {
    m_pValue = new int;
    *m_pValue = value;
  }
}


/******************************************************************************/
/*     Cpp_Type_Registry                                                      */
/******************************************************************************/

class Cpp_Type_Registry
{
public:
  /* Constructor */
  Cpp_Type_Registry(int * pError);
  /* Destructor */
  ~Cpp_Type_Registry();
  /* Others */
  inline void                   registerFullType( int & error, const Cpp_Type * pType );
  inline void                   setHidden( int & error, const Cpp_Type * pType );
  inline void                   registerOnlyName( int & error, const char * pName );
  inline const Cpp_Type *       findType(const char * pName) const;
  inline void                   registerPersInterface( int & error, const Pers_Interface * pPersIF );
  inline const Pers_Interface * findPersInterface(const char * pName) const;
  inline void                   registerPersClass( int & error, const Pers_Class * pPersClass );
  inline const Pers_Class *     findPersClass(const char * pName) const;
  inline void                   setInstantiated(const Pers_Class * pPersClass);
  inline void                   registerConst( int & error, const Cpp_Const * pConst );
  inline const int *            getConstValue(const char * pName) const;
  inline void                   addSynonym( int & error, const Cpp_Type * pType, const char * pSynonym );
  inline void                   addTemplParamInstance( int & error, List<Templ_ParamInstance> * pList,
                                                       Templ_ParamInstance * pTempin );
  inline void                   setTemplClassparamUsage( int & error,
                                                         const Templ_Param * pTempar, ClassparamUsage usage);
  inline const Templ_Param *    findTemplParam( Iterator<Templ_Param> * pIter,
                                                const char * pIdentifier );
  inline const Templ_Param *    findTemplParam( Iterator<Templ_Param> * pIter, int number );
  inline const
         Templ_ParamInstance *  findTemplParamInstance( Iterator<Templ_ParamInstance> * pIter,
                                                        int number );
  inline const char *           genNextTypeName();
  inline void                   instantiateTemplate( int & error, const Cpp_Type * pTempl,
                                                     Iterator<Templ_ParamInstance> * pIter,
                                                     Cpp_Type * pType, bool isPersClass = false );
  inline int                    getExprValue( int & error,
                                              const Cpp_Type * pType, const PTS_Expression * pExpr,
                                              Iterator<Templ_ParamInstance> * pTempinIter ) const;
  int                           Init();
  void                          StartElement( int & error, const char * name, const char ** atts );
  void                          EndElement( int & error, const char * name );
  void                          CharData( int & error, const char * s, int len );
private:
  int *                       m_pError;
  List<Cpp_Type> *            m_pTypeList;
  List<Pers_Interface> *      m_pPersInterfaceList;
  List<Pers_Class> *          m_pPersClassList;
  List<Cpp_Const> *           m_pConstList;
  List<Templ_ParamInstance> * m_pTempinList;
  bool                        m_isWithin_LCTypes;
  bool                        m_isWithin_Enum;
  bool                        m_isWithin_GenEnum;
  bool                        m_isWithin_Struct;
  bool                        m_isWithin_Union;
  bool                        m_isWithin_Class;
  bool                        m_isWithin_TemplClass;
  bool                        m_isWithin_Instantiation;
  bool                        m_isWithin_TemplParams;
  bool                        m_isWithin_PersClass;
  bool                        m_isWithin_PersKeyedClass;
  bool                        m_isWithin_PersInterface;
  bool                        m_isWithin_PersTemplClass;
  bool                        m_isWithin_PersTemplClassSize;
  bool                        m_isWithin_PersInstantiation;
  PTS_Expression *            m_pCurrPtsExpr;
  PTS_Operand *               m_pPtsOper;
  bool                        m_isWithin_Index;
  bool                        m_isWithin_TemplMember;
  bool                        m_isWithin_OidMember;
  bool                        m_is_FullType;
  Cpp_Type *                  m_pType;
  const Cpp_Type *            m_pTempl;
  Member *                    m_pMember;
  Pers_Class *                m_pPersClass;
  int                         m_pcSize;
  const Pers_Class *          m_pPersTempl;
  char *                      m_pID;
  Index *                     m_pIndex;
  Cpp_Const *                 m_pConst;
  Const_Expr *                m_pConstExpr;
  Bit_Fields *                m_pBitFields;
  int                         m_genTypeName_Count;
  int                         m_TemplParam_Count;
  char *                      m_pTabHandle;
};

inline void Cpp_Type_Registry::registerFullType( int & error, const Cpp_Type * pType ) {
  if (!pType) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "Cpp_Type_Registry::registerFullType", "pType == NULL" );
    error = 1;
    return;
  }
  else if (findType(pType->getName())) {
    X2DC_ErrMsg( M__DUPLICATE_TYPE, pType->getName() );
    error = 1;
    return;
  }
  else {
    m_pTypeList->Append(pType);
  }
}

inline void Cpp_Type_Registry::setHidden( int & error, const Cpp_Type * pType ) {
  if (!pType) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "Cpp_Type_Registry::setHidden", "pType == NULL" );
    error = 1;
    return;
  }
  else {
    Cpp_Type * pT = const_cast<Cpp_Type *>(pType);
    pT->setHidden();
  }
}

inline void Cpp_Type_Registry::registerOnlyName( int & error, const char * pName ) {
  if (!pName) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "Cpp_Type_Registry::registerOnlyName", "pName == NULL" );
    error = 1;
    return;
  }
  else if (findType(pName)) {
    X2DC_ErrMsg( M__DUPLICATE_TYPE, pName );
    error = 1;
    return;
  }
  else {
    const Cpp_Type * pT = new Cpp_Type( pName, false );
    m_pTypeList->Append(pT);
  }
}

inline const Cpp_Type * Cpp_Type_Registry::findType(const char * pName) const {
  if ( (!pName) || (m_pTypeList->IsEmpty()) ) {
    return NULL;
  }
  else {
    Iterator<Cpp_Type> iter = m_pTypeList->Begin();
    while (iter()) {
      if ( ( (0 == strcmp( iter()->getName(), pName )) || (iter()->isSynonym(pName)) ) &&
           (!iter()->isHidden()) ) {
        return iter();
      }
      else {
        iter++;
      }
    }
    return NULL;
  }
}

inline void Cpp_Type_Registry::registerPersInterface( int & error, const Pers_Interface * pPersIF ) {
  if (!pPersIF) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "Cpp_Type_Registry::registerPersInterface", "pPersIF == NULL" );
    error = 1;
    return;
  }
  else if (findPersInterface(pPersIF->getName())) {
    X2DC_ErrMsg( M__DUPLICATE_TYPE, pPersIF->getName() );
    error = 1;
    return;
  }
  else {
    m_pPersInterfaceList->Append(pPersIF);
  }
}

inline const Pers_Interface * Cpp_Type_Registry::findPersInterface(const char * pName) const {
  if ( (!pName) || (m_pPersInterfaceList->IsEmpty()) ) {
    return NULL;
  }
  else {
    Iterator<Pers_Interface> iter = m_pPersInterfaceList->Begin();
    while (iter()) {
      if (0 == strcmp( iter()->getName(), pName )) {
        return iter();
      }
      else {
        iter++;
      }
    }
    return NULL;
  }
}

inline void Cpp_Type_Registry::registerPersClass( int & error, const Pers_Class * pPersClass ) {
  if (!pPersClass) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "Cpp_Type_Registry::registerPersClass", "pPersClass == NULL" );
    error = 1;
    return;
  }
  else if (findPersClass(pPersClass->getName())) {
    X2DC_ErrMsg( M__DUPLICATE_TYPE, pPersClass->getName() );
    error = 1;
    return;
  }
  else {
    m_pPersClassList->Append(pPersClass);
  }
}

inline const Pers_Class * Cpp_Type_Registry::findPersClass(const char * pName) const {
  if ( (!pName) || (m_pPersClassList->IsEmpty()) ) {
    return NULL;
  }
  else {
    Iterator<Pers_Class> iter = m_pPersClassList->Begin();
    while (iter()) {
      if (0 == strcmp( iter()->getName(), pName )) {
        return iter();
      }
      else {
        iter++;
      }
    }
    return NULL;
  }
}

inline void Cpp_Type_Registry::registerConst( int & error, const Cpp_Const * pConst ) {
  if (!pConst) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "Cpp_Type_Registry::registerConst", "pConst == NULL" );
    error = 1;
    return;
  }
  else if (getConstValue(pConst->getName())) {
    X2DC_ErrMsg( M__DUPLICATE_CONST, pConst->getName() );
    error = 1;
    return;
  }
  else {
    m_pConstList->Append(pConst);
  }
}

inline const int * Cpp_Type_Registry::getConstValue(const char * pName) const {
  if ( (!pName) || (m_pConstList->IsEmpty()) ) {
    return NULL;
  }
  else {
    Iterator<Cpp_Const> iter = m_pConstList->Begin();
    while (iter()) {
      if ( (0 == strcmp( iter()->getName(), pName )) || (iter()->isSynonym(pName)) ) {
        return iter()->getValue();
      }
      else {
        iter++;
      }
    }
    return NULL;
  }
}

inline void Cpp_Type_Registry::addSynonym( int & error, const Cpp_Type * pType, const char * pSynonym ) {
  if (!pType) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "Cpp_Type_Registry::addSynonym", "pType == NULL" );
    error = 1;
    return;
  }
  else if (!pSynonym) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "Cpp_Type_Registry::addSynonym", "pSynonym == NULL" );
    error = 1;
    return;
  }
  else {
    Cpp_Type * pT = const_cast<Cpp_Type *>(pType);
    pT->addSynonym( error, pSynonym );
  }
}

inline void Cpp_Type_Registry::addTemplParamInstance( int & error, List<Templ_ParamInstance> * pList,
                                                      Templ_ParamInstance * pTempin ) {
  if (!pList) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "Cpp_Type_Registry::addTemplParamInstance", "pList == NULL" );
    error = 1;
    return;
  }
  else if (!pTempin) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "Cpp_Type_Registry::addTemplParamInstance", "pTempin == NULL" );
    error = 1;
    return;
  }
  else {
    if (pList->IsEmpty()) {
      pTempin->setNumber(1);
    }
    else {
      pTempin->setNumber(pList->getLastItem()->getNumber()+1);
    }
    pList->Append(pTempin);
  }
}

inline void Cpp_Type_Registry::setTemplClassparamUsage( int & error,
                                                        const Templ_Param * pTempar, ClassparamUsage usage) {
  Templ_Param * pTP = const_cast<Templ_Param *>(pTempar);
  pTP->setClassparamUsage( error, usage );
}

inline const Templ_Param * Cpp_Type_Registry::findTemplParam( Iterator<Templ_Param> * pIter,
                                                              const char * pIdentifier ) {

  if ( (!pIter) || (!pIdentifier) ) {
    return NULL;
  }

  const Templ_Param * pTempar = NULL;

  Iterator<Templ_Param> iter = *pIter;
  while (iter()) {
    if (0 == strcmp( iter()->getIdentifier(), pIdentifier )) {
      pTempar = iter();
      break;
    }
    iter++;
  }

  iter.reset();
  return pTempar;

}

inline const Templ_Param * Cpp_Type_Registry::findTemplParam( Iterator<Templ_Param> * pIter,
                                                              int number ) {

  if ( (!pIter) || (number < 1) ) {
    return NULL;
  }

  const Templ_Param * pTempar = NULL;

  Iterator<Templ_Param> iter = *pIter;
  while (iter()) {
    if (iter()->getNumber() == number) {
      pTempar = iter();
      break;
    }
    iter++;
  }

  iter.reset();
  return pTempar;

}

inline const Templ_ParamInstance * Cpp_Type_Registry::findTemplParamInstance( 
                                                          Iterator<Templ_ParamInstance> * pIter,
                                                          int number ) {

  if ( (!pIter) || (number < 1) ) {
    return NULL;
  }

  const Templ_ParamInstance * pTempin = NULL;

  Iterator<Templ_ParamInstance> iter =  *pIter;
  while (iter()) {
    if (iter()->getNumber() == number) {
      pTempin = iter();
      break;
    }
    iter++;
  }

  iter.reset();
  return pTempin;

}

inline const char * Cpp_Type_Registry::genNextTypeName() {
  m_genTypeName_Count++;
  char * pName = new char[23+10+1];
  strcpy( pName, "X2DC__Gen__Type__Name__" );
  memset( &pName[23], 0, 11 );
  sprintf( &pName[23], "%d", m_genTypeName_Count );
  return pName;
}

inline void Cpp_Type_Registry::instantiateTemplate( int & error, const Cpp_Type * pTempl,
                                                    Iterator<Templ_ParamInstance> * pTempinIter,
                                                    Cpp_Type * pType, bool isPersClass ) {

  if (!pTempl) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "Cpp_Type_Registry::instantiateTemplate", "pTempl == NULL" );
    error = 1;
    return;
  }
  else if (!pTempl->isTemplate()) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S_S, "Cpp_Type_Registry::instantiateTemplate",
                 pTempl->getName(), "is not a template" );
    error = 1;
    return;
  }
  else if (!pTempinIter) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "Cpp_Type_Registry::instantiateTemplate", "pTempinIter == NULL" );
    error = 1;
    return;
  }
  else if (!pType) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "Cpp_Type_Registry::instantiateTemplate", "pType == NULL" );
    error = 1;
    return;
  }
  else if (pType->isTemplate()) {
    X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S_S, "Cpp_Type_Registry::instantiateTemplate",
                 pType->getName(), "is a template" );
    error = 1;
    return;
  }

  Iterator<Templ_ParamInstance> tempinIter = *pTempinIter;
  int tempinCount = 0;
  while (tempinIter()) {
    if ( (tempinIter()->getKind() != InstanceForClass) &&
         (tempinIter()->getKind() != InstanceForPersclass) &&
         (tempinIter()->getKind() != InstanceForInt) ) {
      X2DC_ErrMsg( M__BAD_TEMPIN_KIND, tempinIter()->getNumber(), pTempl->getName(), pType->getName() );
      error = 1;
      return;
    }
    tempinIter++;
    tempinCount++;
  }
  tempinIter.reset();

  if (pTempl->getTemplParamCount() != tempinCount) {
    X2DC_ErrMsg( M__TEMPIN_TEMPAR_COUNT_MISMATCH, pTempl->getName(), pType->getName() );
    error = 1;
    return;
  }

  Iterator<Templ_Param> temparIter = pTempl->getTemplParamList()->Begin();

  for ( int i = 0; i < tempinCount; i++ ) {
    if (!( ( (temparIter()->getKind() == ClassParam) &&
             (tempinIter()->getKind() == InstanceForClass) ) ||
           ( (temparIter()->getKind() == ClassParam) &&
             (tempinIter()->getKind() == InstanceForPersclass) ) ||
           ( (temparIter()->getKind() == IntParam) &&
             (tempinIter()->getKind() == InstanceForInt) ) )) {
      X2DC_ErrMsg( M__TEMPIN_TEMPAR_MISMATCH, i+1, pTempl->getName(), pType->getName() );
      error = 1;
      return;
    }
    temparIter++;
    tempinIter++;
  }
  temparIter.reset();
  tempinIter.reset();

  if (pTempl->isDerived()) {
    Iterator<Base_Class> bcIter = pTempl->getBaseClassList()->Begin();
    while(bcIter()) {
      Base_Class * pC = new Base_Class(bcIter()->getType());
      pType->addBaseClass( error, pC, isPersClass );
      pType->setSize(pC->getOffset() + pC->getType()->getSize());
      if (error) {
        return;
      }
      bcIter++;
    }
  }

  Iterator<Member> memIter = pTempl->getMemberList()->Begin();
  while (memIter()) {
    Member * pM = new Member(memIter()->getName());
    int dim = memIter()->getDimension();
    if (dim < 0) {
      const Templ_ParamInstance * pTempin = findTemplParamInstance( &tempinIter, -dim );
      if ( (!pTempin) || (!pTempin->getIntPtr()) ) {
        X2DC_ErrMsg( M__NO_VALUE_FOR_DIMPARAM, memIter()->getName(), pTempl->getName(), pType->getName() );
        error = 1;
        return;
      }
      dim = *pTempin->getIntPtr();
    }
    if (dim < 1) {
      X2DC_ErrMsg( M__BAD_VALUE_FOR_DIMPARAM, dim, memIter()->getName(), pTempl->getName(), pType->getName() );
      error = 1;
      return;
    }
    pM->setDimension( error, dim );
    if (error) {
      return;
    }
    switch (memIter()->getKind()) {
      case Initial : {
        X2DC_ErrMsg( M__MEMBER_KIND_UNDEF_TEMPL, memIter()->getName(), pTempl->getName(), pType->getName() );
        error = 1;
        return;
      }
      case CppType : {
        pM->setType( error, CppType, memIter()->getCpptypePtr() );
        if (error) {
          return;
        }
        break;
      }
      case TemparCppType : {
        const Templ_Param * pTempar = findTemplParam( &temparIter, memIter()->getTemplParamIdentifierPtr() );
        if (!pTempar) {
          X2DC_ErrMsg( M__TEMPAR_NOT_FOUND, memIter()->getTemplParamIdentifierPtr(), pTempl->getName(), pType->getName() );
          error = 1;
          return;
        }
        const Templ_ParamInstance * pTempin = findTemplParamInstance( pTempinIter, pTempar->getNumber() );
        if (!pTempin) {
          X2DC_ErrMsg( M__TEMPIN_NOT_FOUND, pTempar->getNumber(), pTempl->getName(), pType->getName() );
          error = 1;
          return;
        }
        const Cpp_Type * pT = pTempin->getCpptypePtr();
        if (!pT) {
          X2DC_ErrMsg( M__TEMPIN_CPPTYPE_NOT_FOUND, pTempar->getNumber(), pTempl->getName(), pType->getName() );
          error = 1;
          return;
        }
        pM->setType( error, CppType, pT );
        if (error) {
          return;
        }
        break;
      }
      case TempinCppType : {
        List<Templ_ParamInstance> * pTpiList = new List<Templ_ParamInstance>();
        Iterator<Templ_ParamInstance> tpiIter = memIter()->getTemplParamInstanceList()->Begin();
        while (tpiIter()) {
          switch (tpiIter()->getKind()) {
            case InstanceForClass : {
              Templ_ParamInstance * pTpi = new Templ_ParamInstance( error, tpiIter()->getCpptypePtr() );
              if (error) {
                return;
              }
              addTemplParamInstance( error, pTpiList, pTpi );
              if (error) {
                return;
              }
              break;
            }
            case ParamForClass : {
              const Templ_ParamInstance * pTempin =
                    findTemplParamInstance( &tempinIter, *tpiIter()->getParamnoPtr() );
              if (!pTempin) {
                X2DC_ErrMsg( M__MEMBER_TEMPIN_NOT_FOUND,
                             *tpiIter()->getParamnoPtr(), pTempl->getName(), memIter()->getName() );
                error = 1;
                return;
              }
              Templ_ParamInstance * pTpi = new Templ_ParamInstance( error, pTempin->getCpptypePtr() );
              if (error) {
                return;
              }
              addTemplParamInstance( error, pTpiList, pTpi );
              if (error) {
                return;
              }
              break;
            }
            case InstanceForInt : {
              Templ_ParamInstance * pTpi = new Templ_ParamInstance( error, InstanceForInt,
                                                                    *tpiIter()->getIntPtr() );
              if (error) {
                return;
              }
              addTemplParamInstance( error, pTpiList, pTpi );
              if (error) {
                return;
              }
              break;
            }
            case Undefined : {
              X2DC_ErrMsg( M__MEMBER_TEMPIN_BAD_KIND,
                           *tpiIter()->getParamnoPtr(), pTempl->getName(), memIter()->getName() );
              error = 1;
              return;
            }
          }
          tpiIter++;
        }
        const char * pN = genNextTypeName();
        Cpp_Type * pT = new Cpp_Type(pN);
        delete [] pN;
        pN = NULL;
        Iterator<Templ_ParamInstance> tpiListIter = pTpiList->Begin();
        instantiateTemplate( error, memIter()->getCpptypePtr(), &tpiListIter, pT );
        if (error) {
          return;
        }
        pT->setVtType(csp1_vt_userdefined);
        registerFullType( error, pT );
        if (error) {
          return;
        }
        pM->setType( error, CppType, pT );
        if (error) {
          return;
        }
        pTpiList->Clear();
        delete pTpiList;
        break;
      }
      case BitFields : {
        Bit_Fields * pBF = new Bit_Fields( memIter()->getBitfieldsPtr()->getType(),
                                           memIter()->getBitfieldsPtr()->getSize() );
        Iterator<Field> fieldIter = memIter()->getBitfieldsPtr()->getFieldList()->Begin();
        while(fieldIter()) {
          Field * pF = new Field( fieldIter()->getName(), fieldIter()->getBitCount() );
          pBF->addField( error, pF );
          if (error) {
            return;
          }
          fieldIter++;
        }
        pM->setType( error, BitFields, pBF );
        if (error) {
          return;
        }
        break;
      }
      case Oid : {
        pM->setType( error, Oid );
        if (error) {
          return;
        }
        Templ_ParamInstance * pTempin = NULL;
        const Pers_Class * pPC = memIter()->getOidPersclassPtr();
        if (pPC) {
          pTempin = new Templ_ParamInstance( error, pPC );
          if (error) {
            return;
          }
        }
        else {
          if (memIter()->getTemplParamInstanceCount() != 1) {
            X2DC_ErrMsg( M__BAD_OIDMEMBER_TEMPIN_COUNT, memIter()->getTemplParamInstanceCount(),
                         memIter()->getName(), pTempl->getName(), pType->getName() );
            error = 1;
            return;
          }
          else {
            const Templ_ParamInstance * pTpi = memIter()->getTemplParamInstanceList()->getFirstItem();
            if ( (!pTpi) || (pTpi->getKind() != ParamForClass) ) {
              X2DC_ErrMsg( M__INVALID_OIDMEMBER_TEMPIN_KIND,
                           memIter()->getName(), pTempl->getName(), pType->getName() );
              error = 1;
              return;
            }
            else {
              const Templ_ParamInstance * pTpi_2 = findTemplParamInstance( &tempinIter, *pTpi->getParamnoPtr() );
              if (!pTpi_2) {
                X2DC_ErrMsg( M__OIDMEMBER_TEMPIN_NOT_FOUND,
                             memIter()->getName(), pTempl->getName(), pType->getName() );
                error = 1;
                return;
              }
              else {
                pTempin = new Templ_ParamInstance( error, pTpi_2->getPersclassPtr() );
                if (error) {
                  return;
                }
              }
            }
          }
        }
        addTemplParamInstance( error, pM->getTemplParamInstanceList(), pTempin );
        if (error) {
          return;
        }
        break;
      }
      case ExtOid : {
        pM->setType( error, ExtOid );
        if (error) {
          return;
        }
        break;
      }
      case VarOid : {
        pM->setType( error, VarOid );
        if (error) {
          return;
        }
        break;
      }
      case Pointer : {
        pM->setType( error, Pointer );
        if (error) {
          return;
        }
        break;
      }
    }
    pType->addMember( error, pM );
    if (error) {
      return;
    }
    memIter++;
  }

}

inline int Cpp_Type_Registry::getExprValue( int & error,
                                            const Cpp_Type * pType, const PTS_Expression * pExpr,
                                            Iterator<Templ_ParamInstance> * pTempinIter ) const {

  if ( (!pType) || (!pTempinIter) || (!pExpr) ||
       (!pExpr->getOperand1()) || (!pExpr->getOperand1()->getPointer()) ||
       (!pExpr->getOperand2()) || (!pExpr->getOperand2()->getPointer()) ) {
    X2DC_ErrMsg( M__BAD_EXPRESSION_EVALUATION );
    return -1;
  }

  Iterator<Templ_ParamInstance> tempinIter = *pTempinIter;

  int val1 = 0, val2 = 0;
  switch (pExpr->getOperand1()->getType()) {
    case Value : {
      val1 = *reinterpret_cast<const int *>(pExpr->getOperand1()->getPointer());
      break;
    }
    case TemplParam : {
      bool valSet = false;
      while (tempinIter()) {
        if ( (tempinIter()->getNumber() ==
              -*reinterpret_cast<const int *>(pExpr->getOperand1()->getPointer())) &&
             (tempinIter()->getKind() == InstanceForInt) && tempinIter()->getIntPtr() ) {
          val1 = *tempinIter()->getIntPtr();
          valSet = true;
          break;
        }
        tempinIter++;
      }
      if (!valSet) {
        X2DC_ErrMsg( M__BAD_EXPRESSION_OPERAND, 1 );
        error = 1;
        return -1;
      }
      tempinIter.reset();
      break;
    }
    case SizeOf : {
      val1 = pType->getMemberSize( error,
                                   reinterpret_cast<const Member *>(pExpr->getOperand1()->getPointer()) );
      if (error) {
        return -1;
      }
      break;
    }
    case Expression : {
      val1 = getExprValue( error, pType,
                           reinterpret_cast<const PTS_Expression *>(pExpr->getOperand1()->getPointer()),
                           pTempinIter );
      if (error) {
        return -1;
      }
      break;
    }
  }
  switch (pExpr->getOperand2()->getType()) {
    case Value : {
      val2 = *reinterpret_cast<const int *>(pExpr->getOperand2()->getPointer());
      break;
    }
    case TemplParam : {
      bool valSet = false;
      while (tempinIter()) {
        if ( (tempinIter()->getNumber() ==
              -*reinterpret_cast<const int *>(pExpr->getOperand2()->getPointer())) &&
             (tempinIter()->getKind() == InstanceForInt) && tempinIter()->getIntPtr() ) {
          val2 = *tempinIter()->getIntPtr();
          valSet = true;
          break;
        }
        tempinIter++;
      }
      if (!valSet) {
        X2DC_ErrMsg( M__BAD_EXPRESSION_OPERAND, 2 );
        error = 1;
        return -1;
      }
      tempinIter.reset();
      break;
    }
    case SizeOf : {
      val2 = pType->getMemberSize( error,
                                   reinterpret_cast<const Member *>(pExpr->getOperand2()->getPointer()) );
      if (error) {
        return -1;
      }
      break;
    }
    case Expression : {
      val2 = getExprValue( error, pType,
                           reinterpret_cast<const PTS_Expression *>(pExpr->getOperand2()->getPointer()),
                           pTempinIter );
      if (error) {
        return -1;
      }
      break;
    }
  }

  int val;
  switch (pExpr->getOperation()) {
    case Add : {
      val = val1 + val2;
      break;
    }
    case Sub : {
      val = val1 - val2;
      break;
    }
    case Mul : {
      val = val1 * val2;
      break;
    }
    case Div : {
      val = val1 / val2;
      break;
    }
  }
  return val;

}


/******************************************************************************/
/*     Currently processed type                                               */
/******************************************************************************/

static Cpp_Type ** CTR_pCurrType = NULL;

static inline const char * CurrTypeName() {
  if ( CTR_pCurrType && (*CTR_pCurrType) ) {
    return (*CTR_pCurrType)->getName();
  }
  else {
    return NULL;
  }
}

static inline bool CurrTypeIsTemplate() {
  if ( CTR_pCurrType && (*CTR_pCurrType) && ((*CTR_pCurrType)->isTemplate()) ) {
    return true;
  }
  else {
    return false;
  }
}


#endif /* X2DC_CPPTYPEREGISTRY_HPP */

