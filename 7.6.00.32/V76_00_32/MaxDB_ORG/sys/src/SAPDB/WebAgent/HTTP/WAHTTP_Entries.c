/*********************************************************
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

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <malloc.h>
#endif
#include <string.h>
#include <limits.h>

#include "SAPDBCommon/SAPDB_string.h"

#include "SAPDB/WebAgent/HTTP/WAHTTP_Misc.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Entries.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Memory.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Log.h"

char *EntryWertCase
(
 unsigned int	ncur_entry,
 st_entry	entries[],
 char		Name[]
)
{
 unsigned long i = 0;

 for (i=0; i<ncur_entry; i++)
	if (entries[i].name && !strcasecmp(entries[i].name, Name))
		return(entries[i].wert);
 return(NULL);
}

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
)
/* allocate enough to copy a unicode string including the two byte null
 * at the end (instead of the one byte null for c-strings).
 */
{
 st_entry	entry;
 unsigned int	l_ncur_entry = *ncur_entry;
 st_entry	*l_entries = *entries;
 size_t		ucwertsize, ucnamesize;
 tsp00_Bool	ok;

 if (!Name)	return(FALSE);

 if (NameLg == 0)
 {
	entry.name = NULL;
	entry.namelg = 0;
 }
 else {
	entry.namelg = NameLg + 1;
	ucnamesize = entry.namelg * sizeof(char);

	wahttp_SQLALLOCAT ( ucnamesize, &entry.name, &ok );
    if (!ok) {
        /* memory alloc failure	*/
		Log_Error( "SetEntryWert: Memory allocation error!", -1 );
		strcpymax( ErrorTextSize, ErrorText, "SetEntryWert: Memory allocation error!" );
		return(FALSE);
 	}
 }

 if (ValueLg == 0)
 {
	entry.wert = NULL;
	entry.wertlg = 0;
 }
 else {
	entry.wertlg = ValueLg + 1;
	ucwertsize = entry.wertlg * sizeof(char);
	wahttp_SQLALLOCAT ( ucwertsize, &entry.wert, &ok );
    if (!ok) {
        /* memory alloc failure	*/
		wahttp_SQLFREE( entry.name );

		Log_Error( "SetEntryWert: Memory allocation error!", -1 );
		strcpymax( ErrorTextSize, ErrorText, "SetEntryWert: Memory allocation error!" );

		return(FALSE);
 	}
 }

 /* Make sure `l_entries' holds an initial value (for realloc) */
 if (l_ncur_entry == 0)
	l_entries = NULL;

	wahttp_SQLREALLOCAT( (l_ncur_entry + 1)*sizeof(*l_entries), &l_entries, &ok );
	if ( !ok )	{
		if (entry.name) wahttp_SQLFREE( entry.name );
		if (entry.wert) wahttp_SQLFREE( entry.wert );

		Log_Error( "SetEntryWert: Memory reallocation error!", -1 );
		strcpymax(ErrorTextSize, ErrorText, "SetEntryWert: Memory reallocation error!");

		return(FALSE);
	} else {
		l_ncur_entry++;
	}

 if (NameLg > 0)
 {
	strncpy(entry.name, Name, NameLg);
	strcpy(&entry.name[NameLg], "");
 }

 if (ValueLg > 0)
 {
	strncpy(entry.wert, Value, ValueLg);
	strcpy(&entry.wert[ValueLg], "");
 }

 SAPDB_memcpy(&l_entries[l_ncur_entry-1], &entry, sizeof(entry));

 *entries = l_entries;
 *ncur_entry = l_ncur_entry;

 return(TRUE);
}

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
)
/* allocate enough to copy a unicode string including the two byte null
 * at the end (instead of the one byte null for c-strings).
 */
{
 unsigned int	i = 0;
 st_entry	*entry = NULL;
 tsp00_Bool	ok;

 if (!Name || !Value || NameLg == 0 || ValueLg == 0) return(FALSE);

 for (i=0; i<*ncur_entry; i++)
	if ((*entries)[i].name && !strcasecmp((*entries)[i].name, Name))
	{
		entry = &(*entries)[i];
		break;
	}

 if (!entry)
 {
	return(SetEntryWert(ncur_entry, entries,
		   NameLg, Name,
		   ValueLg, Value,
		   ErrorTextSize, ErrorText));
 }

/* if (entry->wertlg < ValueLg + 1)*/
 {
	wahttp_SQLREALLOCAT( ValueLg + 1, &entry->wert, &ok );
	if ( !ok )	{
		Log_Error( "UpdEntryWert: Memory reallocation error!", -1 );
		strcpymax(ErrorTextSize, ErrorText, "UpdEntryWert: Memory reallocation error!");
	}
	entry->wertlg = ValueLg + 1;
 }

 strncpy(entry->wert, Value, ValueLg);
 strcpy(&entry->wert[ValueLg], "");
 return(TRUE);
}

