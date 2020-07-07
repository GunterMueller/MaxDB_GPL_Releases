/*!
  @defgroup     Sec     Security
  @file         RTECrypto_EntropyPool.hpp
  @author       ChristophB

  @brief		this class provides and interface to an entropy pool

*/

#ifndef RTECRYPTO_ENTROPYPOOL_HPP
#define RTECRYPTO_ENTROPYPOOL_HPP

#include "RunTime/Security/Crypto/RTECrypto_SHA1.hpp"
#include "RunTime/Security/Crypto/RTECrypto_SHA1PRNG.hpp"

#ifdef WIN32

#include <windows.h>
#include <tlhelp32.h>

/* user32 funtion pointer types */
typedef HWND (WINAPI *GETFOREGROUNDWINDOW)(VOID);
typedef DWORD (WINAPI *GETQUEUESTATUS)(UINT);

/* toolhelp32 function pointer types */
typedef HANDLE (WINAPI *CREATETOOLHELP32SNAPSHOT)(DWORD, DWORD);
typedef BOOL (WINAPI *HEAP32FIRST)(LPHEAPENTRY32, DWORD, DWORD);
typedef BOOL (WINAPI *HEAP32NEXT)(LPHEAPENTRY32);
typedef BOOL (WINAPI *HEAP32LIST)(HANDLE, LPHEAPLIST32);
typedef BOOL (WINAPI *PROCESS32)(HANDLE, LPPROCESSENTRY32);
typedef BOOL (WINAPI *THREAD32)(HANDLE, LPTHREADENTRY32);

#endif

class RTECrypto_EntropyPool
{
public:
	// constructor
	RTECrypto_EntropyPool( );

	// destructor
	~RTECrypto_EntropyPool( );

	// initilizer
	SAPDB_Bool initialize( );

	// there is only one entropy pool
	static RTECrypto_EntropyPool &getInstance( );

	// get next bytes of entropy
	// according to the implemented method, this would take some time,
	// so be carefull
	SAPDB_Bool nextBytes( void *, SAPDB_Int );

private:
	// condens entropy here
	RTECrypto_SHA1 Context;

	// generate bits here
	RTECrypto_SHA1PRNG PRNG;

	SAPDB_Bool gatherEntropy( SAPDB_UInt4 * );

#ifdef WIN32
	HMODULE hUser;
	HMODULE hKernel;

	GETFOREGROUNDWINDOW fGetForegroundWindow;
	GETQUEUESTATUS fGetQueueStatus;

	CREATETOOLHELP32SNAPSHOT fCreateToolhelp32Snapshot;
	HEAP32FIRST fHeap32First;
	HEAP32NEXT fHeap32Next;
	HEAP32LIST fHeap32ListFirst;
	HEAP32LIST fHeap32ListNext;
	PROCESS32 fProcess32First;
	PROCESS32 fProcess32Next;
	THREAD32 fThread32First;
	THREAD32 fThread32Next;

#else
	SAPDB_Int fdDevRandom;
#endif
};

#endif
