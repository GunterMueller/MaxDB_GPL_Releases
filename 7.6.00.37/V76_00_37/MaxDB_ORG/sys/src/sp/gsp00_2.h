
/*!================================================================
 module:        gsp00_0.h

 responsible:   DanielD

 special area:  Common | Pascal compatibility

 description:   Pascal compatibility

 see:           http://pwww.bea.sap-ag.de/Entwicklung/cpp/C-Style/commonDeclarations.html

 Copyright (c) 1999-2005 SAP AG




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




 ===================================================================*/

#ifndef GSP00_2_H
#define GSP00_2_H

#include "gsp00_0.h"

externC void sp36_c2p (
                 void         * instanceData,
                 int            instanceSize,
                 const char   * str);

externC void sp36_BuildSet (
                 unsigned char * data,
                 int             maxVal,
                 int             argc,
                 va_list         argptr);

externC void sp36_BuildSmallSet (
                 unsigned char * data,
                 int             maxVal,
                 int             argc,
                 va_list         argptr);

/* standard pascal types */

typedef teo00_Bool pasbool;   /* pascal boolean is translated to this  */
typedef teo00_Bool boolean;   /* compatibility, use teo00_Bool instead */
typedef teo00_Bool BOOLEAN;   /* gnu p2c emits this sometimes          */
typedef short shortint;
typedef int integer;
typedef int INTEGER;          /* gnu p2c emits this sometimes          */
typedef long longint;
typedef float shortreal;
typedef float real;


#if defined (__cplusplus)
/* arrays */

class tsp00_Void {
  public:
    operator void * ();
    char * asCharp ();
    char & operator[] (int elem);
    char * operator+ (int offset);
};

#define TSP00_ARRAY tsp00_Array<T, elemCount_TV, allocCount_TV>

template<class T, int elemCount_TV, int allocCount_TV>
class tsp00_Array  /* @see http://p26326/Entwicklung/C-Style/commonDeclarations.html#ArrayTemplate */
{
  protected:
    T data_IV [allocCount_TV];

  public:
    char              * asCharp  ();
    const char        * asCharp  () const;
    teo00_Byte        * asBytep  ();
    const teo00_Byte  * asBytep  () const;
    TSP00_ARRAY       & SetZero();
    T                 & operator[] (int elem);
    const T           & operator[] (int elem) const;
    T                 * operator+ (int elements);
    const T           * operator+ (int elements) const;
                        operator T* ();
                        operator const T* () const;
    bool                operator == (const TSP00_ARRAY &other) const;
    bool                operator != (const TSP00_ARRAY &other) const;

#if defined (HAS_BUGGY_OVERLOADING_CONVERSIONS)
#define INDEX_OP_DECLARATION_MSP00(type)                        \
            T                  &operator [] (type elem);        \
            const T	       &operator [] (type elem) const;  \
            T                  *operator+ (type elements);      \
            const T            *operator+ (type elements) const

    INDEX_OP_DECLARATION_MSP00(char);
    INDEX_OP_DECLARATION_MSP00(unsigned char);
    INDEX_OP_DECLARATION_MSP00(short);
    INDEX_OP_DECLARATION_MSP00(unsigned short);
    INDEX_OP_DECLARATION_MSP00(unsigned int);
    INDEX_OP_DECLARATION_MSP00(long);
    INDEX_OP_DECLARATION_MSP00(unsigned long);
#if defined (_WIN64)
	INDEX_OP_DECLARATION_MSP00(teo00_Longuint);
#endif
#undef INDEX_OP_DECLARATION_MSP00
#endif
};


#define TSP00_COMPARABLEARRAY tsp00_ComparableArray<T, elemCount_TV>

template<class T, int elemCount_TV>
class tsp00_ComparableArray : public tsp00_Array<T, elemCount_TV, elemCount_TV>
{
    TSP00_COMPARABLEARRAY &rawAssign (const void *str);
    int    compare (const TSP00_COMPARABLEARRAY& other) const;
    bool   operator <= (const TSP00_COMPARABLEARRAY& other) const;
    bool   operator <  (const TSP00_COMPARABLEARRAY& other) const;
    bool   operator >= (const TSP00_COMPARABLEARRAY& other) const;
    bool   operator >  (const TSP00_COMPARABLEARRAY& other) const;
    bool   rawEqual (const void *str) const;
    int    rawCompare (const void *str) const;
};

#define teo00_ByteARRAY teo00_ByteArray<elemCount_TV>

template<int elemCount_TV>
class teo00_ByteArray : public tsp00_ComparableArray<teo00_Byte, elemCount_TV>
{
};

#define TSP00_PASCALSTRING tsp00_PascalString<elemCount_TV>

template<int elemCount_TV>
class tsp00_PascalString : public tsp00_ComparableArray<char, elemCount_TV>
{
    TSP00_PASCALSTRING &SetBlank();
    TSP00_PASCALSTRING &c2p (const char *str);
    int                 length () const;
};

#define TSP00_OLDPASCALSTRING tsp00_OldPascalString<elemCount_TV>

