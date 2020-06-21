/* @lastChanged: "1999-02-11  12:12"

  * @filename:   ggg90.h
  * @purpose:    "GG_kernel_templates"
  * @release:    7.2.0.0
  * @see:        ""
  *
  * @Copyright (c) 1999-2005 SAP AG"


    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#ifndef GGG90_H
#define GGG90_H

#include <stdlib.h>
#include <string.h>
#include "gsp00.h"
#include "gsp00_1.h"
#include "geo47.h"

#define DUMMY_BYTE_GG90 0xFE
#define NIL_BYTE_GG90   0xFF
#define UNDEF_INT4_GG90 -1

#define TGG90_CINT tgg90_Cint<elemCount_TV>



template<int elemCount_TV>
class tgg90_Cint
{
  protected:

      unsigned char data_IV [elemCount_TV];

  public:
      void                  gg90AppendToLine (int &LineLen, tsp00_Line &Line) const;
      tsp00_Int4            gg90GetInt4 () const;

      bool					gg90IsDummy  () const;
      bool					gg90IsNil    () const;
      bool					gg90IsZero   () const;

      void					gg90SetDummy ();
      void					gg90SetNil   ();
      void					gg90SetZero  ();

//      unsigned char	    &operator [] (int elem);
      const unsigned char	&operator [] (int elem) const;

//      operator unsigned char* ();
//      operator const unsigned char* () const;
//      tsp00_PascalString<elemCount_TV>& asPasStr ();

      void					becomes (const char *CharInt);
      void					becomes (tsp00_Int4 IntVal);
      void					becomes (tsp00_Uint4 IntVal);
      void					operator ++ ();
      void					operator -- ();

      bool					operator == (const TGG90_CINT &other) const;
      bool					operator != (const TGG90_CINT &other) const;
      bool					operator >	(const TGG90_CINT &other) const;
      bool					operator >= (const TGG90_CINT &other) const;


#if defined (HAS_BUGGY_OVERLOADING_CONVERSIONS)
//      unsigned char			&operator [] (long elem);
//      const unsigned char	&operator [] (long elem) const;
#endif // buggy overloading
};


/******************************************
*       T R A C E   F U N C T I O N S     *
*******************************************/

void gg90StrToLine (const char *StrVal,
                    int        &LineLen,
                    tsp00_Line &Line);

void gg90IntToLine (tsp00_Int4  IntVal,
                    int        &LineLen,
                    tsp00_Line &Line);

void gg90HexToLine (unsigned char  CharVal,
                    int           &LineLen,
                    tsp00_Line    &Line);

/******************************************
*       I M P L E M E N T A T I O N       *
*******************************************/


template<int elemCount_TV>
inline void TGG90_CINT::gg90AppendToLine (int &LineLen, tsp00_Line &Line) const
{
    tsp00_Int4 result;

    if ( this->gg90IsDummy() )
    {
        gg90StrToLine (_T("dummy"), LineLen, Line);
        return;
    }

    if ( this->gg90IsNil() )
    {
        gg90StrToLine (_T("nil"), LineLen, Line);
        return;
    }

    result = this->gg90GetInt4();

    if (result != UNDEF_INT4_GG90)
    {
        gg90IntToLine (result, LineLen, Line);
        return;
    }

    if (LineLen + sizeof (_TCHAR) < sizeof (Line))
    {
        Line [LineLen++] = _T('x');
    }

    for (int i = 0; i < elemCount_TV; i++)
        gg90HexToLine (this->data_IV [i], LineLen, Line);
}

template<int elemCount_TV>
inline tsp00_Int4 TGG90_CINT::gg90GetInt4 () const
{
    int         ByteVal;
    int         IntOffset;
    tsp00_Int4	result = UNDEF_INT4_GG90;

    IntOffset = (elemCount_TV > sizeof (tsp00_Int4)) ? elemCount_TV - sizeof (tsp00_Int4) : 0;

    if (IntOffset > 0)
    {
        for (int i = 0; i < IntOffset; i++)
        {
            if (this->data_IV [i] != 0) return (UNDEF_INT4_GG90); // result > max int4
        }
    }

    if (elemCount_TV >= sizeof (tsp00_Int4) && this->data_IV [IntOffset] >= 0x80)
    {
        return (UNDEF_INT4_GG90); // result >= hex ...80000000 (maxint4 + 1)
    }

    result = 0;
    for (int i = IntOffset; i < elemCount_TV; i++)
    {
        ByteVal = this->data_IV [i];
        ByteVal = ByteVal << (8 * (elemCount_TV - 1 - i));
        result  = result + ByteVal;
    }

   return (result);
}

