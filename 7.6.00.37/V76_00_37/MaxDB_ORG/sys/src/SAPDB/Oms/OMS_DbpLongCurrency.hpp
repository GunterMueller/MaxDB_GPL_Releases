/*!
 * \file    OMS_DbpLongCurrency.hpp
 * \author  PeterG (GoertzP)
 * \brief   OMS: Long Currency
 */

/*

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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


#ifndef OMS_DBPLONGCURRENCY_HPP
#define OMS_DBPLONGCURRENCY_HPP


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Oms/OMS_LongPacked.hpp"


/************************************************************************/
/* Class DbpLongCurrency                                                */
/************************************************************************/

class DbpLongCurrency {
public :
  DbpLongCurrency(){}
  inline DbpLongCurrency(const OmsTypePacked_15_3&);
  inline void dbpToPacked_15_3(OmsTypePacked_15_3&);
  inline operator OmsTypePacked_15_3();
  inline OmsTypeInt8  dbpGetHiVal() const;
  inline OmsTypeUInt8 dbpGetLoVal() const;
  inline void dbpPutHiVal(OmsTypeInt8);
  inline void dbpPutLoVal(OmsTypeUInt8);
  inline const DbpLongCurrency & operator = (const OmsTypePacked_15_3&);

private :
  /* members */
  OmsTypeInt8  m_hiVal;
  OmsTypeUInt8 m_loVal;

  /* constructors */
#ifdef AIX
  inline DbpLongCurrency( OmsTypeInt8, OmsTypeUInt8 );
#else
  explicit inline DbpLongCurrency( OmsTypeInt8, OmsTypeUInt8 );
#endif
  inline DbpLongCurrency(unsigned int);

  /* Bit manipulation */
  inline DbpLongCurrency operator<< (unsigned int) const;
  inline DbpLongCurrency operator>> (unsigned int) const;
  inline DbpLongCurrency operator| (const DbpLongCurrency &) const;

  /* math. operations and operators */
  inline bool isNeg() const;
  inline bool unsignedLess(const DbpLongCurrency &) const;
  inline void twoComp();
  inline bool operator == (const DbpLongCurrency &) const;
  inline bool operator >  (const DbpLongCurrency &) const;
  inline void add( const DbpLongCurrency & addend,
                         DbpLongCurrency & res) const;
  inline void sub( const DbpLongCurrency & addend,
                         DbpLongCurrency & res) const;
  inline DbpLongCurrency operator - (const DbpLongCurrency &) const;
  inline bool div( const DbpLongCurrency & divisor,
                         DbpLongCurrency & quotient,
                         DbpLongCurrency & remainder) const;
};



/************************************************************************/
/* Implementation of class OmsDbpLongCurrency                           */
/************************************************************************/

inline DbpLongCurrency::DbpLongCurrency(const OmsTypePacked_15_3 & p) {
  OmsLongPacked * pp = reinterpret_cast<OmsLongPacked*>
                         (const_cast<OmsTypePacked_15_3*>(&p));
  pp->toInt16( m_hiVal, m_loVal );
}

/*----------------------------------------------------------------------*/

inline DbpLongCurrency::DbpLongCurrency( OmsTypeInt8  hi, 
                                         OmsTypeUInt8 lo ) {
  m_hiVal = hi;
  m_loVal = lo;
}

/*----------------------------------------------------------------------*/

inline DbpLongCurrency::DbpLongCurrency(unsigned int i) {
  m_hiVal = 0;
  m_loVal = i;
}

/*----------------------------------------------------------------------*/

inline void DbpLongCurrency::dbpToPacked_15_3 (OmsTypePacked_15_3 & p) {
# if defined (WIN32)
    DbpLongCurrency divisor = DbpLongCurrency( 0, 1000000000000000000I64 );
# else
    DbpLongCurrency divisor = DbpLongCurrency( 0, 1000000000000000000LL );
# endif
  DbpLongCurrency quotient, remainder, t = *this;
  bool isNeg = t.isNeg();
  if (isNeg) {
    t.twoComp();
  }
  t.div( divisor, quotient, remainder );
  OmsLongPacked * pp = reinterpret_cast<OmsLongPacked*>(&p);
  pp->toLongPacked( quotient.m_loVal, remainder.m_loVal, isNeg);
}

/* ----------------------------------------------------------------------*/

inline DbpLongCurrency::operator OmsTypePacked_15_3() {
  OmsTypePacked_15_3 p;
  dbpToPacked_15_3(p);
  return p;
}

/*----------------------------------------------------------------------*/

inline OmsTypeInt8 DbpLongCurrency::dbpGetHiVal() const {
  return m_hiVal;
}

/*----------------------------------------------------------------------*/

inline OmsTypeUInt8 DbpLongCurrency::dbpGetLoVal() const {
  return m_loVal;
}

/*----------------------------------------------------------------------*/

inline void DbpLongCurrency::dbpPutHiVal(OmsTypeInt8 hi) {
  m_hiVal = hi;
}

/*----------------------------------------------------------------------*/

inline void DbpLongCurrency::dbpPutLoVal(OmsTypeUInt8 lo) {
  m_loVal = lo;
}

/*----------------------------------------------------------------------*/

inline const DbpLongCurrency & DbpLongCurrency::operator 
                                 = (const OmsTypePacked_15_3 & p ) {
  OmsLongPacked * pp = reinterpret_cast<OmsLongPacked*>
                         (const_cast<OmsTypePacked_15_3*>(&p));
  pp->toInt16( m_hiVal, m_loVal );
  return *this;
}

/*----------------------------------------------------------------------*/

