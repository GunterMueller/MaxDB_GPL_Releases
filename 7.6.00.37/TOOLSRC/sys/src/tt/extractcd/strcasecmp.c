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
#ifdef RELIANT

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int strcasecmp (char *str0, char *str1) {

	int rc;
	char *ptr;
	char *mystr0;
	char *mystr1;

	mystr0 = malloc (strlen (str0) + 1);
	mystr1 = malloc (strlen (str1) + 1);

	strcpy (mystr0, str0);
	strcpy (mystr1, str1);

	for (ptr = mystr0; *ptr != '\0'; ptr++) {
		*ptr = toupper (*ptr);
	} 

	for (ptr = mystr1; *ptr != '\0'; ptr++) {
		*ptr = toupper (*ptr);
	} 

	rc = strcmp (mystr0, mystr1);	
	free (mystr0);
	free (mystr1);
	return (rc);
}
#endif

