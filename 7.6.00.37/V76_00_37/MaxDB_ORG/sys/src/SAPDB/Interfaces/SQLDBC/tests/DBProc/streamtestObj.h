/***************************************************************************\if EMIT_LICENCE

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
/* streamtestObj.h : Declaration of the class Cstreamtest                 */
/****************************************************************************/

#ifndef __STREAMTESTOBJ_H_
#define __STREAMTESTOBJ_H_

#define SQL_INCLUDED
#define OMS_INCLUDED

#define LC_DBPROC_PROTO
#include "livecachedbproc.h" /* nocheck */

#define LOG_SEVERITY_INFO   1
#define LOG_SEVERITY_ERROR  2

/****************************************************************************/
/* class Cstreamtest  */
class  Cstreamtest :
  public OmsHandle, 
  public SqlHandle, 
  public IliveCacheSpec,
  public Istreamtest
{
private:
  long      m_cRef;
  IUnknown *m_pIUnknown; 
public:
    Cstreamtest() : m_cRef(0), m_pIUnknown(NULL) 
    {
        IUnknown** pIKernelSink = &m_pIUnknown;
        omsInitSession (pIKernelSink);
        INIT_STRING_HANDLING;       /* macro defined within livecachedbproc.h */
        sqlInitSession (pIKernelSink);
    }

#define COM_INTERFACE Istreamtest
#define COM_INTERFACE_IID IID_Istreamtest
#include "livecachedbproc.h" /* nocheck */

/*-----------------------------------------------------*/ 
/* following lines only included for ActiveX Wizard    */
/* no code will be generated                           */
#if defined(CODE_FOR_WIZARD_ONLY)
DECLARE_NOT_AGGREGATABLE(Cstreamtest)
DECLARE_PROTECT_FINAL_CONSTRUCT()
BEGIN_COM_MAP(Cstreamtest)
    COM_INTERFACE_ENTRY(Istreamtest)
END_COM_MAP()
DECLARE_REGISTRY_RESOURCEID(IDR_streamtest)
DECLARE_NOT_AGGREGATABLE(Cstreamtest)
DECLARE_PROTECT_FINAL_CONSTRUCT()
BEGIN_COM_MAP(Cstreamtest)
    COM_INTERFACE_ENTRY(Istreamtest)
END_COM_MAP()
#endif
/*-----------------------------------------------------*/

public:

    STDMETHOD(Register)();
    STDMETHOD(TEST_CHARSTREAM_IN)(CharStreamHandle* instream);
    STDMETHOD(TEST_CHARSTREAM_2)(CharStreamHandle* in1, CharStreamHandle* in2);
    STDMETHOD(TEST_CHARSTREAM_OUT)(CharStreamHandle* outstream, StringName *name);
    STDMETHOD(TEST_CHARSTREAM_ECHO)(CharStreamHandle* instream, CharStreamHandle *outstream, 
                                    StringName* inputname, StringName* outputname);

    STDMETHOD(INCOPY_MESSAGES)(/*[in]*/ StreamHandle *istream);
    STDMETHOD(OUTCOPY_MESSAGES)(/*[in]*/ StreamHandle *ostream);
    STDMETHOD(INCOPY_MESSAGES2)(/*[in]*/ StreamHandle *istream, /*[in]*/ StreamHandle *istream2);
    STDMETHOD(OUTCOPY_MESSAGES2)(/*[in]*/ StreamHandle *ostream, /*[in]*/ StreamHandle *ostream2);
    void log(int severity, const char *fmt, ...);
}; 

#endif //__STREAMTESTOBJ_H_
