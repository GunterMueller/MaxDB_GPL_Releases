/*!**********************************************************************

  module: WAHTTP_Agent.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: SAP DB WWW

  description:  Agent Management

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2001-2005 SAP AG



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





************************************************************************/

#ifndef WAHTTP_AGENT_H
#define WAHTTP_AGENT_H

/****************************************************************************
 ***
 *** Includes
 ***
 ****************************************************************************/

#include "SAPDB/WebAgent/HTTP/WAHTTP_Misc.h"

/****************************************************************************
 ***
 *** 
 ***
 ****************************************************************************/

#define AGENT_SET_COOKIE		0
#define AGENT_VERSION			1
#define AGENT_EXPIRES			2
#define AGENT_CONTENT_TYPE		3
#define AGENT_CONTENT_LENGTH		4
#define AGENT_LAST_MODIFIED		5
#define AGENT_LOCATION			6
#define AGENT_DATE			7
#define AGENT_IF_MODIFIED_SINCE		8
#define AGENT_SERVER			9
#define	AGENT_CONNECTION		10

#define	AGENT_HOST			11
#define	AGENT_PORT			12

#define AGENT_STATUS_OK			0
#define AGENT_STATUS_NOT_MODIFIED	1
#define AGENT_STATUS_BAD_REQUEST	2
#define AGENT_STATUS_NOT_FOUND		3
#define AGENT_STATUS_LENGTH_REQUIRED	4

#define AGENT_STATUS_CREATED		5
#define AGENT_STATUS_NO_CONTENT		6
#define AGENT_STATUS_MOVE_PERM		7
#define AGENT_STATUS_REDIRECT		8
#define AGENT_STATUS_UNAUTHORIZED	9
#define AGENT_STATUS_FORBIDDEN		10
#define AGENT_STATUS_METHOD_NOT_ALLOWED	11
#define AGENT_STATUS_CONFLICT		12
#define AGENT_STATUS_SERVER_ERROR	13
#define AGENT_STATUS_NOT_IMPLEMENTED	14
/* additional states for WebDAV */
#define AGENT_STATUS_MULTISTATUS			15
#define AGENT_STATUS_PRECONDITION_FAILED	16	
#define AGENT_STATUS_UNSUPPORTED_MEDIA_TYPE	17
#define AGENT_STATUS_LOCKED					18
#define AGENT_STATUS_BAD_GATEWAY			19
#define AGENT_STATUS_INSUFFICENT_STORAGE	20
#define AGENT_STATUS_PARTIAL_CONTENT		21	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
#define AGENT_STATUS_RANGE_NOT_SATISFIABLE	22	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */

#ifndef THIS_SERVER
#define THIS_SERVER		"SAP-Internet-SapDb-Server/1.0"
#endif

typedef  struct st_agent_rep	*http_agent_dummy1;
typedef  struct st_agent_req	*http_agent_dummy2;

/* Server description struct */
struct st_agent_server {
    int     InitOk;
    int     HeaderWritten;
    char    iniFile[MAX_FILENAME_LEN+1];

    /* functions ... */
    int (*AddHeader_char)( struct st_agent_rep  *rep,
                           int                   Header,
                           char                 *Value );

    int (*AddHeaderByName_char)( struct st_agent_rep    *rep,
                                 char                   *Header,
                                 char                   *Value );

    int (*AddHeader_size_t)( struct st_agent_rep    *rep,
                             int                     Header,
                             size_t                  Value );

    int (*AddHeader_date)( struct st_agent_rep      *rep,
                           int                       Header,
                           time_t                    Value );

    int (*SendHeader)( struct st_agent_rep  *rep,
                       int                   Status );

    size_t (*Send)( struct st_agent_rep *rep,
                    char                *b,
                    size_t               n );

    char *(*GetHeaderValue)( struct st_agent_req    *port80_req,
                             int                     Header );

    char *(*GetHeaderValueByName)( struct st_agent_req  *req,
                                   char                 *Header );

    int (*GetHeaderNo)( struct st_agent_req  *req,
                        size_t                HeaderNo,
                        char                **HeaderName,
                        char                **HeaderValue );

    size_t (*Receive)( struct st_agent_req  *req,
                       char                 *b,
                       size_t                n );
};

struct st_agent_req
{
 struct st_http_req	*ServerReq;
 char			*method;
 char			*uri;
 char			*version;
 char			*params;
 char			*querystring;
};

struct st_agent_rep
{
 struct st_http_client	*ServerRep;
};

enum en_af_returncode
{
 af_error = 0,
 af_ignored,
 af_accepted
};

int InitAgents( char *iniFile );

int TryAgents( struct st_http_client    *client,
               struct st_http_req       *HttpReq,
               int                      *Persistent );

int ExitAgents( void );

/****************************************************************************
 ***
 *** End
 ***
 ****************************************************************************/

#endif