// template <int elemCount_TV>
// inline tsp00_PascalString<elemCount_TV>& tgg90_Cint<elemCount_TV>::asPasStr ()
// {
/*
* this method is used in GNU p2c generated routines
*    Reason: p2c generates code for PascalStrings because
*            tgg90_Cint2 etc. are declared as such
*/
//     return *(tsp00_PascalString<elemCount_TV> *) data_IV;
// }

template<int elemCount_TV>
inline bool TGG90_CINT::gg90IsDummy () const
{
    int	i;

    // return ( this == "[FF ...] FF FE" )

    for (i = 0; i < elemCount_TV-1; i++)
    {
        if (this->data_IV [i] != NIL_BYTE_GG90)
        {
            return (false);
        }
    }

    return (this->data_IV [elemCount_TV-1] == DUMMY_BYTE_GG90);
}

template<int elemCount_TV>
inline bool TGG90_CINT::gg90IsNil () const
{
    int	i;

    for (i = 0; i < elemCount_TV; i++)
    {
        if (this->data_IV [i] != NIL_BYTE_GG90)
        {
            return (false);
        }
    }

    return (true);
}

template<int elemCount_TV>
inline bool TGG90_CINT::gg90IsZero () const
{
    int	i;

    for (i = 0; i < elemCount_TV; i++)
    {
        if (this->data_IV [i] != 0)
        {
            return (false);
        }
    }

    return (true);
}


template<int elemCount_TV>
inline void TGG90_CINT::gg90SetDummy ()
{
    memset (data_IV, NIL_BYTE_GG90, elemCount_TV-1);
    this->data_IV [elemCount_TV-1] = DUMMY_BYTE_GG90;
}

template<int elemCount_TV>
inline void TGG90_CINT::gg90SetNil ()
{
    memset (data_IV, NIL_BYTE_GG90, elemCount_TV);
}

template<int elemCount_TV>
inline void TGG90_CINT::gg90SetZero ()
{
    memset (data_IV, 0, elemCount_TV);
}

// template<int elemCount_TV>
// inline unsigned char& TGG90_CINT::operator [] (int elem)
// {
//     return this->data_IV [RANGE_CHECK_DBG_MSP00 (elem, 0, elemCount_TV - 1)];
// }

template<int elemCount_TV>
inline const unsigned char& TGG90_CINT::operator [] (int elem) const
{
    return this->data_IV [RANGE_CHECK_DBG_MSP00 (elem, 0, elemCount_TV - 1)];
}

// template<int elemCount_TV>
// inline TGG90_CINT::operator unsigned char* ()
// {
//     return &data_IV [0];
// }

// template<int elemCount_TV>
// inline  TGG90_CINT::operator const unsigned char* () const
// {
//     return &data_IV [0];
// }

template<int elemCount_TV>
inline void TGG90_CINT::becomes (const char *CharInt)
{
    memcpy (this, CharInt, elemCount_TV);
}

template<int elemCount_TV>
inline void TGG90_CINT::becomes (tsp00_Int4 IntVal)
{
    if (IntVal < 0)
    {
        this->gg90SetNil();
        return;
    }

    memset (this->data_IV, 0, elemCount_TV);
    int i = elemCount_TV - 1;
    if ( i > 3 )
    {
        i = 3;
    }

    for ( ; i > 0; i-- )
    {
        this->data_IV[elemCount_TV-(i+1)] = (unsigned char)
            ( (IntVal & (0x000000FFUL<<(i<<3))) >> (i<<3) ); // i<<3 = i*8
    }
    this->data_IV[elemCount_TV-1] = (unsigned char) (IntVal & 0x000000FFUL);
}

