/*********************************************************
 * Name:	vwd0023c.h
 * Version:	V1.01
 * Purpose:	Entries
 * Author:	Tony Guepin
 * Copyright:	Copyright 1996. Juergen Hoehe & Partner Infolytics & Tools
 * Date:	may 1996
 * --------------------------------------------------------



    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

typedef struct st_entry_st {
 unsigned long		namelg;
 char			*name;
 unsigned long		wertlg;
 char			*wert;
} * st_entry_p, st_entry;

char *EntryWertCase
(
 unsigned int	ncur_entry,
 st_entry	entries[],
 char		Name[]
);

int SetEntryWert
(
 unsigned int	*ncur_entry,
 st_entry	*entries[],
 unsigned int	NameLg,
 char		Name[],
 unsigned int	ValueLg,
 char		Value[],
 int		ErrorTextSize,
 char		ErrorText[]
);

int UpdEntryWert
(
 unsigned int	*ncur_entry,
 st_entry	*entries[],
 unsigned int	NameLg,
 char		Name[],
 unsigned int	ValueLg,
 char		Value[],
 int		ErrorTextSize,
 char		ErrorText[]
);

void DeleteEntries
(
 unsigned int	ncur_entry,
 st_entry	entries[]
);
