/* @lastChanged: "1999-04-26"

  * @filename:   ggg92.h
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

#ifndef GGG92_H
#define GGG92_H

/* ========================================================================== */
/* ==========           header file of OmsObjectId                  ========= */
/* ========================================================================== */

#include "geo00.h"
#include "ggg00.h"

#define NIL_OBJ_PAGE_NO          NIL_PAGE_NO_GG00
#define VERSION_OBJ_PAGE_NO_GG92 NIL_PAGE_NO_GG00
#define NIL_OBJ_PAGE_POS         0
#define NIL_OBJ_VERSION          0

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

class tgg92_OmsTypeOid
{
public:
  tgg92_OmsTypeOid () 
  {
    m_oid.m_oidAsStruct.pno        = NIL_OBJ_PAGE_NO;
    m_oid.m_oidAsStruct.pagePos    = NIL_OBJ_PAGE_POS;
    m_oid.m_oidAsStruct.generation = NIL_OBJ_VERSION;
  }

  tgg92_OmsTypeOid (SAPDB_UInt4 p, unsigned short pp, unsigned short g)
  { 
    m_oid.m_oidAsStruct.pno        = p;
    m_oid.m_oidAsStruct.pagePos    = pp;
    m_oid.m_oidAsStruct.generation = g;
  }

  tgg92_OmsTypeOid (SAPDB_UInt8 i8)
  { 
    m_oid.m_oidAsInt8 = i8;
  }


  union  {
    SAPDB_UInt8       m_oidAsInt8;
    struct {
      SAPDB_UInt4     pno;
      unsigned short  pagePos;
      unsigned short  generation;
    } m_oidAsStruct;
  } m_oid;
};

const tgg92_OmsTypeOid nilOid;


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

class tgg92_OmsObjectId  : public  tgg92_OmsTypeOid     // OmsObjectId header file
{
public :
  
  void setPno(unsigned int pno)         { m_oid.m_oidAsStruct.pno = pno;      }
  void setPagePos(unsigned short pos)   { m_oid.m_oidAsStruct.pagePos = pos;  }
  void setGeneration(unsigned short g)  { m_oid.m_oidAsStruct.generation = g; }

  /*----------------------------------------------------------------------------*/

  void setNil()      { m_oid.m_oidAsInt8 = nilOid.m_oid.m_oidAsInt8;         }

  /*----------------------------------------------------------------------------*/

  unsigned int   getPno()        const   { return m_oid.m_oidAsStruct.pno;         }
  unsigned short getPos()        const   { return m_oid.m_oidAsStruct.pagePos;     }
  unsigned short getGeneration() const   { return m_oid.m_oidAsStruct.generation;  }
 
  /*----------------------------------------------------------------------------*/

  //tgg92_OmsTypeOid* pOid() { return &m_oid; }

  /*----------------------------------------------------------------------------*/

  bool operator==( const tgg92_OmsObjectId&r ) const    { return m_oid.m_oidAsInt8 == r.m_oid.m_oidAsInt8;  }
  bool operator!=( const tgg92_OmsObjectId&r ) const    { return m_oid.m_oidAsInt8 != r.m_oid.m_oidAsInt8;  }
  bool operator< ( const tgg92_OmsObjectId&r ) const    { return m_oid.m_oidAsInt8 <  r.m_oid.m_oidAsInt8;  }
  bool operator> ( const tgg92_OmsObjectId&r ) const    { return m_oid.m_oidAsInt8 >  r.m_oid.m_oidAsInt8;  }
  bool operator<=( const tgg92_OmsObjectId&r ) const    { return m_oid.m_oidAsInt8 <= r.m_oid.m_oidAsInt8;  }
  bool operator>=( const tgg92_OmsObjectId&r ) const    { return m_oid.m_oidAsInt8 >= r.m_oid.m_oidAsInt8;  }

  /*----------------------------------------------------------------------------*/

  void operator=(const tgg92_OmsObjectId&r)  { m_oid.m_oidAsInt8 = r.m_oid.m_oidAsInt8; }

  /*----------------------------------------------------------------------------*/

