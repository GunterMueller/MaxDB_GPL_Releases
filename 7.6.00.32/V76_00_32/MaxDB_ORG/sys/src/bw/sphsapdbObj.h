/***************************************************************************

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



*/
/* sphsapdbObj.h : Declaration of the Csphsapdb                                 */
/****************************************************************************/

#ifndef __SPHSAPDBOBJ_H_
#define __SPHSAPDBOBJ_H_

#define SQL_INCLUDED
#define OMS_INCLUDED

#define LC_DBPROC_PROTO
#include "Oms/OMS_DbpGlobals.hpp"

/****************************************************************************/
/* Csphsapdb                                                                */
class  Csphsapdb :
  public OmsHandle,
  public SqlHandle,
  public IliveCacheSpec,
  public Isphsapdb
{
private:
  typedef enum{
				SPHN = 0,
				SPHL = 1
  } SP;
  long      m_cRef;
  IUnknown *m_pIUnknown;
public:
	Csphsapdb() : m_cRef(0), m_pIUnknown(NULL)
	{
        IUnknown** pIKernelSink = &m_pIUnknown;
        omsInitSession (pIKernelSink);
#if defined (WIN32)
/*        static_cast<SqlHandle*>(this)->m_str_assign = &str_assign;
        static_cast<SqlHandle*>(this)->m_str_append = &str_append;*/
#endif
        sqlInitSession (pIKernelSink);
	}

#define COM_INTERFACE Isphsapdb
#define COM_INTERFACE_IID IID_Isphsapdb
#include "Oms/OMS_DbpGlobals.hpp"

/*-----------------------------------------------------*/
/* following lines only included for ActiveX Wizard    */
/* no code will be generated                           */
#define BEGIN_COM_MAP(s)                             /**/
#define END_COM_MAP()                                /**/
#define COM_INTERFACE_ENTRY(s)                       /**/
BEGIN_COM_MAP(Csphsapdb)                             /**/
	COM_INTERFACE_ENTRY(Isphsapdb)                     /**/
END_COM_MAP()                                        /**/
/*-----------------------------------------------------*/

/****************************************************************************/
/* Isphsapdb method prototypes  (to be inserted here)                       */

public:
/****************************************************************************/
/*  IliveCacheSpec 'Register' method prototype                              */
	void initSqlStmt(SP		sp,
								char*	hiertab,
								char*	tmptab,
								char*  hiesid,
								char*	sver,
								SQL*	ins_tmp1,
								SQL*	ins_tmp2,
								SQL*	ins_result,
								SQL*	ins_resconst,
								SQL*	sel_subtmp1,
								SQL*	sel_subtmp2,
								SQL*	upd_result);

public:
/****************************************************************************/
/*  IliveCacheSpec 'Register' method prototype                              */
	STDMETHOD(sphn)(/*[in]*/ TABLENAME* hier_inctab,
		            /*[in]*/ TABLENAME* hiertab,
		            /*[in]*/ OmsTypeInt4 act_level,
					/*[in]*/ OmsTypeInt4 end_level,
					/*[in]*/ SVER* sver,
					/*[in]*/ OmsTypeInt4 hiesid_in,
					/*[in]*/ OmsTypeInt4 start_succ,
					/*[in]*/ OmsTypeInt4 start_is_pred);
	STDMETHOD(sphl)(/*[in]*/ TABLENAME* hier_inctab,
		            /*[in]*/ TABLENAME* hiertab,
		            /*[in]*/ OmsTypeInt4 act_level,
					/*[in]*/ OmsTypeInt4 end_level1,
					/*[in]*/ OmsTypeInt4 end_level2,
					/*[in]*/ SVER* sver,
					/*[in]*/ OmsTypeInt4 hiesid_in,
					/*[in]*/ OmsTypeInt4 start_succ);
	STDMETHOD(test)(/*[in]*/ int a);
   	STDMETHOD(Register)();
};

#endif //__SPHSAPDBOBJ_H_