template<int elemCount_TV>
inline void TGG90_CINT::becomes (tsp00_Uint4 IntVal)
{
    memset (this->data_IV, 0, elemCount_TV);
    int i = elemCount_TV - 1;
    if ( i > 3 )
    {
        i = 3;
    }

    for ( ; i > 0; i-- )
    {
        this->data_IV[elemCount_TV-(i+1)] = (unsigned char)
            ( (IntVal & (0x000000FFUL<<(i<<3))) >> (i<<3) ); // i<<3 = i*8
    }
    this->data_IV[elemCount_TV-1] = (unsigned char) (IntVal & 0x000000FFUL);
}

template<int elemCount_TV>
inline void TGG90_CINT::operator ++ ()
{
    for (int i = elemCount_TV - 1; i >= 0; i--)
    {
        if (this->data_IV [i] < 255)
        {
            this->data_IV [i] ++;
            return;
        }
        this->data_IV [i] = 0;
    }
}

template<int elemCount_TV>
inline void TGG90_CINT::operator -- ()
{
    for (int i = elemCount_TV - 1; i >= 0; i--)
    {
        if (this->data_IV [i] > 0)
        {
            this->data_IV [i] --;
            return;
        }
        this->data_IV [i] = 255;
    }
}

template<int elemCount_TV>
inline bool TGG90_CINT::operator == (const TGG90_CINT& other) const
{
    return memcmp (this, &other, elemCount_TV * sizeof (char)) == 0;
}

template<int elemCount_TV>
inline bool TGG90_CINT::operator != (const TGG90_CINT& other) const
{
    return !(*this == other);
}

template<int elemCount_TV>
inline bool TGG90_CINT::operator > (const TGG90_CINT& other) const
{
    return memcmp (this, &other, elemCount_TV * sizeof (char)) > 0;
}

template<int elemCount_TV>
inline bool TGG90_CINT::operator >= (const TGG90_CINT& other) const
{
    return memcmp (this, &other, elemCount_TV * sizeof (char)) >= 0;
}


#if defined (BUGGY_OVERLOADING_CONVERSIONS)

// template<int elemCount_TV>
// inline unsigned char& TGG90_CINT::operator [] (long elem)
// {
//     return this->data_IV [RANGE_CHECK_DBG_MSP00 (elem, 0, elemCount_TV - 1)];
// }

// template<int elemCount_TV>
// inline const unsigned char& TGG90_CINT::operator [] (long elem) const
// {
//     return this->data_IV [RANGE_CHECK_DBG_MSP00 (elem, 0, elemCount_TV - 1)];
// }

#endif // buggy overloading

#undef TGG90_CINT


/*----------------------------------------------------------------------------*/


class tgg90_Cint2 : public tgg90_Cint<2>
{
public:
    inline tsp00_Int4  gg90GetInt4  () const;
    inline bool        gg90IsDummy  () const;
    inline bool        gg90IsNil    () const;
    inline bool        gg90IsZero   () const;
    inline void        gg90SetDummy ();
    inline void        gg90SetNil   ();
    inline void        gg90SetZero  ();
    inline void        becomes (const char *CharInt) { tgg90_Cint<2>::becomes(CharInt); }
    inline void        becomes (tsp00_Int4 IntVal);
    inline void        operator ++ ();
    inline void        operator -- ();
    inline bool        operator == (const tgg90_Cint2& other) const;
    inline bool        operator != (const tgg90_Cint2& other) const;
    inline bool        operator >  (const tgg90_Cint2& other) const;
    inline bool        operator >= (const tgg90_Cint2& other) const;
private:
    CHECK_MAPPABLE_CLASS_GIVEN_SIZE_SP00 (tgg90_Cint2, 2)
};


inline tsp00_Int4 tgg90_Cint2::gg90GetInt4 () const
{
    return (
        (this->data_IV [0] << 8 ) |
        (this->data_IV [1]      )   );
}


inline bool tgg90_Cint2::gg90IsDummy () const
{
    return (
        this->data_IV [0] == NIL_BYTE_GG90  &&
        this->data_IV [1] == DUMMY_BYTE_GG90  );
}


inline bool tgg90_Cint2::gg90IsNil () const
{
    return (
        this->data_IV [0] == NIL_BYTE_GG90 &&
        this->data_IV [1] == NIL_BYTE_GG90  );
}