void DeleteEntries
(
 unsigned int	ncur_entry,
 st_entry	entries[]
)
{
 unsigned int	i;

 for (i=0; i<ncur_entry; i++)
 {
	if (entries[i].name && entries[i].namelg > 0)
		wahttp_SQLFREE(entries[i].name);
	if (entries[i].wert && entries[i].wertlg > 0)
		wahttp_SQLFREE(entries[i].wert);
 }

 if (ncur_entry > 0)
	wahttp_SQLFREE(entries);
}


#ifdef pipo

#ifdef DEVELOPMENT
#ifdef WDUNIX
#define TMP_ENTRIES_IN	"/tmp/entries"
#define TMP_ENTRIES_OUT	"/tmp/entries"
#elif _WIN32
#define TMP_ENTRIES_IN	"c:\\temp\\entries"
#define TMP_ENTRIES_OUT	"c:\\temp\\entries"
#else
#error "whaaaat"
#endif
#endif


void PrintEntries
(
 FILEHANDLE	*fout,
 unsigned int	ncur_entry,
 st_entry 	entries[]
)
{
 unsigned long	i;
 ucchar		*n = NULL, *v = NULL;
 ucchar		leeg[sizeof(ucchar)*2];

 uc_Ascii2UnicodeChar(leeg[0], '\0');

 uc_fhprintf(fout, "<!--Entries are:-->\n");
 uc_fhprintf(fout, "<!--ncur_entry:%d-->\n", ncur_entry);

 for (i=0; i<ncur_entry; i++) {
	if (entries[i].name)
		n = entries[i].name;
	else
		n = leeg;
	if (entries[i].wert)
		v = entries[i].wert;
	else
		v = leeg;

	uc_fhprintf(fout, "<!--name:(%d) `%us' wert:(%d) `%us'-->\n",
		   uc_strlen(n), n, uc_strlen(v), v);
 }
 uc_fhprintf(fout, "<!--End of Entries-->\n");
}

void PrintEntriesFormat
(
 FILEHANDLE	*fout,
 char		Format[],
 unsigned int	ncur_entry,
 st_entry 	entries[]
)
{
 unsigned long	i;
 ucchar		*n = NULL, *v = NULL;
 ucchar		leeg[sizeof(ucchar)+1];

 uc_Ascii2UnicodeChar(leeg[0], '\0');

 for (i=0; i<ncur_entry; i++) {
	if (entries[i].name)
		n = entries[i].name;
	else
		n = leeg;
	if (entries[i].wert)
		v = entries[i].wert;
	else
		v = leeg;

	uc_fhprintf(fout, Format, n, v);
 }
}

void entries2file
(
 unsigned int	ncur_entry,
 st_entry	entries[]
)
{
#ifdef DEVELOPMENT
 unsigned long	i;
 unsigned long	l;
 FILE			*f =fopen(TMP_ENTRIES_OUT, "w");
 char		*n = NULL, *v = NULL;

 if (f == NULL) return;

 fwrite(&ncur_entry, sizeof(ncur_entry), 1, f);

 for (i=0; i<ncur_entry; i++) {

	fwrite(&entries[i].namelg, 1, sizeof(entries[i].namelg), f);
	fwrite(entries[i].name, 1, entries[i].namelg * sizeof(ucchar), f);

	fwrite(&entries[i].wertlg, 1, sizeof(entries[i].wertlg), f);
	fwrite(entries[i].wert, 1, entries[i].wertlg * sizeof(ucchar), f);

 }
 fclose(f);
#endif
}

int file2entries
(
 unsigned int	*ncur_entry,
 st_entry	*entries[]
)
{
#ifdef DEVELOPMENT
 unsigned long		i;
 unsigned long		l;
 FILE			*f =fopen(TMP_ENTRIES_IN, "r");
 ucchar			*wert = NULL;
 unsigned long		wertlg;
 ucchar			*name = NULL;
 unsigned long		namelg;
 unsigned int		tmp_ncur_entry;
 tsp00_Bool	ok;

 *ncur_entry = 0;
 *entries = NULL;

 if (f == NULL) 
	return(FALSE);

 if (0 == fread(&tmp_ncur_entry, sizeof(tmp_ncur_entry), 1, f)) {
	*ncur_entry = 0;
	return(FALSE);
 }

 for (i=0; i<tmp_ncur_entry; i++) {

	fread(&namelg, 1, sizeof(namelg), f);
	wahttp_SQLREALLOCAT( namelg * sizeof(ucchar), &name, &ok );
	if ( !ok )	{
		Log_Error( "file2entries: Memory reallocation error!", -1 );
	}
	fread(name, 1, namelg * sizeof(ucchar), f);

	fread(&wertlg, 1, sizeof(wertlg), f);
	wahttp_SQLREALLOCAT( wertlg * sizeof(ucchar), &wert, &ok );
	if ( !ok )	{
		Log_Error( "file2entries: Memory reallocation error!", -1 );
	}
	fread(wert, 1, wertlg * sizeof(ucchar), f);

	SetEntryWert(ncur_entry, entries, namelg, name, wertlg, wert,
			0, NULL);


 }
 fclose(f);
#endif
 return(TRUE);
}