template<int elemCount_TV>
class tsp00_OldPascalString
{
  private:
    teo00_Byte          data_IV [elemCount_TV];
  public:
/* begin of copy from tsp00_Void class */
    char * asCharp  ()
    {
        return REINTERPRET_CAST (char*, this->data_IV);
    }
/* end of copy from tsp00_Void class */
/* begin of copy from Array template */
    TSP00_OLDPASCALSTRING &SetZero()
    {
        memset(data_IV, 0, elemCount_TV * sizeof(teo00_Byte));
        return *this;
    }
    bool                operator == (const TSP00_OLDPASCALSTRING &other) const
    {
        return feo00_MemCmp (this->data_IV, other.data_IV,
                   elemCount_TV * sizeof (teo00_Byte)) == 0;
    }
    bool                operator != (const TSP00_OLDPASCALSTRING &other) const
    {
        return !(*this == other);
    }

#define INDEX_OP_DECLARATION_MSP00(type)                        \
    teo00_Byte       &operator [] (type elem)        \
    {\
      return this->data_IV [RANGE_CHECK_DBG_MSP00 (elem, 0, elemCount_TV - 1)];\
    }\
    const teo00_Byte &operator [] (type elem) const  \
    {\
      return this->data_IV [RANGE_CHECK_DBG_MSP00 (elem, 0, elemCount_TV - 1)];\
    }\
    teo00_Byte       *operator+ (type elements)      \
    {\
      return this->data_IV + RANGE_CHECK_DBG_MSP00 (elements, 0, elemCount_TV);\
    }\
    const teo00_Byte *operator+ (type elements) const \
    {\
      return this->data_IV + RANGE_CHECK_DBG_MSP00 (elements, 0, elemCount_TV);\
    }

    INDEX_OP_DECLARATION_MSP00(int);
#if defined (HAS_BUGGY_OVERLOADING_CONVERSIONS)
    INDEX_OP_DECLARATION_MSP00(char);
    INDEX_OP_DECLARATION_MSP00(unsigned char);
    INDEX_OP_DECLARATION_MSP00(short);
    INDEX_OP_DECLARATION_MSP00(unsigned short);
    INDEX_OP_DECLARATION_MSP00(unsigned int);
    INDEX_OP_DECLARATION_MSP00(long);
    INDEX_OP_DECLARATION_MSP00(unsigned long);
#if defined (_WIN64)
	INDEX_OP_DECLARATION_MSP00(teo00_Longuint);
#endif
#endif
#undef INDEX_OP_DECLARATION_MSP00
/* end of copy from Array template */

    TSP00_OLDPASCALSTRING &SetBlank();
    TSP00_OLDPASCALSTRING &rawAssign (const char *str);
    TSP00_OLDPASCALSTRING &c2p (const char *str);
    int                 compare (const TSP00_OLDPASCALSTRING& other) const;
    bool                operator <= (const TSP00_OLDPASCALSTRING& other) const;
    bool                operator <  (const TSP00_OLDPASCALSTRING& other) const;
    bool                operator >= (const TSP00_OLDPASCALSTRING& other) const;
    bool                operator >  (const TSP00_OLDPASCALSTRING& other) const;
    bool                rawEqual (const char *str) const;
    int                 rawCompare (const char* str) const;
    int                 length () const;
    operator char * () {return REINTERPRET_CAST (char*, data_IV); };
};

inline tsp00_OldPascalString<1> fsp00_Char (char literal)
{
    tsp00_OldPascalString<1> result;
    result [0] = literal;
    return result;
}

#define TSP00_CSTRING tsp00_CString<elemCount_TV>

template<int elemCount_TV>
class tsp00_CString : public tsp00_Array<char, elemCount_TV + 1, ALIGN_4BYTE_EO00(elemCount_TV + C_STRING_ZERO_BYTE_LEN_EO00)>
{
  public:
    TSP00_CSTRING &Init ();
    TSP00_CSTRING &rawAssign (const char *str);
    TSP00_CSTRING &p2c (const tsp00_OldPascalString<elemCount_TV>& str);
    int            length () const;
    int            size () const;
};

/* Enumerations */

#define TSP00_ENUM tsp00_Enum<EnumType, maxVal_TV, RepresentationType>

template<class EnumType, int maxVal_TV, class RepresentationType>
class tsp00_Enum      /* @see http://p26326/Entwicklung/C-Style/commonDeclarations.html#EnumTemplate */
{
  private:
    RepresentationType data_IV;

  public:
    static TSP00_ENUM   fromInt (int i);
    static TSP00_ENUM   fromConst (EnumType enumVal);
    TSP00_ENUM         &becomes (EnumType enumVal);
    bool                invariant () const;
    operator            EnumType () const;
};

/* Sets */

#define TSP00_SET tsp00_Set<EnumType, maxVal_TV, bytesRequired_TV>

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
class tsp00_Set      /* @see http://p26326/Entwicklung/C-Style/commonDeclarations.html#SetTemplate */
{
  private:
    enum {byteBits_CV = 8};

    unsigned char data_IV [bytesRequired_TV];
  public:
    TSP00_SET          &clear ();
    TSP00_SET          &addElement (EnumType elem);
    TSP00_SET          &delElement (EnumType elem);
    TSP00_SET          &addRange (EnumType fromElem, EnumType toElem);
    TSP00_SET          &delRange (EnumType fromElem, EnumType toElem);
    TSP00_SET          &addSet (const TSP00_SET &otherSet);
    TSP00_SET          &delSet (const TSP00_SET &otherSet);

    bool                includes (EnumType elem) const;
    bool                equal (const TSP00_SET &otherSet) const;
    bool                operator == (const TSP00_SET &otherSet) const;
    bool                operator != (const TSP00_SET &otherSet) const;
    bool                subsetOf (const TSP00_SET &otherSet) const;
    bool                isEmpty () const;

    TSP00_SET          &sumOf       (const TSP00_SET& op1, const TSP00_SET& op2);
    TSP00_SET          &diffOf      (const TSP00_SET& op1, const TSP00_SET& op2);
    TSP00_SET          &intersectOf (const TSP00_SET& op1, const TSP00_SET& op2);
    TSP00_SET          &xorOf       (const TSP00_SET& op1, const TSP00_SET& op2);

    static TSP00_SET    build (int argc, ...);
};


#define TSP00_SMALLSET tsp00_SmallSet<EnumType, maxVal_TV>

template<class EnumType, int maxVal_TV>
class tsp00_SmallSet      /* @see http://p26326/Entwicklung/C-Style/commonDeclarations.html#SetTemplate */
{
  private:
    unsigned char data_IV;

