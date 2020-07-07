/*!
  @file           ven46.c
  @author         JoergM
  @brief          RunTime: Dump
  @see            


  Dumping of Packets and Other Stuff

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

#define MOD__ "ven46.c:"

#include "gen00.h"
#include "vsp001c.h"
#include "gen46.h"
#include "gen500.h"
#include "hen40.h"
#include "gen60.h"

/*
 * ===========================================================================
 */

externC void    sql46_dump ( 
char                          * s ,
int                             n )
{
int                             fd ;

	fd = open ( "DUMPFILE" , O_WRONLY|O_APPEND|O_CREAT , 0666 );
	if ( fd < 0 )
	{
		MSGCD (( WRN_DUMPFILE_OPEN_FAILED, sqlerrs() ));
		return ;
	}
	(void) write ( fd , s , (unsigned) n );
	(void) close ( fd );
}

/*
 * ===========================================================================
 */

externC  void    sql46_dump_packet ( 
rte_header		*hdr )
{
sqldbgwrite ( "dump: act_send_len      %d \n",  hdr->rh_act_send_len        );
sqldbgwrite ( "dump: max_send_len      %d \n",  hdr->rh_max_send_len        );
sqldbgwrite ( "dump: protocol_id       %d \n",  hdr->rh_protocol_id         );
sqldbgwrite ( "dump: mess_class        %d \n",  hdr->rh_mess_class          );
sqldbgwrite ( "dump: rte_flags         %d \n",  hdr->rh_rte_flags           );
sqldbgwrite ( "dump: residual_packets  %d \n",  hdr->rh_residual_packets    );
sqldbgwrite ( "dump: sender_ref        %d \n",  hdr->rh_sender_ref          );
sqldbgwrite ( "dump: receiver_ref      %d \n",  hdr->rh_receiver_ref        );
sqldbgwrite ( "dump: rte_return_code   %d \n",  hdr->rh_rte_return_code     );
sqldbgwrite ( "dump: rh_new_swap_type  %d \n",  hdr->rh_new_swap_type       );
sqldbgwrite ( "dump: filler1           %d \n",  hdr->rh_filler1             );
}

/*
 * ===========================================================================
 */

externC	char	* sql46hexstr ( 
char	   * buf ,
int			 lgt,
char       * line )
{
	register int		i ;
	register char		* p ;
	register char		* q ;

	i = lgt ;
	if ( i > (sizeof(line)/3)-1 ) i = (sizeof(line)/3)-1 ;
	for ( p = buf , q = line ; i > 0 ; i -- , p ++ )
	{
	    (void) sprintf ( q , "%02x " , * p & 0xff );
	    q += 3 ;
	}

	return ( line );
}
