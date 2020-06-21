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
/* SQLDBCPROCObj.h : Declaration of the class CSQLDBCPROC                 */
/****************************************************************************/

#ifndef __SQLDBCPROCOBJ_H_
#define __SQLDBCPROCOBJ_H_

#undef  SQL_INCLUDED
#undef  OMS_INCLUDED

#define LC_DBPROC_PROTO
#include "livecachedbproc.h" /* no check */

/****************************************************************************/
/* class CSQLDBCPROC  */
class  CSQLDBCPROC :
  public IliveCacheSpec,
  public ISQLDBCPROC
{
private:
    long      m_cRef;
    IUnknown *m_pIUnknown; 
public:
    CSQLDBCPROC() : m_cRef(0), m_pIUnknown(NULL) 
    {
        IUnknown** pIKernelSink = &m_pIUnknown;
    }

#define COM_INTERFACE ISQLDBCPROC
#define COM_INTERFACE_IID IID_ISQLDBCPROC
#include "livecachedbproc.h" /* no check */

/*-----------------------------------------------------*/ 
/* following lines only included for ActiveX Wizard    */
/* no code will be generated                           */
#if defined(CODE_FOR_WIZARD_ONLY)
DECLARE_NOT_AGGREGATABLE(CSQLDBCPROC)
DECLARE_PROTECT_FINAL_CONSTRUCT()
BEGIN_COM_MAP(CSQLDBCPROC)
    COM_INTERFACE_ENTRY(ISQLDBCPROC)
END_COM_MAP()
DECLARE_REGISTRY_RESOURCEID(IDR_SQLDBCPROC)
DECLARE_NOT_AGGREGATABLE(CSQLDBCPROC)
DECLARE_PROTECT_FINAL_CONSTRUCT()
BEGIN_COM_MAP(CSQLDBCPROC)
    COM_INTERFACE_ENTRY(ISQLDBCPROC)
END_COM_MAP()
#endif
/*-----------------------------------------------------*/

public:
    STDMETHOD(Register)();
    /* *** TODO ***
     * Register all stored procedures here
     * E.g. registration is done like:
     *    STDMETHOD(CALC_SUM)(int par1, int par2, int* sum);
     */
  STDMETHOD(SQLDBC2Upper)(TString *par1);

  STDMETHOD(SQLDBCGetKernelVersion)(OmsTypeInt4* par1);
     
};

#endif //__SQLDBCPROCOBJ_H_
