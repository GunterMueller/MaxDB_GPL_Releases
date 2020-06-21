/*!
  @file           ven34.c
  @author         JoergM
  @brief          Client RunTime: Map_Kgs
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



\endif
*/



#define MOD__ "ven34.c:"

/* exported functions */

#include "gen00.h"
#include "geo007_1.h"
#include "geo50_0.h"
#include "gen34.h"
#include "hen40.h"

/* imported functions */

#define ERR_MAP_KGS_SHMAT_PERM         11110,ERR_TYPE,"KGS     ","Cannot attach to Kernel Shared Memory Segment: Permission denied"
#define ERR_MAP_KGS_SHMAT              11208,ERR_TYPE,"KGS     ","Cannot attach to Kernel Shared Memory Segment: %s"
#define ERR_MAP_KGS_SHMAT_RDONLY_PERM  11110,ERR_TYPE,"KGS     ","Cannot attach to Kernel Shared Memory Segment (RO): Permission denied"
#define ERR_MAP_KGS_SHMAT_RDONLY       11208,ERR_TYPE,"KGS     ","Cannot attach to Kernel Shared Memory Segment (RO): %s"
#define WRN_MAP_KGS_SHMDT_FIRST_TRY    11208,WRN_TYPE,"KGS     ","shmdt (preliminary) error, %s"
#if defined(HP9)
#define WRN_MAP_KGS_SHMDT_PREVIOUSLY   11208,WRN_TYPE,"KGS     ","shmdt (previously) error, %s"
#endif
#define ERR_MAP_KGS_WRONG_ADDR0        11210,ERR_TYPE,"KGS     ","got wrong KGS address 0x%08x"
#define ERR_MAP_KGS_WRONG_ADDR1        11210,ERR_TYPE,"KGS     ","wanted    KGS address 0x%08x"
#define ERR_UNMAP_KGS_SHMDT_ARRAY      11210,ERR_TYPE,"KGS     ","Kernel Shared Memory Segment id %d shmdt error, %s"
#define ERR_UNMAP_KGS_SHMDT            11210,ERR_TYPE,"KGS     ","Kernel Shared Memory Segment shmdt error, %s"

/* exported data */

struct  KERNEL_GLOBAL_SECTION           * kgs = (struct KERNEL_GLOBAL_SECTION *)0 ;

/*
 * =====================================================================
 */

