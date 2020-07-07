/*!
 * \file    OMS_Oid.hpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   OID of the persistent class
 */

/*

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



*/

#ifndef __OMS_OID_HPP
#define __OMS_OID_HPP

#include "livecachetypes.h" // nocheck
#include "Oms/OMS_AFX.h"

#define OMS_NIL_OBJ_PAGE_NO  2147483647
#define OMS_NIL_OBJ_PAGE_POS 0
#define OMS_NIL_OBJ_VERSION  0
#define OMS_VERSION_OBJ_PAGE_NO OMS_NIL_OBJ_PAGE_NO


class OMS_ObjectId8;


/*-----------------------------------------------------------------------------*/
/// Defines an OID 
//  Dependent on the setting of the define "OID_8BYTE_ALIGNED" this oid might be 4 or
//  8 byte aligned. If such an oid is entering the liboms via an interface, then then
//  oid must be aligned to an 8 byte oid. This is achieved by creating an instance
//  of OMS_ObjectId8 (see below).
struct AFX_EXT_CLASS OmsObjectId
{
public:
  /*-----------------------------------------------------------------------------*/
  /// Defines the "smaller than" operator for OIDs.
  /*!
  ** Defines the "smaller than" operator for OIDs. The statement that an OID is smaller 
  ** than another must not be understood as having semantic meaning. This operator has 
  ** been provided merely to enable sorting to be carried out according to OIDs.
  */
  inline bool operator< ( const OmsObjectId&r ) const;

  /*-----------------------------------------------------------------------------*/
  /// Defines the "smaller than or equal" operator for OIDs
  /*!
  ** Defines the "smaller than or equal" operator for OIDs. The statement that an OID  
  ** is smaller or equal than another must not be understood as having semantic meaning. 
  ** This operator has been provided merely to enable sorting to be carried out according 
  ** to OIDs.
  */
  inline bool operator<=( const OmsObjectId&r ) const;

  /*-----------------------------------------------------------------------------*/
  /// Defines the "bigger than" operator for OIDs.
  /*!
  ** Defines the "bigger than" operator for OIDs. The statement that an OID is bigger 
  ** than another must not be understood as having semantic meaning. This operator has 
  ** been provided merely to enable sorting to be carried out according to OIDs.
  */
  inline bool operator> ( const OmsObjectId&r ) const;

  /*-----------------------------------------------------------------------------*/
  /// Defines the "bigger than or equal" operator for OIDs.
  /*!
  ** Defines the "bigger than or equal" operator for OIDs. The statement that an OID is bigger 
  ** than another must not be understood as having semantic meaning. This operator has 
  ** been provided merely to enable sorting to be carried out according to OIDs.
  */
  inline bool operator>= ( const OmsObjectId&r ) const;

  /*-----------------------------------------------------------------------------*/
  /// Defines the equality of two OIDs.
  inline bool operator==( const OmsObjectId&r ) const;

  /*-----------------------------------------------------------------------------*/
  /// Defines the inequality of two OIDs.
  inline bool operator!=( const OmsObjectId&r ) const;

  /*-----------------------------------------------------------------------------*/
  /// Provides true for the NIL-OID; otherwise, false;
  inline bool operator! () const { return !isTrue(); }

  /*-----------------------------------------------------------------------------*/
  /// Provides true for all non-NIL OIDs; and for the NIL-OID, false;
  inline operator bool  () const { return isTrue(); }

  /*-----------------------------------------------------------------------------*/
  /// Provides true for all non-NIL OIDs; and for the NIL-OID, false;
  inline OmsObjectId& operator= (const OmsObjectId &r);

  inline int Compare(const OmsObjectId &oid1, const OmsObjectId &oid2);

  inline const unsigned int&     getPno()        const;
  inline const unsigned short&   getPagePos()    const;
  inline const unsigned short&   getGeneration() const;

  /*! \attention
  **  These methods are only provided for compatibility of LC-apps 5.0
  **  They should not be used in other releases.
  */
  inline unsigned int&     getPnoForUpd();
  inline unsigned short&   getPagePosForUpd();
  inline unsigned short&   getGenerationForUpd();