inline bool tgg90_Cint2::gg90IsZero () const
{
    return (
        this->data_IV [0] == 0 &&
        this->data_IV [1] == 0  );
}


inline void tgg90_Cint2::gg90SetDummy ()
{
    this->data_IV [0] = NIL_BYTE_GG90;
    this->data_IV [1] = DUMMY_BYTE_GG90;
}


inline void tgg90_Cint2::gg90SetNil ()
{
    this->data_IV [0] = NIL_BYTE_GG90;
    this->data_IV [1] = NIL_BYTE_GG90;
}


inline void tgg90_Cint2::gg90SetZero ()
{
    this->data_IV [0] = 0;
    this->data_IV [1] = 0;
}


inline void tgg90_Cint2::becomes (tsp00_Int4 IntVal)
{
    if (IntVal < 0)
    {
        this->gg90SetNil();
        return;
    }
    this->data_IV [0] = (unsigned char) ((IntVal & 0x0000FF00UL) >> 8);
    this->data_IV [1] = (unsigned char) ( IntVal & 0x000000FFUL);
}


inline void tgg90_Cint2::operator ++ ()
{
    if (this->data_IV [1] < 255)
    {
        this->data_IV [1] ++;
        return;
    }
    this->data_IV [1] = 0;
    this->data_IV [0]++;
}


inline void tgg90_Cint2::operator -- ()
{

    if (this->data_IV [1] > 0)
    {
        this->data_IV [1] --;
        return;
    }
    this->data_IV [1] = 255;
    this->data_IV [0]--;
}


inline bool tgg90_Cint2::operator == (const tgg90_Cint2& other) const
{
    return (
        this->data_IV[0] == other.data_IV[0] &&
        this->data_IV[1] == other.data_IV[1] );
}


inline bool tgg90_Cint2::operator != (const tgg90_Cint2& other) const
{
    return (
        this->data_IV[0] != other.data_IV[0] ||
        this->data_IV[1] != other.data_IV[1] );
}


inline bool tgg90_Cint2::operator > (const tgg90_Cint2& other) const
{
    return (
        (this->data_IV[0] >  other.data_IV[0])
        ||
        (this->data_IV[0] == other.data_IV[0] && this->data_IV[1] > other.data_IV[1]) );
}


inline bool tgg90_Cint2::operator >= (const tgg90_Cint2& other) const
{
    return (
        (this->data_IV[0] >= other.data_IV[0])
        ||
        (this->data_IV[0] == other.data_IV[0] && this->data_IV[1] >= other.data_IV[1]) );
}


/*----------------------------------------------------------------------------*/


class tgg90_Cint4 : public tgg90_Cint<4>
{
public:
    inline tsp00_Int4  gg90GetInt4  () const;
    inline bool        gg90IsDummy  () const;
    inline bool        gg90IsNil    () const;
    inline bool        gg90IsZero   () const;
    inline void        gg90SetDummy ();
    inline void        gg90SetNil   ();
    inline void        gg90SetZero  ();
    inline void        becomes (const char *CharInt) { tgg90_Cint<4>::becomes(CharInt); }
    inline void        becomes (tsp00_Int4 IntVal);
    inline bool        operator == (const tgg90_Cint4 &other) const;
    inline bool        operator != (const tgg90_Cint4 &other) const;
private:
    CHECK_MAPPABLE_CLASS_GIVEN_SIZE_SP00 (tgg90_Cint4, 4)
};


inline tsp00_Int4 tgg90_Cint4::gg90GetInt4 () const
{
    if (this->data_IV [0] >= 0x80)
    {
        return (UNDEF_INT4_GG90); // result >= hex ...80000000 (maxint4 + 1)
    }

    return (
        (this->data_IV [0] << 24 ) |
        (this->data_IV [1] << 16 ) |
        (this->data_IV [2] <<  8 ) |
        (this->data_IV [3]       )   );
}


inline bool tgg90_Cint4::gg90IsDummy () const
{
    return (
        this->data_IV [0] == NIL_BYTE_GG90 &&
        this->data_IV [1] == NIL_BYTE_GG90 &&
        this->data_IV [2] == NIL_BYTE_GG90 &&
        this->data_IV [3] == DUMMY_BYTE_GG90  );
}