#undef MF__
#define MF__ MOD__"sql34_map_kgs"
int sql34_map_kgs ( 
int shmid ,
int shmflag )
{
int                             flag , rc, num_kgs_shms ;
struct SHARED_SEGMENT           * addr , * naddr ;

	DBG1 (( MF__,"called,    shmid   %d \n", shmid ));
	DBG1 (( MF__,"           shmflag 0%o \n", shmflag ));

/*
 *  if shmflag == -1 first try to attach for rdwr, return 0
 *  if this fails, try to attach for rdonly, return 1
 *  if this fails too, return 2
 */
	flag = shmflag ;
	if ( shmflag == -1 ) flag = 0 ; /* 0 == rdwr */

/*
 *  preliminary attach the kernel shared memory segment at any address
 */
	DBG1 (( MF__,"try to attach with flag 0%o \n", flag ));
	addr = (struct SHARED_SEGMENT *) shmat ( shmid , (char*) 0 , flag );
	DBG1 (( MF__,"shmat  addr    0x%08x \n", addr ));
	if ( addr == (struct SHARED_SEGMENT *) UNDEF )
	{
		if ( errno == EPERM || errno == EACCES )
		{
			if ( shmflag != -1 )
			{
				MSGCD (( ERR_MAP_KGS_SHMAT_PERM ));
				return ( 2 );
			}
			flag = SHM_RDONLY ;
			DBG1 (( MF__,"try to attach with flag 0%o \n", flag ));
			addr = (struct SHARED_SEGMENT *) shmat ( shmid , (char*) 0 , flag );
			DBG1 (( MF__,"shmat  addr    0x%08x \n", addr ));
			if ( addr == (struct SHARED_SEGMENT *) UNDEF )
			{
				if ( errno == EPERM || errno == EACCES )
				{
					MSGCD (( ERR_MAP_KGS_SHMAT_RDONLY_PERM ));
				}
				else
				{
					MSGCD (( ERR_MAP_KGS_SHMAT, sqlerrs() ));
				}
				return ( 2 );
			}
		}
		else
		{
			MSGCD (( ERR_MAP_KGS_SHMAT, sqlerrs() ));
			return ( 2 );
		}
	}
	DBG1 (( MF__,"got addr       0x%08x \n", addr ));

/*
 *  get the correct address for the kernel shared memory segment
 */
	naddr = addr->this_seg ;
	DBG1 (( MF__,"now want naddr 0x%08x \n", naddr ));

	if ( addr != naddr )
	{
		/*
		 *  detach the preliminary attached shared memory segment
		 */
		rc = shmdt ( (char *) addr );
		if ( rc <  0 )
		{
			MSGD (( WRN_MAP_KGS_SHMDT_FIRST_TRY, sqlerrs() ));
		}
		else
		{
			DBG1 (( MF__,"detached addr  0x%08x \n", addr ));
		}

		/*
		 * try to detach an eventually previously attached shared memory segment
		 */
#   if HP9
		rc = shmdt ( (char *) naddr );
		if ( rc <  0 )
		{
			MSGD (( WRN_MAP_KGS_SHMDT_PREVIOUSLY, sqlerrs() ));
		}
		else
		{
			DBG1 (( MF__,"detached addr  0x%08x \n", naddr ));
		}
#   endif

		/*
		 *  attach the kernel shared memory segment at the correct address
		 */
		addr = (struct SHARED_SEGMENT *) shmat ( shmid , (char *) naddr , flag );
		DBG1 (( MF__,"sql34_map_kgs: shmat now addr 0x%08x \n", addr ))
		if ( addr == (struct SHARED_SEGMENT *) UNDEF )
		{
			if ( errno == EPERM || errno == EACCES )
			{
				MSGCD (( ERR_MAP_KGS_SHMAT_RDONLY_PERM ));
				return ( 2 );
			}
			else
			{
				MSGCD (( ERR_MAP_KGS_SHMAT, sqlerrs() ));
				return ( 2 );
			}
		}
	}

	if ( naddr != addr )
    {
		MSGCD (( ERR_MAP_KGS_WRONG_ADDR0, addr ));
		MSGCD (( ERR_MAP_KGS_WRONG_ADDR1, naddr ));
		(void) shmdt ( (char *) addr );
		return ( 2 );
    }

#if ULT || UTS
	if ( addr->kgs != KGS )
    {
		DBG1 (( MF__,"SYSERR: got wrong kgs address 0x%08x \n", addr->kgs ));
		return ( 2 );
    }
#endif

	kgs = (struct KERNEL_GLOBAL_SECTION *) addr->kgs ;
	rc = 0 ;
	if ( shmflag == -1 && flag == SHM_RDONLY )
	{
		DBG1 (( MF__,"could not get write permission \n" ));
		rc = 1 ;
	}
	DBG1 (( MF__,"access flag is %#o \n", flag ));

/* 
 * Since the array of kgs shmids is in the first chunk of shared memory,
 * we can now attach the rest of the chunks.
 */

#ifdef MAP_KGS_SHMID_ARRAY
/* F.H. warum? alle LZU-relevanten Informationen stehen im Ersten Segment ! */
	num_kgs_shms = 1 ;

/* PTS 1000080 */
	if ( kgs->datacache_bot ) num_kgs_shms++ ; 
		/* IGNORE EXTRA SEGMENT FOR DATA_CACHE */
	if ( kgs->convcache_bot ) num_kgs_shms++ ; 
		/* IGNORE EXTRA SEGMENT FOR CONV_CACHE */

	while ( (shmid = kgs-> kgsshmid [ num_kgs_shms ]) != -1 ) 
	{
		naddr = (struct SHARED_SEGMENT *)((char *)addr + kgs->kgs_shm_part_len);
		addr = (struct SHARED_SEGMENT *) shmat ( shmid , (char*) naddr , flag );
		if ( addr == (struct SHARED_SEGMENT *) UNDEF )
		{
			MSGCD (( ERR_MAP_KGS_SHMAT, sqlerrs() ));
			return ( 2 );
		}
		else
		{
			num_kgs_shms ++ ;
			DBG1 (( MF__,"attached shmid %d \n", shmid ));
			DBG1 (( MF__,"shmat add %4d "FMTA" \n", num_kgs_shms, addr ));
		}
	}
#endif

	DBG1 (( MF__,"returning %d \n", rc ));
	return ( rc );
}