  inline unsigned long  omsHashValue() const;                  /* PTS 1125318 */
  inline unsigned long  omsHashValue(unsigned long upb) const; /* PTS 1103189 */

  inline OmsObjectId();

  inline void setNil();
  inline bool isTrue() const;


#ifdef  OMS_IMPLEMENTATION
  inline OmsObjectId(const OMS_ObjectId8 &oid8);
  inline OmsObjectId(OmsTypeInt8 i8);
#endif

protected:
  inline OmsObjectId(unsigned int pno, unsigned short pagePos, unsigned short generation);

private:
  OmsTypeOid  m_oid;
};


// ====================================================================
// ====================================================================


class OmsExternalOid : public OmsObjectId
{
public:
  OmsExternalOid() : OmsObjectId()  {}
  OmsExternalOid( const OmsObjectId&r ) : OmsObjectId(r) {}

//#ifdef OID_8BYTE_ALIGNED
//  OmsExternalOid( const OmsTypeOid&r  ) 
//    : OmsObjectId(r.m_oid.OidAsStruct.pno, r.m_oid.OidAsStruct.pagePos, r.m_oid.OidAsStruct.generation) {}
//#else
//  OmsExternalOid( const OmsTypeOid&r  ) 
//    : OmsObjectId(r.pno, r.pagePos, r.generation) {}
//#endif

  OmsExternalOid( unsigned int pno, unsigned short pagePos, unsigned short generation ) : OmsObjectId( pno, pagePos, generation ) {}

  OmsExternalOid( const OmsTypeByte* r) 
  { 
    OmsTypeByte* pDest = reinterpret_cast<OmsTypeByte*>(this);
    *pDest++ = *r++;
    *pDest++ = *r++;
    *pDest++ = *r++;
    *pDest++ = *r++;
    *pDest++ = *r++;
    *pDest++ = *r++;
    *pDest++ = *r++;
    *pDest   = *r;
  }
  void omsCopyTo( OmsTypeByte* pDest ) 
  {  
    OmsTypeByte* pSource = reinterpret_cast<OmsTypeByte*>(this);
    *pDest++ = *pSource++;
    *pDest++ = *pSource++;
    *pDest++ = *pSource++;
    *pDest++ = *pSource++;
    *pDest++ = *pSource++;
    *pDest++ = *pSource++;
    *pDest++ = *pSource++;
    *pDest   = *pSource;
  }
};


// ====================================================================
// ====================================================================



// Base-type for 8 byte aligned oid. Content of this oid can either be accessed
// as structure (pno, pagePos and generation) or as 8 byte integer
class OMS_TypeOid8
{
public:
  OMS_TypeOid8 (OmsTypeUInt4 p, unsigned short pp, unsigned short g)
  { 
    m_oid.OidAsStruct.pno        = p;
    m_oid.OidAsStruct.pagePos    = pp;
    m_oid.OidAsStruct.generation = g;
  }

  union  {
    OmsTypeUInt8      OidAs8ByteInt;
    struct {
      OmsTypeUInt4    pno;
      unsigned short  pagePos;
      unsigned short  generation;
    } OidAsStruct;
  } m_oid;
};

// ====================================================================

// define a nil-oid which can be used to efficiently check other oids against nil 
const OMS_TypeOid8 nil_TypeOid8 (OMS_NIL_OBJ_PAGE_NO, OMS_NIL_OBJ_PAGE_POS, OMS_NIL_OBJ_VERSION);


#ifdef  OMS_IMPLEMENTATION

// ====================================================================
// ====================================================================
/// Defines an OID which is 8-byte aligned
//  This oid is used inside the liboms and should never be seen outside.
class OMS_ObjectId8 : public OMS_TypeOid8
{
public:
  OMS_ObjectId8() 
    : OMS_TypeOid8(OMS_NIL_OBJ_PAGE_NO, OMS_NIL_OBJ_PAGE_POS, OMS_NIL_OBJ_VERSION)
  {}

