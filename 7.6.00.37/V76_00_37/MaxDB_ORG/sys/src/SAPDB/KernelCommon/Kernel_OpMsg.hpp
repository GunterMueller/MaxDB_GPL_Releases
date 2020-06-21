  /*!
  @file           Kernel_OpMsg.hpp
  @author         ThomasA
  @ingroup        Kernel
  @brief          Stream to write formatted knldiag entries

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



#ifndef KERNEL_OPMSG_HPP
#define KERNEL_OPMSG_HPP

#include "ggg00.h"
#include "gsp03.h"
#include "SAPDBCommon/SAPDB_OStream.hpp"

/*!
   @brief  stream interface to knldiag.
   @code           
    
    // write two lines to the vtrace
    Kernel_VTrace () << "line1" << NewLine << line2;
    // the second new line is implicit when the stream is destructed
    
    @endcode        

     Stream objects are instantiated on the stack rather than
     shared between tasks.
     A new line is written to the vtrace when
   - a '\n' character is found in the stream buffer
   - the buffer size is exceeded
   - the stream object is destructed
     Warning: For performance reasons, strings should be formatted using
     the classes &lt;Class>SAPDB_LeftJ&lt;/> and &lt;Class>SAPDB_RightJ&lt;/>. Unlike
     &lt;Class>SAPDB_ToString&lt;/>, these classes will not copy the string, but
     rather keep only the pointer.
     Warning: Never use multiple temporary Kernel_VTrace () objects in
     the same scope. They will be destructed (and flushed) in the reverse
     order, thus showing all output lines in the reverse order.

 */

class Kernel_OpMsg
    : public SAPDB_OStream
{
protected:
    Kernel_OpMsg (tsp3_priority_Enum prio,tsp3_msg_type_Enum type, int msgNo, const char* label); //!< constructor
    ~Kernel_OpMsg (); //!< destructor
private:
    /*!
       @brief line buffer
    */
    class LineBuffer
        : public SAPDB_StreamBuffer
    {
    public:
        LineBuffer (tsp3_priority_Enum prio,tsp3_msg_type_Enum type, int msgNo, const char* label); //!< constructor
        virtual void Overflow (); //!< overflow handling
    private:
		char               m_txt[40]; 
		tsp3_priority_Enum m_prio;
        tsp3_msg_type_Enum m_type;
        int                m_no;
        tsp00_C8           m_label;
    };

    /* EndClass: LineBuffer */

private:
    LineBuffer m_buf; //!line buffer
};



/*!
  @class          Kernel_OpError
   @brief          stream interface to write errors into knldiag and knldiag.err
   @code           
    
    // write two lines to knldiag.err
    Kernel_OpError (csp3_no_more_memory, csp3_n_memory) << "line1" << NewLine << line2;
    // the second new line is implicit when the stream is destructed
    
    @endcode        
 */


class Kernel_OpError : public Kernel_OpMsg
{
public :
	Kernel_OpError(int msgNo, const char* label)
		: Kernel_OpMsg(sp3p_knldiag, sp3m_error, msgNo, label)
	{
	}
};



/*!
  @class          Kernel_OpInfo
   @brief          stream interface to write info into knldiag

 */


class Kernel_OpInfo : public Kernel_OpMsg
{
public :
	Kernel_OpInfo(const char* label)
		: Kernel_OpMsg(sp3p_knldiag, sp3m_info, csp3_msg, label)
	{
	}
};



/*!
  @class          Kernel_OpWarning
   @brief          stream interface to write warnings into knldiag
   @code           
    
    // write two lines to knldiag.err
    Kernel_OpWarning (csp3_no_more_memory, csp3_n_memory) << "line1" << NewLine << line2;
    // the second new line is implicit when the stream is destructed
    
    @endcode        
 */


class Kernel_OpWarning : public Kernel_OpMsg
{
public :
	Kernel_OpWarning(int msgNo, const char* label)
		: Kernel_OpMsg(sp3p_knldiag, sp3m_warning, msgNo, label)
	{
	}
};



#endif // KERNEL_OPMSG_HPP
