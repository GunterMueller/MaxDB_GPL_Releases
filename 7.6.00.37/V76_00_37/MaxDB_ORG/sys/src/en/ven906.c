/*!
  @file           ven906.c
  @author         JoergM
  @brief          Client RunTime: CServer communication
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

#define MOD__ "ven906.c:"

#include "gsp00.h"
#include "gen00.h"
#include "gen003.h"
#include "gsp09.h"
#include <unistd.h>
#include "geo001.h"
#include "geo57.h"
#include "RunTime/RTE_UNIXAuthenticate.h"
#include "heo05.h"

/* PTS 1106827 */
    #undef MF__
    #define MF__ MOD__"sqlxchown"
void
sqlxchown (
    const char      *Username,
    tsp00_Pathc     Filename,
    tsp9_rte_xerror *xerror )
{
    SAPDB_Int4 userId;
    SAPDB_Int4 groupId;

    if ( !RTE_GetUserAndGroupIdFromPasswdByName( Username
                                               , &userId
                                               , &groupId ) )
    {
        eo44anyError (xerror, "cannot find user in passwd\n");
    }
    else
    {
        if ( ((SAPDB_Int4)geteuid()) != userId
           && chown(Filename, userId, groupId) != 0 )
        {
            eo44sysError(xerror, errno );
        }
        else
        {
            eo44initError (xerror);
        }
    }
}

/*----------------------------------------*/
    #undef MF__
    #define MF__ MOD__"sqlxidlogon"
void
sqlxidlogon (
    int              userid,
    tsp9_rte_xerror *xerror )
{
    /* superuser must be effectivly superuser before it can overwrite its rela user id... */
    if ( getuid() == 0 )
    {
        setuid(0);
    }

    if ( setregid(getgid(), getgid())
      || setreuid((uid_t)userid, (uid_t)userid) )
    {
        eo44anyError (xerror, "logon failed");
    }
    else
    {
        eo44initError (xerror);
    }
}
