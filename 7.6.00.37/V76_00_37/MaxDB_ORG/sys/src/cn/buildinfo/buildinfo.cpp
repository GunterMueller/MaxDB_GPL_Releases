/*
 * --------------------------------------------------------------------------
 *
 * module: buildinfo.cpp
 *
 * --------------------------------------------------------------------------
 * --------------------------------------------------------------------------
\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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


\endif
 */

#ifndef WIN32
#define UNIX
#endif

#include "heo01.h"
#include "cn/buildinfo/buildinfo.h"
#include "SAPDBCommon/SAPDB_string.h"

#ifdef WIN32
#include <WINDOWS.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif

#ifdef UNIX
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef __osf__
#pragma message disable codeunreachable
#endif

#ifdef WIN32
#ifndef alloca
#define alloca(a) _alloca(a)
#endif
#ifndef open
#define open(a,b) _open(a,b)
#endif
#ifndef read
#define read(a,b,c) _read(a,b,c)
#endif
#ifndef close
#define close(a) _close(a)
#endif
#ifndef snprintf
#define snprintf _snprintf
#endif
#ifndef strupr
#define strupr _strupr
#endif
#endif

#ifdef UNIX
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif

#if defined WIN32
#define NEED_TYPEDEF_UCHAR_T NEED_TYPEDEF_UCHAR_T
#define NEED_TYPEDEF_UNIT16_T NEED_TYPEDEF_UNIT16_T
#define NEED_TYPEDEF_UNIT32_T NEED_TYPEDEF_UNIT32_T
#define NEED_TYPEDEF_UNIT64_T NEED_TYPEDEF_UNIT64_T
#endif

#ifdef NEED_TYPEDEF_UCHAR_T
typedef unsigned char  uchar_t;
#endif
#ifdef NEED_TYPEDEF_UNIT16_T
typedef unsigned short uint16_t;
#endif
#ifdef NEED_TYPEDEF_UNIT32_T
typedef unsigned int   uint32_t;
#endif
#ifdef NEED_TYPEDEF_UNIT64_T
typedef unsigned long  uint64_t;
#endif

#if defined _WIN64
typedef __int64 ptr_t;
#else
typedef long ptr_t;
#endif

/*
 * common types and prototypes
 */
typedef struct buildinfo {
	char *key;
	char *val;
	struct buildinfo *next;
} buildinfo_t;

static buildinfo_t *
buildinfo_add (buildinfo_t *, char *, char *);
static void buildinfo_free (buildinfo_t *);

#define BUILD_TYPE_REL 1
#define BUILD_TYPE_APO 2

/*
 * windows part
 */
#ifdef WIN32
typedef struct {
	int value;
	char name[64];
} names_t;

static char *
get_name (int value, names_t *ptr) {
	while (ptr->value >= 0) {
		if (ptr->value == value)
			return (ptr->name);

		ptr++;
	}
	return 0;
}

/*
 * structures from windows PE header
 */
typedef struct {
	uint16_t e_magic;    /* Magic number */
	uint16_t e_cblp;     /* Bytes on last page of file */
	uint16_t e_cp;       /* Pages in file */
	uint16_t e_crlc;     /* Relocations */
	uint16_t e_cparhdr;  /* Size of header in paragraphs */
	uint16_t e_minalloc; /* Minimum extra paragraphs needed */
	uint16_t e_maxalloc; /* Maximum extra paragraphs needed */
	uint16_t e_ss;       /* Initial (relative) SS value */
	uint16_t e_sp;       /* Initial SP value */
	uint16_t e_csum;     /* Checksum */
	uint16_t e_ip;       /* Initial IP value */
	uint16_t e_cs;       /* Initial (relative) CS value */
	uint16_t e_lfarlc;   /* File address of relocation table */
	uint16_t e_ovno;     /* Overlay number */
	uint16_t e_res[4];   /* Reserved words */
	uint16_t e_oemid;    /* OEM identifier (for e_oeminfo) */
	uint16_t e_oeminfo;  /* OEM information; e_oemid specific */
	uint16_t e_res2[10]; /* Reserved words */
	uint16_t e_lfanew;   /* File address of new exe header */
} win_doshdr_t;

typedef struct {
	uchar_t  Signatur[4];
	uint16_t Machine;
	uint16_t NumberOfSections;
	uint32_t TimeDateStamp;
	uint32_t PointerToSymbolTable;
	uint32_t NumberOfSymbols;
	uint16_t SizeOfOptionalHeaders;
	uint16_t Characteristics;
} win_imagefilehdr_t;

typedef struct {
	uint16_t Magic;
	uchar_t MajorLinkerVersion;
	uchar_t MinorLinkerVersion;
	uint32_t SizeOfCode;
	uint32_t SizeOfInitializedData;
	uint32_t SizeOfUninitializedData;
	uint32_t AddressOfEntryPoint;
	uint32_t BaseOfCode;
	uint32_t BaseOfData;
	uint32_t ImageBase;
	uint32_t SectionAlignment;
	uint32_t FileAlignment;
	uint16_t MajorOperatingSystemVersion;
	uint16_t MinorOperatingSystemVersion;
	uint16_t MajorImageVersion;
	uint16_t MinorImageVersion;
	uint16_t MajorSubsystemVersion;
	uint16_t MinorSubsystemVersion;
	uint32_t Win32VersionValue;
	uint32_t SizeOfImage;
	uint32_t SizeOfHeaders;
	uint32_t CheckSum;
	uint16_t Subsystem;
	uint16_t DllCharacteristics;
	uint32_t SizeOfStackReserve;
	uint32_t SizeOfStackCommit;
	uint32_t SizeOfHeapReserve;
	uint32_t SizeOfHeapCommit;
	uint32_t LoaderFlags;
	uint32_t NumberOfRvaAndSizes;
} win_optionalhdr32_t;

typedef struct {
	uint16_t Magic;
	uchar_t MajorLinkerVersion;
	uchar_t MinorLinkerVersion;
	uint32_t SizeOfCode;
	uint32_t SizeOfInitializedData;
	uint32_t SizeOfUninitializedData;
	uint32_t AddressOfEntryPoint;
	uint32_t BaseOfCode;
	uint32_t BaseOfData;
	uint32_t ImageBase;
	uint32_t SectionAlignment;
	uint32_t FileAlignment;
	uint16_t MajorOperatingSystemVersion;
	uint16_t MinorOperatingSystemVersion;
	uint16_t MajorImageVersion;
	uint16_t MinorImageVersion;
	uint16_t MajorSubsystemVersion;
	uint16_t MinorSubsystemVersion;
	uint32_t Win32VersionValue;
	uint32_t SizeOfImage;
	uint32_t SizeOfHeaders;
	uint32_t CheckSum;
	uint16_t Subsystem;
	uint16_t DllCharacteristics;
	uint32_t SizeOfStackReserve0;
	uint32_t SizeOfStackReserve1;
	uint32_t SizeOfStackCommit0;
	uint32_t SizeOfStackCommit1;
	uint32_t SizeOfHeapReserve0;
	uint32_t SizeOfHeapReserve1;
	uint32_t SizeOfHeapCommit0;
	uint32_t SizeOfHeapCommit1;
	uint32_t LoaderFlags;
	uint32_t NumberOfRvaAndSizes;
} win_optionalhdr64_t;

typedef struct {
	uint32_t VirtualAddress;
	uint32_t Size;
} win_datadirectory_t;

typedef struct {
	uchar_t Name[8];
	uint32_t PhysicalAddress;
	uint32_t VirtualAddress;
	uint32_t SizeOfRawData;
	uint32_t PointerToRawData;
	uint32_t PointerToRelocations;
	uint32_t PointerToLinenumbers;
	uint16_t NumberOfRelocations;
	uint16_t NumberOfLinenumbers;
	uint32_t Characteristics;
} win_sectionhdr_t;

typedef struct {
	uint32_t Characteristics;
	uint32_t TimeDateStamp;
	uint16_t MajorVersion;
	uint16_t MinorVersion;
	uint16_t NumberOfNamedEntries;
	uint16_t NumberOfIdEntries;
} win_resdirectory_t;

typedef struct {
	uint32_t Name;
	uint32_t OffsetToData;
} win_resdirentry_t;

typedef struct {
	uint32_t OffsetToData;
	uint32_t Size;
	uint32_t CodePage;
	uint32_t Reserved;
} win_resdataentry_t;

typedef struct {
	uchar_t Signatur[4];
} win_signatur_t;

static uchar_t win_dosmagic[2] = {'M', 'Z'};
static uchar_t win_ntmagic[4] = {'P', 'E', '\0', '\0'};

typedef struct {
	uint16_t Size;
	uint16_t ValueSize;
	uint16_t Type;
	uint16_t Magic[16];
} win_versioninfo_t;

typedef struct {
	uint16_t Size;
	uint16_t ValueSize;
	uint16_t Type;
	uint16_t Magic[15];
} win_stringfileinfo_t;

