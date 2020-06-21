/*!
  @defgroup     Sec     Security
  @file         RTECrypto_EntropyPool.cpp
  @author       ChristophB

  @brief		this class provides and interface to an entropy pool

*/

#include "RunTime/Security/Crypto/RTECrypto_EntropyPool.hpp"

#ifndef WIN32

#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#endif

static RTECrypto_EntropyPool SystemEntropyPool;

RTECrypto_EntropyPool::RTECrypto_EntropyPool( )
{
#ifdef WIN32
	hUser = 0;
	hKernel = 0;

	fGetForegroundWindow = 0;
	fGetQueueStatus = 0;

	fCreateToolhelp32Snapshot = 0;
	fHeap32First = 0;
	fHeap32Next = 0;
	fHeap32ListFirst = 0;
	fHeap32ListNext = 0;
	fProcess32First = 0;
	fProcess32Next = 0;
	fThread32First = 0;
	fThread32Next = 0;

#else
	fdDevRandom = -1;
#endif
}

RTECrypto_EntropyPool::~RTECrypto_EntropyPool( )
{
#ifdef WIN32
	if( hUser )
	{
		fGetForegroundWindow = 0;
		fGetQueueStatus = 0;

		FreeLibrary( hUser );
		hUser = 0;
	}

	if( hKernel )
	{
		fCreateToolhelp32Snapshot = 0;
		fHeap32First = 0;
		fHeap32Next = 0;
		fHeap32ListFirst = 0;
		fHeap32ListNext = 0;
		fProcess32First = 0;
		fProcess32Next = 0;
		fThread32First = 0;
		fThread32Next = 0;

		FreeLibrary( hKernel );
		hKernel = 0;
	}

#else
	if( fdDevRandom >= 0 )
	{
		close( fdDevRandom );
		fdDevRandom = -1;
	}
#endif
}

SAPDB_Bool
RTECrypto_EntropyPool::gatherEntropy( SAPDB_UInt4 *Buffer )
{
	Context.init( );

#ifdef WIN32
	/* memory usage statistics */
	MEMORYSTATUS MemoryStatus;
	GlobalMemoryStatus( &MemoryStatus );
	Context.update( &MemoryStatus, sizeof( MemoryStatus ));

	/* current process id */
	DWORD ProcessId = GetCurrentProcessId( );
	Context.update( &ProcessId, sizeof( ProcessId ));

	/* performance counter */
	LARGE_INTEGER PerfCount;
	if( QueryPerformanceCounter( &PerfCount ))
		Context.update( &PerfCount, sizeof( PerfCount ));

	/* gather some data about the GUI state */
	if( hUser )
	{
		if( fGetForegroundWindow )
		{
			HWND hWindow = fGetForegroundWindow( );
			if( hWindow )
				Context.update( &hWindow, sizeof( HANDLE ));
		}

		if( fGetQueueStatus )
		{
			DWORD QueueStatus = fGetQueueStatus( QS_ALLEVENTS );
			Context.update( &QueueStatus, sizeof( DWORD ));
		}
	}

	/* gather some data about the process state */
	if( hKernel )
	{
		HANDLE hSnap;

		HEAPLIST32 hHeapList;
		HEAPENTRY32 hHeapEntry;
		PROCESSENTRY32 ProcessEntry;
		THREADENTRY32 ThreadEntry;

		if(
		fCreateToolhelp32Snapshot &&
		fHeap32First &&
		fHeap32Next &&
		fHeap32ListFirst &&
		fHeap32ListNext &&
		fProcess32First &&
		fProcess32Next &&
		fThread32First &&
		fThread32Next &&
		(hSnap = fCreateToolhelp32Snapshot( TH32CS_SNAPALL, 0 )) != NULL )
		{
			hHeapList.dwSize = sizeof( HEAPLIST32 );		
			if( fHeap32ListFirst( hSnap, &hHeapList ))
				do
				{
					Context.update( &hHeapList, hHeapList.dwSize );
					hHeapEntry.dwSize = sizeof( HEAPENTRY32 );
					if( fHeap32First( &hHeapEntry, hHeapList.th32ProcessID, hHeapList.th32HeapID ))
					{
						SAPDB_Int Count = 50;
						do
							Context.update( &hHeapEntry, hHeapEntry.dwSize );

						while( fHeap32Next( &hHeapEntry )	&& --Count > 0 );
					}
				} while( fHeap32ListNext( hSnap, &hHeapList ));
			
			ProcessEntry.dwSize = sizeof( PROCESSENTRY32 );
			if( fProcess32First( hSnap, &ProcessEntry ))
				do
					Context.update( &ProcessEntry, ProcessEntry.dwSize );
				while( fProcess32Next( hSnap, &ProcessEntry ));

			ThreadEntry.dwSize = sizeof( THREADENTRY32 );
			if( fThread32First( hSnap, &ThreadEntry ))
				do
					Context.update( &ThreadEntry, ThreadEntry.dwSize );
				while( fThread32Next( hSnap, &ThreadEntry ));

			CloseHandle( hSnap );
		}
	}
#else
	/* use the strong KRNG */
	if( fdDevRandom >= 0 )
	{
		SAPDB_UInt1 Buffer[SHA1HashSize];
		SAPDB_Int got = read( fdDevRandom, Buffer, SHA1HashSize );
		if( got >= 0 )
			Context.update( Buffer, got );
	}

	/* add some simple entropy */
	pid_t Pid = getpid( );
	Context.update( &Pid, sizeof( pid_t ));

	uid_t Uid = getuid( );
	Context.update( &Uid, sizeof( uid_t ));

	time_t Time = time( 0 );
	Context.update( &Time, sizeof( time_t ));

#endif

	Context.final( Buffer, SHA1HashSize );
	return true;
}