  public:
    TSP00_SMALLSET     &clear ();
    TSP00_SMALLSET     &addElement (EnumType elem);
    TSP00_SMALLSET     &delElement (EnumType elem);
    TSP00_SMALLSET     &addRange (EnumType fromElem, EnumType toElem);
    TSP00_SMALLSET     &delRange (EnumType fromElem, EnumType toElem);
    TSP00_SMALLSET     &addSet (const TSP00_SMALLSET &otherSet);
    TSP00_SMALLSET     &delSet (const TSP00_SMALLSET &otherSet);

    bool                includes (EnumType elem) const;
    bool                equal (const TSP00_SMALLSET &otherSet) const;
    bool                operator == (const TSP00_SMALLSET &otherSet) const;
    bool                operator != (const TSP00_SMALLSET &otherSet) const;
    bool                subsetOf (const TSP00_SMALLSET &otherSet) const;
    bool                isEmpty () const;

    TSP00_SMALLSET     &sumOf       (const TSP00_SMALLSET& op1, const TSP00_SMALLSET& op2);
    TSP00_SMALLSET     &diffOf      (const TSP00_SMALLSET& op1, const TSP00_SMALLSET& op2);
    TSP00_SMALLSET     &intersectOf (const TSP00_SMALLSET& op1, const TSP00_SMALLSET& op2);
    TSP00_SMALLSET     &xorOf       (const TSP00_SMALLSET& op1, const TSP00_SMALLSET& op2);

    static TSP00_SMALLSET build (int argc, ...);
};


/*******************************************
 *       I M P L E M E N T A T I O N       *
 *******************************************/

/* arrays */

inline tsp00_Void::operator void * ()
{
    return this;
}

inline char * tsp00_Void::asCharp  ()
{
    return REINTERPRET_CAST (char*, this);
}

inline char & tsp00_Void::operator [] (int index)
{
    return REINTERPRET_CAST (char*, this) [index];
}

inline char * tsp00_Void::operator+ (int offset)
{
    return REINTERPRET_CAST (char*, this) + offset;
}

template<class T, int elemCount_TV, int allocCount_TV>
inline char * TSP00_ARRAY::asCharp  ()
{
    return REINTERPRET_CAST (char*, this->data_IV);
}

template<class T, int elemCount_TV, int allocCount_TV>
inline const char * TSP00_ARRAY::asCharp  () const
{
    return REINTERPRET_CAST (const char*, this->data_IV);
}

template<class T, int elemCount_TV, int allocCount_TV>
inline teo00_Byte * TSP00_ARRAY::asBytep  ()
{
    return REINTERPRET_CAST (teo00_Byte*, this->data_IV);
}

template<class T, int elemCount_TV, int allocCount_TV>
inline const teo00_Byte * TSP00_ARRAY::asBytep  () const
{
    return REINTERPRET_CAST (const teo00_Byte*, this->data_IV);
}

template<class T, int elemCount_TV, int allocCount_TV>
inline TSP00_ARRAY& TSP00_ARRAY::SetZero ()
{
    memset (data_IV, 0, elemCount_TV * sizeof (T));
    return *this;
}

template<class T, int elemCount_TV, int allocCount_TV>
inline T& TSP00_ARRAY::operator [] (int elem)
{
    return this->data_IV [RANGE_CHECK_DBG_MSP00 (elem, 0, elemCount_TV - 1)];
}

template<class T, int elemCount_TV, int allocCount_TV>
inline const T& TSP00_ARRAY::operator [] (int elem) const
{
    return this->data_IV [RANGE_CHECK_DBG_MSP00 (elem, 0, elemCount_TV - 1)];
}

template<class T, int elemCount_TV, int allocCount_TV>
inline T* TSP00_ARRAY::operator+ (int elements)
{
    return this->data_IV + RANGE_CHECK_DBG_MSP00 (elements, 0, elemCount_TV - 1);
}

template<class T, int elemCount_TV, int allocCount_TV>
inline const T* TSP00_ARRAY::operator+ (int elements) const
{
    return this->data_IV + RANGE_CHECK_DBG_MSP00 (elements, 0, elemCount_TV);
}

template<class T, int elemCount_TV, int allocCount_TV>
inline TSP00_ARRAY::operator T* ()
{
    return &data_IV [0];
}

template<class T, int elemCount_TV, int allocCount_TV>
inline TSP00_ARRAY::operator const T* () const
{
    return &data_IV [0];
}

template<class T, int elemCount_TV, int allocCount_TV>
inline bool TSP00_ARRAY::operator == (const TSP00_ARRAY& other) const
{
    return feo00_MemCmp (this->data_IV, other.data_IV,
                   elemCount_TV * sizeof (T)) == 0;
}

template<class T, int elemCount_TV, int allocCount_TV>
inline bool TSP00_ARRAY::operator != (const TSP00_ARRAY& other) const
{
    return !(*this == other);
}

#if defined (HAS_BUGGY_OVERLOADING_CONVERSIONS)
#define INDEX_OP_IMPLEMENTATION_MSP00(type)   \
                                           \
        template<class T, int elemCount_TV, int allocCount_TV>\
        inline T& TSP00_ARRAY::operator [] (type elem)\
        {\
            return this->data_IV [RANGE_CHECK_DBG_MSP00 (elem, 0, elemCount_TV - 1)];\
        }\
                                           \
        template<class T, int elemCount_TV, int allocCount_TV>\
        inline const T& TSP00_ARRAY::operator [] (type elem) const\
        {\
            return this->data_IV [RANGE_CHECK_DBG_MSP00 (elem, 0, elemCount_TV - 1)];\
        }\
                                           \
        template<class T, int elemCount_TV, int allocCount_TV>\
        inline T* TSP00_ARRAY::operator+ (type elements)\
        {\
            return this->data_IV + RANGE_CHECK_DBG_MSP00 (elements, 0, elemCount_TV);\
        }\
                                           \
        template<class T, int elemCount_TV, int allocCount_TV>\
        inline const T* TSP00_ARRAY::operator+ (type elements) const\
        {\
            return this->data_IV + RANGE_CHECK_DBG_MSP00 (elements, 0, elemCount_TV);\
        }

