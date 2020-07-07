/* @lastChanged: "1998-07-15  10:37"
 * @filename:    hco20.h
 * @purpose:     "System Stored Procedures"
 * @release:     7.2.0.0
 * @see:         ""
 *
 * @Copyright (c) 1998-2005 SAP AG"



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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


#ifndef HCO20_H
#define HCO20_H


 /*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

/****************************************************************************/
/* DbpInstallObj.h : Declaration of the CDbpInstall                         */
/****************************************************************************/

#define SQL_INCLUDED
#define OMS_INCLUDED

#define LC_DBPROC_PROTO
#include "livecachedbproc.h" /* no check */

/****************************************************************************/
/* CDbpInstall                                                                */
class  CDbpInstall :
  public OmsHandle, 
  public SqlHandle, 
  public IliveCacheSpec,
  public IDbpInstall
{
private:
  long      m_cRef;
  IUnknown *m_pIUnknown; 
public:
	CDbpInstall() : m_cRef(0), m_pIUnknown(NULL) 
	{
        IUnknown** pIKernelSink = &m_pIUnknown;
        omsInitSession (pIKernelSink);
#if defined (WIN32)
		INIT_STRING_HANDLING;
#endif
        sqlInitSession (pIKernelSink);
	}

#define COM_INTERFACE IDbpInstall
#define COM_INTERFACE_IID IID_IDbpInstall
#include "livecachedbproc.h" /* no check */

/*-----------------------------------------------------*/ 
/* following lines only included for ActiveX Wizard    */
/* no code will be generated                           */
#define BEGIN_COM_MAP(s)                             /**/
#define END_COM_MAP()                                /**/
#define COM_INTERFACE_ENTRY(s)                       /**/
BEGIN_COM_MAP(CDbpInstall)                           /**/
	COM_INTERFACE_ENTRY(IDbpInstall)                   /**/
END_COM_MAP()                                        /**/
/*-----------------------------------------------------*/

/****************************************************************************/
/* IDbpInstall method prototypes  (to be inserted here)                       */

public:
/****************************************************************************/
/*  IliveCacheSpec 'Register' method prototype                              */
  STDMETHOD(ACTIVATE_DLL)(/*[in]*/ CProgId *pProgId);
  STDMETHOD(COPY_AND_REG_DLL)(/*[in]*/ CPath *DllName, /*[in]*/  DllHandle * Handle, /*[in]*/ int regDll);
  STDMETHOD(REG_DLL)(/*[in]*/ CPath *Path);
  STDMETHOD(SYSDDLTRIGGER)();
  STDMETHOD(FORCE_CHECKPOINT)(/*[out]*/ OmsTypeInt4* chptId);
  STDMETHOD(WAIT_FOR_CHECKPOINT)(/*[int]*/ OmsTypeInt4 chptId);
  STDMETHOD(FORCE_AND_WAIT_FOR_CHECKPOINT)();
  STDMETHOD(Register)();
  STDMETHOD(DROP_SCHEMAS)();
  STDMETHOD(OMS_CLEANUP_AFTER_INIT)();
};

#endif  /* HCO20_H */