  //--------------------------------------------------------------------

  OMS_ObjectId8(OmsTypeUInt4 p, unsigned short pp, unsigned short g)
    : OMS_TypeOid8(p, pp, g)
  {}

  //--------------------------------------------------------------------

  OMS_ObjectId8(const OmsObjectId &oid)
    : OMS_TypeOid8(oid.getPno(), oid.getPagePos(), oid.getGeneration())
  {}

  /*-----------------------------------------------------------------------------*/
  /// Defines the "smaller than" operator for OIDs.
  /*!
  ** Defines the "smaller than" operator for OIDs. The statement that an OID is smaller 
  ** than another must not be understood as having semantic meaning. This operator has 
  ** been provided merely to enable sorting to be carried out according to OIDs.
  */
  bool operator< ( const OMS_ObjectId8&r ) const
  { return m_oid.OidAs8ByteInt < r.m_oid.OidAs8ByteInt;  }

  /*-----------------------------------------------------------------------------*/
  /// Defines the "smaller than or equal" operator for OIDs
  /*!
  ** Defines the "smaller than or equal" operator for OIDs. The statement that an OID  
  ** is smaller or equal than another must not be understood as having semantic meaning. 
  ** This operator has been provided merely to enable sorting to be carried out according 
  ** to OIDs.
  */
  bool operator<=( const OMS_ObjectId8&r ) const
  { return m_oid.OidAs8ByteInt <= r.m_oid.OidAs8ByteInt;  }

  /*-----------------------------------------------------------------------------*/
  /// Defines the "bigger than" operator for OIDs.
  /*!
  ** Defines the "bigger than" operator for OIDs. The statement that an OID is bigger 
  ** than another must not be understood as having semantic meaning. This operator has 
  ** been provided merely to enable sorting to be carried out according to OIDs.
  */
  bool operator> ( const OMS_ObjectId8&r ) const
  { return m_oid.OidAs8ByteInt > r.m_oid.OidAs8ByteInt;  }

  /*-----------------------------------------------------------------------------*/
  /// Defines the "bigger than or equal" operator for OIDs.
  /*!
  ** Defines the "bigger than or equal" operator for OIDs. The statement that an OID is bigger 
  ** than another must not be understood as having semantic meaning. This operator has 
  ** been provided merely to enable sorting to be carried out according to OIDs.
  */
  bool operator>= ( const OMS_ObjectId8&r ) const
  { return m_oid.OidAs8ByteInt >= r.m_oid.OidAs8ByteInt;  }

  /*-----------------------------------------------------------------------------*/
  /// Defines the equality of two OIDs.
  bool operator==( const OMS_ObjectId8&r ) const
  { return m_oid.OidAs8ByteInt == r.m_oid.OidAs8ByteInt;  }

  /*-----------------------------------------------------------------------------*/
  /// Defines the inequality of two OIDs.
  bool operator!=( const OMS_ObjectId8&r ) const
  { return m_oid.OidAs8ByteInt != r.m_oid.OidAs8ByteInt;  }

  /*-----------------------------------------------------------------------------*/
  /// Provides true for the NIL-OID; otherwise, false;
   bool operator! () const
  { return !isTrue();  }

  /*-----------------------------------------------------------------------------*/
  /// Assignment
  OMS_ObjectId8& operator= (const OMS_ObjectId8 &r)
  { m_oid.OidAs8ByteInt = r.m_oid.OidAs8ByteInt;  return *this; }

  inline int Compare(const OMS_ObjectId8 &oid1, const OMS_ObjectId8 &oid2);

  unsigned int     getPno()        const { return m_oid.OidAsStruct.pno; }
  unsigned short   getPagePos()    const { return m_oid.OidAsStruct.pagePos; }
  unsigned short   getGeneration() const { return m_oid.OidAsStruct.generation; }

  inline unsigned long  omsHashValue() const;                  
  unsigned long         omsHashValue(unsigned long upb) const {   return (omsHashValue() % upb); }

