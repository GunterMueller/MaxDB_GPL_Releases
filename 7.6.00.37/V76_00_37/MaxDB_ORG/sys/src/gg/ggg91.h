/* @lastChanged: "2000-12-15"

  * @filename:   ggg91.h
  * @purpose:    "GG_kernel_classes"
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

#ifndef GGG91_H
#define GGG91_H

#include "ggg90.h"

#define NIL_PAGE_NO_GG91    MAX_INT4_SP00


/*----------------------------------------------------------------------------*/

class tgg91_FileVersion : public tgg90_Cint<2>
{ 
public:
    
    inline bool gg91IsDummyVers () const
    {
        return ( this->gg90IsNil() );   // \xFF\xFF
    }
    
    inline bool gg91IsSpecialVers () const
    {
        return ( this->gg90IsDummy() ); // \xFF\xFE
    }
    
    inline void gg91SetDummyVers ()
    {
        this->gg90SetNil();             // \xFF\xFF
    }
    
    inline void gg91SetSpecialVers ()
    {
        this->gg90SetDummy();           // \xFF\xFE
    }
    
    inline operator SAPDB_Int2() const
    {
        union {
            unsigned char cvar[ 2 ];
            SAPDB_Int2    ivar;
        } tmp;
        tmp.cvar[ 0 ] = this->data_IV[ 0 ];
        tmp.cvar[ 1 ] = this->data_IV[ 1 ];
        return tmp.ivar;
    }

    void gg91AppendToLine (int &LineLen, tsp00_Line &Line) const;
    
private:
    
    CHECK_MAPPABLE_CLASS_GIVEN_SIZE_SP00 (tgg91_FileVersion, 2)
};


/*----------------------------------------------------------------------------*/

typedef char tgg91_ObjAnyBody [1];


/*----------------------------------------------------------------------------*/

class tgg91_PageRef
{
private:
    
    // 6 bytes: 4 bytes page no + 2 bytes position
    struct PageRefLayout
    {       
        tgg90_Cint4 Pno_IV;
        tgg90_Cint2 Pos_IV;
    } PageRef;

    CHECK_MAPPABLE_CLASS_SIZE_SP00 (tgg91_PageRef, PageRefLayout)
    
    public:
        
        void gg91BuildRef (tsp00_Int4 Pno, tsp00_Int4 Pos)
        {
            if (NIL_PAGE_NO_GG91 == Pno)
            {
                this->gg91SetNilRef();
                return;
            }
            this->PageRef.Pno_IV.becomes (Pno);
            this->PageRef.Pos_IV.becomes (Pos);
        }
        
        
        inline tsp00_Int4 gg91RefPno () const
        {
            return (this->PageRef.Pno_IV.gg90GetInt4() );
        }
        
        
        inline tsp00_Int4 gg91RefPos () const
        {
            return (this->PageRef.Pos_IV.gg90GetInt4() );
        }
        
        
        bool gg91IsDummyRef () const
        {
            return (this->PageRef.Pno_IV.gg90IsDummy() );
        }
        
        
        bool gg91IsNilRef () const
        {
            return (NIL_PAGE_NO_GG91 == this->PageRef.Pno_IV.gg90GetInt4());
        }
        
        
        void gg91SetDummyRef ()
        {
            this->PageRef.Pno_IV.gg90SetDummy();
            this->PageRef.Pos_IV.gg90SetZero ();
        }
        
        
        void gg91SetNilRef ()
        {
            this->PageRef.Pno_IV.becomes (NIL_PAGE_NO_GG91);
            this->PageRef.Pos_IV.gg90SetZero();
        }
        
        
        inline void gg91SetZeroRef ()
        {
            this->PageRef.Pno_IV.gg90SetZero();
            this->PageRef.Pos_IV.gg90SetZero();
        }
        
        
        inline bool	operator == (const tgg91_PageRef &other) const
        {
            return (memcmp (this, &other, sizeof (tgg91_PageRef)) == 0);
        }
        
        
        inline bool	operator != (const tgg91_PageRef &other) const
        {
            return !(*this == other);
        }
        