INDEX_OP_IMPLEMENTATION_MSP00(char);
INDEX_OP_IMPLEMENTATION_MSP00(unsigned char);
INDEX_OP_IMPLEMENTATION_MSP00(short);
INDEX_OP_IMPLEMENTATION_MSP00(unsigned short);
INDEX_OP_IMPLEMENTATION_MSP00(unsigned int);
INDEX_OP_IMPLEMENTATION_MSP00(long);
INDEX_OP_IMPLEMENTATION_MSP00(unsigned long);
#if defined (_WIN64)
        template<class T, int elemCount_TV, int allocCount_TV>
        inline T& TSP00_ARRAY::operator [] (teo00_Longuint elem)
        {
            return this->data_IV [RANGE_CHECK_DBG_MSP00 (elem, 0, elemCount_TV - 1)];
        }
                                           \
        template<class T, int elemCount_TV, int allocCount_TV>
        inline const T& TSP00_ARRAY::operator [] (teo00_Longuint elem) const
        {
            return this->data_IV [RANGE_CHECK_DBG_MSP00 (elem, 0, elemCount_TV - 1)];
        }
                                           \
        template<class T, int elemCount_TV, int allocCount_TV>
        inline T* TSP00_ARRAY::operator+ (teo00_Longuint elements)
        {
            return this->data_IV + RANGE_CHECK_DBG_MSP00 (elements, 0, elemCount_TV);
        }
                                           \
        template<class T, int elemCount_TV, int allocCount_TV>
        inline const T* TSP00_ARRAY::operator+ (teo00_Longuint elements) const
        {
            return this->data_IV + RANGE_CHECK_DBG_MSP00 (elements, 0, elemCount_TV);
        }

// INDEX_OP_IMPLEMENTATION_MSP00(teo00_Longuint);
#endif

#undef INDEX_OP_IMPLEMENTATION_MSP00

#endif

#undef TSP00_ARRAY

/* ComparableArray */

template<class T, int elemCount_TV>
inline TSP00_COMPARABLEARRAY & TSP00_COMPARABLEARRAY::rawAssign (const void *str)
{
    memcpy (this->data_IV, str, sizeof (TSP00_COMPARABLEARRAY));
    return *this;
}

template<class T, int elemCount_TV>
inline int TSP00_COMPARABLEARRAY::compare (const TSP00_COMPARABLEARRAY& other) const
{
    return feo00_MemCmp (this->data_IV, other.data_IV, sizeof (TSP00_COMPARABLEARRAY));
}

template<class T, int elemCount_TV>
inline bool TSP00_COMPARABLEARRAY::operator <= (const TSP00_COMPARABLEARRAY& other) const
{
    return this->compare (other) <= 0;
}

template<class T, int elemCount_TV>
inline bool TSP00_COMPARABLEARRAY::operator < (const TSP00_COMPARABLEARRAY& other) const
{
    return this->compare (other) < 0;
}

template<class T, int elemCount_TV>
inline bool TSP00_COMPARABLEARRAY::operator >= (const TSP00_COMPARABLEARRAY& other) const
{
    return this->compare (other) >= 0;
}

template<class T, int elemCount_TV>
inline bool TSP00_COMPARABLEARRAY::operator > (const TSP00_COMPARABLEARRAY& other) const
{
    return this->compare (other) > 0;
}

template<class T, int elemCount_TV>
inline bool TSP00_COMPARABLEARRAY::rawEqual (const void *str) const
{
    return feo00_MemCmp (this->data_IV, str, sizeof (TSP00_COMPARABLEARRAY));
}

template<class T, int elemCount_TV>
inline int TSP00_COMPARABLEARRAY::rawCompare (const void *str) const
{
    return feo00_MemCmp (this->data_IV, str, sizeof (TSP00_COMPARABLEARRAY));
}

#undef TSP00_COMPARABLEARRAY

/* ByeArray */

#undef teo00_ByteARRAY

/* Pascal Strings */

template<int elemCount_TV>
inline TSP00_PASCALSTRING & TSP00_PASCALSTRING::SetBlank ()
{
    memset (this->data_IV, STATIC_CAST (int, ' '), elemCount_TV);
    return *this;
}

template<int elemCount_TV>
inline TSP00_PASCALSTRING & TSP00_PASCALSTRING::c2p (const char *str)
{
    sp36_c2p (this->data_IV, elemCount_TV, str);
    return *this;
}

template<int elemCount_TV>
inline int TSP00_PASCALSTRING::length () const
{
    int len = elemCount_TV;

    while ((len > 0) && (this->data_IV [len - 1] == ' '))
    {
        --len;
    }
    return len;
}

#undef TSP00_PASCALSTRING

/* Old Pascal Strings */

template<int elemCount_TV>
inline TSP00_OLDPASCALSTRING& TSP00_OLDPASCALSTRING::SetBlank ()
{
    memset (data_IV, STATIC_CAST (int, ' '), elemCount_TV);
    return *this;
}

template<int elemCount_TV>
inline TSP00_OLDPASCALSTRING & TSP00_OLDPASCALSTRING::rawAssign (const char *str)
{
    memcpy (this->data_IV, str, elemCount_TV);
    return *this;
}

template<int elemCount_TV>
inline TSP00_OLDPASCALSTRING & TSP00_OLDPASCALSTRING::c2p (const char *str)
{
    sp36_c2p (this->data_IV, elemCount_TV, str);
    return *this;
}

