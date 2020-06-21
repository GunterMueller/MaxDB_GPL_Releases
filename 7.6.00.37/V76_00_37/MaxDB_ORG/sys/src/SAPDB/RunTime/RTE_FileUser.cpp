/*!---------------------------------------------------------------------
  @file           RTE_FileUser.cpp
  @author         Christoph
  @brief          Find any process who is using a file as executable or module
  @see            

\if EMIT_LICENCE

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



\endif
---------------------------------------------------------------------*/

#ifndef WIN32
#define UNIX UNIX
#endif

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#endif

#ifdef UNIX
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#endif

#include "RunTime/RTE_FileUser.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined UNIX
#define HAS_PS_UID HAS_PS_UID
#undef  HAS_PS_USER
#endif

#if defined SNI
#undef  HAS_PS_UID
#define HAS_PS_USER HAS_PS_USER
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

static int psinit (struct procinfo **, struct modinfo **);
static void free_procinfo (struct procinfo *);
static void free_modinfo (struct modinfo *);

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

/* common used functions */
static void
free_procinfo (struct procinfo *ps) {
	struct procinfo *ptr, *curr;

	if (ps == 0)
		return;

	for (curr = ps; curr != 0;) {
		if (curr->cmd != 0)
			free (curr->cmd);

		ptr = curr;
		curr = curr->next;

		free (ptr);
	}
}

static struct procinfo *
add_procinfo (struct procinfo *ps, ptr_t pid, ptr_t ppid, ptr_t uid, char *cmd) {
	struct procinfo *curr = 0;

	curr = (struct procinfo *) malloc (sizeof (struct procinfo));
	if (curr == 0) {
		free_procinfo (ps);
		return 0;
	}

	SAPDB_MemFillNoCheck (curr, 0, sizeof (struct procinfo));
	if (cmd == 0) {
		curr->cmd = (char *) malloc (1);
		if (curr->cmd == 0) {
			free_procinfo (ps);
			return 0;
		}

		curr->cmd[0] = '\0';
	} else {
		curr->cmd = (char *) malloc (strlen (cmd) + 1);
		if (curr->cmd == 0) {
			free_procinfo (ps);
			return 0;
		}

		strcpy (curr->cmd, cmd);
	}

	curr->uid = uid;
	curr->pid = pid;
	curr->ppid = ppid;

	if (ps == 0) {
		ps = curr;
	} else {
		struct procinfo *last;

		for (last = ps; last->next != 0; last = last->next);
		last->next = curr;
	}

	return (ps);
}

static void
free_modinfo (struct modinfo *mi) {
	struct modinfo *curr, *next;

	if (mi == 0)
		return;

	curr = mi;
	for (;;) {
		if (curr->name != 0)
			free (curr->name);

		next = curr->next;
		if (next == 0)
			break;

		free (curr);
		curr = next;
	}
}

static struct modinfo *
add_modinfo (struct modinfo *mi, char *name, ptr_t pid) {
	struct modinfo *curr, *last;

	curr = (struct modinfo *) malloc (sizeof (struct modinfo));
	if (curr == 0) {
		free_modinfo (mi);
		return 0;
	}

	SAPDB_MemFillNoCheck (curr, 0, sizeof (struct modinfo));

	if (mi == 0) {
		mi = curr;
	} else {
		for (last = mi; last->next != 0; last = last->next) {
			if (last->pid != pid)
				continue;

			 if (strcmp (last->name, name) == 0) {
				free (curr);
				return (mi);
			}
		}

		if ((last->pid == pid) && (strcmp (last->name, name) == 0)) {
			free (curr);
			return (mi);
		}

		last->next = curr;
	}

	curr->name = (char *) malloc (strlen (name) + 1);
	if (curr->name == 0) {
		free (curr);
		free_modinfo (mi);
		return 0;
	}

	strcpy (curr->name, name);
	curr->pid = pid;
	return (mi);
}

/* windows specific functions */
#ifdef WIN32
#include <tlhelp32.h>
#include <psapi.h>

typedef LONG KPRIORITY;
typedef DWORD (CALLBACK *NTQUERYSYSTEMINFORMATION)
              (DWORD, PDWORD, DWORD, PVOID);

#define NT_PROCESS_LIST 05

#pragma pack(push)
#pragma pack(8)

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;

typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID;