  bool isTrue() const { return m_oid.OidAs8ByteInt != nil_TypeOid8.m_oid.OidAs8ByteInt; }
  void setNil()         { m_oid.OidAs8ByteInt = nil_TypeOid8.m_oid.OidAs8ByteInt; }

  void setPno       (OmsTypeUInt4   p)  {  m_oid.OidAsStruct.pno        = p; }
  void setPagePos   (unsigned short pp) {  m_oid.OidAsStruct.pagePos    = pp; }
  void setGeneration(unsigned short g)  {  m_oid.OidAsStruct.generation = g; }

  /// Increments the generation counter of the oid
  inline void operator++();                                      
  /// Returns true if the oid is equal except for the generation
  inline bool equalExceptGeneration(const OMS_ObjectId8 &oid) const; 
  /// Returns true if the oid is equal except for the generation and the generation differs exactly by one
  inline bool isDirectPredOf(const OMS_ObjectId8 &oid) const;        // PTS 1125361 
};
#endif    // OMS_IMPLEMENTATION




// ====================================================================
//     Implementation of OmsObjectId
// ====================================================================

#ifdef OID_8BYTE_ALIGNED
inline const unsigned int&     OmsObjectId::getPno()        const { return m_oid.OidAsStruct.pno;        }
inline const unsigned short&   OmsObjectId::getPagePos()    const { return m_oid.OidAsStruct.pagePos;    }
inline const unsigned short&   OmsObjectId::getGeneration() const { return m_oid.OidAsStruct.generation; }
inline unsigned int&    OmsObjectId::getPnoForUpd()        { return m_oid.OidAsStruct.pno;        }
inline unsigned short&  OmsObjectId::getPagePosForUpd()    { return m_oid.OidAsStruct.pagePos;    }
inline unsigned short&  OmsObjectId::getGenerationForUpd() { return m_oid.OidAsStruct.generation; }
#else
inline const unsigned int&     OmsObjectId::getPno()        const { return m_oid.pno;        }
inline const unsigned short&   OmsObjectId::getPagePos()    const { return m_oid.pagePos;    }
inline const unsigned short&   OmsObjectId::getGeneration() const { return m_oid.generation; }
inline unsigned int&    OmsObjectId::getPnoForUpd()        { return m_oid.pno;        }
inline unsigned short&  OmsObjectId::getPagePosForUpd()    { return m_oid.pagePos;    }
inline unsigned short&  OmsObjectId::getGenerationForUpd() { return m_oid.generation; }
#endif

//--------------------------------------------------------------------

inline OmsObjectId::OmsObjectId()
{
#ifdef OID_8BYTE_ALIGNED
  m_oid.OidAsStruct.pno        = OMS_NIL_OBJ_PAGE_NO;
  m_oid.OidAsStruct.pagePos    = OMS_NIL_OBJ_PAGE_POS;
  m_oid.OidAsStruct.generation = OMS_NIL_OBJ_VERSION;
#else
  m_oid.pno        = OMS_NIL_OBJ_PAGE_NO;
  m_oid.pagePos    = OMS_NIL_OBJ_PAGE_POS;
  m_oid.generation = OMS_NIL_OBJ_VERSION;
#endif
}

//--------------------------------------------------------------------

inline OmsObjectId::OmsObjectId(unsigned int _pno, unsigned short _pagePos, unsigned short _generation)
{
#ifdef OID_8BYTE_ALIGNED
  m_oid.OidAsStruct.pno        = _pno;
  m_oid.OidAsStruct.pagePos    = _pagePos;
  m_oid.OidAsStruct.generation = _generation;
#else
  m_oid.pno        = _pno;
  m_oid.pagePos    = _pagePos;
  m_oid.generation = _generation;
#endif
}

//--------------------------------------------------------------------

inline bool OmsObjectId::operator< ( const OmsObjectId&r ) const
{
#ifdef OID_8BYTE_ALIGNED
  return m_oid.OidAs8ByteInt < r.m_oid.OidAs8ByteInt;
#else
  if (m_oid.pno < r.m_oid.pno) {
    return true;
  }
  else if (m_oid.pno > r.m_oid.pno) {
    return false;
  }
  return *((&m_oid.pno) + 1) < *((&r.m_oid.pno) + 1);
#endif
}

