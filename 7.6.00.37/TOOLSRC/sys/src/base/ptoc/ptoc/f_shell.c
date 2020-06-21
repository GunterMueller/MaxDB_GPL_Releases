/*


    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end


*/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <process.h>

static	int   sinit = 0;
static	int   sexec = 0;
static	char  spath [_MAX_PATH];
static	char  sname [_MAX_FNAME];
static  char *limit = "\"";

int	kshell( const char *command )

{ int	code = -1;
  if ( !sinit )
  { char *shell = getenv ("SHELL");
    if ( shell != NULL )
    { strcpy( spath , shell );
      shell = spath;
      while ( *shell != ' ' && *shell != '\0' )
      { if  ( *shell == '/' )
              *shell  = '\\';
        shell ++;
      }
      _splitpath (spath, NULL, NULL, sname, NULL);
    } else *sname = '\0';
    sinit ++;
  }
  if ( *sname != '\0')
  { char *cmd = (char *)malloc( strlen( command ) + 3 );
    strcpy( cmd , limit );
    strcat( cmd , command );
    strcat( cmd , limit );
    if (sexec) code = execl( spath , sname , "-c" , cmd , NULL );
    else code = spawnl( _P_WAIT , spath , sname , "-c" , cmd , NULL );
    free( cmd );
  } else code = system( command );
  return code;
}

int	ksystem( const char *command )

{ sexec = 0;
  return kshell( command );
}

int	kshexec( const char *command )

{ sexec = 1;
  return kshell( command );
}