typedef struct _SYSTEM_THREAD_INFORMATION {
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    KPRIORITY Priority;
    LONG BasePriority;
    ULONG ContextSwitches;
    ULONG ThreadState;
    ULONG WaitReason;
} SYSTEM_THREAD_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER SpareLi1;
    LARGE_INTEGER SpareLi2;
    LARGE_INTEGER SpareLi3;
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SpareUl2;
    ULONG SpareUl3;
    ULONG PeakVirtualSize;
    ULONG VirtualSize;
    ULONG PageFaultCount;
    ULONG PeakWorkingSetSize;
    ULONG WorkingSetSize;
    ULONG QuotaPeakPagedPoolUsage;
    ULONG QuotaPagedPoolUsage;
    ULONG QuotaPeakNonPagedPoolUsage;
    ULONG QuotaNonPagedPoolUsage;
    ULONG PagefileUsage;
    ULONG PeakPagefileUsage;
    ULONG PrivatePageCount;
	SYSTEM_THREAD_INFORMATION TH[1];
} SYSTEM_PROCESS_INFORMATION;

#pragma pack(pop)

static int
getproc_toolhelp32 (struct procinfo **pi) {
	char idle[] = "idle";
	OSVERSIONINFO os_info[1];
	HMODULE hKERNEL32;
	HANDLE hSnap;
	PROCESSENTRY32 pe[1];
	BOOL rc;
	ptr_t pid, ppid;
	char *cmd;

	HANDLE (WINAPI *fCreateToolhelp32Snapshot) (DWORD, DWORD);
	BOOL (WINAPI *fProcess32First) (HANDLE, LPPROCESSENTRY32);
	BOOL (WINAPI *fProcess32Next) (HANDLE, LPPROCESSENTRY32);

	/* set default return value */
	*pi = 0;

	/* this method does not work up to windows nt 4.0 */
	SAPDB_MemFillNoCheck (os_info, 0, sizeof (OSVERSIONINFO));
	os_info->dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx (os_info);

	if (os_info->dwPlatformId != VER_PLATFORM_WIN32_NT &&
	    os_info->dwPlatformId != VER_PLATFORM_WIN32_WINDOWS)
		return -1;

	if (os_info->dwPlatformId == VER_PLATFORM_WIN32_NT &&
		os_info->dwMajorVersion <= 4)
		return -1;

	/* symbols are not available on nt 4.0 */
	hKERNEL32 = LoadLibrary ("kernel32.dll");
	if (hKERNEL32 == 0)
		return -1;

	fCreateToolhelp32Snapshot = (HANDLE (WINAPI *)(DWORD, DWORD))
	GetProcAddress (hKERNEL32, "CreateToolhelp32Snapshot");

	if (fCreateToolhelp32Snapshot == 0) {
		FreeLibrary (hKERNEL32);
		return -1;
	}

	fProcess32First = (BOOL (WINAPI *)(HANDLE, LPPROCESSENTRY32))
	GetProcAddress (hKERNEL32, "Process32First");

	if (fProcess32First == 0) {
		FreeLibrary (hKERNEL32);
		return -1;
	}

	fProcess32Next = (BOOL (WINAPI *)(HANDLE, LPPROCESSENTRY32))
	GetProcAddress (hKERNEL32, "Process32Next");

	if (fProcess32Next == 0) {
		FreeLibrary (hKERNEL32);
		return -1;
	}

	/* get process table snapshot */
	hSnap = (*fCreateToolhelp32Snapshot)(TH32CS_SNAPPROCESS, 0); 
	if (hSnap == INVALID_HANDLE_VALUE) {
		FreeLibrary (hKERNEL32);
		return -1;
	}

	/* read process table entries and convert into on format */
	SAPDB_MemFillNoCheck (pe, 0, sizeof (pe));
	pe->dwSize = sizeof (PROCESSENTRY32);

	rc = (*fProcess32First)(hSnap, pe);
	if (rc == 0) {
		FreeLibrary (hKERNEL32);
		return -1;
	}

	pid = pid = (ptr_t) (pe->th32ProcessID);
	ppid = (ptr_t) (pe->th32ParentProcessID);

	if (pid == 0) {
		cmd = (char *) malloc (strlen (idle) + 1);
		if (cmd == 0) {
			FreeLibrary (hKERNEL32);
			free_procinfo (*pi);
			*pi = 0;
			return -1;
		}

		strcpy (cmd, idle);
	} else {
		cmd = (char *) malloc (strlen (pe->szExeFile) + 1);
		if (cmd == 0) {
			FreeLibrary (hKERNEL32);
			free_procinfo (*pi);
			*pi = 0;
			return -1;
		}

		strcpy (cmd, pe->szExeFile);
	}

	*pi = add_procinfo (*pi, pid, ppid, 0, cmd);
	if (*pi == 0) {
		FreeLibrary (hKERNEL32);
		return -1;
	}

	for (;;) {
		if ((*fProcess32Next)(hSnap, pe) == 0)
			break;

		pid = (ptr_t) (pe->th32ProcessID);
		ppid = (ptr_t) (pe->th32ParentProcessID);
		if (pid == 0) {
			cmd = (char *) malloc (strlen (idle) + 1);
			if (cmd == 0) {
				FreeLibrary (hKERNEL32);
				free_procinfo (*pi);
				*pi = 0;
				return -1;
			}

			strcpy (cmd, idle);
		} else {
			cmd = (char *) malloc (strlen (pe->szExeFile) + 1);
			if (cmd == 0) {
				FreeLibrary (hKERNEL32);
				free_procinfo (*pi);
				*pi = 0;
				return -1;
			}

			strcpy (cmd, pe->szExeFile);
		}

		*pi = add_procinfo (*pi, pid, ppid, 0, cmd);
		if (*pi == 0) {
			FreeLibrary (hKERNEL32);
			return -1;
		}
	}

	FreeLibrary (hKERNEL32);
	return 0;
}

