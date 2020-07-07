/*

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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
//
// RunAsGUI.cpp
//

#include <windows.h>
#include <stdio.h>

#ifdef _WIN64
typedef __int64 ptr_t;
#else
typedef DWORD ptr_t;
#endif

class Process {
private:
	HANDLE hProcess;
	HANDLE hThread;
	HANDLE hStdIn;
	HANDLE hStdOut;
	HANDLE hStdErr;
	HANDLE hWait;

public:
	Process ();
	~Process () {this->Close ();};

	HANDLE Open (const char *);
	int Read (char *, int);
	int Write (char *, int);
	int Close ();
	void Wait (HANDLE);

	bool IsRead () {
		return (hStdOut == 0 || hStdOut == INVALID_HANDLE_VALUE) ? FALSE : (hWait == hStdOut);
	}

	bool IsExit () {
		return (hProcess == 0 || hProcess == INVALID_HANDLE_VALUE) ? FALSE : (hWait == hProcess);
	}

	bool IsSignaled (HANDLE handle) {
		return (handle == 0 || handle == INVALID_HANDLE_VALUE) ? FALSE : (hWait == handle);
	}
};

#include <string.h>

Process::Process () {
	hStdIn =   INVALID_HANDLE_VALUE;
	hStdOut =  INVALID_HANDLE_VALUE;
	hStdErr =  INVALID_HANDLE_VALUE;
	hProcess = INVALID_HANDLE_VALUE;
	hThread =  INVALID_HANDLE_VALUE;
}

HANDLE
Process::Open (const char *cmd) {
	int rc;
	SECURITY_ATTRIBUTES saPipe[1];
	HANDLE hStdOutChildRead, hStdOutChildReadDup;
	HANDLE hStdInChildRead, hStdInChildWrite;
	HANDLE hStdInChildWriteDup, hStdOutChildWrite;
	HANDLE hStdInSav, hStdOutSav, hStdErrSav;
	STARTUPINFO siChild[1];
	PROCESS_INFORMATION piChild[1];

	// create stdout/stderr pipe
	hStdOutSav = GetStdHandle (STD_OUTPUT_HANDLE); 
	hStdErrSav = GetStdHandle (STD_ERROR_HANDLE); 

	// init security attributes for pipes
	ZeroMemory (saPipe, sizeof (SECURITY_ATTRIBUTES));
	saPipe->nLength = sizeof (SECURITY_ATTRIBUTES);
	saPipe->lpSecurityDescriptor = 0;
	saPipe->bInheritHandle = TRUE;

	rc = CreatePipe (&hStdOutChildRead, &hStdOutChildWrite, saPipe, 0);
	if (rc == 0)
		return 0;

	rc = SetStdHandle (STD_OUTPUT_HANDLE, hStdOutChildWrite);
	if (rc == 0)
		return 0;

	rc = SetStdHandle (STD_ERROR_HANDLE, hStdOutChildWrite);
	if (rc == 0)
		return 0;

	rc = DuplicateHandle (
	     GetCurrentProcess (), hStdOutChildRead,
	     GetCurrentProcess (), &hStdOutChildReadDup,
		 0, FALSE, DUPLICATE_SAME_ACCESS);
	if (rc == 0)
		return 0;

	rc = CloseHandle (hStdOutChildRead);
	if (rc == 0)
		return 0;

	// create stdin pipe
	hStdInSav = GetStdHandle (STD_INPUT_HANDLE); 

	// init security attributes for pipes
	ZeroMemory (saPipe, sizeof (SECURITY_ATTRIBUTES));
	saPipe->nLength = sizeof (SECURITY_ATTRIBUTES);
	saPipe->lpSecurityDescriptor = 0;
	saPipe->bInheritHandle = TRUE;

	rc = CreatePipe (&hStdInChildRead, &hStdInChildWrite, saPipe, 0);
	if (rc == 0)
		return 0;

	rc = SetStdHandle (STD_INPUT_HANDLE, hStdInChildRead); 
	if (rc == 0)
		return 0;

	rc = DuplicateHandle (
	     GetCurrentProcess (), hStdInChildWrite, 
	     GetCurrentProcess (), &hStdInChildWriteDup,
	     0, FALSE, DUPLICATE_SAME_ACCESS); 

	if (rc == 0)
		return 0;
	
	rc = CloseHandle (hStdInChildWrite);
	if (rc == 0)
		return 0;

	// create child process
	ZeroMemory (siChild, sizeof (STARTUPINFO)); 
	siChild->cb = sizeof (STARTUPINFO);
	
	siChild->hStdInput = hStdInChildRead;
	siChild->hStdOutput = hStdOutChildWrite;
	siChild->hStdError = hStdOutChildWrite;
	siChild->dwFlags = STARTF_USESTDHANDLES;

	rc = CreateProcess
	(0, (LPSTR) cmd, 0, 0, TRUE, CREATE_NO_WINDOW, 0, 0, siChild, piChild);
	if (rc == 0)
		return 0;

	// restore handles
	rc = SetStdHandle (STD_INPUT_HANDLE, hStdInSav);
	if (rc == 0)
		return 0;
 
	rc = SetStdHandle (STD_OUTPUT_HANDLE, hStdOutSav);
	if (rc == 0)
		return 0;

	rc = SetStdHandle (STD_ERROR_HANDLE, hStdErrSav);
	if (rc == 0)
		return 0;

	// close childs end of pipe
	rc = CloseHandle (hStdOutChildWrite);
	if (rc == 0)
		return 0;

	rc = CloseHandle (hStdInChildRead);
	if (rc == 0)
		return 0;

	// thats it
	hProcess = piChild->hProcess;
	hThread = piChild->hThread;
	hStdOut = hStdOutChildReadDup;
	hStdIn = hStdInChildWriteDup;

	return hProcess;
}

int
Process::Close () {
	DWORD ExitCode;
	DWORD rc;

	if (hProcess == INVALID_HANDLE_VALUE)
		return -1;

	// wait for process termination to catch exit code of child process
	for (;;) {
		rc = WaitForSingleObject (hProcess, INFINITE);
		if (rc == WAIT_FAILED) {
			ExitCode = -1;
			break;
		}

		rc = GetExitCodeProcess (hProcess, &ExitCode);
		if (rc == 0) {
			ExitCode = -1;
			break;
		}

		if (ExitCode != STILL_ACTIVE)
			break;
	}

	// put away all handles
	CloseHandle (hProcess);
	hProcess = INVALID_HANDLE_VALUE;

	CloseHandle (hThread);
	hThread = INVALID_HANDLE_VALUE;

	CloseHandle (hStdOut);
	hStdOut = INVALID_HANDLE_VALUE;

	CloseHandle (hStdIn);
	hStdIn = INVALID_HANDLE_VALUE;

	return (ExitCode);
}

void
Process::Wait (HANDLE hUser) {
	DWORD WaitState;
	HANDLE Handles [3];
	
	Handles[0] = hProcess;
	Handles[1] = hStdOut;
	Handles[2] = hUser;

	if (hUser == 0 || hUser == INVALID_HANDLE_VALUE || (ptr_t) hUser % 4)
		WaitState =	WaitForMultipleObjects (2, Handles, FALSE, INFINITE);
	else
		WaitState = WaitForMultipleObjects (3, Handles, FALSE, INFINITE);

	switch (WaitState) {
	case (WAIT_FAILED):
		hWait = hProcess;
		break;
	case (WAIT_OBJECT_0):
		hWait = Handles[0];
		break;
	case (WAIT_OBJECT_0 + 1):
		hWait = Handles[1];
		break;
	case (WAIT_OBJECT_0 + 2):
		hWait = Handles[2];
		break;
	default:
		hWait = 0;
	}

	return;
}

int
Process::Read (char *Buffer, int Want) {
	int rc;
	DWORD Got;

	rc = ReadFile (hStdOut, Buffer, Want, &Got, 0);

	// check for broken pipe
	if ((rc == 0) && (GetLastError () == ERROR_BROKEN_PIPE))
		return 0;

	// unexpected error
	if (rc  == 0)
		return -1;

	return (Got);
}

int
Process::Write (char *Buffer, int Want) {
	int rc;
	DWORD Got;

	rc = WriteFile (hStdIn, Buffer, Want, &Got, 0);

	// check for broken pipe
	if ((rc == 0) && (GetLastError () == ERROR_BROKEN_PIPE))
		return 0;

	// unexpected error
	if (rc  == 0)
		return -1;

	return (Got);
}

int PASCAL
WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Process Proc;
	char Buffer[65536];
	DWORD Want, Got;

	if (lpCmdLine[0] == '\0')
		return 1;

	HANDLE hStdOut = GetStdHandle (STD_OUTPUT_HANDLE);
	HANDLE hStdIn = GetStdHandle (STD_INPUT_HANDLE);

	if (Proc.Open (lpCmdLine) == 0)
		return 1;

	for (;;) {
		Proc.Wait (hStdIn);
		if (Proc.IsRead ()) {
			Want = Proc.Read (Buffer, 65536);
			if (Want == 0)
				continue;

			if (Want < 0)
				break;

			if (hStdOut != 0 && hStdOut != INVALID_HANDLE_VALUE)
				WriteFile (hStdOut, Buffer, Want, &Got, 0);
		} else if (Proc.IsSignaled (hStdIn)) {
			if (!ReadFile (hStdIn, Buffer, 65536, &Want, 0))
				break;

			if (Want == 0)
				continue;

			Got = Proc.Write (Buffer, Want);
		} else if (Proc.IsExit ()) {
			break;
		}
	}

	return (Proc.Close ());
}
