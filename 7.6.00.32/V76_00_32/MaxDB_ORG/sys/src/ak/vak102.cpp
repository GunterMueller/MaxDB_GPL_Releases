/****************************************************************************

  module      : vak102.cpp

  -------------------------------------------------------------------------

  responsible : RaymondR

  special area: AKDump
  description : AKDump startup code


  last changed: 2000-07-21  17:01
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG



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


*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg04.h"
#include "vak001.h"
#include "hak10.h"
#include "hak102.h"
#include "hsp77.h"
#include "hta01_3.h"
#include "hgg01_1.h"
#include "gsp03_1.h" // nocheck
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/* PTS 1111327 */
class CacheChecker
{
public :
	CacheChecker(tak_all_command_glob  &  acv);
	bool Execute();
private :
	struct tak10cache_header
	{
        tsp00_C4           ch_label;
        tsp00_Int4         ch_size;
        tak10cache_header* ch_next_cache;
#if !defined (BIT64)
        tsp00_Int4         ch_filler;
#endif
	}; 

	struct ChunkInfo
	{
		unsigned char* m_startPos;
		unsigned char* m_endPos;
	};
	void CheckPointer(const char* msg, tsp00_MoveObjPtr p)
	{
		if (NULL == p)
		{
			return;
		}
		unsigned char* pChar = REINTERPRET_CAST(unsigned char*, p);
		for (int ix = 0; ix < m_cntChunks; ++ix)
		{
			if ((m_chunkInfo[ix].m_startPos < pChar) &&
				(m_chunkInfo[ix].m_endPos > pChar))
			{
				return;
			}
		}
		m_error = true;
		OpMsg("%s invalid : %p", msg, pChar);
	}
	void CheckNullPointer(const char* msg, tsp00_MoveObjPtr p)
	{
		if (NULL != p)
		{
			m_error = true;
			OpMsg("%s not null : %p", msg, p);
		}
	}

  static void OpMsg( const char*fmt, ... );
  
  private :

  bool                   m_error;
	int                    m_cntChunks;
	ChunkInfo              m_chunkInfo[256];
	tak_all_command_glob&  m_acv;
};



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/* PTS 1107248 */

externPascal pasbool a102akdump ( tak_all_command_glob  &  acv)
{
#if COMPILEMODE_MSP00 >= SLOW_MSP00 
    t01disable_debug_or_trace ();
#endif

    return a10akdump (acv);
}

/*----------------------------------------------------------------*/

CacheChecker::CacheChecker(tak_all_command_glob  &  acv)
: m_acv(acv)
, m_error(false)
, m_cntChunks(0)
{
	tak10cache_header* p = REINTERPRET_CAST(tak10cache_header*, acv.a_sysdir.ci_cache_p);
	do
	{
		unsigned char* pChar = REINTERPRET_CAST(unsigned char*, p);
		m_chunkInfo[m_cntChunks].m_startPos = pChar;
		m_chunkInfo[m_cntChunks].m_endPos   = pChar + p->ch_size * sizeof(tsp00_Buf);
		++m_cntChunks;
		p = p->ch_next_cache;
	}
	while (NULL != p);
}

/*----------------------------------------------------------------*/