//--------------------------------------------------------------------

inline bool OmsObjectId::operator> ( const OmsObjectId&r ) const
{
#ifdef OID_8BYTE_ALIGNED
  return m_oid.OidAs8ByteInt > r.m_oid.OidAs8ByteInt;
#else
  if (m_oid.pno > r.m_oid.pno) {
    return true;
  }
  else if (m_oid.pno < r.m_oid.pno) {
    return false;
  }
  return *((&m_oid.pno) + 1) > *((&r.m_oid.pno) + 1);
#endif
}

//--------------------------------------------------------------------

inline bool OmsObjectId::operator<=( const OmsObjectId&r ) const
{
#ifdef OID_8BYTE_ALIGNED
  return m_oid.OidAs8ByteInt <= r.m_oid.OidAs8ByteInt;
#else
  if (m_oid.pno < r.m_oid.pno) {
    return true;
  }
  else if (m_oid.pno > r.m_oid.pno) {
    return false;
  }
  return *((&m_oid.pno) + 1) <= *((&r.m_oid.pno) + 1);
#endif
}

//--------------------------------------------------------------------

inline bool OmsObjectId::operator>= ( const OmsObjectId&r ) const
{
#ifdef OID_8BYTE_ALIGNED
  return m_oid.OidAs8ByteInt >= r.m_oid.OidAs8ByteInt;
#else
  if (m_oid.pno > r.m_oid.pno) {
    return true;
  }
  else if (m_oid.pno < r.m_oid.pno) {
    return false;
  }
  return *((&m_oid.pno) + 1) >= *((&r.m_oid.pno) + 1);
#endif
}

//--------------------------------------------------------------------

inline bool OmsObjectId::operator==( const OmsObjectId&r ) const 
{ 
#ifdef OID_8BYTE_ALIGNED
  return m_oid.OidAs8ByteInt == r.m_oid.OidAs8ByteInt;
#else
  return ((m_oid.pno == r.m_oid.pno) && *((&m_oid.pno) + 1) == *((&r.m_oid.pno) + 1)); 
#endif
}

//--------------------------------------------------------------------

inline bool OmsObjectId::operator!=( const OmsObjectId&r ) const
{
#ifdef OID_8BYTE_ALIGNED
  return m_oid.OidAs8ByteInt != r.m_oid.OidAs8ByteInt;
#else
  return ((m_oid.pno != r.m_oid.pno) || *((&m_oid.pno) + 1) != *((&r.m_oid.pno) + 1)); 
#endif
}

//--------------------------------------------------------------------

inline OmsObjectId& OmsObjectId::operator= (const OmsObjectId &r) 
{ 
#ifdef OID_8BYTE_ALIGNED
  m_oid.OidAs8ByteInt = r.m_oid.OidAs8ByteInt;  
#else 
  m_oid.pno        = r.m_oid.pno;
  m_oid.pagePos    = r.m_oid.pagePos;
  m_oid.generation = r.m_oid.generation;
#endif
  
  return *this; 
}

//--------------------------------------------------------------------

inline void OmsObjectId::setNil()
{
#ifdef OID_8BYTE_ALIGNED
  m_oid.OidAs8ByteInt = nil_TypeOid8.m_oid.OidAs8ByteInt;
#else 
  m_oid.pno        = OMS_NIL_OBJ_PAGE_NO;
  m_oid.pagePos    = OMS_NIL_OBJ_PAGE_POS;
  m_oid.generation = OMS_NIL_OBJ_VERSION;
#endif
}

//--------------------------------------------------------------------

inline bool OmsObjectId:: isTrue() const 
{ 
#ifdef OID_8BYTE_ALIGNED
  return m_oid.OidAs8ByteInt != nil_TypeOid8.m_oid.OidAs8ByteInt; 
#else 
  return OMS_NIL_OBJ_PAGE_NO  != m_oid.pno 
      || OMS_NIL_OBJ_PAGE_POS != m_oid.pagePos
      || OMS_NIL_OBJ_VERSION  != m_oid.generation;
#endif
}
//--------------------------------------------------------------------