template<int elemCount_TV>
inline int TSP00_OLDPASCALSTRING::compare (const TSP00_OLDPASCALSTRING& other) const
{
    return feo00_MemCmp (this->data_IV, other.data_IV, elemCount_TV);
}

template<int elemCount_TV>
inline bool TSP00_OLDPASCALSTRING::operator <= (const TSP00_OLDPASCALSTRING& other) const
{
    return this->compare (other) <= 0;
}

template<int elemCount_TV>
inline bool TSP00_OLDPASCALSTRING::operator < (const TSP00_OLDPASCALSTRING& other) const
{
    return this->compare (other) < 0;
}

template<int elemCount_TV>
inline bool TSP00_OLDPASCALSTRING::operator >= (const TSP00_OLDPASCALSTRING& other) const
{
    return this->compare (other) >= 0;
}

template<int elemCount_TV>
inline bool TSP00_OLDPASCALSTRING::operator > (const TSP00_OLDPASCALSTRING& other) const
{
    return this->compare (other) > 0;
}

template<int elemCount_TV>
inline bool TSP00_OLDPASCALSTRING::rawEqual (const char *str) const
{
    return feo00_MemCmp (this->data_IV, str, elemCount_TV) == 0;
}

template<int elemCount_TV>
inline int TSP00_OLDPASCALSTRING::rawCompare (const char *str) const
{
    return feo00_MemCmp (this->data_IV, str, elemCount_TV);
}

template<int elemCount_TV>
inline int TSP00_OLDPASCALSTRING::length () const
{
    int len = elemCount_TV;

    while ((len > 0) && (this->data_IV [len - 1] == ' '))
    {
        --len;
    }
    return len;
}

#undef TSP00_OLDPASCALSTRING

/* C Strings */

template<int elemCount_TV>
inline TSP00_CSTRING &TSP00_CSTRING::Init ()
{
    this->data_IV [0] = '\0';
    return *this;
}

template<int elemCount_TV>
inline TSP00_CSTRING &TSP00_CSTRING::rawAssign (const char *str)
{
    size_t length=strlen(str);

    if(length>elemCount_TV)
        length=elemCount_TV;

    memmove(this->data_IV, str, length);
    memset(this->data_IV+length, '\0', elemCount_TV+1-length);

    return *this;
}

template<int elemCount_TV>
inline TSP00_CSTRING &TSP00_CSTRING::p2c (const tsp00_OldPascalString<elemCount_TV>& str)
{
    int len = str.length ();

    memcpy (this->data_IV, &str, len);
    this->data_IV [len] = '\0';
    return *this;
}

template<int elemCount_TV>
inline int TSP00_CSTRING::length () const
{
    return (int) strlen (this->data_IV);
}

template<int elemCount_TV>
inline int TSP00_CSTRING::size () const
{
    return elemCount_TV;
}

#undef TSP00_CSTRINGS

/* Enumerations */

template<class EnumType, int maxVal_TV, class RepresentationType>
inline TSP00_ENUM TSP00_ENUM::fromInt (int i)
{
    TSP00_ENUM result;

    (void) RANGE_CHECK_DBG_MSP00 (i, 0, STATIC_CAST (int, maxVal_TV));
    result.data_IV = STATIC_CAST (RepresentationType, i);
    return result;
}

template<class EnumType, int maxVal_TV, class RepresentationType>
inline TSP00_ENUM TSP00_ENUM::fromConst (EnumType enumVal)
{
    TSP00_ENUM result;

    (void) RANGE_CHECK_DBG_MSP00 (enumVal, 0, STATIC_CAST (int, maxVal_TV));
    result.data_IV = STATIC_CAST (RepresentationType, enumVal);
    return result;
}

template<class EnumType, int maxVal_TV, class RepresentationType>
inline TSP00_ENUM& TSP00_ENUM::becomes (EnumType enumVal)
{
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, enumVal), 0, STATIC_CAST (int, maxVal_TV));
    this->data_IV = STATIC_CAST (RepresentationType, enumVal);
    return *this;
}

template<class EnumType, int maxVal_TV, class RepresentationType>
inline bool TSP00_ENUM::invariant () const
{
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (long, this->data_IV), 0, STATIC_CAST (long, maxVal_TV));
    return true; /*(0 <= this->data_IV) && (this->data_IV <= maxVal_TV); */
}

template<class EnumType, int maxVal_TV, class RepresentationType>
inline TSP00_ENUM::operator EnumType () const
{
    return STATIC_CAST (EnumType, this->data_IV);
}

#undef TSP00_ENUM