inline DbpLongCurrency DbpLongCurrency::operator << (unsigned int shift) const {
  if (shift >= 64) {
    return DbpLongCurrency( m_loVal << (shift-64), 0);
  } 
  else {
    return DbpLongCurrency( (m_hiVal << shift) | (m_loVal >> (64-shift)),
                             m_loVal << shift );
  }
}

/*----------------------------------------------------------------------*/

inline DbpLongCurrency DbpLongCurrency::operator >> (unsigned int shift) const {
  if (shift >= 64) {
    return DbpLongCurrency( m_hiVal >> 63, m_hiVal >> (shift-64) );
  }
  else {
    return DbpLongCurrency( m_hiVal >> shift,
                           (m_loVal >> shift) | (m_hiVal << (64-shift)) );
  }
}

/*----------------------------------------------------------------------*/

inline DbpLongCurrency DbpLongCurrency::operator | (const DbpLongCurrency & cur) const {
  return DbpLongCurrency( m_hiVal | cur.m_hiVal, m_loVal | cur.m_loVal );
}

/*----------------------------------------------------------------------*/

inline bool DbpLongCurrency::isNeg() const {
  return m_hiVal < 0;
}

/*----------------------------------------------------------------------*/

inline bool DbpLongCurrency::unsignedLess(const DbpLongCurrency & cur) const {
  OmsTypeUInt8 hi = m_hiVal, curhi = cur.m_hiVal;
  if (hi < curhi) {
    return true;
  }
  if (hi > curhi) {
    return false;
  }
  return m_loVal < cur.m_loVal;
}

/*----------------------------------------------------------------------*/

inline void DbpLongCurrency::twoComp() {
  /* one complement */
  m_hiVal = ~m_hiVal;
  m_loVal = ~m_loVal;
  /* add 1 */
  if (0 == ++m_loVal) {
    ++m_hiVal;
  }
}

/*----------------------------------------------------------------------*/

inline bool DbpLongCurrency::operator == (const DbpLongCurrency & cur) const{
  return ( m_hiVal == cur.m_hiVal && m_loVal == cur.m_loVal );
}

/*----------------------------------------------------------------------*/

inline bool DbpLongCurrency::operator > (const DbpLongCurrency & cur) const{
  if (m_hiVal > cur.m_hiVal) {
    return true;
  }
  if (m_hiVal < cur.m_hiVal) {
    return false;
  }
  return m_loVal > cur.m_loVal;
}

/*----------------------------------------------------------------------*/

inline void DbpLongCurrency::add( const DbpLongCurrency & addend,
                                        DbpLongCurrency & res) const {
  OmsTypeUInt8 old_loVal = m_loVal;
  res = DbpLongCurrency( m_hiVal + addend.m_hiVal,
                         m_loVal + addend.m_loVal);
  if (res.m_loVal < old_loVal) {
    ++res.m_hiVal; /* carry */
  }
}

/*----------------------------------------------------------------------*/

inline void DbpLongCurrency::sub( const DbpLongCurrency & addend,
                                        DbpLongCurrency & res) const {
  DbpLongCurrency tmp = addend;
  tmp.twoComp();
  add( tmp, res );
}

/*----------------------------------------------------------------------*/

inline DbpLongCurrency DbpLongCurrency::operator - (const DbpLongCurrency & addend) const {
  DbpLongCurrency tmp;
  sub( addend, tmp );
  return tmp;
}

/*----------------------------------------------------------------------*/

inline bool DbpLongCurrency::div( const DbpLongCurrency & divisor,
                                        DbpLongCurrency & quotient,
                                        DbpLongCurrency & remainder ) const {
  unsigned int num_bits, i;
  DbpLongCurrency DIVISOR = divisor, DIVIDEND = *this, d;

  remainder = 0;
  quotient = 0;

  /* DIVISOR and DIVIDEND may not be negative */
  if (DIVISOR.isNeg()) {
    DIVISOR.twoComp();
  }
  if (DIVIDEND.isNeg()) {
    DIVIDEND.twoComp();
  }

  /* checks */
  if (DIVISOR == 0) {
    return false;
  }
  if (DIVISOR > DIVIDEND) {
    remainder = DIVIDEND;
    quotient = 0;
    return true;
  }
  if (DIVISOR == DIVIDEND) {
    quotient = 1;
    if ( divisor.isNeg() && !isNeg() ||
        !divisor.isNeg() &&  isNeg() ) {
      quotient.twoComp();
    }
    return true;
  }

  /* division */

  num_bits = 128;

  while (remainder.unsignedLess(DIVISOR)) {
    unsigned int bit = (DIVIDEND.m_hiVal >> 63) != 0;
    remainder = (remainder << 1) | bit;
    d = DIVIDEND;
    DIVIDEND = DIVIDEND << 1;
    num_bits--;
  }

  DIVIDEND = d;
  remainder = remainder >> 1;
  num_bits++;

  for ( i = 0; i < num_bits; i++ ) {
    unsigned int bit = (DIVIDEND.m_hiVal >> 63) != 0;
    remainder = (remainder << 1) | bit;
    DbpLongCurrency t = remainder - divisor;
    unsigned int q = (t.m_hiVal >> 63) == 0;
    DIVIDEND = DIVIDEND << 1;
    quotient = (quotient << 1) | q;
    if (q) {
      remainder = t;
    }
  }

  /* adjust the sign */
  if (isNeg()) {
    remainder.twoComp();
    if (!divisor.isNeg()) {
      quotient.twoComp();
    }
  }
  else {
    if (divisor.isNeg()) {
      quotient.twoComp();
    }
  }

  return true;

}



#endif  // __OMS_DBPCURRENCY_HPP

