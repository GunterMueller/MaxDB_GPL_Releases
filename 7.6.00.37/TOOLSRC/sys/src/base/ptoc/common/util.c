/* @(#)common/util.c

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
#include <ctype.h>

#if ! OSF1
strcmp(s,t)       /*  liefert <0 wenn s<t   */
char *s,*t;       /*          =0 wenn s=t   */
		  /*          >0 wenn s>t   */
{
  int i;
  for (i=0; s[i] == t[i]; i++)
    if (s[i] == '\0')
      return(0);
  return(s[i] - t[i]);
}

atoi(s)        /*   Zeichenkette in int umwandeln   */
char s[];
{
  int i,n;

  n=0;
  for (i=0; s[i]>='0' && s[i]<='9'; ++i)
    n=10*n + s[i]-'0';
  return(n);
}

#endif
