/*!*********************************************************************

    @file         Kernel_Random.hpp

    @author       DanielD

    @brief        pseudo random numbers in presence of kernel tasking

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
***********************************************************************/

#ifndef KERNEL_RANDOM_HPP
#define KERNEL_RANDOM_HPP

#include "RunTime/Security/Crypto/RTECrypto_RNG.hpp"

/*!

  @class Kernel_Random
  @brief pseudo random numbers

  This class uses platform specific random number generators where available.
  Because this might cause blocking or process switches, a special task
  has to do this as an asynchronous job.

  Because the overhead of a task switch should not happen on every connect,
  a shared buffer is used as a cache.

  You shouldn't create a Kernel_Random on the stack because of this buffer.

 */
class Kernel_Random
    : public RTECrypto_RNG
{
public:
    /*!
        @brief Create a new Kernel_Random
     */
    Kernel_Random ();

    /*!
        @brief fills buf with random bytes
     */
    virtual bool nextBytes (void * buf, int buflen);

    /*
        @brief access to random numbers through a static instance
     */
    static Kernel_Random Generator;

    /*
        @brief you cannot set a seed of a Kernel_Random generator
     */
	virtual bool setSeed( void* buf, int buflen ) { return false; };

private:
    bool initialize ();

    bool            initialized;
    unsigned char   data [1024];
    unsigned char * readpos;
    unsigned char * readend;
};

#endif

