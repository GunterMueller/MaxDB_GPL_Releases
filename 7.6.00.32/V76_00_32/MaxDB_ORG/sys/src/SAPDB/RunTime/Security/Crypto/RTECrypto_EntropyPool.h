/*!
  @defgroup     Sec     Security
  @file         RTECrypto_EntropyPool.h
  @author       ChristophB

  @brief		This function initializes the system entropy poolto provide entropy to the SHA1PRNG

*/

#ifndef RTECRYPTO_ENTROPYPOOL_HPP
#define RTECRYPTO_ENTROPYPOOL_HPP

#include "SAPDBCommon/SAPDB_Types.h"

/*!
 @brief Initialize the system entropy poolto provide entropy to the SHA1PRNG
 */
SAPDB_Bool initSystemEntropyPool( );

#endif

