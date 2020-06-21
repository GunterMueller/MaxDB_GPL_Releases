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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define BUILDFILENAME "Build.dat"
#define NOT_OK -1


int main ( int argc, char *argv[] )
{
	char *sapdb_internal, *own;
	char *filename;
	int  fh;
	int  BuildNumber;
	int  handledBytes; 

	if ( getenv("SAPDB_INTERNAL"))
	{
		/* nothing to do */
		fprintf(stderr,"WARNING: SAPDB_INTERNAL is set but GetBuildNr use only local buildnumber !\n"); 
	}
	
	own=getenv("OWN");
	if ( own == 0 )
	{
		(void) fprintf (stderr, "Error: OWN not set !\n"); 
		exit ( NOT_OK );
	}

	filename = malloc (strlen (own) + strlen (BUILDFILENAME) + 2);

	strcpy(filename, own);
	strcat (filename, "/");
	strcat (filename, BUILDFILENAME);
	
#if defined (WIN32)
	fh = open( filename, O_BINARY | O_RDWR );
#else
	fh = open( filename, O_RDWR );
#endif		
	if( fh == -1 )
	{
		/* there aren't a buildnumber file */
		if ( errno == ENOENT )
#if defined (UNIX)
		fh = open( filename, O_RDWR | O_CREAT | O_BINARY , _S_IREAD | _S_IWRITE );
#else
		fh = open( filename, O_RDWR | O_CREAT);
#endif
		if ( fh == -1 )
		{
			fprintf (stderr, "Error while opening %s (%d)\n", filename, errno );
			exit (NOT_OK);
		}
	}
	else
	{
		if ( (handledBytes = read (fh, &BuildNumber, sizeof(BuildNumber))) < 0  )
		{
			fprintf (stderr, "Error while reading %s (%d)\n", filename, errno);
			exit (NOT_OK);
		}
	}
	
	BuildNumber++;
	
	if (lseek (fh, 0, SEEK_SET) < 0)
	{
		fprintf (stderr, "Error while seeking in %s (%d)\n", filename, errno );
		exit (NOT_OK);
	}

	if ( write (fh, &BuildNumber, sizeof(BuildNumber)) < 0)
	{
		fprintf (stderr, "Error while writing %s (%d)\n", filename, errno );
		exit (NOT_OK);
	}
	close (fh);
	BuildNumber+=169000000;

	(void) fprintf ( stdout, "BuildNr: %d", BuildNumber );
}