/* Sets */

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline TSP00_SET& TSP00_SET::clear ()
{
    memset (this->data_IV, '\0', sizeof (this->data_IV));
    return *this;
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline TSP00_SET& TSP00_SET::addElement (EnumType elem)
{
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, elem), 0, STATIC_CAST (int, maxVal_TV));
    int targetByte = elem / byteBits_CV;
    unsigned char compareByte = 1 << (elem - (targetByte * byteBits_CV));
    this->data_IV [targetByte] |= compareByte;
    return *this;
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline TSP00_SET& TSP00_SET::delElement (EnumType elem)
{
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, elem), 0, STATIC_CAST (int, maxVal_TV));
    int targetByte = elem / byteBits_CV;
    unsigned char compareByte = 1 << (elem - (targetByte * byteBits_CV));
    this->data_IV [targetByte] &= STATIC_CAST (unsigned char, ~compareByte);
    return *this;
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline TSP00_SET& TSP00_SET::addRange (EnumType fromElem, EnumType toElem)
{
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, fromElem), 0, STATIC_CAST (int, maxVal_TV));
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, toElem), 0, STATIC_CAST (int, maxVal_TV));
    for (EnumType i = fromElem; i <= toElem;  i = STATIC_CAST (EnumType,  i + 1)) {
        this->addElement (i);
    }
    return *this;
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline TSP00_SET& TSP00_SET::delRange (EnumType fromElem, EnumType toElem)
{
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, fromElem), 0, STATIC_CAST (int, maxVal_TV));
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, toElem), 0, STATIC_CAST (int, maxVal_TV));
    for (EnumType i = fromElem; i <= toElem;  i = STATIC_CAST (EnumType, i + 1)) {
        this->delElement (i);
    }
    return *this;
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline TSP00_SET& TSP00_SET
   ::addSet (const TSP00_SET& otherSet)
{
    for (int i = 0; i < sizeof (this->data_IV); ++ i) {
        this->data_IV [i] |= otherSet.data_IV [i];
    }
    return *this;
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline TSP00_SET& TSP00_SET::delSet (const TSP00_SET& otherSet)
{
    for (int i = 0; i < sizeof (this->data_IV); ++ i) {
        this->data_IV [i] &= ~otherSet.data_IV [i];
    }
    return *this;
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline bool TSP00_SET::includes (EnumType elem) const
{
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, elem), 0, STATIC_CAST (int, maxVal_TV));
    int targetByte = elem / byteBits_CV;
    unsigned int compareByte = 1 << (elem - (targetByte * byteBits_CV));
    if ((this->data_IV [targetByte] & compareByte) == '\0') {
        return false;
    }
    else {
        return true;
    }
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline bool TSP00_SET::equal (const TSP00_SET &otherSet) const
{
    return feo00_MemCmp (this->data_IV, otherSet.data_IV,
                   sizeof (this->data_IV)) == 0;
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline bool TSP00_SET::operator == (const TSP00_SET &otherSet) const
{
    return this->equal (otherSet);
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline bool TSP00_SET::operator != (const TSP00_SET &otherSet) const
{
    return ! this->equal (otherSet);
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline bool TSP00_SET::subsetOf (const TSP00_SET &otherSet) const
{
    for (int i = 0; i < sizeof (this->data_IV); ++ i)
    {
        if ((this->data_IV [i] & otherSet.data_IV [i])
            != this->data_IV [i])
        {
            return false;
        }
    }
    return true;
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline bool TSP00_SET::isEmpty () const
{
    for (int i = 0; i < sizeof (this->data_IV); ++ i)
    {
        if ( this->data_IV [i] )
        {
            return false;
        }
    }
    return true;
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline TSP00_SET &TSP00_SET::sumOf (const TSP00_SET& op1, const TSP00_SET& op2)
{
    for (int i = 0; i < sizeof (this->data_IV); ++ i)
    {
        this->data_IV [i] = op1.data_IV [i] | op2.data_IV [i];
    }
    return *this;
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline TSP00_SET &TSP00_SET::diffOf (const TSP00_SET& op1, const TSP00_SET& op2)
{
    for (int i = 0; i < sizeof (this->data_IV); ++ i)
    {
        this->data_IV [i] = op1.data_IV [i]
                    - (op1.data_IV [i] & op2.data_IV [i]);
    }
    return *this;
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline TSP00_SET &TSP00_SET::intersectOf (const TSP00_SET& op1, const TSP00_SET& op2)
{
    for (int i = 0; i < sizeof (this->data_IV); ++ i)
    {
        this->data_IV [i] = op1.data_IV [i] & op2.data_IV [i];
    }
    return *this;
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
inline TSP00_SET &TSP00_SET::xorOf (const TSP00_SET& op1, const TSP00_SET& op2)
{
    for (int i = 0; i < sizeof (this->data_IV); ++ i)
    {
        this->data_IV [i] = op1.data_IV [i] ^ op2.data_IV [i];
    }
    return *this;
}

template<class EnumType, int maxVal_TV, int bytesRequired_TV>
TSP00_SET TSP00_SET::build (int argc, ...)
{
    TSP00_SET   result;
    va_list     argptr;

#ifdef __opencxx
#pragma -C OldCCast-
#endif

    va_start (argptr, argc);
    sp36_BuildSet (result.data_IV, maxVal_TV, argc, argptr);
    va_end (argptr);
    return result;

#ifdef __opencxx
#pragma -C OldCCast+
#endif
}

#undef TSP00_SET

template<class EnumType, int maxVal_TV>
inline TSP00_SMALLSET& TSP00_SMALLSET::clear ()
{
    this->data_IV = '\0';
    return *this;
}

template<class EnumType, int maxVal_TV>
inline TSP00_SMALLSET& TSP00_SMALLSET::addElement (EnumType elem)
{
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, elem), 0, STATIC_CAST (int, maxVal_TV));
    unsigned char compareByte = 1 << elem;
    this->data_IV |= STATIC_CAST (unsigned char, compareByte);
    return *this;
}

template<class EnumType, int maxVal_TV>
inline TSP00_SMALLSET& TSP00_SMALLSET::delElement (EnumType elem)
{
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, elem), 0, STATIC_CAST (int, maxVal_TV));
    unsigned char compareByte = 1 << elem;
    this->data_IV &= STATIC_CAST (unsigned char, ~compareByte);
    return *this;
}

template<class EnumType, int maxVal_TV>
inline TSP00_SMALLSET& TSP00_SMALLSET::addRange (EnumType fromElem, EnumType toElem)
{
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, fromElem), 0, STATIC_CAST (int, maxVal_TV));
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, toElem), 0, STATIC_CAST (int, maxVal_TV));
    for (EnumType i = fromElem; i <= toElem; i = STATIC_CAST (EnumType, i + 1)) {
        this->addElement (i);
    }
    return *this;
}

template<class EnumType, int maxVal_TV>
inline TSP00_SMALLSET& TSP00_SMALLSET::delRange (EnumType fromElem, EnumType toElem)
{
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, fromElem), 0, STATIC_CAST (int, maxVal_TV));
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, toElem), 0, STATIC_CAST (int, maxVal_TV));
    for (EnumType i = fromElem; i <= toElem; i = STATIC_CAST (EnumType, i + 1)) {
        this->delElement (i);
    }
    return *this;
}

