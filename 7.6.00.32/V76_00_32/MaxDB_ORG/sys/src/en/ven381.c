/*!
  @file           ven381.c
  @author         JoergM
  @brief          RunTime: Enum_to_String
  @see            


  Convert integer codes to printable strings

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

#define MOD__ "ven381.c:"
#include "gen00.h"
#include "geo002.h"
#include "gen50.h"
#include "gen38.h"
#include "RunTime/RTE_ConsoleStandardOutput.h"


/*==========================================================================*/


char * e38_dbstate_name ( UINT1 state )
{
    switch ( state )
	{
		case SERVER_STARTING :          return ( XCONS_SHOW_STATE_STARTING  );  
        case SERVER_COLD :              return ( XCONS_SHOW_STATE_COLD      );
        case SERVER_STANDBY_EO00 :      return ( XCONS_SHOW_STATE_STANDBY   );
	    case SERVER_WARM :              return ( XCONS_SHOW_STATE_WARM      );
		case SERVER_SHUTDOWN :          return ( XCONS_SHOW_STATE_SHTDWN    );
		case SERVER_SHUTDOWNREINIT :    return ( XCONS_SHOW_STATE_SHTDWNREI );
		case SERVER_SHUTDOWNKILL :      return ( XCONS_SHOW_STATE_SHTDWNKIL );
	    case SERVER_STOP :              return ( XCONS_SHOW_STATE_STOP      );
		case SERVER_KILL :              return ( XCONS_SHOW_STATE_KILL      );
		default:                        return ( XCONS_SHOW_STATE_UNKNOWN   );

	}
}

/*==========================================================================*/

char    * e38_type_name ( UINT1 type )
{
    switch ( type )
    {
    case TT_TI_EO00:     return ( "Timer   " );
    case TT_TL_EO00:     return ( "TransLog" );
    case TT_AL_EO00:     return ( "ALogWr. " );
    case TT_TW_EO00:     return ( "TraceWr." );
    case TT_DW_EO00:     return ( "Pager   " );
    case TT_UT_EO00:     return ( "Utility " );
    case TT_EV_EO00:     return ( "Event   " );
    case TT_GC_EO00:     return ( "GarbColl" );
    case TT_BUP_EO00:    return ( "Backup  " );
    case TT_SV_EO00:     return ( "Server  " );
    case TT_US_EO00:     return ( "User    " );
    default:        return ( "UNKNOWN " );
    }
}

/*==========================================================================*/

char    * e38_adv_tskstate_name ( UINT1 state , UINT2 state_reason )
{
  if ( state == TSK_VSUSPEND_EO00 && state_reason > 0 )
    switch ( state_reason )
    {
    case 100:     return ( "No work       " );
    case 101:     return ( "Wait for LW   " );
    case 102:     return ( "Logqueue FULL " );
    default :     return ( "Vsuspend(..)  " );
    }
  else
    return ( e38_tskstate_name ( state ) ) ;
}

/*==========================================================================*/

char    * e38_tskstate_name ( UINT1 state )
{
    switch ( state )
    {
    case TSK_INITIALIZED_EO00:   return ( XCONS_TSK_STATE_INITIALIZE  );
    case TSK_INACTIVE_EO00   :   return ( XCONS_TSK_STATE_INACTIVE    );
	case TSK_MARKED_FOR_STARTUP_EO00: return ( XCONS_TSK_STATE_MARKED_FOR_STRT );
    case TSK_RUNNING_EO00:       return ( XCONS_TSK_STATE_RUNNING     );
    case TSK_CONNECTWAIT_EO00:   return ( XCONS_TSK_STATE_CONNECTWAIT );

    case TSK_VDEVSIZE_EO00:      return ( XCONS_TSK_STATE_VDEVSIZE    );
    case TSK_VATTACH_EO00:       return ( XCONS_TSK_STATE_VATTACH     );

    case TSK_VDETACH_EO00:       return ( XCONS_TSK_STATE_VDETACH     );
    case TSK_VFOPEN_EO00:        return ( XCONS_TSK_STATE_VFOPEN      );
    case TSK_VFWRITE_EO00:       return ( XCONS_TSK_STATE_VFWRITE     );
    case TSK_VFCLOSE_EO00:       return ( XCONS_TSK_STATE_VFCLOSE     );

    case TSK_VRELEASE_EO00:      return ( XCONS_TSK_STATE_VRELEASE    );
    case TSK_VSHUTDOWN_EO00:     return ( XCONS_TSK_STATE_VSHUTDOWN   );
    case TSK_VRECEIVE_EO00:      return ( XCONS_TSK_STATE_VRECEIVE    );
    case TSK_VREPLY_EO00:        return ( XCONS_TSK_STATE_VREPLY      );

    case TSK_VBEGEXCL_EO00:      return ( XCONS_TSK_STATE_VBEGEXCL    );
    case TSK_VENDEXCL_EO00:      return ( XCONS_TSK_STATE_VENDEXCL    );
    case TSK_VSUSPEND_EO00:      return ( XCONS_TSK_STATE_VSUSPEND    );
    case TSK_VWAIT_EO00:         return ( XCONS_TSK_STATE_VWAIT       );
    case TSK_SLEEP_EO00:         return ( XCONS_TSK_STATE_VSLEEP      );

    case TSK_VDIAGINIT_EO00:     return ( XCONS_TSK_STATE_VDIAGINIT   );
    case TSK_VOPMSG_EO00:        return ( XCONS_TSK_STATE_VOPMSG      );
    case TSK_LOCKED_EO00:        return ( XCONS_TSK_STATE_LOCKED      );
    case TSK_TERMINATED_EO00:    return ( XCONS_TSK_STATE_TERMINATED  );
    case TSK_RUNNABLE_EO00:      return ( XCONS_TSK_STATE_RUNNABLE    );

    case TSK_ASYNOPEN_EO00:      return ( XCONS_TSK_STATE_ASYNOPEN    );
    case TSK_ASYNCLOSE_EO00:     return ( XCONS_TSK_STATE_ASYNCLOSE   );
    case TSK_ASYNIO_EO00:        return ( XCONS_TSK_STATE_ASYNIO      );
    case TSK_ASYNWAIT_READ_EO00: return ( XCONS_TSK_STATE_ASYNWAIT_READ );
    case TSK_ASYNWAIT_WRITE_EO00:return ( XCONS_TSK_STATE_ASYNWAIT_WRITE );

    case TSK_IO_READ_EO00:       return ( XCONS_TSK_STATE_IO_READ     );
    case TSK_IO_WRITE_EO00:      return ( XCONS_TSK_STATE_IO_WRITE    );

    case TSK_ASYNCNTL_EO00:       return ( XCONS_TSK_STATE_ASYNCNTL );
    case TSK_DCOM_OBJ_CALLED_EO00:return ( XCONS_TSK_STATE_DCOM_OBJ_CALLED );

    case TSK_ENTERRWREGION_EO00:return ( XCONS_TSK_STATE_VENTERRWREGION );
    case TSK_LEAVERWREGION_EO00:return ( XCONS_TSK_STATE_VLEAVERWREGION );

    case TSK_RESCHEDULE_MSEC_EO00:return ( XCONS_TSK_STATE_RESCHEDULE_MSEC );

	default:                return ( XCONS_TSK_STATE_UNKNOWN     );
    } 
}