bool CacheChecker::Execute()
{
	tak10cache_header* p = REINTERPRET_CAST(tak10cache_header*, m_acv.a_sysdir.ci_cache_p);
	int            pos   = sizeof(tak10cache_header) + 2 * sizeof(tak_globalCacheReference) + 1;
	int chunk            = 0;
	unsigned char* pChar = REINTERPRET_CAST(unsigned char*, p);
	tak_cache_dir_entry* lastOk = NULL;
	bool                 prevFree = false;
	pChar += (pos - 1);
	try
	{
		do
		{
			++chunk;
			tsp00_Int4 size = p->ch_size * sizeof(tsp00_Buf);
			while ((pos < size) && (!m_error))
			{
               tak_cache_dir_entry* pDir = REINTERPRET_CAST(tak_cache_dir_entry*, pChar);
			   if (pDir->cd_check() != cak_cache_chunk_flag)
			   {
				  m_error = true;
                  OpMsg("wrong check sum %d", pDir->cd_check());
			   }
               if (pos != pDir->cd_pos())
               {
                  m_error = true;
                  OpMsg("wrong pos %d, expected %d", pDir->cd_pos(), pos);
               }
			   if (pDir->cd_state().includes(dno_meaning))
			   {
				   this->CheckNullPointer("cd_bl_less", pDir->cd_bl_less().moveobj_ptr());
				   this->CheckPointer("cd_next_free", pDir->cd_next_free().moveobj_ptr());
				   this->CheckNullPointer("cd_bl_fix_prev", pDir->cd_fix_prev().moveobj_ptr());
				   if (dyn_syntax_tree == pDir->cd_mem_type())
				   {
					   this->CheckPointer("cd_bl_fix_next", pDir->cd_bl_fix_next().moveobj_ptr());
				   }
				   else
				   {
					   this->CheckNullPointer("cd_bl_fix_next", pDir->cd_fix_next().moveobj_ptr());
				   }
                   this->CheckNullPointer("cd_bl_lru_prev", pDir->cd_lru_prev().moveobj_ptr());
				   this->CheckNullPointer("cd_bl_lru_next", pDir->cd_lru_next().moveobj_ptr());
				   if ((pDir->cd_mem_type() < dyn_free) || (pDir->cd_mem_type() > dyn_syntax_tree))
				   {
					   OpMsg("wrong mem type %d", (int)(pDir->cd_mem_type()));
				   }
				   if (dyn_free == pDir->cd_mem_type())
				   {
					   if (prevFree)
					   {
						   m_error = true;
						   OpMsg("free block after free block");
					   }
					   else
					   {
					     prevFree = true;
					   }
				   }
				   else
				   {
					   prevFree = false;
				   }
			   }
			   else
			   {
				   this->CheckPointer("cd_less",     pDir->cd_less().moveobj_ptr());
				   this->CheckPointer("cd_greater",  pDir->cd_greater().moveobj_ptr());
				   this->CheckPointer("cd_fix_next", pDir->cd_fix_next().moveobj_ptr());
				   this->CheckPointer("cd_fix_prev", pDir->cd_fix_prev().moveobj_ptr());
				   this->CheckPointer("cd_lru_next", pDir->cd_lru_next().moveobj_ptr());
				   this->CheckPointer("cd_lru_prev", pDir->cd_lru_prev().moveobj_ptr());
				   if (pDir->cd_balance() > right_weighted)
				   {
                       m_error = true;
					   OpMsg("wrong state %d", (int)(pDir->cd_balance())); 
				   }
                   if (pDir->cd_refCount() < 0)
                   {
                       m_error = true;
                       OpMsg("wrong refCount %d", pDir->cd_refCount()); 
                   }
				   prevFree = false;
			   }
			   pos   += pDir->cd_syslen();
			   pChar += pDir->cd_syslen();
			   if (!m_error)
			   {
				   lastOk = pDir;
			   }
			   else
			   {
				   OpMsg("error pos %d", pDir->cd_pos()); 
			   }
			}
			p = p->ch_next_cache;
			if (NULL == p)
			{
				break;
			}
			else
			{ 
				prevFree = false;
				pos      = sizeof(tak10cache_header) + 1;
				pChar    = REINTERPRET_CAST(unsigned char*, p) + pos - 1;
			}
		}
		while (!m_error);
	}
	catch(...)
	{
		m_error = true;
		OpMsg("unknown exception occured, pos = %d, chunk = %d", pos, chunk);
	}
  return !m_error;
}

/*----------------------------------------------------------------*/

void CacheChecker::OpMsg( const char*fmt, ... ) 
{
  char        buffer[256];
  tsp00_Int4  BufLen;

  va_list args;
  va_start (args, fmt);
  SAPDB_MemFillNoCheck (&buffer[0], ' ', sizeof(buffer));
  sp77vsprintf ( buffer, sizeof(buffer), fmt, args );
  BufLen = (tsp00_Int4/*PTS 1109041*/)strlen(buffer);
  buffer[BufLen] = ' ';
  char* pBuffer =  &buffer[0];
  while (BufLen > 0)
  {
    g01optextmsg (sp3p_console, sp3m_error, csp3_ak_msg, csp3_n_akcache, pBuffer);
    pBuffer += sizeof(tsp00_C40);
    BufLen  -= sizeof(tsp00_C40);
  }
  va_end (args);
}

/*----------------------------------------------------------------*/

externPascal pasbool a102IsCacheOk(tak_all_command_glob  &  acv)
{
  CacheChecker checker(acv);
  return checker.Execute();
}

/*----------------------------------------------------------------*/

externPascal pasbool a102PtrOK (tak_all_command_glob & acv, tak_cache_dir_pointer p)
{
    struct tak10cache_header 
    { 
       // chunk header of the AK cache
       tsp00_C4      ch_label;
       tsp00_Int4    ch_size;
       tsp00_BufAddr ch_next_cache;
    };
    // checks if given pointer lies in any cache controlled chunk
    if (p.moveobj_ptr())
    {
        unsigned char* pChar  = REINTERPRET_CAST(unsigned char*, p.moveobj_ptr());
        unsigned char* pStart = REINTERPRET_CAST(unsigned char*, acv.a_sysdir.ci_cache_p);
        int maxLoop = 100000;
        while (pStart)
        {
           --maxLoop;
           if (0 == maxLoop)
           {
               return false;
           }
           int chunkSize = REINTERPRET_CAST(tak10cache_header*, pStart)->ch_size;
           if ((pChar > pStart) && (pChar < pStart + chunkSize))
           {
               return true;
           }
           else
           {
               pStart = REINTERPRET_CAST(unsigned char*, 
                   REINTERPRET_CAST(tak10cache_header*, pStart)->ch_next_cache);
           }
        }
        return false;
    }
    return true;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
