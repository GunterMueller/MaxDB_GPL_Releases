/****************************************************************************

  module      : WDVHandler_CommonUtils.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-03-28  16:58
  see also    : 

  -------------------------------------------------------------------------


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


*****************************************************************************/

	#include "SAPDB/WebDAV/Handler/WDVHandler_CommonUtils.h"
	#include "hwd21req.h"
	#include "SAPDBCommon/SAPDB_string.h"

    /* Set the static values for timestamp conversion */
    static char *Month[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

    static char *WeekDay[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

    static char *WeekDayRFC850[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

	/*------------------------------------------------------------------------
		Function:	propertyToHTTPDate
	--------------------------------------------------------------------------*/
	void propertyToHTTPDate( WDVCAPI_PropfindPropertyName    property,
		                     WDVH_HTTPDate                   httpDate ) 
	{

        struct tm   TM;

        /* Initialize return value */
        httpDate[0] = 0;

        /* Scan input value */
        sscanf(property, "%4d-%2d-%2d %2d:%2d:%2d", 
               &TM.tm_year, &TM.tm_mon, &TM.tm_mday, &TM.tm_hour, &TM.tm_min, &TM.tm_sec);

        /* Check timestamp values */
        TM.tm_year -= 1900;
        TM.tm_mon--;

        if((TM.tm_year < 70 || TM.tm_year > 135) ||
           (TM.tm_mon  < 0  || TM.tm_mon  > 11)  ||
           (TM.tm_mday < 1  || TM.tm_mday > 31)  ||
           (TM.tm_hour < 0  || TM.tm_hour > 23)  ||
           (TM.tm_min  < 0  || TM.tm_min  > 59)  ||
           (TM.tm_sec  < 0  || TM.tm_sec  > 61)) {

            /* Something is wrong with the timestamp */
            return;
        }

        /* Let the system determine wether we have daylight saving time ... */
        TM.tm_isdst = -1;

        /* Get the missing time values */
        if(mktime(&TM) < 0 ||
           (TM.tm_mon  < 0 || TM.tm_mon  > 11)  ||
           (TM.tm_mday < 1 || TM.tm_mday > 31)  ||
           (TM.tm_hour < 0 || TM.tm_hour > 23)  ||
           (TM.tm_min  < 0 || TM.tm_min  > 59)  ||
           (TM.tm_sec  < 0 || TM.tm_sec  > 61)) {

            /* Something is wrong with the timestamp */
            return;
        } else {
            /* Create the return value: HTTP date RFC 2616 Ch: 3.3.1 */
            /* Date format is RFC 822, updated by RFC 1123           */
            
            sprintf(httpDate, "%s, %02d %s %4d %02d:%02d:%02d GMT",
                    WeekDay[TM.tm_wday], TM.tm_mday, Month[TM.tm_mon], 1900 + TM.tm_year,
                    TM.tm_hour, TM.tm_min, TM.tm_sec);
            
            /*
            sprintf(httpDate, "%04d/%02d/%02d %02d:%02d:%02d",
                    1900 + TM.tm_year, TM.tm_mon, TM.tm_mday,
                    TM.tm_hour, TM.tm_min, TM.tm_sec);
            */
        }

        return;

    }

	/*------------------------------------------------------------------------
		Function:	propertyToHTTPDateRFC850
	--------------------------------------------------------------------------*/
	void propertyToHTTPDateRFC850( WDVCAPI_PropfindPropertyName    property,
		                           WDVH_HTTPDate                   httpDate ) 
	{

        struct tm   TM;

        /* Initialize return value */
        httpDate[0] = 0;

        /* Scan input value */
        sscanf(property, "%4d-%2d-%2d %2d:%2d:%2d", 
               &TM.tm_year, &TM.tm_mon, &TM.tm_mday, &TM.tm_hour, &TM.tm_min, &TM.tm_sec);

        /* Check timestamp values */
        TM.tm_year -= 1900;
        TM.tm_mon--;

        if((TM.tm_year < 70 || TM.tm_year > 135) ||
           (TM.tm_mon  < 0  || TM.tm_mon  > 11)  ||
           (TM.tm_mday < 1  || TM.tm_mday > 31)  ||
           (TM.tm_hour < 0  || TM.tm_hour > 23)  ||
           (TM.tm_min  < 0  || TM.tm_min  > 59)  ||
           (TM.tm_sec  < 0  || TM.tm_sec  > 61)) {

            /* Something is wrong with the timestamp */
            return;
        }

        /* Let the system determine wether we have daylight saving time ... */
        TM.tm_isdst = -1;

        /* Get the missing time values */
        if(mktime(&TM) < 0 ||
           (TM.tm_mon  < 0 || TM.tm_mon  > 11)  ||
           (TM.tm_mday < 1 || TM.tm_mday > 31)  ||
           (TM.tm_hour < 0 || TM.tm_hour > 23)  ||
           (TM.tm_min  < 0 || TM.tm_min  > 59)  ||
           (TM.tm_sec  < 0 || TM.tm_sec  > 61)) {

            /* Something is wrong with the timestamp */
            return;
        } else {
            /* Create the return value: HTTP date RFC 850 */
            sprintf(httpDate, "%s, %02d-%s-%2d %02d:%02d:%02d GMT",
                    WeekDayRFC850[TM.tm_wday], TM.tm_mday, Month[TM.tm_mon], TM.tm_year,
                    TM.tm_hour, TM.tm_min, TM.tm_sec);
        }

        return;

    }

	/*------------------------------------------------------------------------
		Function:	propertyToHTTPDateANSI
	--------------------------------------------------------------------------*/
	void propertyToHTTPDateANSI( WDVCAPI_PropfindPropertyName    property,
		                         WDVH_HTTPDate                   httpDate ) 
	{

        struct tm   TM;

        /* Initialize return value */
        httpDate[0] = 0;

        /* Scan input value */
        sscanf(property, "%4d-%2d-%2d %2d:%2d:%2d", 
               &TM.tm_year, &TM.tm_mon, &TM.tm_mday, &TM.tm_hour, &TM.tm_min, &TM.tm_sec);

        /* Check timestamp values */
        TM.tm_year -= 1900;
        TM.tm_mon--;

        if((TM.tm_year < 70 || TM.tm_year > 135) ||
           (TM.tm_mon  < 0  || TM.tm_mon  > 11)  ||
           (TM.tm_mday < 1  || TM.tm_mday > 31)  ||
           (TM.tm_hour < 0  || TM.tm_hour > 23)  ||
           (TM.tm_min  < 0  || TM.tm_min  > 59)  ||
           (TM.tm_sec  < 0  || TM.tm_sec  > 61)) {

            /* Something is wrong with the timestamp */
            return;
        }

        /* Let the system determine wether we have daylight saving time ... */
        TM.tm_isdst = -1;

        /* Get the missing time values */
        if(mktime(&TM) < 0 ||
           (TM.tm_mon  < 0 || TM.tm_mon  > 11)  ||
           (TM.tm_mday < 1 || TM.tm_mday > 31)  ||
           (TM.tm_hour < 0 || TM.tm_hour > 23)  ||
           (TM.tm_min  < 0 || TM.tm_min  > 59)  ||
           (TM.tm_sec  < 0 || TM.tm_sec  > 61)) {

            /* Something is wrong with the timestamp */
            return;
        } else {
            /* Create the return value: HTTP date RFC 850 */
            sprintf(httpDate, "%s %s %d %02d:%02d:%02d %04d",
                    WeekDay[TM.tm_wday], Month[TM.tm_mon], TM.tm_mday,
                    TM.tm_hour, TM.tm_min, TM.tm_sec, TM.tm_year);
        }

        return;

    }

	/*------------------------------------------------------------------------
		Function:	getWebDAVPrefix
	--------------------------------------------------------------------------*/
	void getWebDAVPrefix(sapdbwa_Handle		wa,
						 WDVH_Char *prefix) 
	{
		WDVH_Char   temp[WDVH_MAX_URI_LEN+1];

		if (wa != NULL) {
			/* set server prefix */
			strcpy(temp,sapdbwa_GetServiceName(wa));
			sp77sprintf(prefix,WDVH_MAX_URI_LEN+1,"/%s",temp);
		} else {
			sp77sprintf(prefix,WDVH_MAX_URI_LEN+1,"/");
		}
	}

	/*------------------------------------------------------------------------
		Function:	getFirstHeaderLine
	--------------------------------------------------------------------------*/
	void getFirstHeaderLine(sapdbwa_Handle		wa,
		                    sapdbwa_HttpRequestP request,
		                    WDVH_PChar *host,
							WDVH_PChar *port, 
							WDVH_PChar uri) 
	{
		WDVH_Char	*newUri;
		WDVH_Char   prefix[WDVH_MAX_URI_LEN+1];

		if (host==NULL || port==NULL || wa==NULL || request==NULL || uri==NULL)
			return;

		*host = (char*)sapdbwa_GetHeader(request, "HTTP_HOST" );
		*port = (char*)sapdbwa_GetHeader(request, "HTTP_PORT" );
		/* *port = "85"; */
		newUri = (char*)sapdbwa_GetRequestURI(request);
		/* set server prefix */
		getWebDAVPrefix(wa,prefix);
		/* cut webdav service prefix */
		newUri  = newUri + strlen(prefix);
		/* unescape uri */
		strncpy(uri,newUri,WDVH_MAX_URI_LEN);
		uri[WDVH_MAX_URI_LEN] = '\0';
		wd21_UnescapeUrl(uri,strlen(uri));
		if (strcmp(uri,"")==0) 
			strcpy(uri,"/");
	}

	/*------------------------------------------------------------------------
		Function:	getRequestUri
	--------------------------------------------------------------------------*/
	void getRequestUri(sapdbwa_Handle		wa,
					   sapdbwa_HttpRequestP request,
					   WDVH_PChar uri) 
	{
		WDVH_Char	*newUri;
		WDVH_Char   prefix[WDVH_MAX_URI_LEN+1];

		if (wa==NULL || request==NULL || uri==NULL)
			return;

		newUri = (char*)sapdbwa_GetRequestURI(request);
		/* set server prefix */
		getWebDAVPrefix(wa,prefix);
		/* cut webdav service prefix */
		newUri  = newUri + strlen(prefix);
		/* unescape uri */
		strncpy(uri,newUri,WDVH_MAX_URI_LEN);
		uri[WDVH_MAX_URI_LEN] = '\0';
		wd21_UnescapeUrl(uri,strlen(uri));
		if (strcmp(uri,"")==0) 
			strcpy(uri,"/");
	}

	/*------------------------------------------------------------------------
		Function:	getRequestUriFromString
	--------------------------------------------------------------------------*/
	void getRequestUriFromString(sapdbwa_Handle			wa,
								 sapdbwa_HttpRequestP	request,
								 WDVH_PChar				uri) 
	{
		WDVH_Char	newUri[WDVH_MAX_URI_LEN+1];
		WDVH_Char   prefix[WDVH_MAX_URI_LEN+1];
		WDVH_Char	*pNewUri;
		WDVH_Char	*server=NULL;

		if (wa==NULL || request==NULL || uri==NULL)
			return;

		strcpy(newUri,uri);
		/* set server prefix */
		buildServerString(request,&server);
		getWebDAVPrefix(wa,prefix);
		/* cut webdav service prefix */
		pNewUri  = newUri + strlen(server) + strlen(prefix);
		if (strcmp(pNewUri,"")==0) 
			strcpy(uri,"/");
		else
			strcpy(uri,pNewUri);
		if (server)
			WDVHandler_Free((sapdbwa_UInt1*) server);
	}

	/*------------------------------------------------------------------------
		Function:	buildServerString
	--------------------------------------------------------------------------*/
	void buildServerString(sapdbwa_HttpRequestP request,
		                   WDVH_PChar *server) 
	{
		WDVH_Char				*host = NULL;
		WDVH_Char				*port = NULL;
		WDVH_Char				*newServer;
		sapdbwa_Bool			allocatOK;
		sapdbwa_Int4			length;
	
		if (request==NULL || server==NULL)
			return;

		host = (char*)sapdbwa_GetHeader(request, "HTTP_HOST" );
		port = (char*)sapdbwa_GetHeader(request, "HTTP_PORT" );

		if (host==NULL) {
			WDVHandler_Allocat(sizeof(SAPDB_Char)*2,(sapdbwa_UInt1**) &newServer,&allocatOK);
			if (allocatOK) {
				sp77sprintf(newServer,2,"");
			}
		} else {
			if (port==NULL) {
				length = strlen(WDVH_HTTP_PREFIX) + strlen(host) + 3;
				WDVHandler_Allocat(sizeof(SAPDB_Char)*length,(sapdbwa_UInt1**) &newServer,&allocatOK);
				if (allocatOK) {
					sp77sprintf(newServer,length,"%s%s",WDVH_HTTP_PREFIX,host);
				}
			} else if (strcmp(port,"")==0) {
				length = strlen(WDVH_HTTP_PREFIX) + strlen(host) + 3;
				WDVHandler_Allocat(sizeof(SAPDB_Char)*length,(sapdbwa_UInt1**) &newServer,&allocatOK);
				if (allocatOK) {
					sp77sprintf(newServer,length,"%s%s",WDVH_HTTP_PREFIX,host);
				}
			} else {
				length = strlen(WDVH_HTTP_PREFIX) + strlen(host) + strlen(port) + 3;
				WDVHandler_Allocat(sizeof(SAPDB_Char)*length,(sapdbwa_UInt1**) &newServer,&allocatOK);
				if (allocatOK) {
					sp77sprintf(newServer,length,"%s%s:%s",WDVH_HTTP_PREFIX,host,port);
				}
			}
		}
		
		if (allocatOK) 
			*server = newServer;
		else
			*server = NULL;
	}

	/*------------------------------------------------------------------------
		Function:	buildServerStringLong
	--------------------------------------------------------------------------*/
	void buildServerStringLong(sapdbwa_Handle		wa,
							   sapdbwa_HttpRequestP request,
							   WDVH_PChar *server) 
	{
		WDVH_Char				*host = NULL;
		WDVH_Char				*port = NULL;
		WDVH_Char				*newServer;
		sapdbwa_Bool			allocatOK;
		sapdbwa_Int4			length;
		WDVH_Char				prefix[WDVH_MAX_URI_LEN+1];

		if (wa==NULL || request==NULL || server==NULL)
			return;

		host = (char*)sapdbwa_GetHeader(request, "HTTP_HOST" );
		port = (char*)sapdbwa_GetHeader(request, "HTTP_PORT" );
		/* set server prefix */
		getWebDAVPrefix(wa,prefix);

		if (host==NULL) {
			WDVHandler_Allocat(sizeof(SAPDB_Char)*2,(sapdbwa_UInt1**) &newServer,&allocatOK);
			if (allocatOK) {
				sp77sprintf(newServer,2,"");
			}
		} else {
			if (port==NULL) {
				length = strlen(WDVH_HTTP_PREFIX) + strlen(host) + strlen(prefix) + 3;
				WDVHandler_Allocat(sizeof(SAPDB_Char)*length,(sapdbwa_UInt1**) &newServer,&allocatOK);
				if (allocatOK) {
					sp77sprintf(newServer,length,"%s%s%s",WDVH_HTTP_PREFIX,host,prefix);
				}
			} else {
				length = strlen(WDVH_HTTP_PREFIX) + strlen(host) + strlen(port) + strlen(prefix) + 3;
				WDVHandler_Allocat(sizeof(SAPDB_Char)*length,(sapdbwa_UInt1**) &newServer,&allocatOK);
				if (allocatOK) {
					sp77sprintf(newServer,length,"%s%s:%s%s",WDVH_HTTP_PREFIX,host,port,prefix);
				}
			}
		}

		if (allocatOK) 
			*server = newServer;
		else
			*server = NULL;
	}

	/*------------------------------------------------------------------------
		Function:	getIfHeader
	--------------------------------------------------------------------------*/
	WDVH_Bool getIfHeader(sapdbwa_HttpRequestP request,
		                  WDVH_Char *sIf,
						  WDVH_Int4 len)
	{
		WDVH_Char	*temp1, *temp2, *temp4, *temp5;
		WDVH_UInt4	length;
		WDVH_Char	temp3[WDVH_MAX_IF_HEADER_LEN];

		if (request==NULL || sIf==NULL)
			return WDVH_False;

		strcpy(sIf,"");
		temp4 = (char*)sapdbwa_GetHeader(request,"If");
		if (temp4 != NULL) {
			strcpy(temp3,temp4);
			temp1 = strchr(temp3,'<');
			temp2 = strchr(temp3,'>');
			if (temp1 != NULL &&
				temp2 != NULL) {
				length = temp2 - temp1 - 1;
				strncpy(temp4,temp1+1,length);
				temp4[length] = '\0';
				strcpy(temp3,temp4);
				if (strncmp(temp4,"saplocktoken:",strlen("saplocktoken:"))==0) {
					strncpy(temp4,temp3+strlen("saplocktoken:"),length-strlen("saplocktoken:"));
					temp4[length-strlen("saplocktoken:")] = '\0';
					strncpy(sIf,temp4,len);
					sIf[length-1]='\0';
				}
				return WDVH_True;
			} else {
				return WDVH_False;
			}
		} else {
			return WDVH_False;
		}
	}

	/*------------------------------------------------------------------------
		Function:	getIfHeaderList
	--------------------------------------------------------------------------*/
	WDVH_Bool getIfHeaderList(sapdbwa_Handle		wa,
							  sapdbwa_HttpRequestP request,
							  WDVCAPI_WDV wdv,
							  WDVCAPI_LockIdList *lockTokenList)		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1120166 */
	{
		WDVH_Char	*resourceP,*valueP,*ifheader;
		WDVH_Char	resource[WDVH_MAX_URI_LEN+1],value[WDVH_MAX_URI_LEN+1];
		WDVH_Char	*c;
		WDVH_UInt4	length,state;
		WDVH_Char	*server;

		if (wa==NULL || request==NULL || wdv==NULL || lockTokenList==NULL)
			return WDVH_False;

		strcpy(resource,"");
		strcpy(value,"");
		length = 0;
		state = 0;

		ifheader = (char*)sapdbwa_GetHeader(request,"If");
		
		if (ifheader) {
			/* get server string */
			buildServerStringLong(wa,request,&server);
			if (server!=NULL) {
				c=ifheader;
				while (*c!='\0') {
					if (*c=='<' && state==0) {
						/* begin of resource url */
						state = 1;
						resourceP = c+1;
						length = 0;
					} else if (*c=='<' && state==2) {
						state = 2;
						valueP = c+1;
						length = 0;
					} else if (*c=='(' && state == 0) {
						/* begin of value */
						state = 2;
					} else if (*c==')' && state == 2) {
						state = 0;
					} else if (*c=='>' && state == 1) {
						/* end of resource url */
						state = 0;
						if (strncmp(resourceP,server,strlen(server))==0) {
							resourceP = resourceP+strlen(server);
							length = length-strlen(server);
							strncpy(resource,resourceP,length);
							resource[length] = '\0';
						}
					} else if (*c=='>' && state == 2) {
						/* end of value */
						state = 2;
						if (strncmp(valueP,"saplocktoken:",strlen("saplocktoken:"))==0) {
							valueP = valueP+strlen("saplocktoken:");
							length = length - strlen("saplocktoken:");
							strncpy(value,valueP,length);
							value[length] = '\0';
							if (strcmp(resource,"")==0) {
								WDVCAPI_LockAddIdToList(wdv,lockTokenList,NULL,value);			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1120166 */
							} else {
								WDVCAPI_LockAddIdToList(wdv,lockTokenList,resource,value);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1120166 */
							}
						}
					} else {
						length++;
					}
					c++;
				}
			}
			if (server) {
				WDVHandler_Free((sapdbwa_UInt1*) server);
			}
		}
		return WDVH_True;
	}

	/* Begin Correction: http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
	/*------------------------------------------------------------------------
		Function:	getContentRangeHeader
	--------------------------------------------------------------------------*/
	WDVH_Bool getContentRangeHeader(sapdbwa_HttpRequestP request,
									WDVH_Char **from, WDVH_Char **to)
	{
		WDVH_Char	*crheader, *temp1, *temp2, *temp4, *temp5;
		WDVH_UInt4	length;
		WDVH_Char	temp3[WDVH_MAX_IF_HEADER_LEN];

		if (request==NULL || from==NULL || to==NULL)
			return WDVH_False;

		*from = NULL;
		*to = NULL;
		crheader = (char*)sapdbwa_GetHeader(request,"Range");
		/* 14.16 Content-Range
		14.35 Range
		14.35.1 Byte Ranges
			Range = "Range" ":" ranges-specifier
			ranges-specifier = byte-ranges-specifier
			byte-ranges-specifier = bytes-unit "=" byte-range-set
			byte-range-set  = 1#( byte-range-spec | suffix-byte-range-spec )
			byte-range-spec = first-byte-pos "-" [last-byte-pos]
			first-byte-pos  = 1*DIGIT
			last-byte-pos   = 1*DIGIT
			suffix-byte-range-spec = "-" suffix-length
			suffix-length = 1*DIGIT

		*/

		if (crheader != NULL) {
			if (strncmp(crheader,"bytes=",6)!=0)
				/* a Range Header was found therefore return True -> error handling has to be done by the caller */
				return WDVH_True;
			temp1 = strchr(crheader,'-');
			if (temp1 != NULL) {
				/* all up to the position of the '-' sign represents the "from" byte pos */
				crheader[temp1-crheader] = '\0';
				*from = crheader+6;
				/* all from the position of the '-' sign up to the end of the string represents the "from" byte pos */
				*to = temp1+1;
			}
			/* a Range Header was found therefore return True -> error handling has to be done by the caller */
			return WDVH_True;
		} else {
			return WDVH_False;
		}
	}
	/*------------------------------------------------------------------------
		Function:	getUserAgentHeader
	--------------------------------------------------------------------------*/
	WDVH_Bool getUserAgentHeader(sapdbwa_HttpRequestP request,
		                  WDVH_Char *sUA,
						  WDVH_Int4 len)
	{
		WDVH_Char	*uaheader;
		WDVH_UInt4	length;
		WDVH_Char	temp3[WDVH_MAX_IF_HEADER_LEN];

		if (request==NULL || sUA==NULL)
			return WDVH_False;

		strcpy(sUA,"");
		uaheader = (char*)sapdbwa_GetHeader(request,"User-Agent");
		if (uaheader != NULL) {
			/* User Agent Header found */
			/* return the User-Agent Header Value */
			strncpy(sUA,uaheader,len);
			sUA[len] = '\0';
			return WDVH_True;
		} else {
			return WDVH_False;
		}
	}

	/*------------------------------------------------------------------------
	    Function: isNumber
	--------------------------------------------------------------------------*/
	WDVH_Bool isNumber(WDVH_Char *str) 
	{
		WDVH_Char c;
		WDVH_Long   pos = 0;

		if (str==NULL)
			return WDVH_False;

		c = str[0];
		while(c != '\0') {
			if (!isdigit(c))
				return WDVH_False;
			c = str[pos++];
		}
		return WDVH_True;
	}
	/* End Correction: http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */

	/*------------------------------------------------------------------------
		Function:	getLockTokenHeader
	--------------------------------------------------------------------------*/
	WDVH_Bool getLockTokenHeader(sapdbwa_HttpRequestP request,
								 WDVH_Char *sLockToken,
								 WDVH_Char *errormsg)			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
	{
		WDVH_Char	*temp1, *temp2, *temp4, *temp5;
		WDVH_UInt4	length;
		WDVH_Char	temp3[WDVH_MAX_IF_HEADER_LEN];

		if (request==NULL || sLockToken==NULL || errormsg==NULL)
			return WDVH_False;

		temp4 = (char*)sapdbwa_GetHeader(request,"Lock-Token");
		if (temp4 != NULL) {
			strcpy(temp3,temp4);
			/* characters outside '<' and '>' are ignored. its to much effort to check each time whether these characters are
			   white spaces or not. for performance reasons we do not take this into consideration */
			temp1 = strchr(temp3,'<');
			temp2 = strchr(temp3,'>');
			if (temp1 != NULL &&
				temp2 != NULL) {
				length = temp2 - temp1 - 1;
				strncpy(temp4,temp1+1,length);
				temp4[length] = '\0';
				strcpy(temp3,temp4);
				if (strncmp(temp4,"saplocktoken:",strlen("saplocktoken:"))==0) {
					strncpy(temp4,temp3+strlen("saplocktoken:"),length-strlen("saplocktoken:"));
					temp4[length-strlen("saplocktoken:")] = '\0';
					/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
					if (strlen(temp4) != 48) {
						sp77sprintf(errormsg,1000,WDVH_ERR_TEXT_UNLOCK_WRONG_GUID);
						return WDVH_False;
					}
					/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
					strcpy(sLockToken,temp4);
					return WDVH_True;
				/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
				} else {
					sp77sprintf(errormsg,1000,WDVH_ERR_TEXT_UNLOCK_MISSING_SAPLOCKTOKEN);
					return WDVH_False;
					/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
				}
			} else {
				/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
				if (temp1 == NULL && temp2 != NULL) 
					sp77sprintf(errormsg,1000,WDVH_ERR_TEXT_UNLOCK_MISSING_RIGHT_BRACKET);
				else if (temp1 != NULL && temp2 == NULL) 
					sp77sprintf(errormsg,1000,WDVH_ERR_TEXT_UNLOCK_MISSING_LEFT_BRACKET);
				else if (temp1 == NULL && temp2 == NULL) 
					sp77sprintf(errormsg,1000,WDVH_ERR_TEXT_UNLOCK_MISSING_BRACKETS);
				/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
				WDVCAPI_IdStringInitValue(sLockToken);
				return WDVH_False;
			}
		} else {
			/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
			sp77sprintf(errormsg,1000,WDVH_ERR_TEXT_UNLOCK_MISSING_LOCKTOKEN);
			/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
			WDVCAPI_IdStringInitValue(sLockToken);
			return WDVH_False;
		}
	}

	/*------------------------------------------------------------------------
		Function:	getDcHeader
	--------------------------------------------------------------------------*/
	WDVH_Bool getDcHeader(sapdbwa_HttpRequestP request,
		                  WDVH_Char *sDc)
	{
		WDVH_Char		*temp;

		if (request==NULL || sDc==NULL)
			return WDVH_False;

		temp = (char*)sapdbwa_GetHeader(request,"SAPDB_DocumentClass");
		if (temp != NULL) {
			strcpy(sDc,temp);
			return WDVH_True;
		} else {
			strcpy(sDc,"");
			return WDVH_False;
		}
	}

	/*------------------------------------------------------------------------
		Function:	getDcHeader
	--------------------------------------------------------------------------*/
	WDVH_Bool getAsyncHeader(sapdbwa_HttpRequestP request,
							 WDVH_Bool *async)
	{
		WDVH_Char		*temp;

		if (request==NULL || async==NULL)
			return WDVH_False;

		temp = (char*)sapdbwa_GetHeader(request,"SAPDB_AsyncIndexing");
		if (temp != NULL) {
			if (strcmp(temp,"F")==0 || strcmp(temp,"f")==0)
				*async = WDVH_False;
			else if (strcmp(temp,"T")==0 || strcmp(temp,"t")==0)
				*async = WDVH_True;
			return WDVH_True;
		} else {
			*async = WDVH_False;
			return WDVH_False;
		}
	}

	/*------------------------------------------------------------------------
		Function:	getDcHeader
	--------------------------------------------------------------------------*/
	WDVH_Bool getDeletedItemsHeader(sapdbwa_HttpRequestP request,
									WDVH_Bool *delitems)
	{
		WDVH_Char		*temp;

		if (request==NULL || delitems==NULL)
			return WDVH_False;

		temp = (char*)sapdbwa_GetHeader(request,"SAPDB_DeletedItems");
		if (temp != NULL) {
			if (strcmp(temp,"F")==0 || strcmp(temp,"f")==0)
				*delitems = WDVH_False;
			else if (strcmp(temp,"T")==0 || strcmp(temp,"t")==0)
				*delitems = WDVH_True;
			return WDVH_True;
		} else {
			*delitems = WDVH_False;
			return WDVH_False;
		}
	}

	/*------------------------------------------------------------------------
		Function:	createCapiUserData
	--------------------------------------------------------------------------*/
	WDVH_Bool createCapiUserData(WDVH_CapiUserData *userdata)
	{
		WDVH_Bool			ok = WDVH_False;
		WDVH_CapiUserData	newUserData = NULL;

		if ( !userdata ) {
			return WDVH_False;
		}
    
		WDVHandler_Allocat( sizeof(struct st_capi_session), (WDVH_UInt1**) &newUserData, &ok );
		if (ok == WDVH_True) {
			newUserData->copyReplyHeaderSent = WDVH_False;
			newUserData->moveReplyHeaderSent = WDVH_False;
			newUserData->lockReplyHeaderSent = WDVH_False;
			newUserData->errorsOccured = WDVH_False;
			newUserData->lockScope = 0;
			newUserData->lockType = 0;
			strcpy(newUserData->lockId,"");
			newUserData->timeout = 0;
			strcpy(newUserData->lockOwner,"");
			newUserData->reply = NULL;
			newUserData->server = NULL;
			newUserData->prefix = NULL;

			*userdata = newUserData;

			return WDVH_True;
		} else {
			*userdata = NULL;

			return WDVH_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	destroyCapiUserData
	--------------------------------------------------------------------------*/
	WDVH_Bool destroyCapiUserData(WDVH_CapiUserData userdata)
	{
		if ( !userdata ) {
			return WDVH_False;
		}
    
		WDVHandler_Free((WDVH_UInt1*)userdata);
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	createWdvhHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool createWdvhHandle(sapdbwa_Handle wa,sapdbwa_DbConnectionP conn, WDVH_Handle *handle)
	{
		WDVH_Bool			ok = WDVH_False;
		WDVH_Handle			newHandle = NULL;

		if ( !handle || wa==NULL || conn==NULL) {
			return WDVH_False;
		}
    
		WDVHandler_Allocat( sizeof(struct st_wdvh_handle), (WDVH_UInt1**) &newHandle, &ok );
		if (ok == WDVH_True) {

			newHandle->wa = NULL;
			newHandle->reply = NULL;
			newHandle->request = NULL;
			newHandle->wdvPutBuffer = NULL;
			newHandle->wdvGetBuffer = NULL;
			strcpy(newHandle->locktoken,"");

			/* allocate memory for put and get buffer */
			WDVHandler_Allocat(sizeof(SAPDB_Char)*(wdvh_max_put_buffer_len+1),(sapdbwa_UInt1**) &(newHandle->wdvPutBuffer),&ok);
			if (!ok) {
				*handle = NULL;
				return WDVH_False;
			}
			strcpy(newHandle->wdvPutBuffer,"");

			WDVHandler_Allocat(sizeof(SAPDB_Char)*(wdvh_max_get_buffer_len+1),(sapdbwa_UInt1**) &(newHandle->wdvGetBuffer),&ok);
			if (!ok) {
				*handle = NULL;
				return WDVH_False;
			}
			strcpy(newHandle->wdvGetBuffer,"");

			/* list for docclass caching */
			createDocClassList(&newHandle->docClassList);

			/* get XMLIMAPI_Handle and connection for XMLIMAPI calls */
			getXmlmdlapiHandle(wa,conn,&newHandle->xmlHandle);

			*handle = newHandle;

			return WDVH_True;
		} else {
			*handle = NULL;

			return WDVH_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	resetWdvhHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool resetWdvhHandle(WDVH_Handle handle)
	{
		if ( !handle ) {
			return WDVH_False;
		}
    
		handle->wa = NULL;
		handle->reply = NULL;
		handle->request = NULL;
		strcpy(handle->locktoken,"");

		/* clear put and get buffer */
		strcpy(handle->wdvPutBuffer,"");
		strcpy(handle->wdvGetBuffer,"");

		/* clear error list of xml handle */
		XMLIMAPI_ClearErrorList(handle->xmlHandle);
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	destroyWdvhHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool destroyWdvhHandle(WDVH_Handle handle)
	{
		if ( !handle ) {
			return WDVH_False;
		}
    
		/* free put and get buffer */
		WDVHandler_Free((WDVH_UInt1*) handle->wdvPutBuffer);
		WDVHandler_Free((WDVH_UInt1*) handle->wdvGetBuffer);

		/* destroy docClassList */
		destroyDocClassList(handle->docClassList);

		/* destroy XMLIMAPI handle */
		destroyXmlmdlapiHandle(handle->xmlHandle);

		WDVHandler_Free((WDVH_UInt1*)handle);
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	createConnUserData
	--------------------------------------------------------------------------*/
	WDVH_Bool createConnUserData(WDVH_ConnUserData *userdata)
	{
		WDVH_Bool			ok = WDVH_False;
		WDVH_ConnUserData	newUserData = NULL;

		if ( !userdata ) {
			return WDVH_False;
		}
    
		WDVHandler_Allocat( sizeof(struct st_conn_user_data), (WDVH_UInt1**) &newUserData, &ok );
		if (ok == WDVH_True) {

			*userdata = newUserData;
			(*userdata)->compress = WDVH_False;

			return WDVH_True;
		} else {
			*userdata = NULL;

			return WDVH_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	destroyConnUserData
	--------------------------------------------------------------------------*/
	WDVH_Bool destroyConnUserData(void *userdata)
	{
		if ( !userdata ) {
			return WDVH_False;
		}
    
		WDVCAPI_DestroyWDV(((WDVH_ConnUserData)userdata)->wdvHandle);
		destroyWdvhHandle(((WDVH_ConnUserData)userdata)->wdvhHandle);
		XMLQSLib_DestroyHandle(((WDVH_ConnUserData)userdata)->qsHandle);

		WDVHandler_Free((WDVH_UInt1*)userdata);
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	createDocClassList
	--------------------------------------------------------------------------*/
	WDVH_Bool createDocClassList(WDVH_DocClassList *list)
	{
		WDVH_Bool			ok = WDVH_False;
		WDVH_DocClassList	newList = NULL;

		if ( !list ) {
			return WDVH_False;
		}
    
		WDVHandler_Allocat( sizeof(struct st_docclass_list), (WDVH_UInt1**) &newList, &ok );
		if (ok == WDVH_True) {

			newList->first = NULL;
			newList->last = NULL;
			*list = newList;

			return WDVH_True;
		} else {
			*list = NULL;

			return WDVH_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	destroyDocClassList
	--------------------------------------------------------------------------*/
	WDVH_Bool destroyDocClassList(WDVH_DocClassList list)
	{
		WDVH_DocClassListItem	current,next;
		
		if ( !list ) {
			return WDVH_False;
		}
    
		/* track down the list to destroy every single item */
		next = list->first;
		while (next) {
			current = next;
			next = current->nextItem;
			WDVHandler_Free((WDVH_UInt1*)current);
		}

		WDVHandler_Free((WDVH_UInt1*)list);
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	initDocClassList
	--------------------------------------------------------------------------*/
	WDVH_Bool initDocClassList(WDVH_DocClassList list)
	{
		WDVH_DocClassListItem	current,next;
		
		if ( !list ) {
			return WDVH_False;
		}
    
		/* track down the list to destroy every single item */
		next = list->first;
		while (next) {
			current = next;
			next = current->nextItem;
			WDVHandler_Free((WDVH_UInt1*)current);
		}

		list->first = NULL;
		list->last = NULL;

		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	addItemDocClassList
	--------------------------------------------------------------------------*/
	WDVH_Bool addItemDocClassList(WDVH_DocClassList		list, 
								  WDVH_Char				*docClassName, 
								  WDVCAPI_XmlDocClassId docClassId)
	{
		WDVH_DocClassListItem	newItem;
		WDVH_Bool			ok = WDVH_False;
		
		if (!list) {
			return WDVH_False;
		}
    
		WDVHandler_Allocat( sizeof(struct st_docclass_list_item), (WDVH_UInt1**) &newItem, &ok );
		if (ok == WDVH_True) {

			strcpy(newItem->docClassName,docClassName);
			SAPDB_memcpy(newItem->docClassId,docClassId,XMLIMAPI_MAX_ID_LEN);
			newItem->nextItem = NULL;
			if (!list->first) {
				/* first item */
				list->first = newItem;
				list->last = newItem;
			} else {
				/* all other items */
				list->last->nextItem = newItem;
				list->last = newItem;
			}

			return WDVH_True;
		} else {

			return WDVH_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	findDocClassList
	--------------------------------------------------------------------------*/
	WDVH_Bool findDocClassList(WDVH_DocClassList		list,
							   WDVH_Char				*docClassName,
							   WDVCAPI_XmlDocClassId	*docClassId)
	{
		WDVH_DocClassListItem	current,next;
		
		if ( !list ) {
			return WDVH_False;
		}
    
		/* track down the list to destroy every single item */
		next = list->first;
		while (next) {
			current = next;
			next = current->nextItem;
			if (strcmp(current->docClassName,docClassName)==0) {
				SAPDB_memcpy(*docClassId,current->docClassId,XMLIMAPI_MAX_ID_LEN);
				return WDVH_True;
			}
		}

		WDVCAPI_IdInitValue(*docClassId);
		return WDVH_False;
	}

	/*------------------------------------------------------------------------
		Function:	getXmlmdlapiHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool getXmlmdlapiHandle(sapdbwa_Handle wa,sapdbwa_DbConnectionP conn,XMLIMAPI_Handle *handle)
	{
		XMLIMAPI_Handle	newHandle;
		sapdbwa_SessionPoolP	_pSessionPool;
		sapdbwa_DbConnectionP   _pConnection;
		SQLHDBC					odbchandle;
		SQLHENV					henvhandle;

		if(!handle || wa==NULL || conn==NULL)
			return WDVH_False;

		if (conn) {
			odbchandle = getOdbcHandle(wa,conn);
			henvhandle = getHenvHandle(wa,conn);
			XMLIMAPI_createHandle(handle, henvhandle, odbchandle);
		}

		return WDVH_False;
	}

	/*------------------------------------------------------------------------
		Function:	destroyXmlmdlapiHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool destroyXmlmdlapiHandle(XMLIMAPI_Handle handle)
	{
		if (handle==NULL)
			return WDVH_False;

		XMLIMAPI_destroyHandle(handle);
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	send initial GUI Html Page
	--------------------------------------------------------------------------*/
	void sendGuiStartPage(sapdbwa_Handle    wa,
						  sapdbwa_HttpReplyP   rep,
						  const SAPDB_Char  *server,
						  const SAPDB_Char	*servicename)
	{
		SAPDB_Char	sAsciiFile[512];
		SAPDB_Char   szPath[256] = "";
		sapdbwa_Bool bRet;
		
		if (wa==NULL || rep==NULL || server==NULL || servicename==NULL)
			return;

		/* send page headers */
		sapdbwa_InitHeader(rep,sapdbwa_Status_Ok, "text/html", NULL, NULL,NULL, NULL );
		sapdbwa_SetHeader (rep,"Expires", "Sat, 01 Jan 2000 00:00:00 GMT");
		sapdbwa_SetHeader (rep,"charset","UTF-8");
		sapdbwa_SendHeader(rep);

		/* send the page */
/*		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_START_PAGE_PART_01,
						 strlen(WDVH_HTML_EMBEDDED_START_PAGE_PART_01));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_START_PAGE_PART_02,
						 strlen(WDVH_HTML_EMBEDDED_START_PAGE_PART_02));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_START_PAGE_PART_03,
						 strlen(WDVH_HTML_EMBEDDED_START_PAGE_PART_03));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_START_PAGE_PART_04,
						 strlen(WDVH_HTML_EMBEDDED_START_PAGE_PART_04));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_START_PAGE_PART_05,
						 strlen(WDVH_HTML_EMBEDDED_START_PAGE_PART_05));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_START_PAGE_PART_06,
						 strlen(WDVH_HTML_EMBEDDED_START_PAGE_PART_06));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_START_PAGE_PART_07,
						 strlen(WDVH_HTML_EMBEDDED_START_PAGE_PART_07));
*/
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_INITIAL_FRAME_PART_01,
						 strlen(WDVH_HTML_EMBEDDED_INITIAL_FRAME_PART_01));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_INITIAL_FRAME_PART_02,
						 strlen(WDVH_HTML_EMBEDDED_INITIAL_FRAME_PART_02));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_INITIAL_FRAME_PART_03,
						 strlen(WDVH_HTML_EMBEDDED_INITIAL_FRAME_PART_03));
		sapdbwa_SendBody(rep,
						 server,
						 strlen(server));
		sapdbwa_SendBody(rep,
						 "/",
						 strlen("/"));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 "/",
						 strlen("/"));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_INITIAL_FRAME_PART_04,
						 strlen(WDVH_HTML_EMBEDDED_INITIAL_FRAME_PART_04));

	}

	/*------------------------------------------------------------------------
		Function:	send GUI Navigation Html Page
	--------------------------------------------------------------------------*/
	void sendGuiNaviPage(sapdbwa_Handle    wa,
						  sapdbwa_HttpReplyP   rep,
						  const SAPDB_Char  *querystring,
						  const SAPDB_Char  *server,
						  const SAPDB_Char	*servicename)
	{
		SAPDB_Char	sAsciiFile[512];
		SAPDB_Char   szPath[256] = "";
		sapdbwa_Bool bRet;
		
		if (wa==NULL || rep==NULL || querystring==NULL || server==NULL || servicename==NULL)
			return;

		/* send page headers */
		sapdbwa_InitHeader(rep,sapdbwa_Status_Ok, "text/html", NULL, NULL,NULL, NULL );
		sapdbwa_SetHeader (rep,"Expires", "Sat, 01 Jan 2000 00:00:00 GMT");
		sapdbwa_SetHeader (rep,"charset","UTF-8");
		sapdbwa_SendHeader(rep);

		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_01,
						 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_01));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_02,
						 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_02));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_03,
						 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_03));
/*		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
*/		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_04,
						 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_04));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_05,
						 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_05));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_06,
						 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_06));
/*		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
*/		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_07,
						 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_07));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_08,
						 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_08));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_09,
						 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_09));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_10,
						 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_10));
		sapdbwa_SendBody(rep,
						 server,
						 strlen(server));
		sapdbwa_SendBody(rep,
						 "/",
						 strlen("/"));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_11,
						 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_11));
		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_12,
						 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_12));