typedef struct {
	uint16_t Size;
	uint16_t ValueSize;
	uint16_t Type;
	uint16_t Magic[12];
} win_varfileinfo_t;

typedef struct {
	uint16_t Size;
	uint16_t ValueSize;
	uint16_t Type;
	uint16_t Key[9];
} win_stringtable_t;

typedef struct {
	uint16_t Size;
	uint16_t ValueSize;
	uint16_t Type;
} win_string_t;

static uint16_t win_versioninfo_magic[16] =
	{'V', 'S', '_', 'V', 'E', 'R', 'S', 'I', 'O', 'N', '_',
	 'I', 'N', 'F', 'O', 0};

static uint16_t win_stringfileinfo_magic[15] =
	{'S', 't', 'r', 'i', 'n', 'g', 'F', 'i', 'l', 'e',
	 'I', 'n', 'f', 'o', 0};

static uint16_t win_varfileinfo_magic[12] =
	{'V', 'a', 'r', 'F', 'i', 'l', 'e',
	 'I', 'n', 'f', 'o', 0};

static names_t win_flag_names[] = {
	{0x0002, "executable"},
	{0x1000, "system file"},
	{0x2000, "dll"},
	{-1, ""}
};

static names_t win_machine_names[] = {
	{0x0000, "unknown machine"},
	{0x014c, "i386"},
	{0x0160, "MIPS R3000 big-endian"},
	{0x0166, "MIPS R4000 little-endian"},
	{0x0168, "MIPS R10000 little-endian"},
	{0x0169, "MIPS WCE v2 little-endian"},
	{0x0184, "ALPHA 32-bit"},
	{0x01f0, "PowerPC"},
	{0x0200, "IA64"},
	{0x0284, "ALPHA 64-bit"},
	{-1, ""}
};

static int
is_windowsdos (char *buff, char *sz_text, int cb_text) {
	win_doshdr_t *doshdr;

	doshdr = (win_doshdr_t *) buff;

	if (memcmp
	(&doshdr->e_magic, win_dosmagic, sizeof (win_dosmagic)) != 0) {
		return -1;
	}

	snprintf (sz_text, cb_text - 1, "MS DOS");
	return 0;
}

static int
is_windowspe (char *buff, char *sz_text, int cb_text) {
	win_imagefilehdr_t *winnthdr;
	win_signatur_t *signature;
	char *machine;
	names_t *ptr;

	signature = (win_signatur_t *) buff;
	winnthdr = (win_imagefilehdr_t *) buff;

	if (memcmp
	(&signature->Signatur, win_ntmagic, sizeof (win_ntmagic)) != 0) {
		return -1;
	}

	machine = get_name (winnthdr->Machine, win_machine_names);
	if (machine == 0)
		return -1;

	snprintf (sz_text, cb_text - 1, "MS Windows");
	switch (winnthdr->Machine) {
	case 0x014c:
		/* i386 */
		snprintf (sz_text + strlen (sz_text), cb_text - 1, " 32-bit");
		break;
	case 0x0200:
		/* ia64 */
		snprintf (sz_text + strlen (sz_text), cb_text - 1, " 64-bit");
		break;
	}

	snprintf (sz_text + strlen (sz_text), cb_text - 1, " %s", machine);

	ptr = win_flag_names;
	while (ptr->value != -1) {
		if (ptr->value & winnthdr->Characteristics) {
			snprintf (sz_text + strlen (sz_text),
			          cb_text - 1, " %s", ptr->name);
		}
		ptr++;
	}
	return 0;
}

static int
get_magic (char *buff, char *sz_text, int cb_text) {
	if (is_windowsdos (buff, sz_text, cb_text) == 0)
		return 0;

	if (is_windowspe (buff, sz_text, cb_text) == 0)
		return 0;

	strncpy (sz_text, "unknown magic", cb_text - 1);
	return 0;
}

typedef struct {
	int peoffset;
	struct {
		int offset;
		int size;
	} res;
	win_doshdr_t        * doshdr;
	win_imagefilehdr_t  * filehdr;

	win_optionalhdr32_t * opthdr32;
	win_optionalhdr64_t * opthdr64;
	win_datadirectory_t * datadir;
	win_sectionhdr_t    * sechdr;
} win_image_t;

typedef struct session {
	struct session *next;
	int fd;
	char *buff;
	int offset;
	int size;
} session_t;

static void calc_peoffset (win_image_t *);
static void calc_resoffset (win_image_t *);
static int virtual2real (win_image_t *, int);
static int get_versionresource (char *, int, int, int *);

static session_t * sess_root = 0;

static session_t *
get_session (int fd) {
	session_t *curr, *prev;

	if (sess_root == 0) {
		curr = (session_t *) malloc (sizeof (session_t));
		if (curr == 0)
			return 0;

		memset (curr, 0, sizeof (session_t));
		curr->fd = fd;

		sess_root = curr;
		return (curr);
	}

	for (curr = sess_root; curr != 0;) {
		if (curr->fd == fd)
			return (curr);

		prev = curr;
		curr = curr->next;
	}

	curr = (session_t *) malloc (sizeof (session_t));
	if (curr == 0)
		return 0;

	memset (curr, 0, sizeof (session_t));
	curr->fd = fd;

	prev->next = curr;
	return (curr);
}

static void
del_session (int fd) {
	session_t *curr, *prev;

	if (sess_root == 0)
		return;

	for (curr = sess_root; curr != 0;) {
		if (curr->fd == fd) {
			if (curr == sess_root)
				sess_root = curr->next;
			else
				prev->next = curr->next;

			free (curr);
			return;
		}

		prev = curr;
		curr = curr->next;
	}

	return;
}

