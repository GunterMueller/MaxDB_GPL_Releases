# -----------------------------------------------------------------------------
# 
# PURPOSE
# Handling Description for files related to the livecache tests
# 
# RELEASE:  SAP DB 7.2.04 
# 
# CHANGES
# 29/08/2000 TimW:  First time CheckIn 
# 06/11/2000 TimW:  Introducing _template files
# 13/02/2001 TimW: Modified the cfg- and seq file list
# 12/03/2003 Robert: Unused DEC-specific files removed
#
# -----------------------------------------------------------------------------
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
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

*x -DIMF

# -----------------------------------------------------------------------------
# Java test classes and database parameters
# -----------------------------------------------------------------------------
lctest.py ->$OWN/test/jtest/tests/ ascii
lctest_night.seq ->$OWN/test/jtest/tests/ ascii
lctest_night.cfg ->$OWN/test/jtest/tests/ ascii
lctest_weekend.seq ->$OWN/test/jtest/tests/ ascii
lctest_weekend.cfg ->$OWN/test/jtest/tests/ ascii
lctest_multi.seq ->$OWN/test/jtest/tests/ ascii
lctest_multi.cfg ->$OWN/test/jtest/tests/ ascii
lctest_primary.seq ->$OWN/test/jtest/tests/ ascii
lctest_primary.cfg ->$OWN/test/jtest/tests/ ascii
lctest_recover.seq ->$OWN/test/jtest/tests/ ascii
lctest_recover.cfg ->$OWN/test/jtest/tests/ ascii
lctest_crash.seq ->$OWN/test/jtest/tests/ ascii
lctest_crash.cfg ->$OWN/test/jtest/tests/ ascii
lctest_rollback.seq ->$OWN/test/jtest/tests/ ascii
lctest_rollback.cfg ->$OWN/test/jtest/tests/ ascii
lctest_night_pltf.seq ->$OWN/test/jtest/tests/ ascii
lctest_night_pltf.cfg ->$OWN/test/jtest/tests/ ascii
lctest_hss.seq ->$OWN/test/jtest/tests/ ascii
lctest_hss_weekend.seq ->$OWN/test/jtest/tests/ ascii
lctest_hss.cfg ->$OWN/test/jtest/tests/ ascii

# -----------------------------------------------------------------------------
# Test parameters
# -----------------------------------------------------------------------------
MultiUserRandom_large.par ->$OWN/test/jtest/tests/parameters/ ascii
MultiUserRandom_small.par ->$OWN/test/jtest/tests/parameters/ ascii
MultiUserRandom_crash.par ->$OWN/test/jtest/tests/parameters/ ascii
MultiUserRandom_rollback.par ->$OWN/test/jtest/tests/parameters/ ascii
MultiUserRandom_HSS.par ->$OWN/test/jtest/tests/parameters/ ascii
MultiUserRandom_HSS_large.par ->$OWN/test/jtest/tests/parameters/ ascii
ConsistentRead_small.par ->$OWN/test/jtest/tests/parameters/ ascii
ConsistentRead_large.par ->$OWN/test/jtest/tests/parameters/ ascii
MultiUserRecovery.par ->$OWN/test/jtest/tests/parameters/ ascii
RecoveryCancel_dbstop.par ->$OWN/test/jtest/tests/parameters/ ascii
RecoveryCancel_cancel.par ->$OWN/test/jtest/tests/parameters/ ascii
Performance.par ->$OWN/test/jtest/tests/parameters/ ascii
myLiveCacheTest.par ->$OWN/test/jtest/tests/parameters/ ascii

# -----------------------------------------------------------------------------
# OMS project: LCTEST
# -----------------------------------------------------------------------------

# MS 7.11.03
*.cpp -DLIVECACHE_INTGUIDS

lctestobj.h     dep=OMS_DbpGlobals.hpp,OMS_DbpError.hpp,OMS_ClientInterface.hpp
lctest.xml      lang=r dep=lctestobj.h obj=lctestIspc.o \
                exdep=$WRK/incl/lctest.h,$WRK/incl/lctest_i.c \
                unpack=no noversion \
                -xml2ispc -notdc
&if ($OSSPEC!=WIN32 or defined BIT64) and not ($MACH == SUN || $OSSPEC == SOLARIS)
lctest.xml      -novalidate
&endif


lctesttypes.h inc=:livecache/lctest/lctesterrors.h
lctesterrors.h dep=OMS_DbpError.hpp
lctestversioniter.h 
lctestopenversdir.h inc=ggg250.h

lctest.cpp            -except -DIMF inc=\
                      :livecache/lctest/lctestobj.h,\
					  :livecache/lctest/lctesttypes.h dep=lctest.xml
lctestobj.cpp         -except -DIMF inc=:livecache/lctest/lctestobj.h,\
                      :livecache/lctest/lctestversioniter.h,\
					  :livecache/lctest/lctesterrors.h,:livecache/lctest/lctesttypes.h,\
                      :livecache/lctest/lctestopenversdir.h  
lctesterrors.cpp      -except -DIMF inc=:livecache/lctest/lctesterrors.h
lctestversioniter.cpp -except -DIMF inc=:livecache/lctest/lctestversioniter.h,:livecache/lctest/lctesterrors.h

lcParam.hpp           inc= \
                      dep=OMS_LiveCacheTypes.xml

lcParamSql.h          inc= \
                      dep=OMS_LiveCacheTypes.xml\
                      ,:SAPDB/Interfaces/SQLDBC/SQLDBC.h

lcParamSql.cpp        inc=:livecache/lctest/lcParamSql.h \
                      -I$OWN/usr/sdk/sqldbc/incl

lcBenchSql.h          inc= \
                      dep=OMS_LiveCacheTypes.xml\
                      ,:SAPDB/Interfaces/SQLDBC/SQLDBC.h
                      
lcBenchSql.cpp        inc=:livecache/lctest/lcBenchSql.h \
                      -I$OWN/usr/sdk/sqldbc/incl