template<class EnumType, int maxVal_TV>
inline TSP00_SMALLSET& TSP00_SMALLSET
   ::addSet (const TSP00_SMALLSET& otherSet)
{
    this->data_IV |= otherSet.data_IV;
    return *this;
}

template<class EnumType, int maxVal_TV>
inline TSP00_SMALLSET& TSP00_SMALLSET::delSet (const TSP00_SMALLSET& otherSet)
{
    this->data_IV &= ~otherSet.data_IV;
    return *this;
}

template<class EnumType, int maxVal_TV>
inline bool TSP00_SMALLSET::includes (EnumType elem) const
{
    (void) RANGE_CHECK_DBG_MSP00 (STATIC_CAST (int, elem), 0, STATIC_CAST (int, maxVal_TV));
    unsigned int compareByte = 1 << elem;
    if ((this->data_IV & compareByte) == '\0') {
        return false;
    }
    else {
        return true;
    }
}

template<class EnumType, int maxVal_TV>
inline bool TSP00_SMALLSET::equal (const TSP00_SMALLSET &otherSet) const
{
    return (this->data_IV == otherSet.data_IV);
}

template<class EnumType, int maxVal_TV>
inline bool TSP00_SMALLSET::operator == (const TSP00_SMALLSET &otherSet) const
{
    return this->equal (otherSet);
}

template<class EnumType, int maxVal_TV>
inline bool TSP00_SMALLSET::operator != (const TSP00_SMALLSET &otherSet) const
{
    return ! this->equal (otherSet);
}

template<class EnumType, int maxVal_TV>
inline bool TSP00_SMALLSET::subsetOf (const TSP00_SMALLSET &otherSet) const
{
    return ((this->data_IV & otherSet.data_IV) != this->data_IV);
}

template<class EnumType, int maxVal_TV>
inline bool TSP00_SMALLSET::isEmpty () const
{
    return this->data_IV ? false : true ;
}

template<class EnumType, int maxVal_TV>
inline TSP00_SMALLSET &TSP00_SMALLSET::sumOf (
   const TSP00_SMALLSET& op1, const TSP00_SMALLSET& op2)
{
    this->data_IV = op1.data_IV | op2.data_IV;
    return *this;
}

template<class EnumType, int maxVal_TV>
inline TSP00_SMALLSET &TSP00_SMALLSET::diffOf (
   const TSP00_SMALLSET& op1, const TSP00_SMALLSET& op2)
{
    this->data_IV = op1.data_IV - (op1.data_IV & op2.data_IV);
    return *this;
}

template<class EnumType, int maxVal_TV>
inline TSP00_SMALLSET &TSP00_SMALLSET::intersectOf (
   const TSP00_SMALLSET& op1, const TSP00_SMALLSET& op2)
{
    this->data_IV = op1.data_IV & op2.data_IV;
    return *this;
}

template<class EnumType, int maxVal_TV>
inline TSP00_SMALLSET &TSP00_SMALLSET::xorOf (
   const TSP00_SMALLSET& op1, const TSP00_SMALLSET& op2)
{
    this->data_IV = op1.data_IV ^ op2.data_IV;
    return *this;
}

template<class EnumType, int maxVal_TV>
TSP00_SMALLSET TSP00_SMALLSET::build (int argc, ...)
{
    TSP00_SMALLSET   result;
    va_list     argptr;

#ifdef __opencxx
#pragma -C OldCCast-
#endif

    va_start (argptr, argc);
    sp36_BuildSet (&result.data_IV, maxVal_TV, argc, argptr);
    va_end (argptr);
    return result;

#ifdef __opencxx
#pragma -C OldCCast+
#endif

}

#undef TSP00_SMALLSET


#endif   /* defined (__cplusplus) */

/*
 * hide different declaration syntax C++ vs. C in macros
 * Usage:
 *    // TYPE tsp00_name = ARRAY [1..mxsp_name] Of char;
 *    typedef ARRAY_TYPE_MSP00(tsp00_name, char, mxsp_name);
 *    // localvar : ARRAY [1..20] OF char;
 *    ARRAY_VAR_MSP00(localVar, char, 20);
 */