static void *
get_versioninfo (int fd, void *resinfo, int *p_size) {
	int got = 0;
	int total = 0;
	int offset = 0;

	char *buff = 0;
	int versioninfo = 0;

	session_t *sess;
	win_image_t *image;

	sess = get_session (fd);

	if (sess->buff != 0) {
		*p_size = 0;

		if (sess->buff != 0) {
			if (resinfo != 0) {
				SAPDB_memcpy (resinfo, sess->buff, sess->size);
				*p_size = sess->size;
			}

			free (sess->buff);
			sess->buff = 0;
			sess->offset = 0;
			sess->size = 0;
		}

		del_session (fd);
		return (resinfo);
	}

	image = (win_image_t *) malloc (sizeof (win_image_t));
	if (image == 0)
		return 0;

	memset (image, 0, sizeof (win_image_t));

	for (;;) {
		buff = (char *) realloc (buff, total - offset + 512);

		got =
		read (fd, buff + (total - offset), 512);

		if (got <= 0) break;

		total += got;

		/*
		 * looking for pe-offset from dos-header
		 */
		if (image->peoffset == 0 && image->doshdr == 0) {
			if (offset == 0) {
				char sz_magic[1024];
				int from;
				int to;

				from = 0;
				if (total <= from) {
					offset = total;
					continue;
				}

				to = from + (int) sizeof (win_doshdr_t);
				if (total < to) continue;

				image->doshdr = (win_doshdr_t *) malloc (sizeof (win_doshdr_t));
				if (image->doshdr == 0) break;

				SAPDB_memcpy (image->doshdr,
				buff + from - offset,
				sizeof (win_doshdr_t));

				calc_peoffset (image);

				sz_magic[0] = '\0';
				get_magic (buff + from - offset, sz_magic, 1024);

				if (strlen (sz_magic) != 0) {
					/*
					 * if this is not a least an MS DOS header,
					 * it makes no sense to look for any resources
					 */
					if (memcmp (sz_magic, "MS DOS", 6) != 0) {
						if (image != 0) {
							if (image->doshdr != 0)
								free (image->doshdr);

							if (image->filehdr != 0)
								free (image->filehdr);

							if (image->opthdr32 != 0)
								free (image->opthdr32);

							if (image->opthdr64 != 0)
								free (image->opthdr64);

							if (image->sechdr != 0)
								free (image->sechdr);

							if (image->datadir != 0)
								free (image->datadir);

							free (image);
						}

						if (buff != 0)
							free (buff);

						return 0;
					}
				}
			}
		}

		/*
		 * looking for pe-image-fileheader
		 */
		if (image->filehdr == 0) {
			int from;
			int to;
			char sz_magic[1024];

			from = image->peoffset;
			if (total <= from) {
				offset = total;
				continue;
			}

			to =
			image->peoffset + (int) sizeof (win_imagefilehdr_t);

			if (total < to) continue;

			image->filehdr = (win_imagefilehdr_t *) malloc (sizeof (win_imagefilehdr_t));
			if (image->filehdr == 0) break;

			SAPDB_memcpy (image->filehdr,
			buff + from - offset,
			sizeof (win_imagefilehdr_t));

			sz_magic[0] = '\0';
			get_magic (buff + from - offset, sz_magic, 1024);
		}

		/*
		 * looking for optional-header
		 */
		if (image->opthdr32 == 0 && image->opthdr64 == 0) {
			int from;
			int to;

			from =
			image->peoffset + (int) sizeof (win_imagefilehdr_t);

			if (total <= from) {
				offset = total;
				continue;
			}

			if (image->filehdr->SizeOfOptionalHeaders == 224) {
				to = image->peoffset +
				(int) sizeof (win_imagefilehdr_t) +
				(int) sizeof (win_optionalhdr32_t);
				if (total < to) continue;

				image->opthdr32 =
				(win_optionalhdr32_t *) malloc (sizeof (win_optionalhdr32_t));

				if (image->opthdr32 == 0) break;

				SAPDB_memcpy (image->opthdr32,
				buff + from - offset,
				sizeof (win_optionalhdr32_t));
			} else {
				to = image->peoffset +
				(int) sizeof (win_imagefilehdr_t) +
				(int) sizeof (win_optionalhdr64_t);
				if (total < to) continue;

				image->opthdr64 =
				(win_optionalhdr64_t *) malloc (sizeof (win_optionalhdr64_t));

				if (image->opthdr64 == 0) break;

				SAPDB_memcpy (image->opthdr64,
				buff + from - offset,
				sizeof (win_optionalhdr64_t));
			}
		}

		/*
		 * looking for datadirectory
		 */
		if (image->datadir == 0) {
			int from;
			int to;
			int num_of_rva;

			if (image->opthdr32 != 0) {
				num_of_rva =
				image->opthdr32->NumberOfRvaAndSizes;

				from = image->peoffset +
				(int) sizeof (win_imagefilehdr_t) +
				sizeof (win_optionalhdr32_t);
			} else {
				num_of_rva =
				image->opthdr64->NumberOfRvaAndSizes;

				from = image->peoffset +
				(int) sizeof (win_imagefilehdr_t) +
				sizeof (win_optionalhdr64_t);
			}

			if (total <= from) {
				offset = total;
				continue;
			}

			to = from +
			num_of_rva * (int) sizeof (win_datadirectory_t);

			if (total < to) continue;

			image->datadir =
			(win_datadirectory_t *) malloc (num_of_rva * sizeof (win_datadirectory_t));

			if (image->datadir == 0) break;

			SAPDB_memcpy (image->datadir,
			buff + from - offset,
			num_of_rva  * sizeof (win_datadirectory_t));
		}

		/*
		 * looking for section headers
		 */
		if (image->sechdr == 0) {
			int from;
			int to;
			int entries;
			int num_of_rva;

			entries = image->filehdr->NumberOfSections;
			if (image->opthdr32 != 0) {
				num_of_rva =
				image->opthdr32->NumberOfRvaAndSizes;

				from = image->peoffset +
				(int) sizeof (win_imagefilehdr_t) +
				sizeof (win_optionalhdr32_t) +
				num_of_rva * (int) sizeof (win_datadirectory_t);
			} else {
				num_of_rva =
				image->opthdr64->NumberOfRvaAndSizes;

				from = image->peoffset +
				(int) sizeof (win_imagefilehdr_t) +
				sizeof (win_optionalhdr64_t) +
				num_of_rva * (int) sizeof (win_datadirectory_t);
			}

			if (total <= from) {
				offset = total;
				continue;
			}

			to = from +
			entries * (int) sizeof (win_sectionhdr_t);

			if (total < to) continue;

			image->sechdr =
			(win_sectionhdr_t *) malloc (entries * sizeof (win_sectionhdr_t));

			if (image->sechdr == 0) break;

			SAPDB_memcpy (image->sechdr,
			buff + from - offset,
			entries  * sizeof (win_sectionhdr_t));

			calc_resoffset (image);
		}

		/*
		 * go to resource section
		 * and get versioninfo
		 */
		if (versioninfo == 0) {
			int from;
			int to;
			int ressize;

			from = image->res.offset;
			if (total <= from) {
				offset = total;
				continue;
			}

			if (image->res.size <= 0) {
				if (image != 0) {
					if (image->doshdr != 0)
						free (image->doshdr);

					if (image->filehdr != 0)
						free (image->filehdr);

					if (image->opthdr32 != 0)
						free (image->opthdr32);

					if (image->opthdr64 != 0)
						free (image->opthdr64);

					if (image->sechdr != 0)
						free (image->sechdr);

					if (image->datadir != 0)
						free (image->datadir);

					free (image);
				}

				if (buff != 0)
					free (buff);

				return 0;
			}

			to = image->res.offset + image->res.size;
			if (total < to) continue;

			ressize = 0;

			versioninfo = get_versionresource (
			buff + image->res.offset - offset,
			image->res.offset, 0, &ressize);

			sess->size = ressize;
			sess->buff = (char *) malloc (sess->size);
			sess->offset =
			virtual2real (image, versioninfo) -
			              (image->res.offset - offset);

			*p_size = sess->size;

			SAPDB_memcpy (sess->buff, buff + sess->offset, sess->size);

			free (buff);
			free (image->doshdr);
			free (image->filehdr);
			free (image->opthdr32);
			free (image->opthdr64);
			free (image->sechdr);
			free (image->datadir);
			free (image);

			return 0;
		}

		offset = total;
	}

	free (image->doshdr);
	free (image->filehdr);
	free (image->opthdr32);
	free (image->opthdr64);
	free (image->sechdr);
	free (image->datadir);
	free (image);

	free (buff);
	return 0;
}

static int
get_versionresource (char *resource,
                     int resoffset, int offset, int *p_ressize) {
	int i;
	int num_of_entries;

	win_resdirectory_t *resdir;
	win_resdirentry_t  *resentry;
	int infodata = 0;

	resdir = (win_resdirectory_t *) (resource + offset);

	num_of_entries =
	resdir->NumberOfIdEntries + resdir->NumberOfNamedEntries;

	resentry = (win_resdirentry_t *)
	(resource + offset + sizeof (win_resdirectory_t));

	for (i = 0; i < num_of_entries; i++) {
		if (offset == 0)
			if (!(resentry[i].Name & 0x80000000L))
				/* next if it is it a versioninfo */
				if ((resentry[i].Name & ~0x80000000L)  != 16)
					continue;

		if (resentry[i].OffsetToData & 0x80000000L) {
			infodata = get_versionresource (resource, resoffset,
			resentry[i].OffsetToData & ~0x80000000L, p_ressize);
		} else {
			win_resdataentry_t *dataentry;

			dataentry = (win_resdataentry_t *)
			            (resource + resentry[i].OffsetToData);

			dataentry->OffsetToData;

			*p_ressize = dataentry->Size;
			return (dataentry->OffsetToData);
		}
	}

	return (infodata);
}

void
calc_peoffset (win_image_t *image) {
	if (image->doshdr == 0)
		return;

	if (memcmp (&image->doshdr->e_magic, win_dosmagic,
	sizeof (win_dosmagic)) != 0)
		return;

	image->peoffset = image->doshdr->e_lfanew;
	return;
}

void
calc_resoffset (win_image_t *image) {
	int i;
	int va_image_dir;
	int image_dir = 2;
	int num_of_rva;

	if (image->filehdr->SizeOfOptionalHeaders == 224)
		num_of_rva = image->opthdr32->NumberOfRvaAndSizes;
	else
		num_of_rva = image->opthdr64->NumberOfRvaAndSizes;

	if (image_dir >= (int) (num_of_rva))
		return;

	va_image_dir = image->datadir[image_dir].VirtualAddress;

	for (i = 0; i < (int) (image->filehdr->NumberOfSections); i++) {
		int from;
		int to;

		from = (int) (image->sechdr[i].VirtualAddress);
		to = from + (int) (image->sechdr[i].SizeOfRawData);

		if (from <= va_image_dir && to > va_image_dir) break;
	}

    if (i > image->filehdr->NumberOfSections)
        return;

    image->res.offset =
	(va_image_dir - image->sechdr[i].VirtualAddress +
	image->sechdr[i].PointerToRawData);

	image->res.size = image->sechdr[i].SizeOfRawData;

	return;
}

static int
virtual2real (win_image_t *image, int va) {
	int i;

	for (i = 0; i < (int) (image->filehdr->NumberOfSections); i++) {
		int from;
		int to;

		from = (int) (image->sechdr[i].VirtualAddress);
		to = from + (int) (image->sechdr[i].SizeOfRawData);

		if (from <= va && to > va)
			return (va - from);
	}

	return 0;
}

static int
equals (char *ascii, char *ucs2) {
	int i;

	for (i = 0; i < (int) strlen (ascii); i++)
		 if (ascii[i] != ((uint16_t *)ucs2)[i])
			return -1;

	return 0;
}

static int
ascii_toupper (int c) {
	if (c >= 'a' && c <= 'z')
		return (c - ('a' - 'A'));

	return (c);
}

