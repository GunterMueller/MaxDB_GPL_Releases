/* @(#)tools/makedepend/mkrules.c

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
#define MAXLINE 512

main(argc, argv)
int argc;
char **argv;
{
	char objectfile[80];
	int p2flag = 0;
	int i;
	int len = 0;
	int cnt = 0;

	while(++cnt < argc) {
		if (strcmp(argv[cnt],"-p2") == 0)
			p2flag = 1;
		else {
		   strcpy(objectfile, argv[cnt]);
		   i = 0;
		   while(objectfile[i] != '\0' && objectfile[i] != '\n')
			if(objectfile[i++] == '/') len = i;
		   objectfile[i - 1] = 0;
		   if(argv[cnt][strlen(argv[cnt]) - 1] == 'c') {
		        printf("%s%c: %s\n", objectfile + len,'o',argv[cnt]);
			printf("\t$(CC) -c $(CFLAGS) %s\n\n", argv[cnt]);
		   }
		   else {
			if (p2flag == 0) {
		            printf("%s%c: %s\n",
		            	objectfile + len,'o',argv[cnt]);
			    printf("\t$(PASCAL) -c $(PFLAGS) %s\n\n",
			    	argv[cnt]);
		            printf("%s%c: %s\n",
		            	objectfile+len,'c',argv[cnt]);
			    printf("\t$(PTOC) $(PFLAGS) %s\n\n",
				argv[cnt]);
		        }
		        else {	
		            printf("%s%c: %s%c\n",
		            	objectfile + len,'o', objectfile + len,'c');
			    printf("\t$(CC) -c $(CFLAGS) %s%c\n\n",
				objectfile + len,'c');
			}		
		   }
		}
	}
	return(0);
}
