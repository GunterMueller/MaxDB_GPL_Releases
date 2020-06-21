/*!**********************************************************************

  module: vwd15text.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: Text/Template Repository

  description:  Repository for text pieces

  see also:     

  -----------------------------------------------------------------------

  copyright:          Copyright (c) 2000-2005 SAP AG




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




************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "hwd15text.h"

/****************************************************************************
 ***
 ***  Text Repository
 ***
 ***************************************************************************/

const twd15_Text textArray[] = {
/****************************************************************************
 *** ADMIN_JS_REFRESH_NAVIGATION
 ****************************************************************************/
	{0, ADMIN_JS_REFRESH_NAVIGATION, "\
function openNewBrowser( url ) {\n\
	if (url) \n\
		open( url );\n\
}\n\
\n\
function refreshNavigation() {\n\
\n\
  window.frames[1].location.href=\"waadmin.wa?Service=Navigation&Refresh=1\"\n\
\n\
}\n\
\n\
function showStatus( status ) {\n\
\n\
	window.frames[3].document.open(\"text/html\");\n\
\n\
	window.frames[3].document.writeln('<html>');\n\
	window.frames[3].document.writeln('<head>');\n\
	window.frames[3].document.writeln('<title>Status</title>');\n\
	window.frames[3].document.writeln('</head>');\n\
	window.frames[3].document.writeln('<body bgcolor=\"#DEDEC8\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\">');\n\
	window.frames[3].document.writeln(status);\n\
	window.frames[3].document.writeln('</body>');\n\
	window.frames[3].document.writeln('</html>');\n\
\n\
	window.frames[3].document.close();\n\
\n\
}\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_TITLE
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_TITLE, "\
<title>SAP DB - Web Server Administration</title>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_ERROR
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_ERROR, "\
Error occured. See log file!\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_VALUE_UPDATE_CONFIRMATION
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_VALUE_UPDATE_CONFIRMATION, "\
Values updated.\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_RELOAD_SESSION_POOL_FAILED
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_RELOAD_SESSION_POOL_FAILED, "\
Reload of session pool failed.\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_RESTART_WEBSERVER
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_RESTART_WEBSERVER, "\
<html>\n\
<head>\n\
</head>\n\
<body bgcolor=\"#DEDEC8\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\">\n\
Please restart your web server to become the new settings valid.\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SERVICE_CREATE_CONFIRMATION
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_CREATE_CONFIRMATION, "\
Service created.\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_SERVICE_CONFIRMATION
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_SERVICE_CONFIRMATION, "\
Service deleted.\
"},
/*----------------------------------------------------------------------------\
// ADMIN_JS_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_JS_BEGIN, "\
<script language=\"JavaScript\">\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_JS_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_JS_END, "\
</script>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_FRAMESET
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_FRAMESET, "\
<frameset  rows=\"64,*\" framespacing=\"0\" frameborder=\"0\" border=\"0\">\n\
    <frame name=\"Header\" src=\"/WARoot/HTML/WAHeader.htm\" marginwidth=\"0\" marginheight=\"0\" scrolling=\"no\" noresize>\n\
	<frameset  cols=\"20%,80%\">\n\
		<frame name=\"Navigation\" src=\"waadmin.wa?Service=Navigation\" marginwidth=\"10\" marginheight=\"10\" scrolling=\"auto\" frameborder=\"1\">\n\
		<frameset  rows=\"90%,10%\">\n\
			<frame name=\"Details\" src=\"waadmin.wa?Service=InitDetails\" marginwidth=\"10\" marginheight=\"10\" scrolling=\"auto\" frameborder=\"1\">\n\
			<frame name=\"Status\" src=\"waadmin.wa?Service=InitDetails\" marginwidth=\"10\" marginheight=\"10\" scrolling=\"auto\" frameborder=\"1\">\n\
	    </frameset>\n\
    </frameset>\n\
\n\
	<noframes>\n\
	<body topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\">\n\
	<p>This web site uses frames. Your browser doesn't support frames.</p>\n\
	</body>\n\
	</noframes>\n\
</frameset>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_BEGIN, "\
<html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_END, "\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_HEADER_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_HEADER_BEGIN, "\
<head>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_HEADER_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_HEADER_END, "\
</head>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_JS_TREE_SETTINGS
//----------------------------------------------------------------------------*/
	{0, ADMIN_JS_TREE_SETTINGS, "\
parent.Navigations.addNavigation(new parent.navigation(\"Web Server\", \"Web Server\", \"Navigation\", \"waadmin.wa?Service=Pictures&Name=\", \"\", true, true, 7));\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_JS_TREE_CREATE_ROOT_FOLDERS
//----------------------------------------------------------------------------*/
	{0, ADMIN_JS_TREE_CREATE_ROOT_FOLDERS, "\
parent.Navigations.items[0].addNode(new parent.node(\"View Logfile\", \"waadmin.wa?Service=ShowLogFile\", \"Details\"));\n\
parent.Navigations.items[0].addNode(new parent.node(\"View HTTP Requests\", \"waadmin.wa?Service=ShowHTTPRequests\", \"Details\"));\n\
parent.Navigations.items[0].addNode(new parent.node(\"View HTTP Errors\", \"waadmin.wa?Service=ShowHTTPErrors\", \"Details\"));\n\
parent.Navigations.items[0].addNode(new parent.node(\"Global Settings\"));\n\
parent.Navigations.items[0].addNode(new parent.node(\"Session Pools\"));\n\
parent.Navigations.items[0].addNode(new parent.node(\"Services\"));\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_JS_TREE_CREATE_ROOT_COM_FOLDERS
//----------------------------------------------------------------------------*/
	{0, ADMIN_JS_TREE_CREATE_ROOT_COM_FOLDERS, "\
parent.Navigations.items[0].addNode(new parent.node(\"COM Services\"));\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_JS_TREE_ADD_GLOBAL_SETTINGS
//----------------------------------------------------------------------------*/
	{0, ADMIN_JS_TREE_ADD_GLOBAL_SETTINGS, "\
parent.Navigations.items[0].nodes[3].addNode(new parent.node(\"Logs\", \"waadmin.wa?Service=ShowLogSettings\", \"Details\"));\n\
parent.Navigations.items[0].nodes[3].addNode(new parent.node(\"Resources\", \"waadmin.wa?Service=ShowResourceSettings\", \"Details\"));\n\
parent.Navigations.items[0].nodes[3].addNode(new parent.node(\"General Settings\", \"waadmin.wa?Service=ShowGeneralSettings\", \"Details\"));\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_JS_NEW_SESSIONPOOL
//----------------------------------------------------------------------------*/
	{0, ADMIN_JS_NEW_SESSIONPOOL, "\
parent.Navigations.items[0].nodes[4].addNode(new parent.node(\"New\", \"waadmin.wa?Service=DefineNewSessionPool\", \"Details\"));\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_JS_NEW_SERVICE
//----------------------------------------------------------------------------*/
	{0, ADMIN_JS_NEW_SERVICE, "\
parent.Navigations.items[0].nodes[5].addNode(new parent.node(\"New\", \"waadmin.wa?Service=DefineNewService\", \"Details\"));\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_JS_NEW_COM_SERVICE
//----------------------------------------------------------------------------*/
	{0, ADMIN_JS_NEW_COM_SERVICE, "\
parent.Navigations.items[0].nodes[6].addNode(new parent.node(\"New\", \"waadmin.wa?Service=NewCOMService\", \"Details\"));\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_JS_TREE_ADD_SESSIONPOOL
//----------------------------------------------------------------------------*/
	{0, ADMIN_JS_TREE_ADD_SESSIONPOOL, "\
parent.Navigations.items[0].nodes[4].addNode(new parent.node(\"\
"},
/*----------------------------------------------------------------------------\
// ADMIN_JS_TREE_ADD_SERVICE
//----------------------------------------------------------------------------*/
	{0, ADMIN_JS_TREE_ADD_SERVICE, "\
parent.Navigations.items[0].nodes[5].addNode(new parent.node(\"\
"},
/*----------------------------------------------------------------------------\
// ADMIN_JS_TREE_ADD_COMSERVICE
//----------------------------------------------------------------------------*/
	{0, ADMIN_JS_TREE_ADD_COMSERVICE, "\
parent.Navigations.items[0].nodes[6].addNode(new parent.node(\"\
"},
/*----------------------------------------------------------------------------\
// ADMIN_JS_TREE_ADD_CLOSE
//----------------------------------------------------------------------------*/
	{0, ADMIN_JS_TREE_ADD_CLOSE, "\
\"));\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NAVIGATION
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NAVIGATION, "\
<body bgcolor=\"#EEDBB4\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onLoad=\"parent.showNavigation(0);\">\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SHOW_SESSIONPOOL
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SHOW_SESSIONPOOL, "\
\", \"waadmin.wa?Service=ShowSessionPool&Name=\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SHOW_SERVICE
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SHOW_SERVICE, "\
\", \"waadmin.wa?Service=Service&Name=\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SHOW_COMSERVICE
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SHOW_COMSERVICE, "\
\", \"waadmin.wa?Service=COMService&Name=\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SHOW_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SHOW_END, "\
\", \"Details\"));\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_GLOBAL_SETTINGS_FORM_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_GLOBAL_SETTINGS_FORM_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onload=\"parent.setFocus(document.myForm.LogFile);parent.showStatus('\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_GLOBAL_SETTINGS_FORM_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_GLOBAL_SETTINGS_FORM_CONTINUE_1, "\
');\">\n\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=UpdateLogSettings\">\n\
<table border=0 cellspacing=0 cellpadding=0>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_GLOBAL_SETTINGS_FORM_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_GLOBAL_SETTINGS_FORM_END, "\
</table><p><input type=\"submit\" value=\"Update\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></p>\n\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_MAIN_HEADER
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_MAIN_HEADER, "\
<html>\n\
<head>\n\
</head>\n\
<body bgcolor=\"#DEDEC8\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\">\n\
<img src=\"waadmin.wa?Service=Pictures&Name=WAHeader.png\" \
align=right border=0 vspace=0 hspace=0>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_INIT_DETAILS_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_INIT_DETAILS_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onLoad=\"parent.refreshNavigation();parent.showStatus('\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_INIT_DETAILS_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_INIT_DETAILS_END, "\
');\">\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SERVICE_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_BEGIN, "\
<html>\n\
<head>\n\
<script language=\"JavaScript\">\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SERVICE_BEGIN_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_BEGIN_CONTINUE_1, "\
</script>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" \
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SERVICE_ON_LOAD_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_ON_LOAD_BEGIN, "\
onload=\"parent.refreshNavigation();parent.setFocus(document.myForm.ServiceName);parent.showStatus('\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SERVICE_ON_LOAD_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_ON_LOAD_CONTINUE_1, "\
');parent.openNewBrowser('\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SERVICE_ON_LOAD_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_ON_LOAD_END, "\
');\">\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SERVICE_FORM_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_FORM_BEGIN, "\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=ServiceUpdate&Name=\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SERVICE_FORM_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_FORM_CONTINUE_1, "\
\">\n\
<input type=\"hidden\" name=\"WAServiceEvent\" value>\n\
<input type=\"hidden\" name=\"WAServiceAction\" value>\n\
"},
/*----------------------------------------------------------------------------\
// TO DELETE!!!
// ADMIN_HTML_SERVICE_FORM_END_BEGIN
//----------------------------------------------------------------------------*/

	{0, ADMIN_HTML_SERVICE_FORM_END_BEGIN, "\
<tr>\n\
<td align=\"right\"><img src=\"/WARoot/Images/btnl1.gif\"></td>\n\
<td background=\"/WARoot/Images/btnb1.gif\" align=\"center\" nowrap>&nbsp;&nbsp;\
<a href=\"javascript:Update()\">Update</a>&nbsp;&nbsp;\n\
</td>\n\
<td><img src=\"/WARoot/Images/btnr1.gif\"></td>\n\
</tr>\n\
<tr>\n\
<td><input type=\"submit\" name=\"Button\" value=\"Update\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></td>\n\
<td><input type=\"submit\" name=\"Button\" value=\"New Parameter\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></td>\n\
<td><input type=\"submit\" name=\"Button\" value=\"Delete Parameters\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></td>\n\
</tr>\n\
"},
/*----------------------------------------------------------------------------\
// TO DELETE!!!
// ADMIN_HTML_SERVICE_FORM_END_START_SERVICE
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_FORM_END_START_SERVICE, "\
<tr>\n\
<td><input type=\"submit\" name=\"Button\" value=\"Load Service\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></td>\n\
"},
/*----------------------------------------------------------------------------\
// TO DELETE!!!
// ADMIN_HTML_SERVICE_FORM_END_STOP_SERVICE
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_FORM_END_STOP_SERVICE, "\
<td><input type=\"submit\" name=\"Button\" value=\"Unload Service\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></td>\n\
"},
/*----------------------------------------------------------------------------\
// TO DELETE!!!
// ADMIN_HTML_SERVICE_FORM_END_GOTO_SERVICE
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_FORM_END_GOTO_SERVICE, "\
<tr>\n\
<td><input type=\"submit\" name=\"Button\" value=\"Start Service\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></td>\n\
"},
/*----------------------------------------------------------------------------\
// TO DELETE!!!
// ADMIN_HTML_SERVICE_FORM_END_RELOAD_SERVICE
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_FORM_END_RELOAD_SERVICE, "\
<td><input type=\"submit\" name=\"Button\" value=\"Reload Service\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></td>\n\
"},
/*----------------------------------------------------------------------------\
// TO DELETE!!!
// ADMIN_HTML_SERVICE_FORM_END_END
<td><input type=\"submit\" name=\"Button\" value=\"Delete Service\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"><td>\n\
</tr>\n\
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_FORM_END_END, "\
</table>\n\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_BODY_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_BODY_BEGIN, "\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onLoad=\"parent.showStatus('');\">\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_BODY_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_BODY_END, "\
</body>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_RESOURCE_SETTINGS_FORM_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_RESOURCE_SETTINGS_FORM_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onload=\"parent.setFocus(document.myForm.documentRoot);parent.showStatus('\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_RESOURCE_SETTINGS_FORM_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_RESOURCE_SETTINGS_FORM_CONTINUE_1, "\
');\">\n\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=UpdateResourceSettings\">\n\
<table border=0 cellspacing=0 cellpadding=0>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_RESOURCE_SETTINGS_FORM_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_RESOURCE_SETTINGS_FORM_END, "\
</table><p><input type=\"submit\" value=\"Update\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></p>\n\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_GENERAL_SETTINGS_FORM_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_GENERAL_SETTINGS_FORM_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onload=\"parent.setFocus(document.myForm.developerMode);parent.showStatus('\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_GENERAL_SETTINGS_FORM_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_GENERAL_SETTINGS_FORM_CONTINUE_1, "\
');\">\n\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=UpdateGeneralSettings\">\n\
<table border=0 cellspacing=0 cellpadding=0>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_GENERAL_SETTINGS_FORM_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_GENERAL_SETTINGS_FORM_END, "\
</table><p><input type=\"submit\" value=\"Update\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></p>\n\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_PARAMETER_FORM_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_PARAMETER_FORM_BEGIN, "\
<HTML>\n\
<HEAD>\n\
<SCRIPT LANGUAGE=\"JavaScript\">\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_PARAMETER_FORM_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_PARAMETER_FORM_CONTINUE_1, "\
</SCRIPT>\n\
</HEAD>\n\
<BODY BACKGROUND=\"/WARoot/Images/tatami.gif\" TOPMARGIN=\"0\" LEFTMARGIN=\"0\" MARGINHEIGHT=\"0\" MARGINWIDTH=\"0\" ONLOAD=\"parent.showStatus('');parent.setFocus(document.myForm.ParameterName)\">\n\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=NewParameter&Name=\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_PARAMETER_FORM_CONTINUE_2
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_PARAMETER_FORM_CONTINUE_2, "\
\">\n\
<input type=\"hidden\" name=\"WAServiceEvent\" value>\n\
<input type=\"hidden\" name=\"WAServiceAction\" value>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_PARAMETER_FORM_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_PARAMETER_FORM_END, "\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_INVALID_PARAMETER_NAME
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_INVALID_PARAMETER_NAME, "\
<html>\n\
<head>\n\
</head>\n\
<body bgcolor=\"#DEDEC8\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\">\n\
Invalid parameter name!\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_HORIZONTAL_RULE
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_HORIZONTAL_RULE, "\
<HR>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_SERVICE_FORM_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_SERVICE_FORM_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onload=\"parent.setFocus(document.myForm.Name);parent.showStatus('\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_SERVICE_FORM_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_SERVICE_FORM_CONTINUE_1, "\
');\">\n\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=CreateNewService\">\n\
<table border=0 cellspacing=0 cellpadding=0>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_SERVICE_FORM_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_SERVICE_FORM_END, "\
</table><p>\n\
<input type=\"submit\" name=\"Create\" value=\"Create\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\">\n\
</p>\n\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_JS_CLEAR_TREE
//----------------------------------------------------------------------------*/
	{0, ADMIN_JS_CLEAR_TREE, "\
parent.Navigations = new parent.navigations();\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_SERVICE_GET_CONFIRMATION_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_SERVICE_GET_CONFIRMATION_BEGIN, "\
<HTML>\n\
<HEAD>\n\
<SCRIPT LANGUAGE=\"JavaScript\">\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_SERVICE_GET_CONFIRMATION_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_SERVICE_GET_CONFIRMATION_CONTINUE_1, "\
</SCRIPT>\n\
</HEAD>\n\
<BODY BACKGROUND=\"/WARoot/Images/tatami.gif\" TOPMARGIN=\"0\" LEFTMARGIN=\"0\" MARGINHEIGHT=\"0\" MARGINWIDTH=\"0\" ONLOAD=\"parent.showStatus('');\">\n\
<FORM METHOD=\"POST\" ACTION=\"waadmin.wa?Service=DeleteService&Name=\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_SERVICE_GET_CONFIRMATION_CONTINUE_2
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_SERVICE_GET_CONFIRMATION_CONTINUE_2, "\
\">\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_SERVICE_GET_CONFIRMATION_CONTINUE_3
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_SERVICE_GET_CONFIRMATION_CONTINUE_3, "\
Please confirm deleting of service: <STRONG>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_SERVICE_GET_CONFIRMATION_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_SERVICE_GET_CONFIRMATION_END, "\
</STRONG>\n\
</FORM>\n\
</BODY>\n\
</HTML>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_PARAMETERS_FORM_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_PARAMETERS_FORM_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onload=\"parent.showStatus('');\">\n\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=DeleteParameters&Name=\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_PARAMETERS_FORM_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_PARAMETERS_FORM_CONTINUE_1, "\
\">\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_PARAMETERS_FORM_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_PARAMETERS_FORM_END, "\
<table border=0 cellspacing=0 cellpadding=0 bgcolor=\"#DEDEC8\" width=\"100%\">\n\
<tr><td><input type=\"submit\" name=\"Delete\" value=\"Delete\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></td></tr>\n\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_PARAMETERS_CONFIRMATION
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_PARAMETERS_CONFIRMATION, "\
Parameters deleted.\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_PARAMETERS_NOTHING
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_PARAMETERS_NOTHING, "\
Nothing deleted.\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_COMSERVICE_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_COMSERVICE_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onload=\"parent.setFocus(document.myForm.classId)\">\n\
<P>COM Service: <STRONG>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_COMSERVICE_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_COMSERVICE_CONTINUE_1, "\
</STRONG></P>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_COMSERVICE_FORM_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_COMSERVICE_FORM_BEGIN, "\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=UpdateCOMService&Name=\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_COMSERVICE_FORM_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_COMSERVICE_FORM_CONTINUE_1, "\
\">\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_COMSERVICE_CLASS_ID_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_COMSERVICE_CLASS_ID_BEGIN, "\
<p>Class Id:<br><input type=\"text\" \
name=\"classId\" size=\"80\" value=\"\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_COMSERVICE_CLASS_ID_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_COMSERVICE_CLASS_ID_END, "\
\"></p>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_COMSERVICE_LOGFILE_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_COMSERVICE_LOGFILE_BEGIN, "\
<p>Log Filename:<br><input type=\"text\" \
name=\"LogFile\" size=\"80\" value=\"\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_COMSERVICE_LOGFILE_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_COMSERVICE_LOGFILE_END, "\
\"></p>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_COMSERVICE_SERVICE_NAME_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_COMSERVICE_SERVICE_NAME_BEGIN, "\
<p>Service Name:<br><input type=\"text\" \
name=\"ServiceName\" size=\"80\" value=\"\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_COMSERVICE_SERVICE_NAME_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_COMSERVICE_SERVICE_NAME_END, "\
\"></p>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_COMSERVICE_FORM_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_COMSERVICE_FORM_END, "\
<p>\n\
<input type=\"submit\" name=\"Button\" value=\"Update\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\">&nbsp;&nbsp;&nbsp;&nbsp;\n\
<input type=\"submit\" name=\"Button\" value=\"New Parameter\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\">&nbsp;&nbsp;\n\
<input type=\"submit\" name=\"Button\" value=\"Delete Parameters\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\">&nbsp;&nbsp;&nbsp;&nbsp;\n\
<input type=\"submit\" name=\"Button\" value=\"Delete Service\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\">\n\
</p>\n\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SHOW_SESSION_POOL_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SHOW_SESSION_POOL_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onload=\"parent.setFocus(document.myForm.serverDB);parent.refreshNavigation();parent.showStatus('\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SHOW_SESSION_POOL_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SHOW_SESSION_POOL_CONTINUE_1, "\
');\">\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SHOW_SESSION_POOL_FORM_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SHOW_SESSION_POOL_FORM_BEGIN, "\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=UpdateSessionPool&Name=\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SHOW_SESSION_POOL_FORM_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SHOW_SESSION_POOL_FORM_CONTINUE_1, "\
\">\n\
<table border=0 cellspacing=0 cellpadding=0>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SHOW_SESSION_POOL_FORM_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SHOW_SESSION_POOL_FORM_END, "\
</table><p>\n\
<input type=\"submit\" name=\"Button\" value=\"Update\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\">&nbsp;&nbsp;&nbsp;&nbsp;\n\
<input type=\"submit\" name=\"Button\" value=\"Delete\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\">\n\
</p>\n\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_OPTION
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_OPTION, "\
<OPTION>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_OPTION_SELECTED
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_OPTION_SELECTED, "\
><OPTION SELECTED>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_OPTION_CLOSE
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_OPTION_CLOSE, "\
</OPTION>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_SESSION_POOL_FORM_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_SESSION_POOL_FORM_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onload=\"parent.setFocus(document.myForm.newSessionPoolName);parent.showStatus('\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_SESSION_POOL_FORM_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_SESSION_POOL_FORM_CONTINUE_1, "\
');\">\n\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=CreateNewSessionPool\">\n\
<table border=0 cellspacing=0 cellpadding=0>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_SESSION_POOL_FORM_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_SESSION_POOL_FORM_END, "\
</table><p>\n\
<input type=\"submit\" name=\"Create\" value=\"Create\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\">\n\
</p>\n\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SESSION_POOL_CREATE_CONFIRMATION
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SESSION_POOL_CREATE_CONFIRMATION, "\
Session pool created.\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SESSION_POOL_COULD_NOT_LOAD
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SESSION_POOL_COULD_NOT_LOAD, "\
Session pool could not be loaded.\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SERVICE_ALREADY_EXISTS_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_ALREADY_EXISTS_BEGIN, "\
Service <STRONG>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SERVICE_ALREADY_EXISTS_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_ALREADY_EXISTS_END, "\
</STRONG> already exists!\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SESSION_POOL_ALREADY_EXISTS_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SESSION_POOL_ALREADY_EXISTS_BEGIN, "\
Session pool <STRONG>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SESSION_POOL_ALREADY_EXISTS_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SESSION_POOL_ALREADY_EXISTS_END, "\
</STRONG> already exists!\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_SESSION_POOL_GET_CONFIRMATION_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_SESSION_POOL_GET_CONFIRMATION_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onload=\"parent.showStatus('');\">\n\
<FORM METHOD=\"POST\" ACTION=\"waadmin.wa?Service=DeleteSessionPool&Name=\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_SESSION_POOL_GET_CONFIRMATION_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_SESSION_POOL_GET_CONFIRMATION_CONTINUE_1, "\
\">\n\
Please confirm deleting of session pool: <STRONG>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_SESSION_POOL_GET_CONFIRMATION_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_SESSION_POOL_GET_CONFIRMATION_END, "\
</STRONG><p><input type=\"submit\" value=\"Delete\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></p>\n\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_SESSION_POOL_CONFIRMATION
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_SESSION_POOL_CONFIRMATION, "\
Session pool deleted.\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_COMPARAMETER_FORM_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_COMPARAMETER_FORM_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onload=\"parent.setFocus(document.myForm.ParameterName)\">\n\
<p>New parameter for COM service: <STRONG>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_COMPARAMETER_TITLE_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_COMPARAMETER_TITLE_END, "\
</STRONG></p>\n\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=NewCOMParameter&Name=\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_COMPARAMETER_SERVICE_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_COMPARAMETER_SERVICE_END, "\
\">\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_COMPARAMETER_NAME
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_COMPARAMETER_NAME, "\
<p>Parameter Name:<br><input type=\"text\" \
name=\"ParameterName\" size=\"80\"\"></p>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_COMPARAMETER_DEFAULT_VALUE
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_COMPARAMETER_DEFAULT_VALUE, "\
<p>Default Value:<br><input type=\"text\" \
name=\"DefaultValue\" size=\"80\"\"></p>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_COMPARAMETER_FORM_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_COMPARAMETER_FORM_END, "\
<p><input type=\"submit\" value=\"Create\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></p>\n\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_COMPARAMETERS_FORM_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_COMPARAMETERS_FORM_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" >\n\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=DeleteCOMParameters&Name=\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_COMPARAMETERS_FORM_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_COMPARAMETERS_FORM_CONTINUE_1, "\
\">\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_COMPARAMETERS_PARAMETER_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_COMPARAMETERS_PARAMETER_BEGIN, "\
<P><INPUT type=checkbox name=\"parameter\" value=\"\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_COMPARAMETERS_PARAMETER_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_COMPARAMETERS_PARAMETER_CONTINUE_1, "\
\">\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_COMPARAMETERS_PARAMETER_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_COMPARAMETERS_PARAMETER_END, "\
</P>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_COMPARAMETERS_FORM_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_COMPARAMETERS_FORM_END, "\
<p><input type=\"submit\" name=\"Button\" value=\"Delete\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></p>\n\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_COMSERVICE_GET_CONFIRMATION_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_COMSERVICE_GET_CONFIRMATION_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" >\n\
<FORM METHOD=\"POST\" ACTION=\"waadmin.wa?Service=DeleteCOMService&Name=\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_COMSERVICE_GET_CONFIRMATION_CONTINUE_1
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_COMSERVICE_GET_CONFIRMATION_CONTINUE_1, "\
\">\n\
Please confirm deleting of COM service: <STRONG>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_COMSERVICE_GET_CONFIRMATION_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_COMSERVICE_GET_CONFIRMATION_END, "\
</STRONG><p><input type=\"submit\" value=\"Delete\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\"></p>\n\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_COMSERVICE_CONFIRMATION
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_COMSERVICE_CONFIRMATION, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onLoad=\"parent.refreshNavigation();\">\n\
COM service deleted.\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_COMPARAMETERS_CONFIRMATION
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_COMPARAMETERS_CONFIRMATION, "\
<html>\n\
<head>\n\
</head>\n\
<body bgcolor=\"#DEDEC8\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\">\n\
COM parameters deleted.\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_DELETE_COMPARAMETERS_NOTHING
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_DELETE_COMPARAMETERS_NOTHING, "\
<html>\n\
<head>\n\
</head>\n\
<body bgcolor=\"#DEDEC8\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\">\n\
Nothing deleted.\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_COMSERVICE_FORM_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_COMSERVICE_FORM_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onload=\"parent.setFocus(document.myForm.newCOMServiceName)\">\n\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=CreateNewCOMService\">\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_COMSERVICE_NAME
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_COMSERVICE_NAME, "\
<p>Logical name of the new COM service:<br><input type=\"text\" \
name=\"newCOMServiceName\" size=\"80\"></p>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_COMSERVICE_CLASS_ID
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_COMSERVICE_CLASS_ID, "\
<p>Class Id:<br><input type=\"text\" \
name=\"ClassId\" size=\"80\"></p>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_COMSERVICE_LOGFILE
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_COMSERVICE_LOGFILE, "\
<p>Log Filename:<br><input type=\"text\" \
name=\"LogFile\" size=\"80\"></p>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_COMSERVICE_SERVICE_NAME
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_COMSERVICE_SERVICE_NAME, "\
<p>Service Name:<br><input type=\"text\" \
name=\"ServiceName\" size=\"80\"></p>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_NEW_COMSERVICE_FORM_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_NEW_COMSERVICE_FORM_END, "\
<p>\n\
<input type=\"submit\" name=\"Create\" value=\"Create\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\">\n\
</p>\n\
</FORM>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_COMSERVICE_ALREADY_EXISTS_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_COMSERVICE_ALREADY_EXISTS_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body bgcolor=\"#DEDEC8\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\">\n\
COM service \"<STRONG>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_COMSERVICE_ALREADY_EXISTS_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_COMSERVICE_ALREADY_EXISTS_END, "\
</STRONG>\" already exists!\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_COMSERVICE_CREATE_CONFIRMATION
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_COMSERVICE_CREATE_CONFIRMATION, "\
<html>\n\
<head>\n\
</head>\n\
<body bgcolor=\"#DEDEC8\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" onLoad=\"parent.refreshNavigation();\">\n\
COM service created.\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_RESERVED_SERVICE_NAME_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_RESERVED_SERVICE_NAME_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body bgcolor=\"#DEDEC8\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\">\n\
Reserved service name: <STRONG>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_RESERVED_SERVICE_NAME_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_RESERVED_SERVICE_NAME_END, "\
</STRONG>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_RESERVED_COMSERVICE_NAME_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_RESERVED_COMSERVICE_NAME_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body bgcolor=\"#DEDEC8\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\">\n\
Reserved COM service name: <STRONG>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_RESERVED_COMSERVICE_NAME_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_RESERVED_COMSERVICE_NAME_END, "\
</STRONG>\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_CLEAR_LOG_FILE
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_CLEAR_LOG_FILE, "\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=ClearLogFile\">\n\
<input type=\"submit\" name=\"ClearLogfile\" value=\"Clear Logfile\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\">\n\
</p>\n\
</FORM>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_CLEAR_LOG_FILE_HTTP_REQUESTS
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_CLEAR_LOG_FILE_HTTP_REQUESTS, "\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=ClearLogFileHTTPRequests\">\n\
<input type=\"submit\" name=\"ClearLogfile\" value=\"Clear Logfile\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\">\n\
</p>\n\
</FORM>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_CLEAR_LOG_FILE_HTTP_ERRORS
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_CLEAR_LOG_FILE_HTTP_ERRORS, "\
<FORM name=\"myForm\" METHOD=\"POST\" ACTION=\"waadmin.wa?Service=ClearLogFileHTTPErrors\">\n\
<input type=\"submit\" name=\"ClearLogfile\" value=\"Clear Logfile\" style=\"HEIGHT: 24px; WIDTH: 120px; background-color: #52638C; color: White;\">\n\
</p>\n\
</FORM>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_LOG_FILE_CLEAR_CONFIRMATION
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_LOG_FILE_CLEAR_CONFIRMATION, "\
Log file cleared.\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_EMPTY_LOG_FILE
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_EMPTY_LOG_FILE, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" >\n\
No log entries!\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_EMPTY_LOG_FILE_HTTP_REQUESTS
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_EMPTY_LOG_FILE_HTTP_REQUESTS, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" >\n\
No HTTP requests in log file!\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_EMPTY_LOG_FILE_HTTP_ERRORS
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_EMPTY_LOG_FILE_HTTP_ERRORS, "\
<html>\n\
<head>\n\
</head>\n\
<body background=\"/WARoot/Images/tatami.gif\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\" >\n\
No HTTP errors in log file!\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_START_SERVICE_CONFIRMATION_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_START_SERVICE_CONFIRMATION_BEGIN, "\
Service <STRONG>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_START_SERVICE_OK
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_START_SERVICE_OK, "\
</STRONG> started.\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_START_SERVICE_NOT_OK
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_START_SERVICE_NOT_OK, "\
Service could not be started.\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_STOP_SERVICE_CONFIRMATION_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_BEGIN, "\
Stopping following service(s):<P>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_STOP_SERVICE_CONFIRMATION_NAME
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_NAME, "\
<STRONG>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_STOP_SERVICE_CONFIRMATION_OK
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_OK, "\
</STRONG> stopped.<BR>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_STOP_SERVICE_CONFIRMATION_NOT_OK
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_NOT_OK, "\
</STRONG> not stopped!<BR>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_STOP_SERVICE_LIST_OF_SERVICES_TITLE
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_STOP_SERVICE_LIST_OF_SERVICES_TITLE, "\
Following services use same library:<BR>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_STOP_SERVICE_CONFIRMATION_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_END, "\
</P>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SERVICE_ALREADY_LOADED_BEGIN
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_ALREADY_LOADED_BEGIN, "\
<html>\n\
<head>\n\
</head>\n\
<body bgcolor=\"#DEDEC8\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\">\n\
Service <STRONG>\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SERVICE_ALREADY_LOADED_END
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_ALREADY_LOADED_END, "\
</STRONG> already loaded!.\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SERVICE_MISSING_SSL_URL
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_MISSING_SSL_URL, "\
The specified SSL URL is not valid.<BR><BR>\
Sample SSL URL: https://www.mycompany.com\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_SERVICE_NOT_FOUND
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_SERVICE_NOT_FOUND, "\
<html>\n\
<head>\n\
</head>\n\
<body bgcolor=\"#DEDEC8\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\" marginwidth=\"0\">\n\
Service not found!\n\
</body>\n\
</html>\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_CONTROL_TABLE_START_WIDTH_100
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_CONTROL_TABLE_START_WIDTH_100, "\
<table border=0 cellspacing=0 cellpadding=0 bgcolor=\"#DEDEC8\" width=\"100%\">\n\
"},
/*----------------------------------------------------------------------------\
// ADMIN_HTML_CONTROL_TABLE_START
//----------------------------------------------------------------------------*/
	{0, ADMIN_HTML_CONTROL_TABLE_START, "\
<table border=0 cellspacing=0 cellpadding=0 bgcolor=\"#DEDEC8\">\n\
"},
/*----------------------------------------------------------------------------\
// Last entry
//----------------------------------------------------------------------------*/
	{0, 1000000, "\
"}
};

/*=======================================================================
 *  FUNCTIONS
 *=====================================================================*/
	
sapdbwa_Bool wd15GetString ( sapdbwa_Int4	  groupId,
							 sapdbwa_Int4	  textId,
							 const char		**text     ) {

	sapdbwa_Int4	i = 0;

	for ( i = 0; i < sizeof(textArray)/sizeof(twd15_Text); i++ ) {
		if ( textArray[i].groupId == groupId && textArray[i].textId == textId ) {
			*text = textArray[i].text;
			return true;
		}
	}

	return false;

}