static int
get_string (char *raw, int rawsize, int offset,
char *desc, char **info, int *infosize) {
	win_string_t *s;
	char *key;
	char *val;

	if (offset >= rawsize)
		return 0;

	s = (win_string_t *) (raw + offset);

	key = ((char *) s) + sizeof (win_string_t);

	if (strlen (desc) > 0 && equals (desc, key) == 0) {
		val = key + 2 * (strlen (desc) + 1);
		if ((ptr_t) val % 4 != 0)
			val += 4 - ((ptr_t) val % 4);

		*infosize = s->ValueSize;
		*info = val;
		return 0;
	}

	offset += s->Size;
	if (offset % 4 != 0)
		offset += 4 - offset % 4;

	return (get_string (raw, rawsize, offset, desc, info, infosize));
}

static int
get_stringtable (char *raw, int rawsize, int offset,
char *desc, char **info, int *infosize) {
	int string;
	int size;
	int rc;
	win_stringtable_t *st;

	if (offset >= rawsize)
		return 0;

	st = (win_stringtable_t *) (raw + offset);

	if (offset + st->Size > rawsize)
		return 0;

	string = offset + sizeof (win_stringtable_t);
	if (string % 4 != 0)
		string += 4 - string % 4;

	size = st->Size - (string - offset);

	if (strlen (desc) > 9 &&
	desc[0] == ascii_toupper (st->Key[0]) &&
	desc[1] == ascii_toupper (st->Key[1]) &&
	desc[2] == ascii_toupper (st->Key[2]) &&
	desc[3] == ascii_toupper (st->Key[3]) &&
	desc[4] == ascii_toupper (st->Key[4]) &&
	desc[5] == ascii_toupper (st->Key[5]) &&
	desc[6] == ascii_toupper (st->Key[6]) &&
	desc[7] == ascii_toupper (st->Key[7]) &&
	desc[8] == '\\') {
		desc += 9;
		rc = get_string (raw + string, size, 0, desc, info, infosize);
	}

	rc = get_stringtable
	(raw, rawsize, offset + st->Size, desc, info, infosize);

	return (rc);
}

static int
get_fileinfo (void *raw, int rawsize, int offset, char *desc,
char **info, int *infosize) {
	int rc;
	win_versioninfo_t *vi;
	win_varfileinfo_t *vfi;
	win_stringfileinfo_t *sfi;

	if (offset >= rawsize)
		return 0;

	/*
	 * VS_VERSIONINFO structure
	 */
	vi = (win_versioninfo_t *) ((char *) raw + offset);
	if (memcmp (vi->Magic, win_versioninfo_magic,
	sizeof (win_versioninfo_magic)) == 0) {
		if (offset != 0)
			return 0;

		if (vi->Size != rawsize)
			return 0;

		offset = sizeof (win_versioninfo_t);
		if (offset % 4 != 0)
			offset += 4 - offset % 4;

		offset += vi->ValueSize;
		if (offset % 4 != 0)
			offset += 4 - offset % 4;

		rc = get_fileinfo (raw, rawsize, offset, desc, info, infosize);
		return (rc);
	}

	/*
	 * VarFileInfo structure
	 */
	vfi = (win_varfileinfo_t *) ((char *) raw + offset);
	if (memcmp (vfi->Magic, win_varfileinfo_magic,
	sizeof (win_varfileinfo_magic)) == 0) {
		if (offset + vfi->Size > rawsize)
			return 0;

		offset += vfi->Size;

		rc = get_fileinfo (raw, rawsize, offset, desc, info, infosize);
		return (rc);
	}

	/*
	 * StringFileInfo structure
	 */
	sfi = (win_stringfileinfo_t *) ((char *) raw + offset);
	if (memcmp (sfi->Magic, win_stringfileinfo_magic,
		sizeof (win_stringfileinfo_magic)) == 0) {
		char *inforoot = "\\StringFileInfo";
		int stringtable;
		int size;

		/*
		 * we are interested in StringFileInfo
		 */
		if (memcmp (desc, inforoot, strlen (inforoot)) == 0) {
			stringtable = offset + sizeof (win_stringfileinfo_t);
			if (stringtable % 4 != 0)
				stringtable += 4 - stringtable % 4;

			size = sfi->Size - (stringtable - offset);
			desc += strlen (inforoot) + 1;
			rc = get_stringtable
			((char *) raw + stringtable, size, 0, desc, info, infosize);

			if (*info != 0 && *infosize != 0)
				return (rc);
		}

		if (offset + sfi->Size > rawsize)
			return 0;

		offset += sfi->Size;

		rc = get_fileinfo (raw, rawsize, offset, desc, info, infosize);
		return (rc);
	}

	return 0;
}

static int
version_query_value (void *raw, char *desc, void **info, int *infosize) {
	int rc;
	uint16_t rawsize;

	*info = 0;
	*infosize = 0;

	rawsize = * (uint16_t *) raw;
	rc = get_fileinfo (raw, rawsize, 0, desc, (char **) info, infosize);

	return (rc);
}

static char *
convert_slashes (char *filename) {
	int i;

	for (i = 0; i <= (int) strlen (filename); i++)
		if (filename[i] == '/')
			filename[i] ='\\';

	return (filename);
}

static buildinfo_t *
get_buildinfo_using_res (int fd) {
	int i;
	void *raw;
	int raw_size;
	void *untrans;
	int untrans_size;
	char *ascii;

	int type;
	int major;
	int minor;
	int corr;
	int build;
	char *next;
	char *str;
	buildinfo_t *buildinfo;
	char buildnumber_key[] = "BUILDNUMBER";

	ascii = 0;
	buildinfo = 0;

	/*
	 * look for version information
	 */
	get_versioninfo (fd, 0, &raw_size);

	raw = malloc (raw_size);
	if (raw == 0)
		return (buildinfo);

	get_versioninfo (fd, raw, &raw_size);

	/*
	 * first try us-english ascii
	 */
	untrans = 0;
	version_query_value
	(raw, "\\StringFileInfo\\040904E4\\FileVersion",
	&untrans, &untrans_size);

	/*
	 * try us-english unicode
	 */
	if (untrans_size == 0) {
		version_query_value
		(raw, "\\StringFileInfo\\040904B0\\FileVersion",
		&untrans, &untrans_size);
	}

	/*
	 * or give up
	 */
	if (untrans_size == 0) {
		free (raw);
		return (buildinfo);
	}

	ascii = (char *) malloc (untrans_size);
	if (ascii == 0) {
		free (raw);
		return (buildinfo);
	}

	for (i = 0; i < untrans_size; i++)
		ascii[i] = (char)(((uint16_t *)untrans)[i]);

	type = 0;
	if (strchr (ascii, '.') == 0) {
		/*
		 * no dots, this is an apo version
		 * apo version looks like
		 */
		type = BUILD_TYPE_APO;

		version_query_value
		(raw, "\\StringFileInfo\\040904E4\\Build", &untrans, &untrans_size);

		if (untrans_size == 0) {
			version_query_value
			(raw, "\\StringFileInfo\\040904B0\\Build", &untrans, &untrans_size);
		}

		if (untrans_size == 0) {
			free (ascii);
			ascii = 0;
		} else {
			ascii = (char *) realloc (ascii, untrans_size);
			if (ascii == 0) {
				free (raw);
				free (ascii);
				return (buildinfo);
			}

			for (i = 0; i < untrans_size; i++)
				ascii[i] = (char)(((uint16_t *)untrans)[i]);
		}

		if (ascii == 0) {
			/*
			 * FileInfo without dots and no Buildinfo,
			 * this is no valid version format
			 */
			free (raw);
			return (buildinfo);
		} else if (memcmp (ascii, "APO", 3) != 0) {
			/*
			 * apo old style,
			 * no buildnumber
			 * 3029 => APO 3.0A Build 29
			 * or
			 * 3103 => APO 3.10 Build 03
			 */
			major = ascii[0] - '0';
			minor = ascii[1] - '0';
			build = strtol (ascii + 2, &next, 10);

			/*
			 * '3.00 Build: xxx-000-000-000'
			 */
			str = (char *) malloc (33);
			if (str == 0) {
				free (raw);
				free (ascii);
				return (buildinfo);
			}

			sprintf (str, "APO %01d.%02d   Build %03d-000-000-000", major, minor, build);
			free (ascii);
			ascii = 0;

			buildinfo = buildinfo_add (buildinfo, buildnumber_key, str);
			free (str);
		} else {
			/*
			 * apo new style
			 * using buildnumber
			 */
			ascii = (char *) realloc (ascii, untrans_size);
			if (ascii == 0) {
				free (raw);
				free (ascii);
				return (buildinfo);
			}

			for (i = 0; i < untrans_size; i++)
				ascii[i] = (char)(((uint16_t *)untrans)[i]);

			major = ascii[4] - '0';
			minor = 10 * (ascii[6] - '0') + ascii[7] - '0';

			str = (char *) malloc (33);
			if (str == 0) {
				free (raw);
				free (ascii);
				return (buildinfo);
			}

			sprintf (str, "APO %01d.%02d   Build %15s", major, minor, ascii + 10);
			free (ascii);
			ascii = 0;

			buildinfo = buildinfo_add (buildinfo, buildnumber_key, str);
			free (str);
		}
	} else {
		/*
		 * sapdb version
		 */
		type = BUILD_TYPE_REL;
		major = strtol (ascii, &next, 10);
		minor = strtol (next + 1, &next, 10);
		corr = strtol (next + 1, &next, 10);

		version_query_value
		(raw, "\\StringFileInfo\\040904E4\\Build", &untrans, &untrans_size);

		if (untrans_size == 0) {
			version_query_value
			(raw, "\\StringFileInfo\\040904B0\\Build", &untrans, &untrans_size);
		}

		if (untrans_size == 0) {
			free (raw);
			free (ascii);
			return (buildinfo);
		}

		ascii = (char *) realloc (ascii, untrans_size);
		if (ascii == 0) {
			free (raw);
			free (ascii);
			return (buildinfo);
		}

		for (i = 0; i < untrans_size; i++)
			ascii[i] = (char)(((uint16_t *)untrans)[i]);

		/*
		 * '7.4.03 Build xxx-000-000-000'
		 */
		str = (char *) malloc (33);
			if (str == 0) {
			free (raw);
			free (ascii);
			return (buildinfo);
		}

		sprintf (str, "Rel %01d.%01d.%02d Build %15s", major, minor, corr, ascii);
		free (ascii);
		ascii = 0;

		buildinfo = buildinfo_add (buildinfo, buildnumber_key, str);
		free (str);
	}

	if (type == BUILD_TYPE_REL) {
		version_query_value
		(raw, "\\StringFileInfo\\040904E4\\Date", &untrans, &untrans_size);

		if (untrans_size == 0) {
			free (raw);
			free (ascii);
			return (buildinfo);
		}

		ascii = (char *) realloc (ascii, untrans_size);
		if (ascii == 0) {
			free (raw);
			free (ascii);
			return (buildinfo);
		}

		for (i = 0; i < untrans_size; i++)
			ascii[i] = (char)(((uint16_t *)untrans)[i]);

		buildinfo = buildinfo_add (buildinfo, "DATE", ascii);
		free (ascii);
		ascii = 0;
	}

	if (type == BUILD_TYPE_APO) {
		/* ChangeList */
		version_query_value
		(raw, "\\StringFileInfo\\040904E4\\ChangeList", &untrans, &untrans_size);

		if (untrans_size == 0) {
			free (raw);
			free (ascii);
			return (buildinfo);
		}

		ascii = (char *) realloc (ascii, untrans_size);
		if (ascii == 0) {
			free (raw);
			free (ascii);
			return (buildinfo);
		}

		for (i = 0; i < untrans_size; i++)
			ascii[i] = (char)(((uint16_t *)untrans)[i]);

		buildinfo = buildinfo_add (buildinfo, "CHANGELIST", ascii);
		free (ascii);
		ascii = 0;

		/* PatchLevel */
		version_query_value
		(raw, "\\StringFileInfo\\040904E4\\PatchLevel", &untrans, &untrans_size);

		if (untrans_size == 0) {
			free (raw);
			free (ascii);
			return (buildinfo);
		}

		ascii = (char *) realloc (ascii, untrans_size);
		if (ascii == 0) {
			free (raw);
			free (ascii);
			return (buildinfo);
		}

		for (i = 0; i < untrans_size; i++)
			ascii[i] = (char)(((uint16_t *)untrans)[i]);

		buildinfo = buildinfo_add (buildinfo, "PATCHLEVEL", ascii);
		free (ascii);
		ascii = 0;

	}

	free (raw);
	return (buildinfo);
}
#endif

