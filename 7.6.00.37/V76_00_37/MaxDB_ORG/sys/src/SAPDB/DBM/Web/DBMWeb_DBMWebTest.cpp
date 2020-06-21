/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWebTeste.cpp
  -----------------------------------------------------------------------------
  responsible:  BerndV
  special area: Database Manager WebAgent Extension
  description:  "Test" functions of "the" DBMWeb Class
  version:      7.3 and higher 
  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------


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




*/

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <stdlib.h>

#include "ToolsCommon/Tools_TemplateSimpleTable.hpp"
#include "DBM/Web/DBMWeb_DBMWeb.hpp"
#include "DBM/Web/DBMWeb_Web.hpp"

#include "hsp77.h"

/*
  -----------------------------------------------------------------------------
  private macros
  -----------------------------------------------------------------------------
 */
#define WA_TRACE(s)       wa.WriteLogMsg(s)
#define TXT_EXPIRE        "Sat, 01 Jan 1990 00:00:00 GMT"

/*    
  =============================================================================
  class:        DBMWeb_DBMWeb
  =============================================================================
 */

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: internTest
  -----------------------------------------------------------------------------
 */
/*
bool DBMWeb_DBMWeb :: internTest (  sapdbwa_WebAgent      &wa,
                                    sapdbwa_HttpRequest   &request,
                                    sapdbwa_HttpReply     &reply )
{
  Tools_TemplateSimpleTable aTable(wa);


  aTable.setHeadLine("Hallo Bernd!");
  aTable.setTableTitle("Das ist eine Tabelle!");
  aTable.setColumnTitle("Spalte1", 1);
  aTable.setColumnTitle("Spalte2", 2);
  
  aTable.setCell("Zelle 1 1", 1, 1);
  aTable.setCell("Zelle 1 2", 1, 2);
  aTable.setCell("Zelle 1 3", 1, 3);
  aTable.setCell("Zelle 2 1", 2, 1);
  aTable.setCell("Zelle 2 2", 2, 2);
  aTable.setCell("Zelle 3 1", 3, 1);
  aTable.setCell("Zelle 3 2", 3, 2);
  aTable.setCell("Zelle 3 3", 3, 3);

  aTable.setColumnSummary("Summe1", 1);
  aTable.setColumnSummary("Summe2", 2);

  aTable.addButton("tilli", "http://pwww");
  aTable.addButton("willi", "http://p26234/webquery");

  reply.InitHeader(sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL);
  reply.SetHeader ("Expires", TXT_EXPIRE);
  reply.SendHeader();

  aTable.writePage(Tools_TemplateWriterWA(reply));

  return true;

} // end DBMWeb_DBMWeb :: internTest
*/

/*
bool DBMWeb_DBMWeb :: internTest (  sapdbwa_WebAgent      &wa,
                                    sapdbwa_HttpRequest   &request,
                                    sapdbwa_HttpReply     &reply )
{
  reply.InitHeader(301, "text/html", NULL, NULL, NULL, NULL);
  reply.SetHeader ("Location", "http://pwww.bea.sap-ag.de/");
  reply.SendHeader();

  return true;

} // end DBMWeb_DBMWeb :: internTest
*/

/*
#define RUNNING "<html><body><h2>Backup is running</h2></body></html>"
#define READY   "<html><body><h2>Backup is ready</h2></body></html>"
#define STATE1  "<META HTTP-EQUIV=\"Refresh\" CONTENT=\"1\"><html><body><h1>Backup State</h1></body></html>"
#define STATE2  "<META HTTP-EQUIV=\"Refresh\" CONTENT=\"1\"><html><body><h2>Backup State</h2></body></html>"
#define STATE3  "<html><body><h2>Backup Ready</h2></body></html>"

static int nState = 1;

bool DBMWeb_DBMWeb :: internTest (  sapdbwa_WebAgent      &wa,
                                    sapdbwa_HttpRequest   &request,
                                    sapdbwa_HttpReply     &reply )
{
  DBMCli_String sAction;

  GetParameterValue(PARAM_ACTION, request, sAction);

  if( sAction == "START") {
    reply.InitHeader(301, "text/html", NULL, NULL, NULL, NULL);
    reply.SetHeader ("Location", "/WARoot/HTML/DBMBackupStateFrame.htm");
    reply.SendHeader();
  } else if( sAction == "RUN") {

    // Header
    reply.InitHeader(sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL);
    reply.SetHeader ("Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
    reply.SendHeader();

    // Page
    reply.SendBody(RUNNING, strlen(RUNNING));

    // Simulate Backup
    sqlsleep(30);

    // Header
    reply.InitHeader(sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL);
    reply.SetHeader ("Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
    reply.SendHeader();

    // Page
    reply.SendBody(READY, strlen(READY));

    nState = 3;

  } else if( sAction == "STATE") {

    // Header
    reply.InitHeader(sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL);
    reply.SetHeader ("Expires",    "Sat, 01 Jan 1990 00:00:00 GMT");
    reply.SendHeader();

    // Page
    if (nState == 1) {
      reply.SendBody(STATE1, strlen(STATE1));
      nState = 2;
    } else if (nState == 2) {
      reply.SendBody(STATE2, strlen(STATE2));
      nState = 1;
    } else if (nState == 3) {
      reply.SendBody(STATE3, strlen(STATE3));
    } // end if

  } else {
    reply.InitHeader(301, "text/html", NULL, NULL, NULL, NULL);
    reply.SetHeader ("Location", "/WARoot/HTML/DBMBackupStart.htm");
    reply.SendHeader();
  } // end if

  return true;

} // end DBMWeb_DBMWeb :: internTest
*/


