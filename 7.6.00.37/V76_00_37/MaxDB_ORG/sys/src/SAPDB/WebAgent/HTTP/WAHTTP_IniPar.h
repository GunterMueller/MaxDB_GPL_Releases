/*!**********************************************************************

  module: WAHTTP_IniPar.c

  -----------------------------------------------------------------------

  responsible:  Tony Guepin,
				Markus Oezgen

  special area: SAP DB WWW

  description:  Registry, INI Files, Settings

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

#ifndef WAHTTP_INIPAR_H
#define WAHTTP_INIPAR_H

/****************************************************************************
 ***
 *** Includes
 ***
 ****************************************************************************/

#include "hwd01wadef.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Misc.h"


/****************************************************************************
 ***
 *** 
 ***
 ****************************************************************************/

struct wahttp_alternate_settings {
    char    iniFile[1024];
    char    port[32];
    char    docRoot[1024];
    char    logDir[1024];
    char    threads[32];
};

/****************************************************************************
 ***
 *** 
 ***
 ****************************************************************************/

sapdbwa_UInt4 GetPort();
sapdbwa_Bool GetThreading();
sapdbwa_Bool GetBrowsable();
sapdbwa_Bool GetLogRequests();
char* GetLogDirectory();
char* GetDocumentRoot();
char* GetDefaultDocument();
char* GetMimeTypeFile();
sapdbwa_UInt4 GetMaxThreads();
sapdbwa_Bool GetEnableWebDav();

int IniReadPar( char			 Parameter[],
				unsigned short	 ValueSize,
				char			 Value[],
				unsigned short	 ErrorTextSize,
				char			*ErrorText			);

int IniSetPar( char				 Parameter[],
			   char				 Value[],
			   unsigned short	 ErrorTextSize,
			   char				*ErrorText		);


int InitIniPar ( char                                iniPath[],
                 struct wahttp_alternate_settings   *alternateSettings,
                 char                               *usedIniFile,
                 char                               *errorText,
                 sapdbwa_UInt2                       errorTextSize );

void ExitIniPar( void );

int IniReadParFile( char	 IPAR[],
					DynStr	*Str	);

int IniGetMimeType( unsigned int	 MimeTypeSize,
					char			 MimeType[],
					char			 FileExtension[],
					unsigned short	 ErrorTextSize,
					char			*ErrorText			);

int ReadMimeTypes( char		      *errorText,
				   unsigned short  errorTextSize  );

extern void wahttp_get_info(char *wahttp_name, char *wahttp_version, char *wahttp_build);

#endif
