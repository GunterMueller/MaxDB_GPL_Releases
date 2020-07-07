/*!
  @file           ven73.c
  @author         JoergM
  @brief          Kernel RunTime: Queue Handling
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

#define MOD__ "ven73.c:"

/* exported functions */

#include "gen00.h"
#include "heo00.h"
#include "geo50_0.h"
#include "gen500.h"
#include "gen73.h"

/*
 *============================================================================
 */

#undef MF__
#define MF__ MOD__"e73_dev_ioc_queue_init"
void e73_dev_ioc_queue_init ( 
struct   IOC_QUEUE             * ioc ,
tsp00_Int4                         number )
{
    int                             i ;
    struct  IOC_QUEUE             * ioc1 ;
    
    
    DBG1 (( MF__,"called,    ioc     0x%lx \n", ioc  ));
    DBG1 (( MF__,"           number  %ld   \n", number ));
    
    SAPDB_memset ( ioc , 0 , sizeof(struct IOC_QUEUE) * number );
    
    /*
    * create a circular list !!
    */
    for ( ioc1 = ioc , i = 0 ; i < number ; i ++ )
    {
        ioc1->next     = ioc1 + 1;
        ioc1->previous = ioc1 - 1;
        ioc1 ++ ;
    }
    
    ioc1 -- ;
    ioc->previous = ioc1 ;
    ioc1->next    = ioc ;
    
    DBGOUT;
}

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"e73_xxx_queue_init"
void e73_xxx_queue_init ( 
struct  XXX_QUEUE_HEAD        * xxx ,
int     nr_of_elem )
{
    int                             i ;
    struct  XXX_QUEUE             * xxx1 ;
    
    DBG1 (( MF__,"called,    xxx 0x%lx \n", xxx  ));
    SAPDB_memset ( xxx->element , 0 , sizeof(struct XXX_QUEUE) * nr_of_elem );
    /*
    * create a circular list !!
    */
    
    for ( xxx1 = & xxx->element [ 0 ] , i = 0 ; i < nr_of_elem ; i ++ )
    {
        xxx1->next = xxx1 + 1 ;
        xxx1 ++ ;
    }
    
    xxx1 -- ;
    xxx1->next      = & xxx->element [ 0 ];
    xxx->xx_work    = & xxx->element [ 0 ];
    xxx->xx_request = & xxx->element [ 0 ];
    xxx->num_overflows = 0;
    INIT_LOCK (xxx->exclusive) ;
    DBGOUT;
}

/*
 *============================================================================
 */
#undef MF__
#define MF__ MOD__"e73_ioc_queue_init"
void e73_ioc_queue_init ( 
struct  IOC_QUEUE_HEAD        * head ,
struct  IOC_QUEUE             * ioc ,
tsp00_Int4                        number )
{
    int                             i ;
    struct  IOC_QUEUE             * ioc1 ;
    
    DBG1 (( MF__,"called, head    0x%lx \n", head ));
    DBG1 (( MF__,"        ioc     0x%lx \n", ioc  ));
    DBG1 (( MF__,"        number  %d    \n", number ));
    
    ioc1 = ioc ;
    for ( i = 0 ; i < number ; i ++ )
    {
        ioc1->request = NULL ;
        ioc1->next    = ioc1 + 1 ;
        ioc1->previous= ioc1 - 1;
        ioc1 ++ ;
    }
    
    ioc1 -- ;
    ioc->previous   = ioc1 ;
    ioc1->next      = ioc ;
    
    head->first     = ioc ;
    head->last      = ioc ;
    INIT_LOCK (head->exclusive) ;
#ifdef DEBUG_RTE
    ioc1 = ioc + ( number - 1 );
    DBG1 (( MF__,"Last element %d ioc_last 0x%lx previous 0x%lx next 0x%lx\n", number, ioc1, ioc1->previous, ioc1->next  ));
#endif /* DEBUG_RTE */    
    DBGOUT;
}

/*
 *============================================================================
 */

#undef MF__
#define MF__ MOD__"e73_ior_queue_init"
void e73_ior_queue_init ( 
struct  IOR_QUEUE_HEAD        * head ,
struct  IOR_QUEUE             * ior ,
tsp00_Int4                        number )
{
    int                             i ;
    struct  IOR_QUEUE             * ior1 ;
    
    DBG1 (( MF__,"called, head    0x%lx \n", head ));
    DBG1 (( MF__,"        ior     0x%lx \n", ior  ));
    DBG1 (( MF__,"        number  %d    \n", number ));
    
    ior1 = ior ;
    for ( i = 0 ; i < number ; i ++ )
    {
        ior1->request = NULL ;
        ior1->next    = ior1 + 1 ;
        ior1->previous= ior1 - 1;
        ior1 ++ ;
    }
    
    ior1 -- ;
    ior->previous   = ior1 ;
    ior1->next      = ior ;
    
    head->first     = ior ;
    head->last      = ior ;
    
    DBGOUT;
}

/*
 *============================================================================
 */
#undef MF__
#define MF__ MOD__"e73_ioc_enqu"
int e73_ioc_enqu ( 
struct  IOC_QUEUE_HEAD        * head ,
struct  DOUBLY_LINKED         * new_entry )
{
	int     enqu_ok ;

    DBG1 (( MF__,"head                 0x%lx \n", head ));
	DBG1 (( MF__,"head->first          0x%lx \n", head->first ));
	DBG1 (( MF__,"head->first->request 0x%lx \n", head->first->request ));
	DBG1 (( MF__,"new_entry            0x%lx \n", new_entry ));

	WAIT_UNTIL_ALONE ( head->exclusive );
	/*
	 * we are alone now
	 */
	if ( (enqu_ok = head->first->request == 0) ) 
	{
	    head->first->request = new_entry ;
		head->first          = head->first->next ;
	}
	CLEARLOCK ( head->exclusive );

	DBGOUT;
	return ( enqu_ok ) ;
}

/*
 *============================================================================
 */

#undef MF__
#define MF__ MOD__"e73_dl_init"
void e73_dl_init ( 
struct  DOUBLE_Q_HEAD         * head ,
char                          * name )
{
	DBGIN;
	head->first     = 0 ;
	head->last      = 0 ;
	INIT_LOCK (head->dq_exclusive) ;
	(void) strncpy ( head->name , name , sizeof(head->name)-1 );
	DBGOUT;
}