#if defined (__cplusplus)
  #define ARRAY_TYPE_MSP00(newName, elemType, byteSize) \
                tsp00_Array<elemType, byteSize, byteSize> newName
  #define ARRAY_VAR_MSP00(varName, elemType, byteSize) \
                tsp00_Array<elemType, byteSize, byteSize> varName
  #define ARRAY_TYPESTRING_MSP00(elemType, byteSize) \
                tsp00_Array<elemType, byteSize, byteSize>
  #define ARRAY2_TYPE_MSP00(newName, elemType, byteSize0, byteSize1) \
                tsp00_Array<tsp00_Array<elemType, byteSize1, byteSize1>, byteSize0, byteSize0> newName
  #define ARRAY2_VAR_MSP00(varName, elemType, byteSize0, byteSize1) \
                tsp00_Array<tsp00_Array<elemType, byteSize1, byteSize1>, byteSize0, byteSize0> varName
  #define BYTEARRAY_TYPE_MSP00(newName, byteSize) \
                teo00_ByteArray<byteSize> newName
  #define BYTEARRAY_VAR_MSP00(varName, byteSize) \
                teo00_ByteArray<byteSize> varName
  #define BYTEARRAY2_TYPE_MSP00(newName, byteSize0, byteSize1) \
                tsp00_Array<teo00_ByteArray<byteSize1>, byteSize0, byteSize0> newName
  #define BYTEARRAY2_VAR_MSP00(varName, byteSize0, byteSize1) \
                tsp00_Array<teo00_ByteArray<byteSize1>, byteSize0, byteSize0> varName
  #define PASCALSTRING_TYPE_MSP00(newName, byteSize) \
                tsp00_PascalString<byteSize> newName
  #define PASCALSTRING_VAR_MSP00(varName, byteSize) \
                tsp00_PascalString<byteSize> varName
  #define PASCALSTRING2_TYPE_MSP00(newName, byteSize0, byteSize1) \
                tsp00_Array<tsp00_PascalString<byteSize1>, byteSize0, byteSize0> newName
  #define PASCALSTRING2_VAR_MSP00(varName, byteSize0, byteSize1) \
                tsp00_Array<tsp00_PascalString<byteSize1>, byteSize0, byteSize0> varName
  #define OLDPASCALSTRING_TYPE_MSP00(newName, byteSize) \
                tsp00_OldPascalString<byteSize> newName
  #define OLDPASCALSTRING_VAR_MSP00(varName, byteSize) \
                tsp00_OldPascalString<byteSize> varName
  #define OLDPASCALSTRING2_TYPE_MSP00(newName, byteSize0, byteSize1) \
                tsp00_Array<tsp00_OldPascalString<byteSize1>, byteSize0, byteSize0> newName
  #define OLDPASCALSTRING2_VAR_MSP00(varName, byteSize0, byteSize1) \
                tsp00_Array<tsp00_OldPascalString<byteSize1>, byteSize0, byteSize0> varName
  #define CSTRING_TYPE_MSP00(newName, byteSize) \
                tsp00_CString<byteSize> newName
  #define ENUM_MSP00(enumType, maxVal, reprType) \
                tsp00_Enum<enumType, maxVal, reprType>
  #define ENUM_TYPE_MSP00(newName, enumType, maxVal, reprType) \
                tsp00_Enum<enumType, maxVal, reprType> newName
  #define ENUM_VAR_MSP00(varName, enumType, maxVal, reprType) \
                tsp00_Enum<enumType, maxVal, reprType> varName
  #define SET_TYPE_MSP00(newName, enumType, maxVal, bytesRequ) \
                tsp00_Set<enumType, maxVal, bytesRequ> newName
  #define SET_VAR_MSP00(varName, enumType, maxVal, bytesRequ) \
                tsp00_Set<enumType, maxVal, bytesRequ> varName
  #define SMALLSET_TYPE_MSP00(newName, enumType, maxVal) \
                tsp00_SmallSet<enumType, maxVal> newName
  #define SMALLSET_VAR_MSP00(varName, enumType, maxVal) \
                tsp00_SmallSet<enumType, maxVal> varName
#else
  #define ARRAY_TYPE_MSP00(newName, elemType, byteSize) \
                elemType newName[byteSize]
  #define ARRAY_VAR_MSP00(varName, elemType, byteSize) \
                elemType varName[byteSize]
  #define ARRAY2_TYPE_MSP00(newName, elemType, byteSize0, byteSize1) \
                elemType newName[byteSize0][byteSize1]
  #define ARRAY2_VAR_MSP00(varName, elemType, byteSize0, byteSize1) \
                elemType varName[byteSize0][byteSize1]
  #define BYTEARRAY_TYPE_MSP00(newName, byteSize) \
                teo00_Byte newName[byteSize]
  #define BYTEARRAY_VAR_MSP00(varName, byteSize) \
                teo00_Byte varName[byteSize]
  #define BYTEARRAY2_TYPE_MSP00(newName, byteSize0, byteSize1) \
                teo00_Byte newName[byteSize0][byteSize1]
  #define BYTEARRAY2_VAR_MSP00(varName, byteSize0, byteSize1) \
                teo00_Byte varName[byteSize0][byteSize1]
  #define PASCALSTRING_TYPE_MSP00(newName, byteSize) \
                char newName[byteSize]
  #define PASCALSTRING_VAR_MSP00(varName, byteSize) \
                char varName[byteSize]
  #define PASCALSTRING2_TYPE_MSP00(newName, byteSize0, byteSize1) \
                char newName[byteSize0][byteSize1]
  #define PASCALSTRING2_VAR_MSP00(varName, byteSize0, byteSize1) \
                char varName[byteSize0][byteSize1]
  #define OLDPASCALSTRING_TYPE_MSP00(newName, byteSize) \
                teo00_Byte newName[byteSize]
  #define OLDPASCALSTRING_VAR_MSP00(varName, byteSize) \
                teo00_Byte varName[byteSize]
  #define OLDPASCALSTRING2_TYPE_MSP00(newName, byteSize0, byteSize1) \
                teo00_Byte newName[byteSize0][byteSize1]
  #define OLDPASCALSTRING2_VAR_MSP00(varName, byteSize0, byteSize1) \
                teo00_Byte varName[byteSize0][byteSize1]
  #define CSTRING_TYPE_MSP00(newName, byteSize) \
                char newName[ALIGN_4BYTE_EO00(byteSize + C_STRING_ZERO_BYTE_LEN_EO00)]
  #define ENUM_MSP00(enumType, maxVal, reprType) \
                reprType
  #define ENUM_TYPE_MSP00(newName, enumType, maxVal, reprType) \
                reprType newName
  #define ENUM_VAR_MSP00(varName, enumType, maxVal, reprType) \
                reprType varName
  #define SET_TYPE_MSP00(newName, enumType, maxVal, bytesRequ) \
                unsigned char newName [bytesRequ]
  #define SET_VAR_MSP00(varName, enumType, maxVal, bytesRequ) \
                unsigned char varName [bytesRequ]
  #define SMALLSET_TYPE_MSP00(newName, enumType, maxVal) \
                unsigned char newName [1]
  #define SMALLSET_VAR_MSP00(varName, enumType, maxVal) \
                unsigned char varName [1]
#endif   /* defined (__cplusplus) */


#endif