/*==========================================================================*/

char * e38_req_name ( UINT1 request )
{
    switch ( request )
    {
    case REQ_VATTACH_EO00:         return ( XCONS_REQ_VATTACH );
    case REQ_VBLOCKIO_EO00:        return ( XCONS_REQ_VBLOCKIO );
    case REQ_VDETACH_EO00:         return ( XCONS_REQ_VDETACH );
    case REQ_INITOK_EO00:          return ( XCONS_REQ_INITOK );
    case REQ_VRESUME_EO00:         return ( XCONS_REQ_VRESUME );
    case REQ_VSIGNAL_EO00:         return ( XCONS_REQ_VSIGNAL );
    case REQ_VSLEEP_EO00:          return ( XCONS_REQ_VSLEEP );
    case REQ_CONNECT_EO00:         return ( XCONS_REQ_CONNECT );
    case REQ_VRECEIVE_EO00:        return ( XCONS_REQ_VRECEIVE );
    case REQ_SHUTDOK_EO00:         return ( XCONS_REQ_SHUTDOK );

    case REQ_BIG_CONNECT_EO00 :     return ( XCONS_REQ_BIG_CONNECT );
    case REQ_ADDRESS_CONNECT_EO00 : return ( XCONS_REQ_ADDRESS_CONNECT );
    case REQ_BIG_ADDR_CONNECT_EO00: return ( XCONS_REQ_BIG_ADDR_CONNECT );
    case REQ_ASYNOPEN_EO00: return ( XCONS_REQ_ASYNOPEN ) ;
    case REQ_ASYNCLOSE_EO00: return ( XCONS_REQ_ASYNCLOSE ) ;
    case REQ_ASYNIO_EO00: return ( XCONS_REQ_ASYNIO ) ;
    case REQ_ASYNWAIT_EO00: return ( XCONS_REQ_ASYNWAIT ) ;
    case REQ_EXCLUSIVE_EO00: return ( XCONS_REQ_EXCLUSIVE ) ;
    case REQ_VNSHUTDOWN_EO00: return ( XCONS_REQ_VNSHUTDOWN ) ;
    case REQ_RESCHEDULE_EO00: return ( XCONS_REQ_RESCHEDULE ) ;
    case REQ_YIELDING_EO00:   return ( XCONS_REQ_YIELDING ) ;

    case REQ_ASYNCNTL_EO00:         return ( XCONS_REQ_ASYNCNTL ) ;
    case REQ_VMARK_BAD_DEV_EO00 :   return ( XCONS_REQ_VMARK_BAD_DEV ) ;
    case REQ_TEST_AND_SET_EO00    : return ( XCONS_REQ_TEST_AND_SET ) ;
    case REQ_IO_FINISHED_EO00:      return ( XCONS_REQ_IO_FINISHED ) ;
    case REQ_VVECTORIO_EO00:      return ( XCONS_REQ_VVECTORIO ) ;
    default:                        return ( XCONS_REQ_TYPE_UNKNOWN ) ;
    }

}

/*==========================================================================*/

char * en38KtStateName ( tsp00_Int4 KtState )
{
    switch ( KtState )
    {
    case KT_INITIALIZED:            return (XCONS_KT_STATE_INITIALIZED);
    case KT_RUNNING:                return (XCONS_KT_STATE_RUNNING);
    case KT_TERMINATING:            return (XCONS_KT_STATE_TERMINATING);
    case KT_TERMINATED:             return (XCONS_KT_STATE_TERMINATED);
    case KT_SLEEPING:               return (XCONS_KT_STATE_SLEEPING);
    case KT_DETACHED:               return (XCONS_KT_STATE_DETACHED);
    case KT_ATTACHED:               return (XCONS_KT_STATE_ATTACHED);
    default:                        return (XCONS_KT_STATE_UNKNOWN);
    }

}