ucchar *EntryWert
(
 unsigned int	ncur_entry,
 st_entry	entries[],
 ucchar		Name[]
)
{
 unsigned long i = 0;

 for (i=0; i<ncur_entry; i++)
	if (entries[i].name && !uc_strcmp(entries[i].name, Name))
		return(entries[i].wert);
 return(NULL);
}

ucchar *EntryWertCase
(
 unsigned int	ncur_entry,
 st_entry	entries[],
 ucchar		Name[]
)
{
 unsigned long i = 0;

 for (i=0; i<ncur_entry; i++)
	if (entries[i].name && !uc_strcasecmp(entries[i].name, Name))
		return(entries[i].wert);
 return(NULL);
}

ucchar *EntryWertasc
(
 unsigned int	ncur_entry,
 st_entry	entries[],
 char		Name[]
)
{
 unsigned long i = 0;

 for (i=0; i<ncur_entry; i++)
	if (entries[i].name && !uc_ascstrcmp(entries[i].name, Name))
		return(entries[i].wert);
 return(NULL);
}

ucchar *EntryWertCaseAsc
(
 unsigned int	ncur_entry,
 st_entry	entries[],
 char		Name[]
)
{
 unsigned long i = 0;

 for (i=0; i<ncur_entry; i++)
	if (entries[i].name && !uc_ascstrcasecmp(entries[i].name, Name))
		return(entries[i].wert);
 return(NULL);
}

ucchar *EntryWertNo
(
 unsigned int	ncur_entry,
 st_entry	entries[],
 unsigned int	no
)
{
 if (entries && no >= 1 && no <= ncur_entry)
	return(entries[no-1].wert);
 return(NULL);
}

ucchar *EntryNameNo
(
 unsigned int	ncur_entry,
 st_entry	entries[],
 unsigned int	no
)
{
 if (entries && no >= 1 && no <= ncur_entry)
	return(entries[no-1].name);
 return(NULL);
}

int ascSetEntryWert
(
 unsigned int	*ncur_entry,
 st_entry	*entries[],
 unsigned int	NameLg,
 char		Name[],
 unsigned int	ValueLg,
 char		Value[],
 int		ErrorTextSize,
 ucchar		ErrorText[]
)
/* allocate enough to copy a unicode string including the two byte null
 * at the end (instead of the one byte null for c-strings).
 */
{
 st_entry	entry;
 unsigned int	l_ncur_entry = *ncur_entry;
 st_entry	*l_entries = *entries;
 size_t		ucwertsize, ucnamesize;
 tsp00_Bool	ok;

 if (!Name)	return(FALSE);

 if (NameLg == 0)
 {
	entry.name = NULL;
	entry.namelg = 0;
 }
 else {
	entry.namelg = NameLg + 1;
	ucnamesize = entry.namelg * sizeof(ucchar);
	wahttp_SQLALLOCAT ( ucnamesize, &entry.name, &ok );
    if (!ok) {
        /* memory alloc failure	*/
		Log_Error( "ascSetEntryWert: Memory allocation error!", -1 );
		uc_ascstrcpymax(ErrorTextSize, ErrorText, "SetEntryWert: Memory allocation error!", NULL);

		return(FALSE);
 	}
 }

 if (ValueLg == 0)
 {
	entry.wert = NULL;
	entry.wertlg = 0;
 }
 else {
	entry.wertlg = ValueLg + 1;
	ucwertsize = entry.wertlg * sizeof(ucchar);
	wahttp_SQLALLOCAT ( ucwertsize, &entry.wert, &ok );
    if (!ok) {
        /* memory alloc failure	*/
		wahttp_SQLFREE ( entry.name );

		Log_Error( "ascSetEntryWert: Memory allocation error!", -1 );
		uc_ascstrcpymax(ErrorTextSize, ErrorText, "SetEntryWert: Memory allocation error!", NULL);

		return(FALSE);
 	}
 	}
 }

 /* Make sure `l_entries' holds an initial value (for realloc) */
 if (l_ncur_entry == 0)
	l_entries = NULL;

	wahttp_SQLREALLOCAT( (l_ncur_entry + 1)*sizeof(*l_entries), &l_entries, &ok );
	if ( !ok )	{
		if (entry.name) wahttp_SQLFREE(entry.name);
		if (entry.wert) wahttp_SQLFREE(entry.wert);

		Log_Error( "ascSetEntryWert: Memory reallocation error!", -1 );
		uc_ascstrcpymax(ErrorTextSize, ErrorText, "ascSetEntryWert: Memory reallocation error!", NULL);

		return(FALSE);
	} else {
		l_ncur_entry++;
	}

 if (NameLg > 0)
 {
	uc_ascstrncpy(entry.name, Name, NameLg);
	uc_ascstrcpy(&entry.name[NameLg], "");
 }

 if (ValueLg > 0)
 {
	uc_ascstrncpy(entry.wert, Value, ValueLg);
	uc_ascstrcpy(&entry.wert[ValueLg], "");
 }

 SAPDB_memcpy(&l_entries[l_ncur_entry-1], &entry, sizeof(entry));

 *entries = l_entries;
 *ncur_entry = l_ncur_entry;

 return(TRUE);
}

