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
#ifndef __OMS_DBPCURRENCY_HPP
#define __OMS_DBPCURRENCY_HPP

#include "Oms/OMS_Packed.hpp"


#define MAX_CURRENCY_VAL 9223372036854775807I64
#define DBP_CURRENCY_1   1000

class DbpCurrency {
public :
  DbpCurrency(){}

  inline DbpCurrency(const OmsTypePacked_8_3&);
  inline void dbpToPacked_8_3 (OmsTypePacked_8_3&);
  inline operator OmsTypePacked_8_3();
  inline OmsTypeInt64 dbpGetVal() const;
  inline void dbpPutVal(OmsTypeInt64);

  inline const DbpCurrency& operator =( const OmsTypePacked_8_3& cur );

  // Currency Comparison
  inline bool operator==(const DbpCurrency& cur) const;
  inline bool operator!=(const DbpCurrency& cur) const;
  inline bool operator< (const DbpCurrency& cur) const;
  inline bool operator> (const DbpCurrency& cur) const;
  inline bool operator<=(const DbpCurrency& cur) const;
  inline bool operator>=(const DbpCurrency& cur) const;

  // Currency math
  inline DbpCurrency operator+(const DbpCurrency& cur) const;
  inline DbpCurrency operator+(long  nOperand);
  inline DbpCurrency operator-(const DbpCurrency& cur) const;
  inline DbpCurrency operator-(long  nOperand);
  inline const DbpCurrency& operator+=(const DbpCurrency& cur);
  inline const DbpCurrency& operator-=(const DbpCurrency& cur);
  inline DbpCurrency operator-() const;
  inline DbpCurrency operator*(long nOperand) const;
  inline DbpCurrency operator/(long nOperand) const;
  inline const DbpCurrency& operator*=(long nOperand);
  inline const DbpCurrency& operator/=(long nOperand);
private :
  OmsTypeInt64 val;
#ifdef AIX
  inline DbpCurrency(OmsTypeInt64);
#else
  explicit inline DbpCurrency(OmsTypeInt64);
#endif
};

// Implementation

inline DbpCurrency::DbpCurrency(OmsTypeInt64 i)
{
  val = i;
}

inline OmsTypeInt64 DbpCurrency::dbpGetVal() const
{
  return val;
}

inline void DbpCurrency::dbpPutVal(OmsTypeInt64 v)
{
  val = v;
}

inline DbpCurrency::DbpCurrency(const OmsTypePacked_8_3& p)
{
  OmsPacked *pp = reinterpret_cast<OmsPacked*>(const_cast<OmsTypePacked_8_3*>(&p));
  val = pp->toInt64_16();
}

inline void DbpCurrency::dbpToPacked_8_3 (OmsTypePacked_8_3& p)
{
  OmsPacked *pp = reinterpret_cast<OmsPacked*>(&p);
  pp->toPacked(val, 8);
}

inline DbpCurrency::operator OmsTypePacked_8_3()
{
  OmsTypePacked_8_3 p;
  dbpToPacked_8_3(p);
  return p;
};

inline const DbpCurrency& DbpCurrency::operator =( const OmsTypePacked_8_3& cur )
{
  OmsPacked *pp = reinterpret_cast<OmsPacked*>(const_cast<OmsTypePacked_8_3*>(&cur));
  val      = pp->toInt64_16();
  return *this;
}


inline bool DbpCurrency::operator==(const DbpCurrency& cur) const
{
  return (val == cur.val);
}

inline bool DbpCurrency::operator!=(const DbpCurrency& cur) const
{
  return (val != cur.val);
}

inline bool DbpCurrency::operator< (const DbpCurrency& cur) const
{
  return (val < cur.val);
}

inline bool DbpCurrency::operator> (const DbpCurrency& cur) const
{
  return (val > cur.val);
}

inline bool DbpCurrency::operator<=(const DbpCurrency& cur) const
{
  return (val <= cur.val);
}

inline bool DbpCurrency::operator>=(const DbpCurrency& cur) const
{
  return (val >= cur.val);
}

inline DbpCurrency DbpCurrency::operator+(const DbpCurrency& s) const
{
  return DbpCurrency (val + s.val);
}

inline DbpCurrency DbpCurrency::operator+(long nOperand)
{
  OmsTypeInt64 op = nOperand * DBP_CURRENCY_1;
  return DbpCurrency (val + op);
}

inline DbpCurrency DbpCurrency::operator-(const DbpCurrency& s) const
{
  return DbpCurrency (val - s.val);
}

inline DbpCurrency DbpCurrency::operator-(long nOperand)
{
  DbpCurrency op(nOperand * DBP_CURRENCY_1);
  return (*this - op);
}

inline const DbpCurrency& DbpCurrency::operator+=(const DbpCurrency& cur)
{
  val += cur.val;
  return *this;
}

inline const DbpCurrency& DbpCurrency::operator-=(const DbpCurrency& cur)
{
  val -= cur.val;
  return *this;
}

inline DbpCurrency DbpCurrency::operator-() const
{
  return DbpCurrency(-val);
}

inline DbpCurrency DbpCurrency::operator*(long nOperand) const
{
  return DbpCurrency(val * nOperand);
}

inline DbpCurrency DbpCurrency::operator/(long nOperand) const
{
  return DbpCurrency(val / nOperand);
}

inline const DbpCurrency& DbpCurrency::operator*=(long nOperand)
{
  val *= nOperand;
  return *this;
}

inline const DbpCurrency& DbpCurrency::operator/=(long nOperand)
{
  val /= nOperand;
  return *this;
}

inline bool operator== (const OmsTypePacked_8_3& l, const OmsTypePacked_8_3& r) 
{
  return ((l.m_packed[0] == r.m_packed[0]) &&
    (l.m_packed[1] == r.m_packed[1]) && 
    (l.m_packed[2] == r.m_packed[2]) && 
    (l.m_packed[3] == r.m_packed[3]) &&
    (l.m_packed[4] == r.m_packed[4]) &&
    (l.m_packed[5] == r.m_packed[5]) &&
    (l.m_packed[6] == r.m_packed[6]) &&
    (l.m_packed[7] == r.m_packed[7])); 
}

inline bool operator!= (const OmsTypePacked_8_3& l, const OmsTypePacked_8_3& r) 
{
  return ((l.m_packed[0] != r.m_packed[0]) ||
    (l.m_packed[1] != r.m_packed[1]) || 
    (l.m_packed[2] != r.m_packed[2]) || 
    (l.m_packed[3] != r.m_packed[3]) ||
    (l.m_packed[4] != r.m_packed[4]) ||
    (l.m_packed[5] != r.m_packed[5]) ||
    (l.m_packed[6] != r.m_packed[6]) ||
    (l.m_packed[7] != r.m_packed[7])); 
}

#endif  // __OMS_DBPCURRENCY_HPP