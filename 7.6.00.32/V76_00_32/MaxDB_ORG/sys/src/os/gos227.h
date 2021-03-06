/*!
  @file           gos227.h
  @author         RaymondR
  @brief          Microsoft Cluster Server   ( generated by AppWizard! )
              Definitions of the resource extension property page classes.
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



#ifndef GOS227_H
#define GOS227_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gos222.h"	// for CBasePropertyPage
#include "gos224.h"	// for CObjectPropert


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

class CCluParamsPage;

/////////////////////////////////////////////////////////////////////////////
// External Class Declarations
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//	CCluParamsPage
//
//	Purpose:
//		Parameters page for resources.
//
/////////////////////////////////////////////////////////////////////////////

class CCluParamsPage : public CBasePropertyPage
{
	DECLARE_DYNCREATE(CCluParamsPage)

// Construction
public:
	CCluParamsPage(void);

// Dialog Data
	//{{AFX_DATA(CCluParamsPage)
	enum { IDD = IDD_PP_SAPDB_MSCS_PARAMETERS };
	CEdit	  m_editSERVERDB;
	CString	m_strSERVERDB;
	CString	m_strRemark;
	BOOL	  m_AutoRestSht;
	//}}AFX_DATA
	BOOL	  m_PrevAutoRestSht;
	CString	m_strPrevSERVERDB;
	CString	m_strPrevRemark;

protected:
	enum
	{
		epropSERVERDB,
		epropAutoRestSht,
		epropRemark,
		epropMAX
	};
	CObjectProperty		m_rgProps[epropMAX];

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCluParamsPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	virtual const CObjectProperty *	Pprops(void) const	{ return m_rgProps; }
	virtual DWORD					Cprops(void) const	{ return sizeof(m_rgProps) / sizeof(CObjectProperty); }

// Implementation
protected:
	BOOL	BAllRequiredFieldsPresent(void) const;

	// Generated message map functions
	//{{AFX_MSG(CCluParamsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeRequiredField();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};  //*** class CCluParamsPage


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GOS227_H */