static int
getproc_systeminfo (struct procinfo **pi) {
	OSVERSIONINFO os_info[1];
	NTQUERYSYSTEMINFORMATION NtQuerySystemInformation;	
	DWORD want;
	DWORD got;
	DWORD *buff;
	HMODULE hNTDLL;
	SYSTEM_PROCESS_INFORMATION *curr;

	/* set default return value */
	*pi = 0;

	/* this method works only on windows nt */

	SAPDB_MemFillNoCheck (os_info, 0, sizeof (OSVERSIONINFO));
	os_info->dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx (os_info);

	if (os_info->dwPlatformId != VER_PLATFORM_WIN32_NT)
		return -1;

	/* get initial buffer */
	want = 1000;
	buff = (DWORD *) malloc (want);
	if (buff == 0)
		return -1;

	/* get NtQuerySystemInformation */
	hNTDLL = LoadLibrary ("ntdll.dll");
	if (hNTDLL == 0) {
		free (buff);
		return -1;
	}

	NtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)
	GetProcAddress (hNTDLL, "NtQuerySystemInformation");

	if (NtQuerySystemInformation == 0) {
		CloseHandle (hNTDLL);
		free (buff);
		return -1;
	}

	/* get process list form winnt kernel */
	for (;;) {
		(*NtQuerySystemInformation)
		(NT_PROCESS_LIST, buff, want, &got);
		if (got != 0)
			break;

		free (buff);
		want += 10000;
		buff = (DWORD *) malloc (want);
		if (buff == 0) {
			CloseHandle (hNTDLL);
			return -1;
		}
	}

	FreeLibrary (hNTDLL);

	/* convert it to own format */
	curr = (SYSTEM_PROCESS_INFORMATION *) buff;
	for (;;) {
		char *cmd;
		ptr_t pid, ppid;

		pid = (ptr_t) curr->UniqueProcessId;
		ppid = (ptr_t) curr->InheritedFromUniqueProcessId;

		if (pid == 0) {
			char idle[] = "idle";

			cmd = (char *) malloc (strlen (idle) + 1);
			if (cmd == 0)
				break;

			strcpy (cmd, idle);
		} else {
			int len;

			len = wcslen (curr->ImageName.Buffer);
			cmd = (char *) malloc (len + 1);
			if (cmd == 0)
				break;

			cmd[0] = '\0';
			wcstombs (cmd, curr->ImageName.Buffer, len + 1);
		}

		*pi = add_procinfo (*pi, pid, ppid, 0, cmd);
		free (cmd);
		if (*pi == 0)
			break;

		if (curr->NextEntryOffset == 0)
			break;

		curr = (SYSTEM_PROCESS_INFORMATION *)
		((ptr_t)curr + curr->NextEntryOffset);
	}

	free (buff);
	return 0;
}