inline bool tgg90_Cint4::gg90IsNil () const
{
    return (
        this->data_IV [0] == NIL_BYTE_GG90 &&
        this->data_IV [1] == NIL_BYTE_GG90 &&
        this->data_IV [2] == NIL_BYTE_GG90 &&
        this->data_IV [3] == NIL_BYTE_GG90  );
}


inline bool tgg90_Cint4::gg90IsZero () const
{
    return (
        this->data_IV [0] == 0 &&
        this->data_IV [1] == 0 &&
        this->data_IV [2] == 0 &&
        this->data_IV [3] == 0  );
}


inline void tgg90_Cint4::gg90SetDummy ()
{
    this->data_IV [0] = NIL_BYTE_GG90;
    this->data_IV [1] = NIL_BYTE_GG90;
    this->data_IV [2] = NIL_BYTE_GG90;
    this->data_IV [3] = DUMMY_BYTE_GG90;
}


inline void tgg90_Cint4::gg90SetNil ()
{
    this->data_IV [0] = NIL_BYTE_GG90;
    this->data_IV [1] = NIL_BYTE_GG90;
    this->data_IV [2] = NIL_BYTE_GG90;
    this->data_IV [3] = NIL_BYTE_GG90;
}


inline void tgg90_Cint4::gg90SetZero ()
{
    this->data_IV [0] = 0;
    this->data_IV [1] = 0;
    this->data_IV [2] = 0;
    this->data_IV [3] = 0;
}


inline void tgg90_Cint4::becomes (tsp00_Int4 IntVal)
{
    if (IntVal < 0)
    {
        this->gg90SetNil();
        return;
    }
    this->data_IV [0] = (unsigned char) ((IntVal & 0xFF000000UL) >> 24);
    this->data_IV [1] = (unsigned char) ((IntVal & 0x00FF0000UL) >> 16);
    this->data_IV [2] = (unsigned char) ((IntVal & 0x0000FF00UL) >>  8);
    this->data_IV [3] = (unsigned char) ( IntVal & 0x000000FFUL);
}


inline bool tgg90_Cint4::operator == (const tgg90_Cint4 &other) const
{
    return (
        this->data_IV[0] == other.data_IV[0] &&
        this->data_IV[1] == other.data_IV[1] &&
        this->data_IV[2] == other.data_IV[2] &&
        this->data_IV[3] == other.data_IV[3]    );
}

inline bool tgg90_Cint4::operator != (const tgg90_Cint4 &other) const
{
    return (
        this->data_IV[0] != other.data_IV[0] ||
        this->data_IV[1] != other.data_IV[1] ||
        this->data_IV[2] != other.data_IV[2] ||
        this->data_IV[3] != other.data_IV[3]    );
}



/*----------------------------------------------------------------------------*/


class tgg90_Cint6 : public tgg90_Cint<6>
{
public:
    inline tsp00_Int4  gg90GetInt4  () const;
    inline bool        gg90IsDummy  () const;
    inline bool        gg90IsNil    () const;
    inline bool        gg90IsZero   () const;
    inline void        gg90SetDummy ();
    inline void        gg90SetNil   ();
    inline void        gg90SetZero  ();
    inline void        becomes (const char *CharInt) { tgg90_Cint<6>::becomes(CharInt); }
    inline void        becomes (tsp00_Int4 IntVal);
    inline bool        operator == (const tgg90_Cint6 &other) const;
    inline bool        operator != (const tgg90_Cint6 &other) const;
private:
    CHECK_MAPPABLE_CLASS_GIVEN_SIZE_SP00 (tgg90_Cint6, 6)
};


inline tsp00_Int4 tgg90_Cint6::gg90GetInt4 () const
{
    if (this->data_IV[0] != 0 ||
        this->data_IV[1] != 0 ||
        this->data_IV[2] >= 0x80)
    {
        return (UNDEF_INT4_GG90); // result >= hex ...80000000 (maxint4 + 1)
    }

    return (
        (this->data_IV [2] << 24 ) |
        (this->data_IV [3] << 16 ) |
        (this->data_IV [4] <<  8 ) |
        (this->data_IV [5]       )   );
}


