/*!
  @defgroup     Sec     Security
  @file         RTECrypto_SysRNG.cpp
  @author       ChristophB

  @brief		this class provides a System Random Number Generator

\if EMIT_LICENCE

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
\endif
*/

#include "RunTime/Security/Crypto/RTECrypto_SysRNG.hpp"

#ifdef WIN32
#else
#include <unistd.h>
#include <fcntl.h>
#endif

RTECrypto_SysRNG::RTECrypto_SysRNG( )
{
	Allocator = 0;
	RNGType = RTECrypto_RNG::SysRNG;

#ifdef WIN32
	hAdvapi = 0;
	fRtlGenRandom  = 0;

#else
	fdDevURandom = -1;
#endif
}

RTECrypto_SysRNG::~RTECrypto_SysRNG( )
{
#ifdef WIN32
	if( hAdvapi )
	{
		fRtlGenRandom  = 0;

		FreeLibrary( hAdvapi );
		hAdvapi = 0;
	}
#else
	if( fdDevURandom >= 0 )
	{
		close( fdDevURandom );
		fdDevURandom = 0;
	}
#endif
}

SAPDB_Bool
RTECrypto_SysRNG::nextBytes( void *OutputData, SAPDB_Int Length )
{
#ifdef WIN32
	if( fRtlGenRandom == 0 )
	{
		OSVERSIONINFO OSVersionInfo;
		OSVersionInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
		if( !GetVersionEx( &OSVersionInfo ))
			return false;

		// only implemented on Windows XP or Windows Server 2003 or better
		if(
		OSVersionInfo.dwPlatformId != VER_PLATFORM_WIN32_NT ||
		OSVersionInfo.dwMajorVersion != 5 ||
		OSVersionInfo.dwMinorVersion == 0 )
			return false;

		if( hAdvapi == 0 )
			hAdvapi = LoadLibraryEx( "advapi32", 0, 0 );

		// We do not use it on MajorVersion == 6 ( aka Longhorn or Vista ) right now,
		// because nobody knows if they change the name of the procedure entry.
		//
        // Anyway, this method is documented in MSDN (seach for "RtlGenRandom") for 
		// Windows XP, Server 2003 and Longhorn.
		if( hAdvapi )
			fRtlGenRandom = (RTLGENRANDOM) GetProcAddress( hAdvapi, "SystemFunction036" );
	}

	if( fRtlGenRandom == 0 )
		return false;

	return ( fRtlGenRandom( OutputData, Length ) == 0 ) ? false : true;
#else
	if( fdDevURandom < 0 )
		fdDevURandom = open( "/dev/urandom", O_RDONLY | O_NONBLOCK );

	if( fdDevURandom < 0 )
		return false;

	SAPDB_Int got = read( fdDevURandom, OutputData, Length );
	if( got != Length )
	{
		// system random number generator went into problems
		close( fdDevURandom );
		fdDevURandom = -1;
		return false;
	}

	return true;
#endif
}