inline unsigned long OmsObjectId::omsHashValue() const
{
#ifdef OID_8BYTE_ALIGNED
  if (m_oid.OidAsStruct.pno == OMS_VERSION_OBJ_PAGE_NO){  
    // Version OID: pno is a constant therefore not relevant for hash value computation
    return (m_oid.OidAsStruct.pagePos ^ (((long)m_oid.OidAsStruct.generation) << 16));
  }
  else {  
    // normal OID: all oids with the same pno and pagePos should have the same
    // hash-value. This is needed for the oid-hash, to ensure, that these objects
    // are placed in the same bucket-chain.
    return (m_oid.OidAsStruct.pno ^ (((long)m_oid.OidAsStruct.pno) << 7) ^ (m_oid.OidAsStruct.pno >> 7) ^ (m_oid.OidAsStruct.pagePos >> 3) ^ (((long)m_oid.OidAsStruct.pagePos) << 9));
  }
#else
  if (m_oid.pno == OMS_VERSION_OBJ_PAGE_NO){  
    // Version OID: pno is a constant therefore not relevant for hash value computation
    return (m_oid.pagePos ^ (((long)m_oid.generation) << 16));
  }
  else {  
    // normal OID: all oids with the same pno and pagePos should have the same
    // hash-value. This is needed for the oid-hash, to ensure, that these objects
    // are placed in the same bucket-chain.
    return (m_oid.pno ^ (((long)m_oid.pno) << 7) ^ (m_oid.pno >> 7) ^ (m_oid.pagePos >> 3) ^ (((long)m_oid.pagePos) << 9));
  }
#endif
}

//--------------------------------------------------------------------

inline unsigned long OmsObjectId::omsHashValue(unsigned long upb) const 
{ 
  return (omsHashValue() % upb);
}

//--------------------------------------------------------------------