/*
 * what part
 */
typedef struct {
	int type;
	int major;
	int minor;
	int correction;
	int build;
} build_t;

static buildinfo_t *check_vars (buildinfo_t *, char *);
static buildinfo_t *check_sysinfo (buildinfo_t *, char *);
static int  check_buildstr (char *, char *);
static char *get_what (char *, char *, char *, int);
static int  split_build (char *, build_t *);
static int  gt_build (build_t *, build_t *);
static void remove_colon (char *);
static void remove_dot (char *);

#define BUFFER_SIZE         (512 * 1024)
#define LINE_SIZE            (64 * 1024)

static buildinfo_t *
get_buildinfo_using_what (int fd) {
	int got;
	int got_buildstr;
	int done;
	char *end;
	char *at;
	static char buff[BUFFER_SIZE + 3];
	char line[LINE_SIZE];
	char *buildstr, *apo_buildstr;
	buildinfo_t *buildinfo;
	char buildnumber_key[] = "BUILDNUMBER";

	buildinfo = 0;

	buildstr = (char *) malloc (LINE_SIZE);
	if (buildstr == 0)
		return 0;

	memset (buildstr, 0, LINE_SIZE);
	buff[0] = buff[1] = buff[2] = '\0';
	for (;;) {
		got = read (fd, buff + 3, BUFFER_SIZE);
		if (got <= 0)
			break;

		got_buildstr = 0;
		done = 0;
		at = buff;
		end = buff + got;
		for (;;) {
			at = (char *) memchr (at, '@', end - at);
			if (at == 0)
				break;

			if (at[1] == '(' && at[2] == '#' && at[3] == ')') {
				at = get_what (at + 4, end + 3, line, fd);
				if (at == 0) {
					done = 1;
					break;
				}

				check_buildstr (line, buildstr);
				buildinfo = check_sysinfo (buildinfo, line);
				buildinfo = check_vars (buildinfo, line);
			}

			at++;
			if (at >= end)
				break;
		}

		if (done != 0)
			break;

		buff[0] = end[0];
		buff[1] = end[1];
		buff[2] = end[2];
	}

	if (strlen (buildstr) == 0)
		return 0;

	remove_colon (buildstr);
	remove_dot (buildstr);

	if (memcmp (buildstr, "APO", 3) != 0) {
		buildinfo = buildinfo_add (buildinfo, buildnumber_key, buildstr);
		free (buildstr);
		return (buildinfo);
	}

	apo_buildstr = (char *) malloc (35);
	free (buildstr);

	if (apo_buildstr == 0)
		return (buildinfo);

	memset (apo_buildstr, '\0', 35);
	SAPDB_memcpy (apo_buildstr, buildstr, 8);
	strcat (apo_buildstr, "   Build ");
	SAPDB_memcpy (apo_buildstr + strlen (apo_buildstr), buildstr + 15, 15);

	buildinfo = buildinfo_add (buildinfo, buildnumber_key, apo_buildstr);
	free (apo_buildstr);
	return (buildinfo);
}

static char *
get_what (char *begin, char *end, char *line, int fd) {
	char *ptr = line;

	while (begin < end) {
		*ptr = *begin;

		if (*begin == '\0')
			return (begin);

		ptr++;
		begin++;
	}

	for (;;) {
		if (read (fd, ptr, 1) <= 0) {
			*ptr = '\0';
			return 0;
		}

		if (*ptr == '\0')
			return (end);

		ptr++;
	}

	return 0;
}

static buildinfo_t *
check_vars (buildinfo_t *buildinfo, char *line) {
	char *pkey, *pval;

	pkey = pval = 0;
	if (pkey = strstr (line, "Architecture")) {
		char keyname[] = "ARCHITECTURE";

		pval = strchr (pkey, ':');
		if (pval == 0)
			return (buildinfo);

		pval += 2;
		buildinfo = buildinfo_add (buildinfo, keyname, pval);
		return (buildinfo);
	}

	if (pkey = strstr (line, "Operating System")) {
		char keyname[] = "OS-VERSION";

		pval = strchr (pkey, ':');
		if (pval == 0)
			return (buildinfo);

		pval += 2;
		buildinfo = buildinfo_add (buildinfo, keyname, pval);
		return (buildinfo);
	}

	if (pkey = strstr (line, "C Compiler")) {
		char keyname[] = "C-COMPILER";

		pval = strchr (pkey, ':');
		if (pval == 0)
			return (buildinfo);

		pval += 2;
		buildinfo = buildinfo_add (buildinfo, keyname, pval);
		return (buildinfo);
	}

	if (pkey = strstr (line, "C++ Compiler")) {
		char keyname[] = "C++-COMPILER";

		pval = strchr (pkey, ':');
		if (pval == 0)
			return (buildinfo);

		pval += 2;
		buildinfo = buildinfo_add (buildinfo, keyname, pval);
		return (buildinfo);
	}

	if (pkey = strstr (line, "changelist")) {
		char keyname[] = "CHANGELIST";

		pval = strchr (pkey, ':');
		if (pval == 0)
			return (buildinfo);

		pval += 2;
		buildinfo = buildinfo_add (buildinfo, keyname, pval);
		return (buildinfo);
	}

	if (pkey = strstr (line, "makedate")) {
		char keyname[] = "MAKEDATE";

		pval = strchr (pkey, ':');
		if (pval == 0)
			return (buildinfo);

		pval += 2;
		buildinfo = buildinfo_add (buildinfo, keyname, pval);
		return (buildinfo);
	}

	if (pkey = strstr (line, "makeuser")) {
		char keyname[] = "MAKEUSER";

		pval = strchr (pkey, ':');
		if (pval == 0)
			return (buildinfo);

		pval += 2;
		buildinfo = buildinfo_add (buildinfo, keyname, pval);
		return (buildinfo);
	}

	if (pkey = strstr (line, "makeid")) {
		char keyname[] = "MAKEID";

		pval = strchr (pkey, ':');
		if (pval == 0)
			return (buildinfo);

		pval += 2;
		buildinfo = buildinfo_add (buildinfo, keyname, pval);
		return (buildinfo);
	}

	if (pkey = strstr (line, "makestate")) {
		char keyname[] = "MAKESTATE";

		pval = strchr (pkey, ':');
		if (pval == 0)
			return (buildinfo);

		pval += 2;
		buildinfo = buildinfo_add (buildinfo, keyname, pval);
		return (buildinfo);
	}

	if (pkey = strstr (line, "makepath")) {
		char keyname[] = "MAKEPATH";

		pval = strchr (pkey, ':');
		if (pval == 0)
			return (buildinfo);

		pval += 2;
		buildinfo = buildinfo_add (buildinfo, keyname, pval);
		return (buildinfo);
	}

	if (pkey = strstr (line, "toolchangelist")) {
		char keyname[] = "TOOLCHANGELIST";

		pval = strchr (pkey, ':');
		if (pval == 0)
			return (buildinfo);

		pval += 2;
		buildinfo = buildinfo_add (buildinfo, keyname, pval);
		return (buildinfo);
	}

	if (pkey = strstr (line, "purpose")) {
		char keyname[] = "PURPOSE";

		pval = strchr (pkey, ':');
		if (pval == 0)
			return (buildinfo);

		pval += 2;
		buildinfo = buildinfo_add (buildinfo, keyname, pval);
		return (buildinfo);
	}

	return (buildinfo);
}