        inline bool	operator >	(const tgg91_PageRef &other) const
        {
            return (memcmp (this, &other, sizeof (tgg91_PageRef)) > 0);
        }
        
        inline bool	operator >= (const tgg91_PageRef &other) const
        {
            return (memcmp (this, &other, sizeof (tgg91_PageRef)) >= 0);
        }
};


/*----------------------------------------------------------------------------*/
class tgg91_TransNo : public tgg90_Cint<6>
{
private:
    CHECK_MAPPABLE_CLASS_GIVEN_SIZE_SP00 (tgg91_TransNo, 6)
    static const char * gg91_MAX_TRANS_ID;
public:
    
    inline SAPDB_UInt8 gg91GetUInt8 () const
		{
    return (
        (static_cast<SAPDB_UInt8>(this->data_IV [0]) << 40 ) |
        (static_cast<SAPDB_UInt8>(this->data_IV [1]) << 32 ) |
        (static_cast<SAPDB_UInt8>(this->data_IV [2]) << 24 ) |
        (static_cast<SAPDB_UInt8>(this->data_IV [3]) << 16 ) |
        (static_cast<SAPDB_UInt8>(this->data_IV [4]) <<  8 ) |
        (static_cast<SAPDB_UInt8>(this->data_IV [5])       )   );
		}

    inline bool        gg91IsMaxTrans () const
    {
        return (
            this->data_IV [0] == gg91_MAX_TRANS_ID [0] &&
            this->data_IV [1] == gg91_MAX_TRANS_ID [1] &&
            this->data_IV [2] == gg91_MAX_TRANS_ID [2] &&
            this->data_IV [3] == gg91_MAX_TRANS_ID [3] &&
            this->data_IV [4] == gg91_MAX_TRANS_ID [4] &&
            this->data_IV [5] == gg91_MAX_TRANS_ID [5] );
    }
    inline tgg91_TransNo gg91SetMaxTrans()
    {
        this->becomes(gg91_MAX_TRANS_ID);
        return *this;
    }
    inline static tgg91_TransNo gg91MaxTransNo() 
    {
        tgg91_TransNo transNo;
        transNo.gg91SetMaxTrans();
        return transNo;
    }
    void gg91BuildComplement(tgg91_TransNo& Complement) const
    {
        Complement.data_IV [0] = 255 - this->data_IV [0];
        Complement.data_IV [1] = 255 - this->data_IV [1];
        Complement.data_IV [2] = 255 - this->data_IV [2];
        Complement.data_IV [3] = 255 - this->data_IV [3];
        Complement.data_IV [4] = 255 - this->data_IV [4];
        Complement.data_IV [5] = 255 - this->data_IV [5];
    }
};

/*----------------------------------------------------------------------------*/

class tgg91_SessionNo : public tgg90_Cint<4>
{ 
public:
    
    void gg91BuildServerTaskTraceSession (SAPDB_UInt1 jobtype, /*SrvTasks_JobType*/
                                          SAPDB_UInt2 jobno);
    
    inline void gg91BuildInternalTraceSession (tsp00_Uint4 SubtransNo)
    {
        this->becomes(SubtransNo);
    }

    inline void gg91BuildTaskSession (unsigned char TaskType)
    {
        this->data_IV [0] = NIL_BYTE_GG90;
        this->data_IV [1] = NIL_BYTE_GG90;
        this->data_IV [2] = 0;
        this->data_IV [3] = TaskType;
    }
    
    inline bool gg91IsMaxSession () const
    {
        return (
            this->data_IV [0] == NIL_BYTE_GG90 &&
            this->data_IV [1] == 0             &&
            this->data_IV [2] == 0             &&
            this->data_IV [3] == 0                );
    }
    
    void gg91AppendToLine (int &LineLen, tsp00_Line &Line) const;
    
private:
    
    CHECK_MAPPABLE_CLASS_GIVEN_SIZE_SP00 (tgg91_SessionNo, 4)
};


#endif /* GGG91_H */
