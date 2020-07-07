#    ========== licence begin  GPL
#    Copyright (c) 2005 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end

#
# include dependencies (do not edit)
#

# Demo Program

RTECrypto_Test.cpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_RNG.hpp,\
	:SAPDB/RunTime/MemoryManagement/RTEMem_RteAllocator.hpp,\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_EntropyPool.hpp

# Entropy Pool

RTECrypto_EntropyPool.h inc=\
	:SAPDB/SAPDBCommon/SAPDB_Types.h

RTECrypto_EntropyPool.hpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_SHA1.hpp,\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_SHA1PRNG.hpp

RTECrypto_EntropyPool.cpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_EntropyPool.hpp

# Random Number Generator

RTECrypto_RNG.hpp inc=\
	:SAPDB/SAPDBCommon/SAPDB_Types.h,\
	:SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\

RTECrypto_RNG.cpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_RNG.hpp,\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_SysRNG.hpp,\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_SHA1PRNG.hpp

RTECrypto_SysRNG.hpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_RNG.hpp

RTECrypto_SysRNG.cpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_SysRNG.hpp

RTECrypto_SHA1PRNG.hpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_RNG.hpp,\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_HMACSHA1.hpp

RTECrypto_SHA1PRNG.cpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_SHA1PRNG.hpp,\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_EntropyPool.hpp

# Keyed Hash Algorithm

RTECrypto_HMAC.hpp inc=\
	:SAPDB/SAPDBCommon/SAPDB_Types.h,\
	:SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp

RTECrypto_HMACMD5.hpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_HMAC.hpp,\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_MD5.hpp

RTECrypto_HMACMD5.cpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_HMACMD5.hpp

RTECrypto_HMACSHA1.hpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_HMAC.hpp,\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_SHA1.hpp

RTECrypto_HMACSHA1.cpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_HMACSHA1.hpp

RTECrypto_HMAC.cpp inc=\
	:SAPDB/SAPDBCommon/SAPDB_string.h,\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_HMAC.hpp,\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_HMACMD5.hpp,\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_HMACSHA1.hpp

# Basic Hash Algorithm

RTECrypto_MAC.hpp inc=\
	:SAPDB/SAPDBCommon/SAPDB_Types.h,\
	:SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp

RTECrypto_MAC.cpp inc=\
	:SAPDB/SAPDBCommon/SAPDB_string.h,\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_MAC.hpp,\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_MD5.hpp,\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_SHA1.hpp

RTECrypto_MD5.hpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_MAC.hpp

RTECrypto_MD5.cpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_MD5.hpp

RTECrypto_SHA1.hpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_MAC.hpp

RTECrypto_SHA1.cpp inc=\
	:SAPDB/RunTime/Security/Crypto/RTECrypto_SHA1.hpp