static buildinfo_t *
check_sysinfo (buildinfo_t *buildinfo, char *line) {
	char sz_sysinfo[] = "sysinfo.";
	char *pkey, *key;
	char *pval, *val;
	int len;

	pkey = strstr (line, sz_sysinfo);
	if (pkey == 0)
		return (buildinfo);

	pkey += strlen (sz_sysinfo);

	pval = strchr (pkey, ':');
	if (pval == 0)
		return (buildinfo);

	len = (int)(pval - pkey);
	if (len <= 0)
		return (buildinfo);

	pval += 2;

	val = (char *) malloc (strlen (pval) + 1);
	if (val == 0)
		return (buildinfo);

	SAPDB_strcpy (val, pval);

	key = (char *) malloc (len + 1);
	if (key == 0)
		return (buildinfo);

	memset (key, 0, len + 1);
	SAPDB_memcpy (key, pkey, len);

#if 0
	if (strcmp (key, "architecture") == 0) {
		char keyname[] = "ARCHITECTURE";
		buildinfo = buildinfo_add (buildinfo, keyname, val);
	}

	if (strcmp (key, "system") == 0) {
		char keyname[] = "OSNAME";
		buildinfo = buildinfo_add (buildinfo, keyname, val);
	}

	if (strcmp (key, "version") == 0) {
		char keyname[] = "OSVERSION";
		buildinfo = buildinfo_add (buildinfo, keyname, val);
	}
#endif

	if (strcmp (key, "subversion") == 0) {
		char keyname[] = "OS-SUBVERSION";
		buildinfo = buildinfo_add (buildinfo, keyname, val);
	}

	if (strcmp (key, "c_runtime") == 0) {
		char keyname[] = "C-RUNTIME";
		buildinfo = buildinfo_add (buildinfo, keyname, val);
	}

	if (strcmp (key, "cpp_runtime") == 0) {
		char keyname[] = "C++-RUNTIME";
		buildinfo = buildinfo_add (buildinfo, keyname, val);
	}

	if (strcmp (key, "cpp_runtime64") == 0) {
		char keyname[] = "C++-RUNTIME-64";
		buildinfo = buildinfo_add (buildinfo, keyname, val);
	}

	free (key);
	free (val);

	return (buildinfo);
}

static int
check_buildstr (char *line, char *buildstr) {
	build_t my[1], their[1];

	if (split_build (line, my) != 0)
		return 0;

	/*
	 * it is the first buildstring we found
	 */
	if (strlen (buildstr) == 0) {
		SAPDB_strcpy (buildstr, line);
		return 1;
	}

	if (split_build (buildstr, their) != 0)
		return 0;

	if (gt_build (my, their) != 0)
		return 0;

	/*
	 * my buildsting this greater than theirs
	 */

	SAPDB_strcpy (buildstr, line);
	return 1;
}

static int
split_build (char *line, build_t *my) {
	char sz_apo[]   = "APO ";
	char sz_rel[]   = "Rel. ";
	char sz_build[] = "Build: ";
	char *rel;
	char *build;
	char *next;
	char *tmp;

	rel = strstr (line, sz_rel);
	my->type = BUILD_TYPE_REL;

	if (rel == 0) {
		rel = strstr (line, sz_apo);
		my->type = BUILD_TYPE_APO;
	}

	if (rel == 0)
		return -1;

	build = strstr (line, sz_build);
	if (build == 0)
		return -1;

	switch (my->type) {
	case BUILD_TYPE_REL:
		tmp = (char *) alloca (strlen (build) + 1);
		SAPDB_strcpy (tmp, build);

		my->major = strtol (rel + strlen (sz_rel), &next, 10);
		my->minor = strtol (next + 1, &next, 10);
		my->correction = strtol (next + 1, &next, 10);
		my->build = strtol (build + strlen (sz_build), &next, 10);

		sprintf (line, "Rel. %1d.%1d.%02d %s",
		my->major, my->minor, my->correction, tmp);
		break;
	case BUILD_TYPE_APO:
		my->major = strtol (rel + strlen (sz_apo), &next, 10);
		my->minor = next[1] - '0';
		my->correction = next[2];
		if (my->correction >= 'A' && my->correction <= 'Z') {
			my->correction -= 'A';
		} else if (my->correction >= '0' && my->correction <= '9') {
			my->correction -= '0';
		} else if (my->correction == ' ' || my->correction == '\t') {
			my->correction = 0;
		}
		my->build = strtol (build + strlen (sz_build), &next, 10);
		break;
	default:
		return -1;
	}

	return 0;
}

static int
gt_build (build_t *my, build_t *their) {

	if (my->type < their->type) {
		return -1;
	}

	if ((my->type == their->type) && (my->major < their->major)) {
		return -1;
	}

	if ((my->type == their->type) && (my->major == their->major) &&
	    (my->minor < their->minor)) {
		return -1;
	}

	if ((my->type == their->type) && (my->major == their->major) &&
	(my->minor == their->minor) && (my->correction < their->correction)) {
		return -1;
	}

	if ((my->major == their->major) && (my->minor == their->minor) &&
	(my->correction == their->correction) && (my->build < their->build)) {
		return -1;
	}

	return 0;
}

static void
remove_colon (char *line) {
	char sz_build[] = "Build: ";
	char *ptr;

	ptr = strstr (line, sz_build);
	if (ptr == 0)
		return;

	SAPDB_strcpy (ptr + 5, ptr + 6);
	return;
}

static void
remove_dot (char *line) {
	char sz_rel[] = "Rel. ";
	char *ptr;

	ptr = strstr (line, sz_rel);
	if (ptr == 0)
		return;

	SAPDB_strcpy (ptr + 3, ptr + 4);
	return;
}

/*
 * common part
 */
static buildinfo_t *
buildinfo_add (buildinfo_t *buildinfo, char *key, char *val) {
	buildinfo_t *next, *curr;

	next = (buildinfo_t *) malloc (sizeof (buildinfo_t));
	if (next == 0) {
		buildinfo_free (buildinfo);
		return 0;
	}

	memset (next, 0, sizeof (buildinfo_t));

	next->key = (char *) malloc (strlen (key) + 1);
	if (next->key == 0) {
		buildinfo_free (buildinfo);
		free (next);
		return 0;
	}

	next->val = (char *) malloc (strlen (val) + 1);
	if (next->val == 0) {
		buildinfo_free (buildinfo);
		free (next->key);
		free (next);
		return 0;
	}

	SAPDB_strcpy (next->key, key);
	SAPDB_strcpy (next->val, val);

	if (buildinfo == 0)
		return (next);

	for (curr = buildinfo; curr->next != 0; curr = curr->next) {
		if (strcmp (curr->key, key) == 0) {
			/* update existing key */
			free (curr->val);
			curr->val = next->val;
			free (next->key);
			free (next);

			return (buildinfo);
		}
	}

	curr->next = next;
	return (buildinfo);
}