/*		sapdbwa_SendBody(rep,
						 servicename,
						 strlen(servicename));
*/		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_13,
						 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_13));
		if (strcmp(querystring,WDVH_HTML_EMBEDDED_GUI_NAVI_TAB1)==0) {
			sapdbwa_SendBody(rep,
							 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_16,
							 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_16));
		} else if (strcmp(querystring,WDVH_HTML_EMBEDDED_GUI_NAVI_TAB2)==0) {
			sapdbwa_SendBody(rep,
							 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_17,
							 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_17));
		} else if (strcmp(querystring,WDVH_HTML_EMBEDDED_GUI_NAVI_TAB3)==0) {
			sapdbwa_SendBody(rep,
							 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_15,
							 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_15));
		} else if (strcmp(querystring,WDVH_HTML_EMBEDDED_GUI_NAVI_TAB4)==0) {
			sapdbwa_SendBody(rep,
							 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_14,
							 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_14));
		}
		sapdbwa_SendBody(rep,
						 WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_18,
						 strlen(WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_18));

	}

	WDVH_Bool getParameterValue(const WDVH_Char *pParamName,
				 	 		    sapdbwa_HttpRequestP req,
				 	 		    WDVH_Char *sValue)
	{
		sapdbwa_StringSeqP	__values;
		
		if (pParamName==NULL || req==NULL || sValue==NULL)
			return WDVH_False;

		__values = sapdbwa_CreateStringSeq();

		strcpy(sValue,"");
		if (sapdbwa_GetParameterValues(req, pParamName, __values) == sapdbwa_True) {
			if (sapdbwa_GetNumElem(__values) > 0) {
				strcpy(sValue,sapdbwa_GetStringByIndex(__values,0));
				return WDVH_True;
			} 
		} else {
			return WDVH_False;
		}
		return WDVH_False;
	}

	const WDVH_Char* getParameterValueWithoutCopy(const WDVH_Char *pParamName,
				 	 					sapdbwa_HttpRequestP req)
	{
		sapdbwa_StringSeqP	__values;

		if (pParamName==NULL || req==NULL)
			return NULL;

		__values = sapdbwa_CreateStringSeq();

		if (sapdbwa_GetParameterValues(req, pParamName, __values) == sapdbwa_True) {
			if (sapdbwa_GetNumElem(__values) > 0) {
/*				strcpy(sValue,sapdbwa_GetStringByIndex(__values,0));*/
				return sapdbwa_GetStringByIndex(__values,0);
			} 
		} else {
			return NULL;
		}
		return NULL;
	}


	void transformFileSize(WDVH_Char *in, WDVH_Char *out, WDVH_Bool complete)
	{
		WDVH_Char	strSize[50];
		/* WDVH_ULong	size=0; */
		WDVH_Double size=0;
		WDVH_Int2	firstpos,i,pos;
		
		if (in==NULL || out==NULL)
			return;

		/* size = atol(in); */
		size = strtod(in,(char**)NULL);
		firstpos = strlen(in)%3;
		if (firstpos == 0)
			firstpos = 3;
		pos = 0;
		strcpy(strSize,"");
		for (i=0;i<strlen(in);i++) {
			if (i==firstpos) { 
				strSize[pos++] = ',';
				firstpos+=3;
			}
			strSize[pos++] = in[i];
		}
		strSize[pos] = '\0';
		if (size < 1024) 
			sprintf(out,"%.0f Bytes",size);
		else if (size / 1024 >= 1 && size / 1024 < 1024) 
			if (complete)
				sprintf(out,"%.0f KB (%s Bytes)",size/1024,strSize);
			else
				sprintf(out,"%.0f KB",size/1024);
		else if (size / (1024*1024) >= 1 && size / (1024*1024) < 1024) 
			if (complete)
/*				sprintf(out,"%d.%03d MB (%s Bytes)",size/(1024*1024),(int)((float)(size%(1024*1024))/(1024*1024)*1000),strSize);*/
				sprintf(out,"%.3f MB (%s Bytes)",size/(1024*1024),strSize);
			else
/*				sprintf(out,"%d.%03d MB",size/(1024*1024),(int)((float)(size%(1024*1024))/(1024*1024)*1000)); */
				sprintf(out,"%.3f MB",size/(1024*1024));
		else 
			if (complete)
/*				sprintf(out,"%d.%03d GB (%s Bytes)",size/(1024*1024*1024),(int)((float)(size%(1024*1024*1024))/(1024*1024*1024)*1000),strSize);*/
				sprintf(out,"%.3f GB (%s Bytes)",size/(1024*1024*1024),strSize);
			else
/*				sprintf(out,"%d.%03d GB",size/(1024*1024*1024),(int)((float)(size%(1024*1024*1024))/(1024*1024*1024)*1000)); */
				sprintf(out,"%.3f GB",size/(1024*1024*1024));
	}

	void WDVH_EscapeQuotes(WDVH_Char *oldstring, WDVH_Int4 len, WDVH_Char *newstring, WDVH_Int4 len_new)
	{
		WDVH_Int4 posnew,i;

		if (oldstring==NULL || newstring==NULL)
			return;

		posnew = 0;
		for (i=0;i<=len;i++) {
			if (oldstring[i]=='"') {
				if (posnew < len_new - 7) {
					newstring[posnew++] = '&';
					newstring[posnew++] = 'q';
					newstring[posnew++] = 'u';
					newstring[posnew++] = 'o';
					newstring[posnew++] = 't';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]=='&') {
				if (posnew < len_new - 6) {
					newstring[posnew++] = '&';
					newstring[posnew++] = 'a';
					newstring[posnew++] = 'm';
					newstring[posnew++] = 'p';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]=='°') {
				if (posnew < len_new - 7) {
					newstring[posnew++] = '&';
					newstring[posnew++] = '#';
					newstring[posnew++] = '1';
					newstring[posnew++] = '8';
					newstring[posnew++] = '6';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]=='²') {
				if (posnew < len_new - 7) {
					newstring[posnew++] = '&';
					newstring[posnew++] = 's';
					newstring[posnew++] = 'u';
					newstring[posnew++] = 'p';
					newstring[posnew++] = '2';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]=='³') {
				if (posnew < len_new - 7) {
					newstring[posnew++] = '&';
					newstring[posnew++] = 's';
					newstring[posnew++] = 'u';
					newstring[posnew++] = 'p';
					newstring[posnew++] = '3';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]=='´') {
				if (posnew < len_new - 8) {
					newstring[posnew++] = '&';
					newstring[posnew++] = 'a';
					newstring[posnew++] = 'c';
					newstring[posnew++] = 'u';
					newstring[posnew++] = 't';
					newstring[posnew++] = 'e';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]=='µ') {
				if (posnew < len_new - 8) {
					newstring[posnew++] = '&';
					newstring[posnew++] = 'm';
					newstring[posnew++] = 'i';
					newstring[posnew++] = 'c';
					newstring[posnew++] = 'r';
					newstring[posnew++] = 'o';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]=='€') {
				if (posnew < len_new - 8) {
					newstring[posnew++] = '&';
					newstring[posnew++] = '#';
					newstring[posnew++] = '8';
					newstring[posnew++] = '3';
					newstring[posnew++] = '6';
					newstring[posnew++] = '4';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]=='§') {
				if (posnew < len_new - 7) {
					newstring[posnew++] = '&';
					newstring[posnew++] = '#';
					newstring[posnew++] = '1';
					newstring[posnew++] = '6';
					newstring[posnew++] = '7';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]==' ') {
				if (posnew < len_new - 4) {
					newstring[posnew++] = '%';
					newstring[posnew++] = '2';
					newstring[posnew++] = '0';
				}
			} else if (oldstring[i]=='ä') {
				if (posnew < len_new - 7) {
					newstring[posnew++] = '&';
					newstring[posnew++] = 'a';
					newstring[posnew++] = 'u';
					newstring[posnew++] = 'm';
					newstring[posnew++] = 'l';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]=='Ä') {
				if (posnew < len_new - 7) {
					newstring[posnew++] = '&';
					newstring[posnew++] = 'A';
					newstring[posnew++] = 'u';
					newstring[posnew++] = 'm';
					newstring[posnew++] = 'l';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]=='ö') {
				if (posnew < len_new - 7) {
					newstring[posnew++] = '&';
					newstring[posnew++] = 'o';
					newstring[posnew++] = 'u';
					newstring[posnew++] = 'm';
					newstring[posnew++] = 'l';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]=='Ö') {
				if (posnew < len_new - 7) {
					newstring[posnew++] = '&';
					newstring[posnew++] = 'O';
					newstring[posnew++] = 'u';
					newstring[posnew++] = 'm';
					newstring[posnew++] = 'l';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]=='ü') {
				if (posnew < len_new - 7) {
					newstring[posnew++] = '&';
					newstring[posnew++] = 'u';
					newstring[posnew++] = 'u';
					newstring[posnew++] = 'm';
					newstring[posnew++] = 'l';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]=='Ü') {
				if (posnew < len_new - 7) {
					newstring[posnew++] = '&';
					newstring[posnew++] = 'U';
					newstring[posnew++] = 'u';
					newstring[posnew++] = 'm';
					newstring[posnew++] = 'l';
					newstring[posnew++] = ';';
				}
			} else if (oldstring[i]=='ß') {
				if (posnew < len_new - 8) {
					newstring[posnew++] = '&';
					newstring[posnew++] = 's';
					newstring[posnew++] = 'z';
					newstring[posnew++] = 'l';
					newstring[posnew++] = 'i';
					newstring[posnew++] = 'g';
					newstring[posnew++] = ';';
				}
			} else {
				if (posnew < len_new)
					newstring[posnew++] = oldstring[i];
			}
		}
		newstring[posnew] = '\0';
	}

	/*------------------------------------------------------------------------
		Function:	WDVH_UnEscapeUri
	--------------------------------------------------------------------------*/
	void WDVH_UnEscapeUri(WDVH_PChar uri) 
	{
		if (uri==NULL)
			return;

		wd21_UnescapeUrl(uri,strlen(uri));
		if (strcmp(uri,"")==0) 
			strcpy(uri,"/");
	}

	/*------------------------------------------------------------------------
		Function:	WDVH_CheckContentType
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123371
	--------------------------------------------------------------------------*/
	WDVH_Bool WDVH_CheckContentType(WDVH_PChar contenttype) 
	{
		/* contenttype must fit to this syntax:
		token          = 1*<any CHAR except CTLs or tspecials>
        tspecials      = "(" | ")" | "<" | ">" | "@"
                       | "," | ";" | ":" | "\" | <">
                       | "/" | "[" | "]" | "?" | "="
                       | "{" | "}" | SP | HT
		CTL            = <any US-ASCII control character
                         (octets 0 - 31) and DEL (127)>
        CHAR           = <any US-ASCII character (octets 0 - 127)>
        SP             = <US-ASCII SP, space (32)>
        HT             = <US-ASCII HT, horizontal-tab (9)>

        media-type     = type "/" subtype
        type           = token
        subtype        = token

		no whitespace allowed between type and "/" as well as between "/" and subtype
		*/
		WDVH_Char	*subtype;
		WDVH_Char	*slash;
		WDVH_Int2	i;

		if (contenttype == NULL)
			return WDVH_False;

		slash = strchr(contenttype,'/');
		subtype = slash+1;
		if (slash==NULL)
			return WDVH_False;

		/* at least there is something similar to type/subtype
		   check the characters and whitespace */
		for (i=0;i<slash-contenttype;i++) {
			if (contenttype[i] == '(' || contenttype[i] == ')' || contenttype[i] == '<' || contenttype[i] == '>' || contenttype[i] == '@' ||
				contenttype[i] == ',' || contenttype[i] == ';' || contenttype[i] == ':' || contenttype[i] == '\\' || contenttype[i] == '"' ||
				contenttype[i] == '/' || contenttype[i] == '[' || contenttype[i] == ']' || contenttype[i] == '?' || contenttype[i] == '=' ||
				contenttype[i] == '{' || contenttype[i] == '}' || contenttype[i] == ' ' || contenttype[i] == '\t' ||
				(contenttype[i] >= 0x00 && contenttype[i] <= 0x1F) || contenttype[i] >= 0x7F) {
				return WDVH_False;
			}
		}
		for (i=0;i<strlen(subtype);i++) {
			if (subtype[i] == '(' || subtype[i] == ')' || subtype[i] == '<' || subtype[i] == '>' || subtype[i] == '@' ||
				subtype[i] == ',' || subtype[i] == ';' || subtype[i] == ':' || subtype[i] == '\\' || subtype[i] == '"' ||
				subtype[i] == '/' || subtype[i] == '[' || subtype[i] == ']' || subtype[i] == '?' || subtype[i] == '=' ||
				subtype[i] == '{' || subtype[i] == '}' || subtype[i] == ' ' || subtype[i] == '\t' ||
				(subtype[i] >= 0x00 && subtype[i] <= 0x1F) || subtype[i] >= 0x7F) {
				return WDVH_False;
			}
		}
		return WDVH_True;

	}

	/*------------------------------------------------------------------------
		Function:	WDVH_CheckPropertySet
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123441
	--------------------------------------------------------------------------*/
	WDVH_Bool WDVH_CheckPropertySet(WDVH_PChar tag) 
	{
		if (tag==NULL)
			return WDVH_False;

		/* Live Properties that are forbidden to change:
		<!ELEMENT creationdate (#PCDATA) >
		<!ELEMENT getcontentlength (#PCDATA) >
		<!ELEMENT getetag (#PCDATA) >
		<!ELEMENT getlastmodified (#PCDATA) >
		<!ELEMENT lockdiscovery (activelock)* >
		<!ELEMENT resourcetype ANY >
		<!ELEMENT source (link)* >
		<!ELEMENT supportedlock (lockentry)* > */
		if (strcmp(tag,"creationdate")==0 || strcmp(tag,"getcontentlength")==0 || strcmp(tag,"getetag")==0 ||
			strcmp(tag,"getlastmodified")==0 || strcmp(tag,"lockdiscovery")==0 || strcmp(tag,"resourcetype")==0 ||
			strcmp(tag,"source")==0 || strcmp(tag,"supportedlock")==0) {
			return WDVH_False;
		}
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	WDVH_CheckPropertyRemove
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123441
	--------------------------------------------------------------------------*/
	WDVH_Bool WDVH_CheckPropertyRemove(WDVH_PChar tag) 
	{
		if (tag==NULL)
			return WDVH_False;

		/* Live Properties that are forbidden to change:
		<!ELEMENT creationdate (#PCDATA) >
		<!ELEMENT displayname (#PCDATA) >
		<!ELEMENT getcontentlanguage (#PCDATA) >
		<!ELEMENT getcontentlength (#PCDATA) >
		<!ELEMENT getcontenttype (#PCDATA) >
		<!ELEMENT getetag (#PCDATA) >
		<!ELEMENT getlastmodified (#PCDATA) >
		<!ELEMENT lockdiscovery (activelock)* >
		<!ELEMENT resourcetype ANY >
		<!ELEMENT source (link)* >
		<!ELEMENT supportedlock (lockentry)* >*/

		if (strcmp(tag,"creationdate")==0 || strcmp(tag,"displayname")==0 || strcmp(tag,"getcontentlanguage")==0 || 
			strcmp(tag,"getcontenttype")==0 || strcmp(tag,"getcontentlength")==0 || strcmp(tag,"getetag")==0 ||
			strcmp(tag,"getlastmodified")==0 || strcmp(tag,"lockdiscovery")==0 || strcmp(tag,"resourcetype")==0 ||
			strcmp(tag,"source")==0 || strcmp(tag,"supportedlock")==0) {
			return WDVH_False;
		}
		return WDVH_True;
	}

	void WDVH_sendBodyChunk(sapdbwa_HttpReplyP reply, WDVH_Char *data)
	{
		WDVH_Char	chunk[101];

		if (reply==NULL || data==NULL)
			return;

		if (strlen(data)>0) {
			sp77sprintf(chunk,100,"%x%c%c",strlen(data),0x0D,0x0A);
			sapdbwa_SendBody(reply,chunk,strlen(chunk));
			sapdbwa_SendBody(reply,data,strlen(data));
			sp77sprintf(chunk,100,"%c%c",0x0D,0x0A);
			sapdbwa_SendBody(reply,chunk,strlen(chunk));
		}
	}

	void WDVH_sendBodyChunkPropfind(sapdbwa_HttpReplyP reply, WDVH_Char *data, WDVH_Bool isMSWebFolder)
	{
		WDVH_Char	chunk[101];

		if (reply==NULL || data==NULL)
			return;

		if (strlen(data)>0) {
			if (isMSWebFolder == WDVH_False) {
				sp77sprintf(chunk,100,"%x%c%c",strlen(data),0x0D,0x0A);
				sapdbwa_SendBody(reply,chunk,strlen(chunk));
			}
			sapdbwa_SendBody(reply,data,strlen(data));
			if (isMSWebFolder == WDVH_False) {
				sp77sprintf(chunk,100,"%c%c",0x0D,0x0A);
				sapdbwa_SendBody(reply,chunk,strlen(chunk));
			}
		}
	}

	void WDVH_sendBodyChunkEnd(sapdbwa_HttpReplyP reply)
	{
		WDVH_Char            chunk[20];

		if (reply==NULL)
			return;

		chunk[0] = '0';
		chunk[1] = 0x0D;
		chunk[2] = 0x0A;
		chunk[3] = 0x0D;
		chunk[4] = 0x0A;
		chunk[5] = 0x00;
		sapdbwa_SendBody(reply,chunk,strlen(chunk));

	}
	
	WDVH_Char bin2hex (WDVH_Char ch)         /* return ascii char for hex value of */
	{  									     /* rightmost 4 bits of input */
		ch = ch & 0x0f;            /* mask off right nibble  - & bitwise AND*/
		ch += '0';                 /* make ascii '0' - '9'  */
		if (ch > '9')
			ch += 7;               /* account for 7 chars between '9' and 'A' */
		return (ch);
	}

	WDVH_Int4 AlphaNumeric (WDVH_Char ch){
		return ((ch >='a') && (ch <= 'z') ||   /* logical AND &&, OR || */
				(ch >='A') && (ch <= 'Z') ||
				(ch >='0') && (ch <= '9') ||
				(ch =='!') ||
				(ch =='$') ||
				(ch =='\'') ||
				(ch =='(') ||
				(ch ==')') ||
				(ch ==',') ||
				(ch =='-') ||
				(ch =='.') ||
				(ch =='=') ||
				(ch =='@') ||
				(ch =='_') ||
				(ch =='~') ||
				(ch =='/') );
	}


	WDVH_Int4 URLencode (const WDVH_Char * plain, WDVH_Char * encode, WDVH_Int4 maxlen){
		WDVH_Char ch;            /* each char, use $t2 */
		WDVH_Char * limit;       /* point to last available location in encode */
		WDVH_Char * start;       /* save start of encode for length calculation */

		start = encode;
		limit = encode + maxlen - 4;    /* need to store 3 chars and a zero */
		ch = *plain ++;    /* get first character */
		while (ch != 0) {       /* end of string, asciiz */
			if (ch == ' ') {
				* encode ++ = '%';
				* encode ++ = '2';
				* encode ++ = '0';
			} else if (AlphaNumeric (ch))
				* encode ++ = ch;
			else {
				* encode ++ = '%';
				* encode ++ = bin2hex (ch >> 4); /*shift right for left nibble*/
				* encode ++ = bin2hex (ch);      /* right nibble */
			}
			ch = *plain ++;               /* ready for next character */
			if (encode > limit){
				*encode = 0;        /* still room to terminate string */
				return (-1);        /* get out with error indication */
			}
		}
		* encode = 0;               /* store zero byte to terminate string */
		return (encode - start);    /* done, return count of characters */
	}   /* URLencode*/