/*
 * =====================================================================
 */
#undef MF__
#define MF__ MOD__"sql34_unmap_kgs"
int     sql34_unmap_kgs () 
{
int num_kgs_shms = 1 ;
char      * addr ;

    if ( !kgs )
    {
        /* already unmapped */
        return (0);
    }

#ifdef MAP_KGS_SHMID_ARRAY
/* F.H. warum? alle LZU-relevanten Informationen stehen im Ersten Segment ! */
/* PTS 1000080 */
	if ( kgs->datacache_bot ) num_kgs_shms++ ; /* IGNORE EXTRA SEGMENT FOR DATA_CACHE */
	if ( kgs->convcache_bot ) num_kgs_shms++ ; /* IGNORE EXTRA SEGMENT FOR CONV_CACHE */

	addr = (char *)kgs->seg + kgs->kgs_shm_part_len;
	while ( kgs->kgsshmid [ num_kgs_shms ] != -1 ) 
	{ 
		DBG1 (( MF__,"attached shmid %d \n", 
								 kgs->kgsshmid [ num_kgs_shms ] ));
		DBG1 (( MF__,"shmdt addr %4d "FMTA" \n", num_kgs_shms, addr ));
		if ( shmdt ( addr ) < 0 )    
		{
			MSGD(( ERR_UNMAP_KGS_SHMDT_ARRAY, 
				kgs->kgsshmid [ num_kgs_shms ], sqlerrs() ));
			return ( 2 );
		}
		else
		{
			num_kgs_shms ++ ;
			addr += kgs->kgs_shm_part_len;
		}
	}
#endif
	DBG1 (( MF__,"attached shmid %d \n", kgs->kgsshmid [ 0 ] ));
	DBG1 (( MF__,"shmdt addr %4d "FMTA" \n", 0, kgs->seg ));
	if ( shmdt ( (char *)  kgs->seg ) < 0 )    
    {
        MSGD (( ERR_UNMAP_KGS_SHMDT, sqlerrs() ));
        return ( 2 );
    }
    return(0) ;
}

#undef MF__
#define MF__ MOD__"sql34MapKgsNotRemapped"
struct  KERNEL_GLOBAL_SECTION * sql34MapKgsNotRemapped ( 
int shmid,
int writeAccess )
{
struct SHARED_SEGMENT           * ShmAddr;
unsigned long                     KgsOffset;

	DBG1 (( MF__,"called,    shmid   %d \n", shmid ));

    ShmAddr = (struct SHARED_SEGMENT *) shmat ( shmid , (char*) 0 , (writeAccess ? 0 : SHM_RDONLY) );
	DBG1 (( MF__,"shmat  ShmAddr    0x%lx \n", ShmAddr ));
	if ( ShmAddr == (struct SHARED_SEGMENT *) UNDEF )
	{
		if ( errno == EPERM || errno == EACCES )
		{
		    MSGCD (( ERR_MAP_KGS_SHMAT_RDONLY_PERM ));
		}
		else
		{
			MSGCD (( ERR_MAP_KGS_SHMAT, sqlerrs() ));
		}
		return ( NULL );
	}

    KgsOffset = ALIGN_EO00(sizeof(struct SHARED_SEGMENT), sizeof(void *));

	DBG1 (( MF__,"Returning not remapped KGS at 0x%lx\n", ((char *)ShmAddr)+KgsOffset ));

    DBGOUT;

    return ((struct KERNEL_GLOBAL_SECTION *)( ((char *)ShmAddr)+KgsOffset ));
}

#undef MF__
#define MF__ MOD__"sql34UnmapKgsNotRemapped"
void sql34UnmapKgsNotRemapped (struct  KERNEL_GLOBAL_SECTION *KgsNotRemapped)
{
    unsigned long KgsOffset;

    DBGIN;

    KgsOffset = ALIGN_EO00(sizeof(struct SHARED_SEGMENT), sizeof(void *));

	if ( shmdt ( ((char *)KgsNotRemapped) - KgsOffset ) < 0 )    
    {
        MSGD (( ERR_UNMAP_KGS_SHMDT, sqlerrs() ));
    }

    DBGOUT;
}