int SetEntryWert
(
 unsigned int	*ncur_entry,
 st_entry	*entries[],
 unsigned int	NameLg,
 ucchar		Name[],
 unsigned int	ValueLg,
 ucchar		Value[],
 int		ErrorTextSize,
 ucchar		ErrorText[]
)
/* allocate enough to copy a unicode string including the two byte null
 * at the end (instead of the one byte null for c-strings).
 */
{
 st_entry	entry;
 unsigned int	l_ncur_entry = *ncur_entry;
 st_entry	*l_entries = *entries;
 size_t		ucwertsize, ucnamesize;
 ucchar		*t;
 tsp00_Bool	ok;

 if (!Name)	return(FALSE);

 if (NameLg == 0)
 {
	entry.name = NULL;
	entry.namelg = 0;
 }
 else {
	entry.namelg = NameLg + 1;
	ucnamesize = entry.namelg * sizeof(ucchar);

	wahttp_SQLALLOCAT ( ucnamesize, &t, &ok );
    if (!ok) {
        /* memory alloc failure	*/
		Log_Error( "SetEntryWert: Memory allocation error!", -1 );
		uc_ascstrcpymax(ErrorTextSize, ErrorText, "SetEntryWert: Memory allocation error!", NULL);

		return(FALSE);
 	}
	entry.name = t;
 }

 if (ValueLg == 0)
 {
	entry.wert = NULL;
	entry.wertlg = 0;
 }
 else {
	entry.wertlg = ValueLg + 1;
	ucwertsize = entry.wertlg * sizeof(ucchar);

	wahttp_SQLALLOCAT ( ucwertsize, &entry.wert, &ok );
    if (!ok) {
        /* memory alloc failure	*/
		wahttp_SQLFREE ( entry.name );

		Log_Error( "SetEntryWert: Memory allocation error!", -1 );
		uc_ascstrcpymax(ErrorTextSize, ErrorText, "SetEntryWert: Memory allocation error!", NULL);

		return(FALSE);
 	}
 }

 /* Make sure `l_entries' holds an initial value (for realloc) */
 if (l_ncur_entry == 0)
	l_entries = NULL;

	wahttp_SQLREALLOCAT( (l_ncur_entry + 1)*sizeof(*l_entries), &l_entries, &ok );
	if ( !ok )	{
		if (entry.name) wahttp_SQLFREE(entry.name);
		if (entry.wert) wahttp_SQLFREE(entry.wert);

		Log_Error( "SetEntryWert: Memory reallocation error!", -1 );
		uc_ascstrcpymax(ErrorTextSize, ErrorText, "SetEntryWert: Memory reallocation error!", NULL);

		return(FALSE);
	} else {
		l_ncur_entry++;
	}

 if (NameLg > 0)
 {
	uc_strncpy(entry.name,  Name, NameLg);
	uc_ascstrcpy(&entry.name[NameLg], "");
 }

 if (ValueLg > 0)
 {
	uc_strncpy(entry.wert, Value, ValueLg);
	uc_ascstrcpy(&entry.wert[ValueLg], "");
 }

 SAPDB_memcpy(&l_entries[l_ncur_entry-1], &entry, sizeof(entry));

 *entries = l_entries;
 *ncur_entry = l_ncur_entry;

 return(TRUE);
}
#endif
