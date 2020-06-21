#include <sys/utime.h>

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#if  defined(WIN32)
#   include	<winsock.h>
#endif

/*==========================================================================*/

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

/*==========================================================================*/

MODULE = ICopy		PACKAGE = ICopy


long 
get_real_name ( path )
    char    * path
    CODE:
	struct _finddata_t found_file;
	
	
	if ( _findfirst(path, &found_file) == -1L ) 
		XSRETURN_EMPTY;
	
	EXTEND(SP, 1);
	XST_mPV(0,found_file.name);