static buildinfo_t *
buildinfo_del (buildinfo_t *buildinfo, buildinfo_t *curr) {
	buildinfo_t *last;

	if (buildinfo == 0)
		return 0;

	if (curr == 0)
		return (buildinfo);

	for (last = buildinfo; last->next != 0 && last->next != curr; last = last->next);

	if (curr == buildinfo)
		buildinfo = curr->next;
	else
		last->next = curr->next;

	if (curr->key)
		free (curr->key);

	if (curr->val)
		free (curr->val);

	free (curr);
	return (buildinfo);
}

static void
buildinfo_free (buildinfo_t *buildinfo) {
	buildinfo_t *curr, *last;

	curr = buildinfo;
	for (;;) {
		if (curr == 0)
			break;

		if (curr->key != 0)
				free (curr->key);

		if (curr->val != 0)
				free (curr->val);

		last = curr;
		curr = curr->next;

		free (last);
	}

	return;
}

static buildinfo_t *
buildinfo_merge (buildinfo_t *to, buildinfo_t *from) {
	buildinfo_t *curr;

	if (to == 0 && from == 0)
		return 0;

	if (to != 0 && from == 0)
		return (to);

	if (to == 0 && from != 0)
		return (from);

	for (curr = from; curr != 0; curr = curr->next) {
		to = buildinfo_add (to, curr->key, curr->val);
	}

	buildinfo_free (from);
	return (to);
}

static buildinfo_t *
get_buildinfo (int fd) {
#ifdef WIN32
	buildinfo_t *buildinfo;

	if (lseek (fd, 0, SEEK_SET) != 0)
		return 0;

	buildinfo = get_buildinfo_using_res (fd);

	if (lseek (fd, 0, SEEK_SET) != 0)
		return (buildinfo);

	return (buildinfo_merge (buildinfo, get_buildinfo_using_what (fd)));
#endif
#ifdef UNIX
	return (get_buildinfo_using_what (fd));
#endif
}

#ifdef WIN32
static	char exeext[] = ".exe";
static char dllext[] = ".dll";
#endif
#if defined UNIX && defined __hpux && !defined HP_IA64
static char exeext[] = "";
static char dllext[] = ".sl";
#endif
#if defined UNIX && defined __hpux && defined HP_IA64
static char exeext[] = "";
static char dllext[] = ".so";
#endif
#if defined UNIX && !defined __hpux
static char exeext[] = "";
static char dllext[] = ".so";
#endif

static int
isa_pathname (const char *filename) {
	if (filename == 0)
		return 0;

#ifdef WIN32
	if (strlen (filename) < 2)
		return 0;

	if (filename[0] == '\\' && filename[1] == '\\')
		return 1;

	if (strlen (filename )< 4)
		return 0;

	if (
	filename[0] >= 'A' && filename[0] <= 'Z' &&
	filename [1] == ':' && filename[2] == '\\')
		return 1;

	if (
	filename[0] >= 'a' && filename[0] <= 'z' &&
	filename [1] == ':' && filename[2] == '\\')
		return 1;
#endif
#ifdef UNIX
	if (strlen (filename) < 1)
		return 0;

	if (filename[0] =='/')
		return 1;
#endif

	return 0;
}

static char *
get_filename (const char *logical) {
	tsp01_RteError err[1];
	char *buff;

	buff = (char *) malloc (PATH_LEN_EO01);
	if (buff == 0)
		return 0;

	if (isa_pathname (logical) != 0) {
		SAPDB_strncpy (buff, logical, PATH_LEN_EO01-1);
        buff[PATH_LEN_EO01-1]='\0';
		return (buff);
	}

	if (strcmp (logical, "KERNEL") == 0) {
		sqlGetDbrootPgmPath ((tsp00_Pathc &) *buff, 1, err);
		strcat (buff, "kernel");
		strcat (buff, exeext);
		return (buff);
	}

	if (strcmp (logical, "LIBOMS") == 0) {
#if defined WIN32
		sqlGetDbrootPgmPath ((tsp00_Pathc &) *buff, 1, err);
#else
		sqlGetDbrootLibPath ((tsp00_Pathc &) *buff, 1, err);
#endif
		strcat (buff, "liboms");
		strcat (buff, dllext);
		return (buff);
	}

	if (strcmp (logical, "LIBDBPINSTALL") == 0) {
#if defined WIN32
		sqlGetDbrootPgmPath ((tsp00_Pathc &) *buff, 1, err);
#else
		sqlGetDbrootLibPath ((tsp00_Pathc &) *buff, 1, err);
#endif
		strcat (buff, "dbpinstall");
		strcat (buff, dllext);
		return (buff);
	}

#ifdef WIN32
	if (memcmp (logical, "LIB", 3) == 0) {
		HANDLE hdir;
		WIN32_FIND_DATA	dirent[1];
		char *normalized;

		sqlGetDbrootSapPath ((tsp00_Pathc &) *buff, 1, err);
		strcat (buff, "*");
		strcat (buff, dllext);

		hdir = FindFirstFile (buff, dirent);
		if (hdir == INVALID_HANDLE_VALUE) {
			free (buff);
			return 0;
		}

		normalized = 0;
		for (;;) {
			char *ptr;

			normalized = (char *) realloc (normalized, strlen (dirent->cFileName) + 1);
			if (normalized == 0) {
				free (buff);
				FindClose (hdir);
				return 0;
			}

			SAPDB_strcpy (normalized, dirent->cFileName);
			ptr = strstr (normalized, dllext);
			if (ptr != 0)
				*ptr = '\0';

			strupr (normalized);
			if ((strcmp (normalized, logical) == 0) ||
			(strcmp (normalized, logical + 3) == 0)) {
				sqlGetDbrootSapPath ((tsp00_Pathc &) *buff, 1, err);
				strcat (buff, dirent->cFileName);
				free (normalized);
				FindClose (hdir);
				return (buff);
			}

			if (FindNextFile (hdir, dirent) == 0)
				break;
		}

		free (normalized);
		FindClose (hdir);
	}
#endif
#ifdef UNIX
	if (memcmp (logical, "LIB", 3) == 0) {
		int i;
		DIR *dp;
		struct dirent *dirent;
		char *normalized;
		char *ptr;

		sqlGetDbrootSapPath ((tsp00_Pathc &) *buff, 0, err);
		dp = opendir (buff);
		if (dp == 0) {
			free (buff);
			return 0;
		}

		normalized = 0;
		for (;;) {
			dirent = readdir (dp);
			if (dirent == 0) {
				free (normalized);
				free (buff);
				closedir (dp);
				return 0;
			}

			normalized =
			(char *) realloc (normalized, strlen (dirent->d_name) + 1);
			if (normalized == 0) {
				free (normalized);
				free (buff);
				closedir (dp);
				return 0;
			}

			SAPDB_strcpy (normalized, dirent->d_name);
			ptr = strstr (normalized, dllext);
			if (ptr != 0)
				*ptr = '\0';

			for (i = 0; i < strlen (normalized); i++)
				normalized[i] = toupper (normalized[i]);

			if ((strcmp (normalized, logical) == 0) ||
			(strcmp (normalized, logical + 3) == 0)) {
				sqlGetDbrootSapPath ((tsp00_Pathc &) *buff, 1, err);
				strcat (buff, dirent->d_name);
				free (normalized);
				closedir (dp);
				return (buff);
			}
		}

		free (normalized);
		closedir (dp);
	}
#endif

	free (buff);
	return 0;
}

static char *
read_sapdir (void *pdir, int *pidx) {
#ifdef WIN32
	HANDLE dir;
	WIN32_FIND_DATA	dirent[1];
	tsp01_RteError err[1];
	char path[sizeof(tsp00_Pathc)];
	char *out;
	char *ptr;

	if (pdir == 0)
		return 0;

	if (*(void **) pdir == 0) {
		*pidx = 0;

		sqlGetDbrootSapPath ((tsp00_Pathc &) path, 1, err);
		strcat (path, "*");
		strcat (path, dllext);

		dir =  FindFirstFile (path, dirent);
		if (dir == INVALID_HANDLE_VALUE)
			return 0;

		*(void **) pdir = (void *) dir;
	} else {
		dir = *(HANDLE*)pdir;
		if (FindNextFile (dir, dirent) == 0) {
			FindClose (dir);
			return 0;
		}

		*pidx++;
	}

	out = (char *) malloc (strlen (dirent->cFileName) + 4);
	if (out == 0) {
		FindClose (dir);
		return 0;
	}

	SAPDB_strcpy (out, dirent->cFileName);
	ptr = strstr (out, dllext);
	if (ptr != 0)
		*ptr = '\0';

	strupr (out);
	if (memcmp (out, "LIB", 3) == 0)
		return (out);

	SAPDB_strcpy (out, "LIB");
	strcat (out, dirent->cFileName);
	ptr = strstr (out, dllext);
	if (ptr != 0)
		*ptr = '\0';

	strupr (out);
	return (out);
#endif
#ifdef UNIX
	tsp01_RteError err[1];
	tsp00_Pathc path;
	DIR *dir;
	struct dirent *dirent;
	int i;
	char *out;
	char *ptr;

	if (pdir == 0)
		return 0;

	if (*(void **) pdir == 0) {
		*pidx = 0;

		sqlGetDbrootSapPath (path, 0, err);
		dir = opendir ((char *) path);
		if (dir == 0)
			return 0;

		*(void **) pdir = (void *) dir;
		for (;;) {
			char *ptr;
			dirent = readdir (dir);
			if (dirent == 0) {
				closedir (dir);
				return 0;
			}

			ptr = strstr (dirent->d_name, dllext);
			if (ptr != 0 && strcmp (ptr, dllext) == 0)
				break;
		}
	} else {
		dir = *(DIR **)pdir;
		for (;;) {
			char *ptr;
			dirent = readdir (dir);
			if (dirent == 0) {
				closedir (dir);
				return 0;
			}

			ptr = strstr (dirent->d_name, dllext);
			if (ptr != 0 && strcmp (ptr, dllext) == 0)
				break;
		}

		*pidx++;
	}

	out = (char *) malloc (strlen (dirent->d_name) + 4);
	if (out == 0) {
		closedir (dir);
		return 0;
	}

	SAPDB_strcpy (out, dirent->d_name);
	ptr = strstr (out, dllext);
	if (ptr != 0)
		*ptr = '\0';

	for (i = 0; i < strlen (out); i++)
		out[i] = toupper (out[i]);

	if (memcmp (out, "LIB", 3) == 0)
		return (out);

	SAPDB_strcpy (out, "LIB");
	strcat (out, dirent->d_name);
	ptr = strstr (out, dllext);
	if (ptr != 0)
		*ptr = '\0';

	for (i = 0; i < strlen (out); i++)
		out[i] = toupper (out[i]);

	return (out);
#endif
}