static int
getmod_psapi (DWORD pid, struct modinfo **mi) {
	BOOL rc;
	HANDLE hProcess;
	HMODULE hPSAPI;
	DWORD want, got;
	HMODULE *hModules;
	char buff[MAX_PATH + 1];
	int i, items;

	BOOL (WINAPI *fEnumProcessModules)(HANDLE, HMODULE*, DWORD, LPDWORD);
	DWORD (WINAPI *fGetModuleFileNameEx)(HANDLE, HMODULE, LPTSTR, DWORD);

	hPSAPI = LoadLibrary ("psapi.dll");
	if (hPSAPI == 0)
		return -1;

	fEnumProcessModules =
	(BOOL (WINAPI *)(HANDLE, HMODULE*, DWORD, LPDWORD))
	GetProcAddress (hPSAPI, "EnumProcessModules");

	if (fEnumProcessModules == 0) {
		CloseHandle (hPSAPI);
		return -1;
	}

	fGetModuleFileNameEx =
	(DWORD (WINAPI *)(HANDLE, HMODULE, LPTSTR, DWORD))
	GetProcAddress (hPSAPI, "GetModuleFileNameExA");

	if (fGetModuleFileNameEx == 0) {
		CloseHandle (hPSAPI);
		return -1;
	}

	hProcess =
	OpenProcess (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid);
	if (hProcess == INVALID_HANDLE_VALUE) {
		CloseHandle (hPSAPI);
		return -1;
	}

	rc = (* fEnumProcessModules)(hProcess, 0, 0, &want);	
	if (rc == 0) {
		CloseHandle (hProcess);
		CloseHandle (hPSAPI);
		return -1;
	}

	hModules = (HMODULE *) malloc (want);
	if (hModules == 0) {
		CloseHandle (hProcess);
		CloseHandle (hPSAPI);
		return -1;
	}

	rc = (* fEnumProcessModules)(hProcess, hModules, want, &want);	
	if (rc == 0) {
		free (hModules);
		CloseHandle (hProcess);
		CloseHandle (hPSAPI);
		return -1;
	}

	items = want / sizeof (HMODULE);
	for (i = 0; i < items; i++) {
		got = (* fGetModuleFileNameEx)
		(hProcess, hModules[i], buff, sizeof (buff));
		if (got <= 0)
			continue;

		*mi = add_modinfo (*mi, buff, pid);
	}

	free (hModules);
	CloseHandle (hProcess);
	CloseHandle (hPSAPI);
	return 0;
}

static int
getmod_toolhelp32 (DWORD pid, struct modinfo **mi) {
	BOOL rc;
	HMODULE hKERNEL32;
	HANDLE hSnapMod;
	MODULEENTRY32 me[1];

	HANDLE (WINAPI *fCreateToolhelp32Snapshot) (DWORD, DWORD);
	BOOL (WINAPI *fModule32First) (HANDLE, LPMODULEENTRY32);
	BOOL (WINAPI *fModule32Next) (HANDLE, LPMODULEENTRY32);

	if (pid == 0)
		return -1;

	hKERNEL32 = LoadLibrary ("kernel32.dll");
	if (hKERNEL32 == 0)
		return -1;
	
	fCreateToolhelp32Snapshot = (HANDLE (WINAPI *)(DWORD, DWORD))
	GetProcAddress (hKERNEL32, "CreateToolhelp32Snapshot");

	if (fCreateToolhelp32Snapshot == 0) {
		FreeLibrary (hKERNEL32);
		return -1;
	}

	fModule32First = (BOOL (WINAPI *)(HANDLE, LPMODULEENTRY32))
	GetProcAddress (hKERNEL32, "Module32First");

	if (fModule32First == 0) {
		FreeLibrary (hKERNEL32);
		return -1;
	}

	fModule32Next = (BOOL (WINAPI *)(HANDLE, LPMODULEENTRY32))
	GetProcAddress (hKERNEL32, "Module32Next");

	if (fModule32Next == 0) {
		FreeLibrary (hKERNEL32);
		return -1;
	}

	SAPDB_MemFillNoCheck (me, 0, sizeof (me));
	me->dwSize = sizeof (MODULEENTRY32);

	/* get process table snapshot */
	hSnapMod = (*fCreateToolhelp32Snapshot) (TH32CS_SNAPMODULE, pid); 
	if (hSnapMod == INVALID_HANDLE_VALUE) {
		CloseHandle (hSnapMod);
		FreeLibrary (hKERNEL32);
		return -1;
	}

	rc = (*fModule32First)(hSnapMod, me); 
	if (rc == 0) {
		CloseHandle (hSnapMod);
		FreeLibrary (hKERNEL32);
		return -1;
	}

	*mi = add_modinfo (*mi, me->szExePath, pid);

	for (;;) {
		rc = (*fModule32Next)(hSnapMod, me);
		if (rc == 0) {
			CloseHandle (hSnapMod);
			FreeLibrary (hKERNEL32);
			return -1;
		}

		*mi = add_modinfo (*mi, me->szExePath, pid);
	}

	CloseHandle (hSnapMod);
	FreeLibrary (hKERNEL32);
	return 0;
}

static int
psinit (struct procinfo **pi, struct modinfo **mi) {
	int rc;
	struct procinfo *curr;

	*pi = 0;
	*mi = 0;

	/* first get proc list */
	rc = getproc_toolhelp32 (pi);
	if (rc != 0)
		rc = getproc_systeminfo (pi);

	if (rc != 0)
		return -1;

	/* then try to get mod list */
	for (curr = *pi; curr != 0; curr = curr->next) {
		rc = getmod_toolhelp32 (curr->pid, mi);
		if (rc != 0)
			rc = getmod_psapi (curr->pid, mi);

		if (rc != 0)
			continue;
	}

	return 0;
}