  bool operator! () const { return m_oid.m_oidAsInt8 == nilOid.m_oid.m_oidAsInt8; }
  operator bool () const  { return m_oid.m_oidAsInt8 != nilOid.m_oid.m_oidAsInt8; }
};


/* ========================================================================== */
/* ==========               header file ggg92.h                     ========= */
/* ========================================================================== */

#define NIL_OBJ_FILE_NO_GG92         0
#define MIN_OBJ_FILE_NO_GG92         1
#define MAX_OBJ_FILE_NO_GG92         MAX_INT4_SP00

#define MIN_OBJ_FRAME_VERS_GG92      1
#define MAX_OBJ_FRAME_VERS_GG92      MAX_UINT1_SP00
#define NIL_OBJ_FRAME_VERS_GG92      NIL_OBJ_VERSION

#define NIL_OBJ_PAGE_POS_GG92        NIL_OBJ_PAGE_POS


/*---------------------------------------------------------------------------*/

class tgg92_KernelOid : protected tgg92_OmsObjectId
{
public:
    
    void gg92AppendToLine (int &LineLen, tsp00_Line &Line);

    tsp00_PageNo     gg92GetPno()       const { return getPno();}
    tgg00_ObjPagePos gg92GetPos()       const { return getPos(); }
    int              gg92GetFrameVers() const { return getGeneration(); }
    
    bool gg92IsEqIgnoreFrameVers (const tgg92_KernelOid &r) const {  return getPno() == r.getPno() && getPos() == r.getPos(); }
    bool gg92IsEqPnoPos (const tgg92_KernelOid &r)          const{ return gg92IsEqIgnoreFrameVers(r); }
    
    bool gg92IsNil ()          const  { return ( ! bool (*this) ); }
    
    bool gg92IsNilFileNo ()    const { return (getPos() == NIL_OBJ_PAGE_POS); }
    bool gg92IsNilPno ()       const { return (getPno() == NIL_OBJ_PAGE_NO); }
    bool gg92IsNilPos ()       const { return (getPos() == NIL_OBJ_PAGE_POS_GG92); }
    bool gg92IsNilFrameVers () const { return (getGeneration() == NIL_OBJ_VERSION); }


    void gg92Set (tsp00_PageNo pno, tgg00_ObjPagePos pos, int FrameVers)
    {
        setPno       (pno);
        setPagePos   (pos);
        setGeneration(FrameVers);
    }
    
    void gg92SetNil ()                     { setNil (); }
    void gg92SetNilPno ()                  { setPno       (NIL_OBJ_PAGE_NO); }
    void gg92SetNilPos ()                  { setPagePos   (NIL_OBJ_PAGE_POS_GG92); }
    void gg92SetNilFrameVers ()            { setGeneration(NIL_OBJ_FRAME_VERS_GG92); }
    void gg92SetPno (tsp00_PageNo pno)     { setPno (pno); }
    void gg92SetPos (tgg00_ObjPagePos pos) { setPagePos(pos);    }
    void gg92SetFrameVers (int vers)       { setGeneration(vers); }
    
private:
    CHECK_MAPPABLE_CLASS_GIVEN_SIZE_SP00 (tgg92_KernelOid, sizeof (tgg00_OidSpace))
};

/*----------------------------------------------------------------------------*/

//class tgg92_KernelOidSpace 
//// designed for 'union' structures (without any ctor)
//{
//public:
//    tgg92_KernelOid* gg92PtrOid ()
//    {
//        return ( REINTERPRET_CAST(tgg92_KernelOid *, this));
//    }
//    
//private:
//    tgg00_OidSpace oidSpace;
//
//    CHECK_MAPPABLE_CLASS_GIVEN_SIZE_SP00 (tgg92_KernelOidSpace, sizeof (tgg92_KernelOid))
//};

/*----------------------------------------------------------------------------*/

typedef struct tgg92_ObjHeader
{
    tgg00_ObjTransInfo ohdTransInfo_gg92;
    tgg00_ObjPagePos   ohdNextFreeObj_gg92;
    tgg00_ObjState     ohdState_gg92;
    tgg00_ObjFrameVers ohdFrameVersion_gg92;
} tgg92_ObjHeader;

#endif /* GGG92_H */
