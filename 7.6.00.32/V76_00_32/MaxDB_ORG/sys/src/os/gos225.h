/*!
  @file           gos225.h
  @author         RaymondR
  @brief          Microsoft Cluster Server   ( generated by AppWizard! )
              Definition of the CExtObject class
  @see            

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



#ifndef GOS225_H
#define GOS225_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <CluAdmEx.h>	  // for CLUADMEX_OBJECT_TYPE and interfaces
#include "vos228_i.h"  	// MIDL generated (no check)


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/////////////////////////////////////////////////////////////////////////////
// Forward Class Declarations
/////////////////////////////////////////////////////////////////////////////

class CExtObject;
class CObjData;
class CResData;

/////////////////////////////////////////////////////////////////////////////
// External Class Declarations
/////////////////////////////////////////////////////////////////////////////

class CBasePropertyPage;

/////////////////////////////////////////////////////////////////////////////
// CPageList
/////////////////////////////////////////////////////////////////////////////

typedef CList<CBasePropertyPage *, CBasePropertyPage *> CPageList;

/////////////////////////////////////////////////////////////////////////////
// class CObjData
/////////////////////////////////////////////////////////////////////////////

class CObjData
{
public:
	CString					m_strName;
	CLUADMEX_OBJECT_TYPE	m_cot;

	virtual ~CObjData(void) { }

};  //*** class CObjData

/////////////////////////////////////////////////////////////////////////////
// class CNodeData
/////////////////////////////////////////////////////////////////////////////

class CNodeData : public CObjData
{
public:
	HNODE		m_hnode;

};  //*** class CNodeData

/////////////////////////////////////////////////////////////////////////////
// class CGroupData
/////////////////////////////////////////////////////////////////////////////

class CGroupData : public CObjData
{
public:
	HGROUP		m_hgroup;

};  //*** class CGroupData

/////////////////////////////////////////////////////////////////////////////
// class CResData
/////////////////////////////////////////////////////////////////////////////

class CResData : public CObjData
{
public:
	HRESOURCE	m_hresource;
	CString		m_strResTypeName;

};  //*** class CResData

/////////////////////////////////////////////////////////////////////////////
// class CNetworkData
/////////////////////////////////////////////////////////////////////////////

class CNetworkData : public CObjData
{
public:
	HNETWORK	m_hnetwork;

};  //*** class CNetworkData

/////////////////////////////////////////////////////////////////////////////
// class CNetInterfaceData
/////////////////////////////////////////////////////////////////////////////

class CNetInterfaceData : public CObjData
{
public:
	HNETINTERFACE	m_hnetinterface;

};  //*** class CNetInterfaceData

/////////////////////////////////////////////////////////////////////////////
// class CExtObject
/////////////////////////////////////////////////////////////////////////////

//REVIEW -- using pointers to ID's is necessary because some compilers don't like
//references as template arguments.

class CExtObject :
	public IWEExtendPropertySheet,
	public IWEExtendWizard,
	public ISupportErrorInfo,
	public CComObjectRoot,
	public CComCoClass<CExtObject, &CLSID_CoSAPDBCluEx>
{
public:
	CExtObject(void);

#if _MSC_VER > 1100
BEGIN_COM_MAP(CExtObject)
	COM_INTERFACE_ENTRY_IID(IID_IWEExtendPropertySheet, IWEExtendPropertySheet)
	COM_INTERFACE_ENTRY_IID(IID_IWEExtendWizard, IWEExtendWizard)
	COM_INTERFACE_ENTRY_IID(IID_ISupportErrorInfo, ISupportErrorInfo)
END_COM_MAP()
#else
BEGIN_COM_MAP(CExtObject)
	COM_INTERFACE_ENTRY(IWEExtendPropertySheet)
	COM_INTERFACE_ENTRY(IWEExtendWizard)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
#endif 
//DECLARE_NOT_AGGREGATABLE(CExtObject) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation.  The default is to support it

#pragma warning(disable : 4996)
DECLARE_REGISTRY(CExtObject, _T("CLUADMEX.CluEx"), _T("CLUADMEX.CluEx"), IDS_CLUADMEX_COMOBJ_DESC, THREADFLAGS_APARTMENT)
#pragma warning(default : 4996)

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IWEExtendPropertySheet
public:
	STDMETHOD(CreatePropertySheetPages)(
					IN IUnknown *					piData,
					IN IWCPropertySheetCallback *	piCallback
					);

// IWEExtendWizard
public:
	STDMETHOD(CreateWizardPages)(
					IN IUnknown *			piData,
					IN IWCWizardCallback *	piCallback
					);

// Attributes
protected:
	IUnknown *					m_piData;
	IWCWizardCallback *			m_piWizardCallback;
	BOOL						m_bWizard;
	DWORD						m_istrResTypeName;

	// IGetClusterUIInfo data
	LCID						m_lcid;
	HFONT						m_hfont;
	HICON						m_hicon;

	// IGetClusterDataInfo data
	HCLUSTER					m_hcluster;
	LONG						m_cobj;

	CObjData *					m_podObjData;

	CObjData *					PodObjDataRW(void) const		{ return m_podObjData; }
	CResData *					PrdResDataRW(void) const		{ return (CResData *) m_podObjData; }

public:
	IUnknown *					PiData(void) const				{ return m_piData; }
	IWCWizardCallback *			PiWizardCallback(void) const	{ return m_piWizardCallback; }
	BOOL						BWizard(void) const				{ return m_bWizard; }
	DWORD						IstrResTypeName(void) const		{ return m_istrResTypeName; }

	// IGetClusterUIInfo data
	LCID						Lcid(void) const				{ return m_lcid; }
	HFONT						Hfont(void) const				{ return m_hfont; }
	HICON						Hicon(void) const				{ return m_hicon; }

	// IGetClusterDataInfo data
	HCLUSTER					Hcluster(void) const			{ return m_hcluster; }
	LONG						Cobj(void) const				{ return m_cobj; }

	const CObjData *			PodObjData(void) const			{ return m_podObjData; }
	const CNodeData *			PndNodeData(void) const			{ ASSERT(Cot() == CLUADMEX_OT_NODE); return (CNodeData *) m_podObjData; }
	const CGroupData *			PgdGroupData(void) const		{ ASSERT(Cot() == CLUADMEX_OT_GROUP); return (CGroupData *) m_podObjData; }
	const CResData *			PrdResData(void) const			{ ASSERT(Cot() == CLUADMEX_OT_RESOURCE); return (CResData *) m_podObjData; }
	const CNetworkData *		PndNetworkData(void) const		{ ASSERT(Cot() == CLUADMEX_OT_NETWORK); return (CNetworkData *) m_podObjData; }
	const CNetInterfaceData *	PndNetInterfaceData(void) const	{ ASSERT(Cot() == CLUADMEX_OT_NETINTERFACE); return (CNetInterfaceData *) m_podObjData; }

	CLUADMEX_OBJECT_TYPE		Cot(void) const					{ ASSERT(PodObjData() != NULL); return PodObjData()->m_cot; }

	HRESULT						HrGetUIInfo(IUnknown * piData);
	HRESULT						HrSaveData(IUnknown * piData);
	HRESULT						HrGetObjectInfo(void);
	HRESULT						HrGetObjectName(IN OUT IGetClusterObjectInfo * pi);
	HRESULT						HrGetResourceTypeName(IN OUT IGetClusterResourceInfo * pi);

// Implementation
protected:
	virtual ~CExtObject(void);

	CPageList					m_lpg;
	CPageList &					Lpg(void)						{ return m_lpg; }

};  //*** class CExtObject


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GOS225_H */