/*
 * top level function
 */
bool
cn46BuildInfo (const char *logical, char *buff, int size) {
	char *filename;
	int fd;
	buildinfo_t *buildinfo, *curr;

	char fmt[] = "%-15s = %-61s\n";

	if (buff == 0 || size == 0)
		return 0;

	buff[0] = '\0';

	filename = get_filename (logical);
	if (filename == 0)
		return 0;

	fd = open (filename, O_RDONLY | O_BINARY);
	free (filename);

	if (fd < 0) {
		snprintf (buff, size, (char *) "cannot open %s", logical);
		return 0;
	}

	buildinfo = get_buildinfo (fd);
	close (fd);

	if (buildinfo == 0) {
		snprintf (buff, size, (char *) "cannot find info in %s", logical);
		return 0;
	}

	for (curr = buildinfo; curr != 0; curr = curr->next) {
		if (strcmp (curr->key, "BUILDNUMBER") == 0) {
			snprintf (buff + strlen (buff), size - strlen (buff), fmt,
			curr->key, curr->val);
			break;
		}
	}

	buildinfo_free (buildinfo);
	return 1;
}

bool
cn46BuildInfoFull (const char *logical, char *buff, int size) {
	int fd;
	char *filename;
	buildinfo_t *buildinfo, *curr;

	char fmt[] = "%-15s = %-61s\n";

	if (buff == 0 || size == 0)
		return 0;

	buff[0] = '\0';

	filename = get_filename (logical);
	if (filename == 0)
		return 0;

	fd = open (filename, O_RDONLY | O_BINARY);

	if (fd < 0) {
		snprintf (buff, size, (char *) "cannot open %s", logical);
		free (filename);
		return 0;
	}

	buildinfo = get_buildinfo (fd);
	close (fd);

	if (buildinfo == 0) {
		snprintf (buff, size, (char *) "cannot find info in %s", logical);
		free (filename);
		return 0;
	}

	snprintf (buff + strlen (buff), size - strlen (buff), fmt,
			"FILENAME", filename);

	for (curr = buildinfo; curr != 0; curr = curr->next) {
		if (strcmp (curr->key, "BUILDNUMBER") == 0) {
			snprintf (buff + strlen (buff), size - strlen (buff), fmt,
			curr->key, curr->val);
			buildinfo = buildinfo_del (buildinfo, curr);
		}
	}

	for (curr = buildinfo; curr != 0; curr = curr->next) {
		if (strcmp (curr->key, "OS-VERSION") == 0) {
			snprintf (buff + strlen (buff), size - strlen (buff), fmt,
			curr->key, curr->val);
			buildinfo = buildinfo_del (buildinfo, curr);
		}
	}

	for (curr = buildinfo; curr != 0; curr = curr->next) {
		if (strcmp (curr->key, "ARCHITECTURE") == 0) {
			snprintf (buff + strlen (buff), size - strlen (buff), fmt,
			curr->key, curr->val);
			buildinfo = buildinfo_del (buildinfo, curr);
		}
	}

	for (curr = buildinfo; curr != 0; curr = curr->next) {
		if (strcmp (curr->key, "C-COMPILER") == 0) {
			snprintf (buff + strlen (buff), size - strlen (buff), fmt,
			curr->key, curr->val);
			buildinfo = buildinfo_del (buildinfo, curr);
		}
	}

	for (curr = buildinfo; curr != 0; curr = curr->next) {
		if (strcmp (curr->key, "C++-COMPILER") == 0) {
			snprintf (buff + strlen (buff), size - strlen (buff), fmt,
			curr->key, curr->val);
			buildinfo = buildinfo_del (buildinfo, curr);
		}
	}

	for (curr = buildinfo; curr != 0; curr = curr->next) {
		if (strcmp (curr->key, "C-RUNTIME") == 0) {
			snprintf (buff + strlen (buff), size - strlen (buff), fmt,
			curr->key, curr->val);
			buildinfo = buildinfo_del (buildinfo, curr);
		}
	}

	for (curr = buildinfo; curr != 0; curr = curr->next) {
		if (strcmp (curr->key, "C++-RUNTIME") == 0) {
			snprintf (buff + strlen (buff), size - strlen (buff), fmt,
			curr->key, curr->val);
			buildinfo = buildinfo_del (buildinfo, curr);
		}
	}

	for (curr = buildinfo; curr != 0; curr = curr->next) {
		if (strcmp (curr->key, "C++-RUNTIME-64") == 0) {
			snprintf (buff + strlen (buff), size - strlen (buff), fmt,
			curr->key, curr->val);
			buildinfo = buildinfo_del (buildinfo, curr);
		}
	}

	for (curr = buildinfo; curr != 0; curr = curr->next) {
		if (strcmp (curr->key, "MAKEPATH") == 0) {
			char *from, *to;

			from = curr->val;
			for (;;) {
				to = strchr (from, ',');
				if (to != 0)
					*to = '\0';

				snprintf (buff + strlen (buff), size - strlen (buff), fmt,
				curr->key, from);

				if (to == 0)
					break;

				from = to + 1;
			}

			buildinfo = buildinfo_del (buildinfo, curr);
		}
	}

	for (curr = buildinfo; curr != 0; curr = curr->next) {
		if (curr->key != 0) {
			snprintf (buff + strlen (buff), size - strlen (buff), fmt,
			curr->key, curr->val);
		}
	}

	buildinfo_free (buildinfo);
	free (filename);
	return 1;
}

bool
cn46BuildInfoList (char *buff, int size) {
	char *logical;
	char *filename;
	void *dp;
	int idx;
	int fd;
	buildinfo_t *buildinfo, *curr;

	char fmt[] = "%-31s = %-45s\n";

	char szkernel[]        = "KERNEL";
	char szliboms[]        = "LIBOMS";
	char szlibdbpinstall[] = "LIBDBPINSTALL";

	if (buff == 0 || size == 0)
		return 0;

	buff[0] = '\0';

	logical = 0;
	for (;;) {
		if (logical == 0) {
			logical = (char *) malloc (strlen (szkernel) + 1);
			SAPDB_strcpy (logical, szkernel);
		} else if (strcmp (logical, szkernel) == 0) {
			logical = (char *) realloc (logical, strlen (szliboms) + 1);
			SAPDB_strcpy (logical, szliboms);
		} else if (strcmp (logical, szliboms) == 0) {
			logical = (char *) realloc (logical, strlen (szlibdbpinstall) + 1);
			SAPDB_strcpy (logical, szlibdbpinstall);
		} else {
			if (strcmp (logical, szlibdbpinstall) == 0) {
				dp = 0; idx = 0;
			}

			free (logical);
			logical = read_sapdir (&dp, &idx);
			if (logical == 0)
				return 1;
		}

		filename = get_filename (logical);
		if (filename == 0) {
			continue;
		}

		fd = open (filename, O_RDONLY | O_BINARY);
		free (filename);

		if (fd < 0)
			continue;

		buildinfo = get_buildinfo (fd);
		close (fd);

		if (buildinfo == 0)
			continue;

		for (curr = buildinfo; curr != 0; curr = curr->next) {
			if (strcmp (curr->key, "BUILDNUMBER") == 0) {
				snprintf (buff + strlen (buff), size - strlen (buff), fmt,
				logical, curr->val);
				break;
			}
		}

		buildinfo_free (buildinfo);
	}

	free (logical);
	return 1;
}