// initialize the entropy source
SAPDB_Bool
RTECrypto_EntropyPool::initialize( )
{
#ifdef WIN32
	hUser = LoadLibraryEx( "user32.dll", 0, 0 );
	if( hUser )
	{
		fGetForegroundWindow = (GETFOREGROUNDWINDOW) GetProcAddress( hUser, "GetForegroundWindow" );
		fGetQueueStatus = (GETQUEUESTATUS) GetProcAddress( hUser, "GetQueueStatus" );
	}

	hKernel = LoadLibraryEx( "kernel32.dll", 0, 0 );
	if( hKernel )
	{
		fCreateToolhelp32Snapshot = (CREATETOOLHELP32SNAPSHOT)GetProcAddress( hKernel, "CreateToolhelp32Snapshot" );
		fHeap32First = (HEAP32FIRST) GetProcAddress( hKernel, "Heap32First" );
		fHeap32Next = (HEAP32NEXT) GetProcAddress( hKernel, "Heap32Next" );
		fHeap32ListFirst = (HEAP32LIST) GetProcAddress( hKernel, "Heap32ListFirst" );
		fHeap32ListNext = (HEAP32LIST) GetProcAddress( hKernel, "Heap32ListNext" );
		fProcess32First = (PROCESS32) GetProcAddress( hKernel, "Process32First" );
		fProcess32Next = (PROCESS32) GetProcAddress( hKernel, "Process32Next" );
		fThread32First = (THREAD32) GetProcAddress( hKernel, "Thread32First" );
		fThread32Next = (THREAD32) GetProcAddress( hKernel, "Thread32Next" );
	}

#else
	/* use the strong KRNG */
	fdDevRandom = open( "/dev/random", O_RDONLY | O_NONBLOCK );
#endif

	SAPDB_UInt4 Buffer[SHA1HashSize / 4];
	gatherEntropy( Buffer );
	PRNG.setSeed( Buffer, SHA1HashSize );
	memset( Buffer, 0, SHA1HashSize );

	return true;
}

// returns pointer to entropy Pool;
RTECrypto_EntropyPool &
RTECrypto_EntropyPool::getInstance( )
{
	return SystemEntropyPool;
}

// returns some bytes of entropy
SAPDB_Bool
RTECrypto_EntropyPool::nextBytes( void *DataOut, SAPDB_Int Length )
{
	return PRNG.nextBytes( DataOut, Length );
}

extern "C" SAPDB_Bool
initSystemEntropyPool( )
{
	return ( SystemEntropyPool.initialize( ));
}