static int
is_same_file (const char *name0, const char *name1) {
	BOOL rc;
	HANDLE hnd;
	BY_HANDLE_FILE_INFORMATION fi0[1], fi1[1];

	hnd = CreateFile (name0, 0,
	FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	if (hnd == INVALID_HANDLE_VALUE)
		return 0;

	rc = GetFileInformationByHandle(hnd, fi0);
	CloseHandle(hnd);
	if (rc == 0)
		return 0;

	hnd = CreateFile (name1, 0,
	FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	if (hnd == INVALID_HANDLE_VALUE)
		return 0;

	rc = GetFileInformationByHandle(hnd, fi1);
	CloseHandle(hnd);
	if (rc == 0)
		return 0;

	if (fi0->dwVolumeSerialNumber != fi1->dwVolumeSerialNumber)
		return 0;

	if (fi0->nFileIndexHigh != fi1->nFileIndexHigh)
		return 0;

	if (fi0->nFileIndexLow != fi1->nFileIndexLow)
		return 0;

	return 1;
}

static int
terminate (int pid) {
	HANDLE hProc;

	hProc = OpenProcess (PROCESS_TERMINATE, 0, pid);
	if (hProc == 0)
		return 0;

	TerminateProcess (hProc, 0);
	CloseHandle (hProc);
	return 0;
}

#endif

/* common Unix functions */
#ifdef UNIX
static char *
strstrip (char *txt) {
	int i;

	if (txt == 0)
		return 0;

	for (;;) {
		i = strlen (txt) - 1;
		if (i < 0)
			break;

		if (txt[i] > ' ')
			break;

		txt[i] = '\0';
	}

	return (txt);
}

static int
terminate (int pid) {
	int i;
	if (kill (pid, 0) != 0 && errno == EPERM)
		return (-1);

	if (kill (pid, 0) != 0 && errno == ESRCH)
		return 0;

	kill (pid, SIGTERM);
	if (kill (pid, 0) != 0 && errno == ESRCH)
		return 0;

	kill (pid, SIGKILL);
	return 0;
}
#endif

/* Unix specific functions, 'ps' and 'fuser' are used */
#if defined UNIX && !defined linux 
static int
psinit (struct procinfo **ps, struct modinfo **mi) {
	int fdout[2];
	pid_t pid;
	int state;
	char buff[4096 + 1];
	FILE *fp;

	*ps = 0;
	*mi = 0;

	if (pipe (fdout) < 0)
		return -1;

	pid = fork ();
	if (pid < 0) {
		/* fork failed */
		return -1;
	} else if (pid == 0) {
		/*
		 * child,
		 * do a ps command, redirect STDOUT and STDERR to parent
		 */
		close (fdout[0]);
		dup2 (fdout[1], 1);
		dup2 (fdout[1], 2);

#ifdef __hpux
		/* this is to get XPG4 behavior of 'ps' on HP-UX */
		putenv ("UNIX95=1");
#endif
#if defined HAS_PS_UID
		execlp ("ps", "ps", "-e", "-opid,ppid,uid,comm", 0);
#elif defined HAS_PS_USER
		execlp ("ps", "ps", "-e", "-opid,ppid,user,comm", 0);
#endif
		exit (-1);
	}

	/* parent,  get a file pointer to STDOUT and STDERR of child */
	close (fdout[1]);
	fp = fdopen (fdout[0], "r");
	if (fp == 0) {
		close (fdout[1]);
		waitpid (pid, &state, 0);
		return -1;
	}

	/* read every line of command output and convert it to own format */
	while (fgets (buff, 4096, fp) != 0) {
		char *p_beg;
		char *p_end;

		pid_t pid, ppid;
		uid_t uid;
		char *cmd;

		strstrip (buff);

		/*
		 * each line matches /\s+(\d+)\s+(\d+)\s+(\d+)\s+(.+)$/
		 * find 'pid', 'ppid', 'uid' and at the end 'cmd'
		 * or /\s+(\d+)\s+(\d+)\s+(\S+)\s+(.+)$/
		 * find 'pid', 'ppid', 'user' and at the end 'cmd'
		 */
		p_beg = buff;
		pid = (pid_t) strtol (p_beg, &p_end, 10);
		if (p_end == p_beg)
			continue;

		p_beg = p_end;
		ppid = (pid_t) strtol (p_beg, &p_end, 10);
		if (p_end == p_beg)
			continue;

#if defined HAS_PS_UID
		p_beg = p_end;
		uid = (uid_t) strtol (p_beg, &p_end, 10);
		if (p_end == p_beg)
			continue;

#elif defined HAS_PS_USER
		for (p_beg = p_end; *p_beg == ' ' || *p_beg == '\t'; p_beg++);
		for (p_end = p_beg; *p_end != ' ' || *p_end == '\t'; p_end++);

		{
			struct passwd *pw;
			char *user;
			int size;

			size = p_end - p_beg;
			if (size <= 0)
				continue;

			user = (char *) malloc (size + 1);
			if (user == 0)
				continue;

			SAPDB_MemFillNoCheck (user, 0, size + 1);
			SAPDB_MemCopyNoCheck (user, p_beg, size);

			pw = getpwnam (user);
			uid = pw->pw_uid;
			free (user);
		}
#endif

		for (p_beg = p_end; *p_beg == ' ' || *p_beg == '\t'; p_beg++);
		cmd = p_beg;

		*ps = add_procinfo (*ps, pid, ppid, uid, cmd);
		if (*ps == 0) {
			fclose (fp);
			close (fdout[1]);
			return -1;
		}
	}

	fclose (fp);
	close (fdout[1]);

	return 0;
}
#endif

#ifdef UNIX
static struct procinfo *
who_uses_module (struct procinfo *ps, struct modinfo *mi, const char *modname) {
	pid_t pid;
	int state;
	int fd, fdout[2];
	int off, got, want;
	char *buff, *ptr;
	struct procinfo *currproc, *who;

	who = 0;

	if (pipe (fdout) < 0)
		return 0;	

	pid = fork ();
	if (pid < 0) {
		/* fork failed */
		return 0;
	} else if (pid == 0) {
		/* do a fuser command, redirect STDOUT and STDERR to parent */

		char fuserbincmd[] = "/bin/fuser";
		char fusersbincmd[] = "/sbin/fuser";
		char fuserposixcmd[] = "/usr/sbin/fuser";
		struct stat sb[1];

		close (fdout[0]);
		dup2 (fdout[1], 1);
		dup2 (fdout[1], 2);

#ifdef _AIX
		if (stat (fuserposixcmd, sb) == 0)
			execl (fuserposixcmd, "fuser", "-x", modname, 0);
		else
			execlp ("fuser", "fuser", "-x", modname, 0);
#else
		if (stat (fuserposixcmd, sb) == 0)
			execl (fuserposixcmd, "fuser", modname, 0);
		else if (stat (fuserbincmd, sb) == 0)
			execl (fuserbincmd, "fuser", modname, 0);
		else if (stat (fusersbincmd, sb) == 0)
			execl (fusersbincmd, "fuser", modname, 0);
		else
			execlp ("fuser", "fuser", modname, 0);
#endif
		exit (-1);
	}

	/* read output from fuser */
	close (fdout[1]);

	fd = fdout[0];

	off = 0;
	buff = 0;
	want = 1024;

	for (;;) {
		buff = (char *) realloc (buff, want + off);
		SAPDB_MemFillNoCheck (buff + off, 0 , want);

		got = read (fd, buff + off, want);
		if (got <= 0)
			break;

		off += got;
	}

	close (fd);
	waitpid (pid, &state, 0);
	if (state != 0) {
		free (buff);
		return 0;
	}

	/* skip pattern /.+:\s/, "<modulename>: " */
	ptr = strstr (buff, ": ");
	if (ptr == 0) {
		free (buff);
		return 0;
	}
	ptr++;

	/* excpect pattern /\d+[mtse]/ */
	for (;;) {
		/* skip white spaces */
		for (; *ptr != '\0' && *ptr <= ' '; ptr++);
		if (*ptr == '\0')
			break;

#ifdef _AIX
		/* skip error messages, aix pollutes stderr */
		if (*ptr < '0' || *ptr > '9') {
			for (; *ptr != '\0' && *ptr != '\n'; ptr++);
			if (*ptr == '\0')
				break;

			continue;
		}
#endif

		pid = strtol (ptr, &ptr, 10);

#ifdef _AIX
		/* aix uses a 's' for 'shared lib' and 'e' for 'executable */
		if (*ptr == 's' || *ptr == 'e') {
#elif defined linux
		/* linux uses a 't' for 'text' or a 'e' for 'executable' and a 'm' for 'maped' */
		if (*ptr == 'm' || *ptr == 't' || *ptr == 'e') {
#else
		/* unix uses a 't' for 'text' and a 'm' for 'maped' */
		if (*ptr == 'm' || *ptr == 't') {
#endif
			for (procinfo *curr = ps; curr != 0; curr = curr->next) {
				if (curr->pid != pid)
					continue;
			
				who = add_procinfo (who,
				curr->pid,	curr->ppid, curr->uid, curr->cmd);
				break;
			}
		}

		/* skip following non whitspaces */
		for (; *ptr != '\0' && *ptr > ' '; ptr++);
		if (*ptr == '\0')
			break;
	}

	free (buff);
	return (who);
}
#endif

/* Linux specifc functions, /proc filesystem is used */
#ifdef linux

static int
isa_number (char *str) {
	char *ptr;

	for (ptr = str; *ptr != 0; ptr++)
		if (*ptr < '0' || *ptr > '9')
			return 0;

	return 1;
}

static int
isa_directory (char *filename) {
	int rc;
	struct stat sb[1];

	rc = stat (filename, sb);
	if (rc != 0)
		return 0;

	if (S_ISDIR (sb->st_mode) == 0)
		return 0;

	return 1;
}

static int
is_same_file (const char *name0, const char *name1) {
	int rc;
	struct stat sb0[1], sb1[1];

	rc = stat (name0, sb0);
	if (rc != 0)
		return 0;

	rc = stat (name1, sb1);
	if (rc != 0)
		return 0;

	if (sb0->st_dev != sb1->st_dev)
		return 0;

	if (sb0->st_ino != sb1->st_ino)
		return 0;

	return 1;
}

static char *
unbracket (char *txt) {
	int i;

	if (txt == 0)
		return 0;

	for (;;) {
		i = strlen (txt) - 1;
		if (i < 0)
			break;

		if (txt[i] > ' ')
			break;

		txt[i] = '\0';
	}

	i = strlen (txt) - 1;
	if (txt[i] == ')')
		txt[i] = '\0';

	if (txt[0] == '(') {
		char *tmp;

		tmp = (char *) malloc (strlen (txt));
		if (tmp == 0)
			return 0;

		strcpy (tmp, txt + 1);
		strcpy (txt, tmp);
		free (tmp);
	}

	return (txt);
}

/*
 * line looks like
 * 4013e000-40145000 rw-p 0011a000 08:01 312        /lib/libc.so.6
 * or
 * 4013e000-40145000 rw-p 0011a000 08:01 0
 * that means we have to get the sixth column if there is one
 */
static int
get_module (char *line, char **name, long *maj, long *min, long *ino) {
	char sz_deleted[] = " (deleted)";
	char *ptr, *end;
	int len, col;

	*name = 0;
	*maj  = 0;
	*min  = 0;
	*ino  = 0;

	col = 0;
	for (ptr = line; *ptr != 0; ptr++) {
		if (ptr == line)
			continue;

		if (*ptr == ' ' || *ptr == '\t')
			continue;

		if (*(ptr - 1) != ' ' && *(ptr - 1) != '\t')
			continue;

		col++;

		switch (col) {
		case 3:
			*maj = strtol (ptr, &end, 10);
			*min = strtol (end + 1, &end, 10);
			break;

		case 4:
			*ino = strtol (ptr, &end, 10);
			break;

		case 5:
			strstrip (ptr);
			*name = ptr;

			/* cut off string "(deleted)" from the end */
			len = strlen (ptr);
			end = ptr + len - strlen (sz_deleted);
			if (end - ptr <= 0)
				return 0;

			if (strcmp (end, sz_deleted) == 0) {
				*end = '\0';
				strstrip (ptr);
			}

			return 0;
		}
	}

	return -1;
}

static int
psinit (struct procinfo **ps, struct modinfo **mi) {
	char procdirname[]  = "/proc";
	char mapfilename[]  = "maps";
	char statfilename[] = "stat";
	char *piddirname;
	char *pidmapfilename;
	char *pidstatfilename;

	struct stat sb[1];

	DIR *dp;
	struct dirent *de;

	FILE *fpmap, *fpstat;
	char line[4096];
	char cmd[4096];

	*mi = 0;
	*ps = 0;

	dp = opendir (procdirname);
	if (dp == 0)
		return -1;

	for (;;) {
		de = readdir (dp);
		if (de == 0)
			break;

		if (isa_number (de->d_name) == 0)
			continue; 
				
		piddirname = (char *)
		malloc (strlen (procdirname) + 1 + strlen (de->d_name) + 1);

		if (piddirname == 0) {
			free_modinfo (*mi);
			closedir (dp);
			return -1;
		}

		strcpy (piddirname, procdirname); 
		strcat (piddirname, "/"); 
		strcat (piddirname, de->d_name); 

		if (isa_directory (piddirname) == 0) {
			free (piddirname);
			continue;
		}

		pidstatfilename = (char *)
		malloc (strlen (piddirname) + 1 + strlen (statfilename) + 1);

		if (pidstatfilename == 0) {
			free_procinfo (*ps);
			free_modinfo (*mi);
			free (piddirname);
			closedir (dp);
			return -1;
		}

		strcpy (pidstatfilename, piddirname);
		strcat (pidstatfilename, "/");
		strcat (pidstatfilename, statfilename);

#if 0
		pidmapfilename = (char *)
		malloc (strlen (piddirname) + 1 + strlen (mapfilename) + 1);

		if (pidmapfilename == 0) {
			free_procinfo (*ps);
			free_modinfo (*mi);
			free (piddirname);
			free (pidstatfilename);
			closedir (dp);
			return -1;
		}

		strcpy (pidmapfilename, piddirname);
		strcat (pidmapfilename, "/");
		strcat (pidmapfilename, mapfilename);
#endif

		free (piddirname);

		if (stat (pidstatfilename, sb) != 0) {
			free (pidstatfilename);
			free (pidmapfilename);
			free (piddirname);
			continue;
		}

		fpstat = fopen (pidstatfilename, "r");
		free (pidstatfilename);

#if 0
		fpmap = fopen (pidmapfilename, "r");
		free (pidmapfilename);
#endif

		if (fpstat != 0) {
			char state;
			pid_t pid;
			pid_t ppid;

			if (fgets (line, sizeof (line), fpstat) == 0) {
				fclose (fpstat);
				fclose (fpmap);
				continue;
			}

			strstrip (line);

			sscanf (line, "%d %s %c %d", &pid, cmd, &state, &ppid);
			unbracket (cmd);

			*ps = add_procinfo (*ps, pid, ppid, sb->st_uid, cmd);
			if (*ps == 0) {
				fclose (fpstat);
				fclose (fpmap);
				closedir (dp);
				return -1;
			}

			fclose (fpstat);
		}

#if 0
		if (fpmap != 0) {
			pid_t pid;
			pid = (pid_t) atol (de->d_name);

			for (;;) {
				char *name;
				long maj, min, ino;

				if (fgets (line, sizeof (line), fpmap) == 0)
					break;

				strstrip (line);
				get_module (line, &name, &maj, &min, &ino);

				if (name == 0)
					continue;

				/*
				 * skip major devno == 0,
				 * this is not a regular file
				 */
				if (maj == 0)
					continue;

				*mi = add_modinfo (*mi, name, pid);
				if (*mi == 0) {
					fclose (fpmap);
					closedir (dp);
					return -1;
				}
			}
			fclose (fpmap);
		}
#endif

	}

	closedir (dp);
	return 0;
}
#endif

#ifdef WIN32
static struct procinfo *
who_uses_module (struct procinfo *ps, struct modinfo *mi, const char *modname) {
	struct modinfo *currmod;
	struct procinfo *currproc, *who;

	who = 0;
	if (ps == 0)
		return 0;

	if (mi == 0)
		return 0;

	if (modname == 0 || *modname == '\0')
		return 0;

	for (currmod = mi; currmod->next != 0; currmod = currmod->next) {
		if (is_same_file (currmod->name, modname) == 0)
			continue;

		for (currproc = ps; currproc != 0; currproc = currproc->next) {
			if (currproc->pid != currmod->pid)
				continue;
			
			who = add_procinfo (who, currproc->pid,
			currproc->ppid, currproc->uid, currproc->cmd);
			break;
		}
	}

	return (who);
}
#endif

/*
 * C++ interface
 */
RTE_FileUser::RTE_FileUser () {
	this->mi = 0;
	this->ps = 0;

	psinit (&ps, &mi);
}

RTE_FileUser::~RTE_FileUser () {
	free_modinfo (this->mi);
	free_procinfo (this->ps);
}

RTE_ProcInfo::RTE_ProcInfo (procinfo *mypi) {
	this->pi = mypi;
}

RTE_ProcInfo::~RTE_ProcInfo () {
	free_procinfo (this->pi);
}

procinfo *
RTE_ProcInfo::First () {
	return (this->pi);
}

procinfo *
RTE_ProcInfo::Next (procinfo *mypi) {
	if (mypi == 0)
		return (this->pi);

	return (mypi->next);
}

int
RTE_ProcInfo::Terminate (procinfo *mypi) {
	if (mypi == 0)
		return (-1);

	return (terminate (mypi->pid));
}

RTE_ProcInfo
RTE_FileUser::WhoUsesModule (const char *modname) {
	return (RTE_ProcInfo (who_uses_module (this->ps, this->mi, modname)));
}

