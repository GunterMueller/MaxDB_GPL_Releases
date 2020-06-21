/***************************************************************************


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
/* lcsqlcppObj.h : Declaration of the Clcsqlcpp                                 */
/****************************************************************************/

#ifndef __LCSQLCPPOBJ_H_
#define __LCSQLCPPOBJ_H_

#define SQL_INCLUDED
#define OMS_INCLUDED

#define LC_DBPROC_PROTO
#include "livecachedbproc.h" /* no check */

/****************************************************************************/
/* Clcsqldbc                                                                */
class  Clcsqldbc :
  public OmsHandle, 
  public SqlHandle, 
  public IliveCacheSpec,
  public Ilcsqldbc
{
private:
  long      m_cRef;
  IUnknown *m_pIUnknown; 
  SQL       m_insert_parseagain;
  SQL       m_select_parseagain;
  SQL       m_fetch_parseagain;
public:
	Clcsqldbc() : m_cRef(0), m_pIUnknown(NULL) 
	{
        IUnknown** pIKernelSink = &m_pIUnknown;
        omsInitSession (pIKernelSink);
        INIT_STRING_HANDLING;       /* macro defined within livecachedbproc.h */
        sqlInitSession (pIKernelSink);
	}


#define COM_INTERFACE Ilcsqldbc
#define COM_INTERFACE_IID IID_Ilcsqldbc
#include "livecachedbproc.h" /* no check */

/*-----------------------------------------------------*/ 
/* following lines only included for ActiveX Wizard    */
/* no code will be generated                           */
#if defined(CODE_FOR_WIZARD_ONLY)
DECLARE_NOT_AGGREGATABLE(Clcsqldbc)
DECLARE_PROTECT_FINAL_CONSTRUCT()
BEGIN_COM_MAP(Clcsqldbc)
	COM_INTERFACE_ENTRY(Ilcsqldbc)
END_COM_MAP()
DECLARE_REGISTRY_RESOURCEID(IDR_lcsqldbc)
DECLARE_NOT_AGGREGATABLE(Clcsqldbc)
DECLARE_PROTECT_FINAL_CONSTRUCT()
BEGIN_COM_MAP(Clcsqldbc)
	COM_INTERFACE_ENTRY(Ilcsqldbc)
END_COM_MAP()
#endif
/*-----------------------------------------------------*/
private:

public:
  STDMETHOD(LCSQLDBC_TEST1)();
  STDMETHOD(OUTCOPY_MESSAGES2)(/*[in]*/ StreamHandle *ostream, /*[in]*/ StreamHandle *ostream2);
  STDMETHOD(INCOPY_MESSAGES2)(/*[in]*/ StreamHandle *istream, /*[in]*/ StreamHandle *istream2);
  STDMETHOD(Register)();
};

#endif //__LCSQLDBCOBJ_H_
