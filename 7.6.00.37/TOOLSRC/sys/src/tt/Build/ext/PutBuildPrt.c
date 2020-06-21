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
#include <stdio.h>

/* nothing to do if not SAPDB_INTERNAL */
int main ( int argc, char *argv[] )
{
	if ( getenv("SAPDB_INTERNAL"))
	{
		fprintf(stderr,"WARNING: SAPDB_INTERNAL is set but PutBuildNr don't save the protocol in the database !\n"); 
	}
	return (0);
}
