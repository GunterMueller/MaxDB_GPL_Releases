/*!
 * \file    OMS_Sequence.hpp
 * \author  ThomasA
 * \brief   OMS Sequence
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#ifndef OMS_SEQUENCE_HPP
#define OMS_SEQUENCE_HPP

#include "Oms/OMS_Defines.h"
#include "livecachetypes.h" // nocheck


struct OmsSequenceBody;
class  OmsHandle;

/*-------------------------------------------------------------------------*/
/// OmsSequence provides a number generator.
/*!
** OmsSequence provides a number generator.
** OmsSequences are independend of the transaction handling, which means, that 
** the returned numbers are always monotonously increasing, even after a rollback
** or a rollback a subtransaction.
** The first generated number of a newly created sequence equals 1.
**
** \see \ref example_OmsSequence
*/
class AFX_EXT_CLASS OmsSequence 
{
public :
  /*-------------------------------------------------------------------------*/
  /// Constructor
  /*!
  ** Constructs an OmsSequence instance. An OmsSequence is a handle for a SAPDB sequence.
  ** The Sequence is identified by a sequence name. 
  ** If a sequence of that name does not exist in the catalog, it is
  ** created automatically. Otherwise the existing sequence is used.
  **
  ** \param h [in] handle
  ** \param sequenceName [in] the name of the sequence. Must fulfil the rules 
  **        for identifiers.
  */
  OmsSequence(const OmsHandle& h, const char* sequenceName);

  OmsSequence(const OmsSequence& source);

  /*-------------------------------------------------------------------------*/
  /// Destructor
  /*!
  ** Destructs an OmsSequence instance. The sequence will still remain in the
  ** catalog and may be reused, if a new instance of the same name is constructed.
  */
  virtual ~OmsSequence();

  /*-------------------------------------------------------------------------*/
  /// Returns the next number generated by the sequence
  /*!
  ** Returns the next number generated by the sequence.
  ** The first call of this method on a newly created sequence returns the value 1.
  **
  ** \param h [in] handle
  ** \return the next number generated by the sequence 
  */
  OmsTypeUInt8 omsNextVal(const OmsHandle& h);

  /*-------------------------------------------------------------------------*/
  /// Assigns a sequence
  void operator= (const OmsSequence& source);

private :
    OmsSequenceBody* m_pBody;
};

/*! endclass: OmsSequence */

#endif