static void testShowVal( sapdbwa_HttpReply & rep, const char *name, const char *val )
{
  char textBuffer[10000];
  val  = (val  == NULL) ? "NULL" : val;
  name = (name == NULL) ? "NULL" : name;
  //sprintf( textBuffer, "<tr><td>%s&nbsp;</td><td>%s&nbsp;</td></tr>\n", name, val );
  sp77sprintf(textBuffer, 10000, "<tr><td>%s&nbsp;</td><td>%s&nbsp;</td></tr>\n", name, val);
  rep.SendBody( textBuffer, (sapdbwa_UInt4) strlen( textBuffer ) );
} // testShowVal 

static void testShowHeader( sapdbwa_HttpReply    & rep,
                            sapdbwa_HttpRequest  & req,
                            const char           * name)
{
  testShowVal( rep, name, req.GetHeader ( name ));
} // testShowHeader

bool DBMWeb_DBMWeb :: internTest (  sapdbwa_WebAgent      &wa,
                                    sapdbwa_HttpRequest   &request,
                                    sapdbwa_HttpReply     &reply )
{
  reply.InitHeader(sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
  reply.SetHeader ( "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
  reply.SendHeader( );

  reply.SendBody( "<body topmargin=0 leftmargin=0 marginwidth=0 marginheight=0 background=/WARoot/Images/tatami.gif>\n", 0);
  reply.SendBody( "<a href=\"javascript:parent.GotoWebDBMURL(this, 'Event=DBM_INTERN_TEST&Action=REFRESH')\">Test</a>", 0 );
  reply.SendBody( "<table style=\"font-family:courier new,monospace; font-size:8pt;\" border=1 cellspacing=0 cellpadding=1>\n", 0 );

  // special values
  testShowVal( reply, "sapdbwa_GetRequestURI"     ,  request.GetRequestURI      ( ));
  testShowVal( reply, "sapdbwa_GetIfModifiedSince",  request.GetIfModifiedSince ( ));
  testShowVal( reply, "sapdbwa_GetQueryString"    ,  request.GetQueryString     ( ));
  testShowVal( reply, "sapdbwa_GetPathInfo"       ,  request.GetPathInfo        ( ));
  testShowVal( reply, "sapdbwa_GetMethod"         ,  request.GetMethod          ( ));
  testShowVal( reply, "sapdbwa_GetContentType"    ,  request.GetContentType     ( ));
  testShowVal( reply, "sapdbwa_GetContentLength"  ,  request.GetContentLength   ( ));
  testShowVal( reply, "sapdbwa_GetPathTranslated" ,  request.GetPathTranslated  ( ));
  testShowVal( reply, "sapdbwa_GetServerName"     ,  request.GetServerName      ( ));

  // headers
  testShowHeader( reply, request,  "AUTH_TYPE"                );
  testShowHeader( reply, request,  "CONTENT_LENGTH"           );
  testShowHeader( reply, request,  "CONTENT_TYPE"             );
  testShowHeader( reply, request,  "GATEWAY_INTERFACE"        );
  testShowHeader( reply, request,  "HTTP_ACCEPT"              );
  testShowHeader( reply, request,  "PATH_INFO"                );
//  testShowHeader( reply, request,  "PATH_TRANSLATED"          );
  testShowHeader( reply, request,  "QUERY_STRING"             );
  testShowHeader( reply, request,  "REMOTE_ADDR"              );
  testShowHeader( reply, request,  "REMOTE_HOST"              );
  testShowHeader( reply, request,  "REMOTE_USER"              );
  testShowHeader( reply, request,  "REQUEST_METHOD"           );
  testShowHeader( reply, request,  "SCRIPT_NAME"              );
  testShowHeader( reply, request,  "SERVER_NAME"              );
  testShowHeader( reply, request,  "SERVER_PORT"              );
  testShowHeader( reply, request,  "SERVER_PROTOCOL"          );
  testShowHeader( reply, request,  "SERVER_SOFTWARE"          );
  testShowHeader( reply, request,  "HTTP_ACCEPT"              );
  testShowHeader( reply, request,  "HTTP_ACCEPT_CHARSET"      );
  testShowHeader( reply, request,  "HTTP_ACCEPT_ENCODING"     );
  testShowHeader( reply, request,  "HTTP_ACCEPT_LANGUAGE"     );
  testShowHeader( reply, request,  "HTTP_ACCEPT_RANGES"       );
  testShowHeader( reply, request,  "HTTP_AGE"                 );
  testShowHeader( reply, request,  "HTTP_ALLOW"               );
  testShowHeader( reply, request,  "HTTP_AUTHORIZATION"       );
  testShowHeader( reply, request,  "HTTP_CACHE_CONTROL"       );
  testShowHeader( reply, request,  "HTTP_CONNECTION"          );
  testShowHeader( reply, request,  "HTTP_CONTENT_ENCODING"    );
  testShowHeader( reply, request,  "HTTP_CONTENT_LANGUAGE"    );
  testShowHeader( reply, request,  "HTTP_CONTENT_LENGTH"      );
  testShowHeader( reply, request,  "HTTP_CONTENT_LOCATION"    );
  testShowHeader( reply, request,  "HTTP_CONTENT_MD5"         );
  testShowHeader( reply, request,  "HTTP_CONTENT_RANGE"       );
  testShowHeader( reply, request,  "HTTP_CONTENT_TYPE"        );
  testShowHeader( reply, request,  "HTTP_DATE"                );
  testShowHeader( reply, request,  "HTTP_ETAG"                );
  testShowHeader( reply, request,  "HTTP_EXPECT"              );
  testShowHeader( reply, request,  "HTTP_EXPIRES"             );
  testShowHeader( reply, request,  "HTTP_FROM"                );
  testShowHeader( reply, request,  "HTTP_HOST"                );
  testShowHeader( reply, request,  "HTTP_IF_MATCH"            );
  testShowHeader( reply, request,  "HTTP_IF_MODIFIED_SINCE"   );
  testShowHeader( reply, request,  "HTTP_IF_NONE_MATCH"       );
  testShowHeader( reply, request,  "HTTP_IF_RANGE"            );
  testShowHeader( reply, request,  "HTTP_IF_UNMODIFIED_SINCE" );
  testShowHeader( reply, request,  "HTTP_LAST_MODIFIED"       );
  testShowHeader( reply, request,  "HTTP_LOCATION"            );
  testShowHeader( reply, request,  "HTTP_MAX_FORWARDS"        );
  testShowHeader( reply, request,  "HTTP_PRAGMA"              );
  testShowHeader( reply, request,  "HTTP_PROXY_AUTHENTICATE"  );
  testShowHeader( reply, request,  "HTTP_PROXY_AUTHORIZATION" );
  testShowHeader( reply, request,  "HTTP_RANGE"               );
  testShowHeader( reply, request,  "HTTP_REFERER"             );
  testShowHeader( reply, request,  "HTTP_RETRY_AFTER"         );
  testShowHeader( reply, request,  "HTTP_SERVER"              );
  testShowHeader( reply, request,  "HTTP_TE"                  );
  testShowHeader( reply, request,  "HTTP_TRAILER"             );
  testShowHeader( reply, request,  "HTTP_TRANSFER_ENCODING"   );
  testShowHeader( reply, request,  "HTTP_UPGRADE"             );
  testShowHeader( reply, request,  "HTTP_USER_AGENT"          );
  testShowHeader( reply, request,  "HTTP_VARY"                );
  testShowHeader( reply, request,  "HTTP_VIA"                 );
  testShowHeader( reply, request,  "HTTP_WARNING"             );
  testShowHeader( reply, request,  "HTTP_WWW_AUTHENTICATE"    );
  testShowHeader( reply, request,  "HTTP_COOKIE"              );
  testShowHeader( reply, request,  "HTTP_SESSION_ID"          );

  // parameters
  sapdbwa_StringSeq params; 
  sapdbwa_StringSeq values; 
  int nParamIndex=0;
  int nValueIndex=0;

  if (request.GetParameterNames(params) == sapdbwa_True) {
    for (nParamIndex = 0; nParamIndex < params.GetNumElem(); ++nParamIndex) {
      if (request.GetParameterValues(params[nParamIndex], values) == sapdbwa_True) {
        for (nValueIndex = 0; nValueIndex < values.GetNumElem(); ++nValueIndex) {
          testShowVal( reply, params[nParamIndex], values[nValueIndex]);
        } // end for
      } // end if
    } // end for
  } // end if

  reply.SendBody( "</table>\n", 0 );
  reply.SendBody( "</body>\n", 0 );

  return true;
} // end DBMWeb_DBMWeb :: internTest