inline int OmsObjectId::Compare(const OmsObjectId &oid1, const OmsObjectId &oid2)
{
  if (oid1 < oid2){
    return -1;
  }
  else if (oid2 < oid1){
    return 1;
  }
  else {
    return 0;
  }
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#ifdef OMS_IMPLEMENTATION

#ifdef OID_8BYTE_ALIGNED
inline OmsObjectId::OmsObjectId(const OMS_ObjectId8 &oid8)
{
  m_oid.OidAs8ByteInt = oid8.m_oid.OidAs8ByteInt;
}
#else
inline OmsObjectId::OmsObjectId(const OMS_ObjectId8 &oid8)
{
  m_oid.pno        = oid8.m_oid.OidAsStruct.pno;
  m_oid.pagePos    = oid8.m_oid.OidAsStruct.pagePos;
  m_oid.generation = oid8.m_oid.OidAsStruct.generation;
}
#endif


#ifdef OID_8BYTE_ALIGNED
OmsObjectId::OmsObjectId (OmsTypeInt8 i8) 
{
  m_oid.OidAs8ByteInt = i8;
}
#endif

#endif



// ====================================================================
//     Implementation of OMS_ObjectId8
// ====================================================================

#ifdef OMS_IMPLEMENTATION

inline unsigned long OMS_ObjectId8::omsHashValue() const
{
  if (m_oid.OidAsStruct.pno == OMS_VERSION_OBJ_PAGE_NO){  
    // Version OID: pno is a constant therefore not relevant for hash value computation
    return (m_oid.OidAsStruct.pagePos ^ (((long)m_oid.OidAsStruct.generation) << 16));
  }
  else {  
    // normal OID: all oids with the same pno and pagePos should have the same
    // hash-value. This is needed for the oid-hash, to ensure, that these objects
    // are placed in the same bucket-chain.
    return (m_oid.OidAsStruct.pno ^ (((long)m_oid.OidAsStruct.pno) << 7) ^ (m_oid.OidAsStruct.pno >> 7) ^ (m_oid.OidAsStruct.pagePos >> 3) ^ (((long)m_oid.OidAsStruct.pagePos) << 9));
  }
}

//--------------------------------------------------------------------

inline int OMS_ObjectId8::Compare(const OMS_ObjectId8 &oid1, const OMS_ObjectId8 &oid2)
{
  if (oid1 < oid2){
    return -1;
  }
  else if (oid2 < oid1){
    return 1;
  }
  else {
    return 0;
  }
}

//--------------------------------------------------------------------

inline void OMS_ObjectId8::operator++()  
{
  // Only ONE byte is used for the generation in the kernel, although two bytes
  // are reserved in the oid, so the maximal allowed range is [0..255]
  // Attention: The value 0x00 is no valid generation.
  if (m_oid.OidAsStruct.generation == 0xff){
    m_oid.OidAsStruct.generation = 1;
  }
  else { 
    m_oid.OidAsStruct.generation++;
  }
}

//--------------------------------------------------------------------

inline bool OMS_ObjectId8::equalExceptGeneration(const OMS_ObjectId8 &oid) const  
{
  return (m_oid.OidAsStruct.pno == oid.m_oid.OidAsStruct.pno && m_oid.OidAsStruct.pagePos == oid.m_oid.OidAsStruct.pagePos);
}

//--------------------------------------------------------------------

inline bool OMS_ObjectId8::isDirectPredOf(const OMS_ObjectId8 &oid) const        
{
  // Only ONE byte is used for the generation in the kernel, although two bytes
  // are reserved in the oid, so the maximal allowed range is [0..255]
  // Attention: The value 0x00 is no valid generation.
  if (equalExceptGeneration(oid)){
    if (m_oid.OidAsStruct.generation == 0xff){
      return (oid.m_oid.OidAsStruct.generation == 1);
    }
    else{
      return (m_oid.OidAsStruct.generation+1 == oid.m_oid.OidAsStruct.generation);
    }
  }
  else {
    return false;
  }
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------

#ifdef OID_8BYTE_ALIGNED
#define ALIGN_OID(oid8, oid)        OMS_ObjectId8 oid8 = reinterpret_cast<OMS_ObjectId8&>(oid);
#define ALIGN_OID_CONST(oid8, oid)  const OMS_ObjectId8 oid8 = reinterpret_cast<const OMS_ObjectId8&>(oid);
#else
#define ALIGN_OID(oid8, oid)        OMS_ObjectId8 oid8(oid);
#define ALIGN_OID_CONST(oid8, oid)  const OMS_ObjectId8 oid8(oid);
#endif

#endif



// ====================================================================
// ====================================================================



// forward declaration
struct OmsVarOid;

/*---------------------------------------------------------------------------*/
/// Interface for the enumeration of OID's
/*!
** This interface defines methods for the enumeration of OID's which e.g. be
** stored in arrays, chained lists,...
**
** \since 7.4.3  PTS 1115027
*/
class AFX_EXT_CLASS OmsIOidReadIterator   
{
public:
  /*---------------------------------------------------------------------------*/
  /// Resets the enumeration, so that the next access will return the first oid again
  virtual void omsReset() = 0;

  /*---------------------------------------------------------------------------*/
  /// Returns the current oid
  virtual const OmsObjectId& omsGet() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Increments to the next oid
  virtual void operator++ () = 0;

  /*---------------------------------------------------------------------------*/
  /// Return true if omsGet() wourd return a valid oid; otherwise false.
  virtual operator bool () const = 0;
};


/*---------------------------------------------------------------------------*/
/// Interface to collect OID's
/*!
** This interface defines a method to insert OID's e.g. into arrays, 
** chained lists,...
**
** \since 7.4.3  PTS 1115027
*/
class AFX_EXT_CLASS OmsIOidAppender   
{
public:
  /*---------------------------------------------------------------------------*/
  /// Appends the oid and returns whether the action have been executed successfully
  virtual bool omsAppend(const OmsObjectId& currOid) = 0;
}; 

#endif  // __OMS_OID_HPP