inline bool tgg90_Cint6::gg90IsDummy () const
{
    return (
        this->data_IV [0] == NIL_BYTE_GG90 &&
        this->data_IV [1] == NIL_BYTE_GG90 &&
        this->data_IV [2] == NIL_BYTE_GG90 &&
        this->data_IV [3] == NIL_BYTE_GG90 &&
        this->data_IV [4] == NIL_BYTE_GG90 &&
        this->data_IV [5] == DUMMY_BYTE_GG90  );
}


inline bool tgg90_Cint6::gg90IsNil () const
{
    return (
        this->data_IV [0] == NIL_BYTE_GG90 &&
        this->data_IV [1] == NIL_BYTE_GG90 &&
        this->data_IV [2] == NIL_BYTE_GG90 &&
        this->data_IV [3] == NIL_BYTE_GG90 &&
        this->data_IV [4] == NIL_BYTE_GG90 &&
        this->data_IV [5] == NIL_BYTE_GG90  );
}


inline bool tgg90_Cint6::gg90IsZero () const
{
    return (
        this->data_IV [0] == 0 &&
        this->data_IV [1] == 0 &&
        this->data_IV [2] == 0 &&
        this->data_IV [3] == 0 &&
        this->data_IV [4] == 0 &&
        this->data_IV [5] == 0  );
}


inline void tgg90_Cint6::gg90SetDummy ()
{
    this->data_IV [0] = NIL_BYTE_GG90;
    this->data_IV [1] = NIL_BYTE_GG90;
    this->data_IV [2] = NIL_BYTE_GG90;
    this->data_IV [3] = NIL_BYTE_GG90;
    this->data_IV [4] = NIL_BYTE_GG90;
    this->data_IV [5] = DUMMY_BYTE_GG90;
}


inline void tgg90_Cint6::gg90SetNil ()
{
    this->data_IV [0] = NIL_BYTE_GG90;
    this->data_IV [1] = NIL_BYTE_GG90;
    this->data_IV [2] = NIL_BYTE_GG90;
    this->data_IV [3] = NIL_BYTE_GG90;
    this->data_IV [4] = NIL_BYTE_GG90;
    this->data_IV [5] = NIL_BYTE_GG90;
}


inline void tgg90_Cint6::gg90SetZero ()
{
    this->data_IV [0] = 0;
    this->data_IV [1] = 0;
    this->data_IV [2] = 0;
    this->data_IV [3] = 0;
    this->data_IV [4] = 0;
    this->data_IV [5] = 0;
}


inline void tgg90_Cint6::becomes (tsp00_Int4 IntVal)
{
    if (IntVal < 0)
    {
        this->gg90SetNil();
        return;
    }
    this->data_IV [0] = 0;
    this->data_IV [1] = 0;
    this->data_IV [2] = (unsigned char) ((IntVal & 0xFF000000UL) >> 24);
    this->data_IV [3] = (unsigned char) ((IntVal & 0x00FF0000UL) >> 16);
    this->data_IV [4] = (unsigned char) ((IntVal & 0x0000FF00UL) >>  8);
    this->data_IV [5] = (unsigned char) ( IntVal & 0x000000FFUL);
}


inline bool tgg90_Cint6::operator == (const tgg90_Cint6 &other) const
{
    return (
        this->data_IV[0] == other.data_IV[0] &&
        this->data_IV[1] == other.data_IV[1] &&
        this->data_IV[2] == other.data_IV[2] &&
        this->data_IV[3] == other.data_IV[3] &&
        this->data_IV[4] == other.data_IV[4] &&
        this->data_IV[5] == other.data_IV[5]    );
}

inline bool tgg90_Cint6::operator != (const tgg90_Cint6 &other) const
{
    return (
        this->data_IV[0] != other.data_IV[0] ||
        this->data_IV[1] != other.data_IV[1] ||
        this->data_IV[2] != other.data_IV[2] ||
        this->data_IV[3] != other.data_IV[3] ||
        this->data_IV[4] != other.data_IV[4] ||
        this->data_IV[5] != other.data_IV[5]    );
}


/*----------------------------------------------------------------------------*/

typedef tgg90_Cint<8> tgg90_Cint8;

#endif /* GGG90_H */

