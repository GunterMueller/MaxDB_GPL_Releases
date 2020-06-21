/*!
  @file           vos227.cpp
  @author         RaymondR
  @brief          MSCS- Implementation of the resource extension property page classes.
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gos220.h"
#include "gos221.h"
#include "gos227.h"
#include "gos225.h"
#include "gos223.h"
#include "gos200.h"
#include "gos00.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef _DEBUG
# define new DEBUG_NEW
# undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/////////////////////////////////////////////////////////////////////////////
// CCluParamsPage property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCluParamsPage, CBasePropertyPage)

/////////////////////////////////////////////////////////////////////////////
// Message Maps

BEGIN_MESSAGE_MAP(CCluParamsPage, CBasePropertyPage)
	//{{AFX_MSG_MAP(CCluParamsPage)
	ON_EN_CHANGE(IDC_SAPDB_MSCS_SERVERDB,       OnChangeRequiredField)
	//}}AFX_MSG_MAP
	// TODO: Modify the following lines to represent the data displayed on this page.
	ON_EN_CHANGE(IDC_SAPDB_MSCS_SERVERDB,          CBasePropertyPage::OnChangeCtrl)
	ON_EN_CHANGE(IDC_SAPDB_MSCS_AUTO_REST_SHT,     CBasePropertyPage::OnChangeCtrl)
	ON_EN_CHANGE(IDC_SAPDB_MSCS_REMARK,            CBasePropertyPage::OnChangeCtrl)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//++
//
//	CCluParamsPage::CCluParamsPage
//
//	Routine Description:
//		Default constructor.
//
//	Arguments:
//		None.
//
//	Return Value:
//		None.
//
//--
/////////////////////////////////////////////////////////////////////////////
CCluParamsPage::CCluParamsPage(void) : CBasePropertyPage(CCluParamsPage::IDD)
{
	// TODO: Modify the following lines to represent the data displayed on this page.
	//{{AFX_DATA_INIT(CCluParamsPage)
	m_strSERVERDB = _T("");
	m_strRemark   = _T("");
	m_AutoRestSht = FALSE;
	//}}AFX_DATA_INIT

	// Setup the property array.
	{
		m_rgProps[epropSERVERDB].Set   (REG_MSCS_PARAM_SERVERDB_OS00,     m_strSERVERDB, m_strPrevSERVERDB);
		m_rgProps[epropAutoRestSht].Set(REG_MSCS_PARAM_AUTO_RES_SHT_OS00, m_AutoRestSht, m_PrevAutoRestSht);
		m_rgProps[epropRemark].Set     (REG_MSCS_PARAM_REMARK_OS00,       m_strRemark,   m_strPrevRemark);
	}  // Setup the property array

	m_iddPropertyPage = IDD_PP_SAPDB_MSCS_PARAMETERS;
	m_iddWizardPage   = IDD_WIZ_SAPDB_MSCS_PARAMETERS;

}  //*** CCluParamsPage::CCluParamsPage()

/////////////////////////////////////////////////////////////////////////////
//++
//
//	CCluParamsPage::DoDataExchange
//
//	Routine Description:
//		Do data exchange between the dialog and the class.
//
//	Arguments:
//		pDX		[IN OUT] Data exchange object 
//
//	Return Value:
//		None.
//
//--
/////////////////////////////////////////////////////////////////////////////
void CCluParamsPage::DoDataExchange(CDataExchange * pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Modify the following lines to represent the data displayed on this page.
	//{{AFX_DATA_MAP(CCluParamsPage)
	DDX_Control(pDX, IDC_SAPDB_MSCS_SERVERDB,    m_editSERVERDB);
	DDX_Text(pDX,  IDC_SAPDB_MSCS_SERVERDB,      m_strSERVERDB);
	DDX_Text(pDX,  IDC_SAPDB_MSCS_REMARK,        m_strRemark);
	DDX_Check(pDX, IDC_SAPDB_MSCS_AUTO_REST_SHT, m_AutoRestSht);
	//}}AFX_DATA_MAP

	// Handle numeric parameters.
	// TODO: Change the last argument to TRUE if any of these are signed values.
	if (!BBackPressed())
	{
	}  // if:  back button not pressed

	// TODO: Add any additional field validation here.
	if (pDX->m_bSaveAndValidate)
	{
		// Make sure all required fields are present.
		if (!BBackPressed())
		{
			DDV_RequiredText(pDX, IDC_SAPDB_MSCS_SERVERDB, IDC_SAPDB_MSCS_SERVERDB_LABEL, m_strSERVERDB);
		}  // if:  back button not pressed
	}  // if:  saving data from dialog

	CBasePropertyPage::DoDataExchange(pDX);

}  //*** CCluParamsPage::DoDataExchange()

/////////////////////////////////////////////////////////////////////////////
//++
//
//	CCluParamsPage::OnInitDialog
//
//	Routine Description:
//		Handler for the WM_INITDIALOG message.
//
//	Arguments:
//		None.
//
//	Return Value:
//		TRUE		We need the focus to be set for us.
//		FALSE		We already set the focus to the proper control.
//
//--
/////////////////////////////////////////////////////////////////////////////
BOOL CCluParamsPage::OnInitDialog(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CBasePropertyPage::OnInitDialog();

	// TODO:
	// Limit the size of the text that can be entered in edit controls.

	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE

}  //*** CCluParamsPage::OnInitDialog()

/////////////////////////////////////////////////////////////////////////////
//++
//
//	CCluParamsPage::OnSetActive
//
//	Routine Description:
//		Handler for the PSN_SETACTIVE message.
//
//	Arguments:
//		None.
//
//	Return Value:
//		TRUE	Page successfully initialized.
//		FALSE	Page not initialized.
//
//--
/////////////////////////////////////////////////////////////////////////////
BOOL CCluParamsPage::OnSetActive(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Enable/disable the Next/Finish button.
	if (BWizard())
		EnableNext(BAllRequiredFieldsPresent());

	return CBasePropertyPage::OnSetActive();

}  //*** CCluParamsPage::OnSetActive()

/////////////////////////////////////////////////////////////////////////////
//++
//
//	CCluParamsPage::OnChangeRequiredField
//
//	Routine Description:
//		Handler for the EN_CHANGE message on the Share name or Path edit
//		controls.
//
//	Arguments:
//		None.
//
//	Return Value:
//		None.
//
//--
/////////////////////////////////////////////////////////////////////////////
void CCluParamsPage::OnChangeRequiredField(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	OnChangeCtrl();

	if (BWizard())
		EnableNext(BAllRequiredFieldsPresent());

}  //*** CCluParamsPage::OnChangeRequiredField()

/////////////////////////////////////////////////////////////////////////////
//++
//
//	CCluParamsPage::BAllRequiredFieldsPresent
//
//	Routine Description:
//		Handler for the EN_CHANGE message on the Share name or Path edit
//		controls.
//
//	Arguments:
//		None.
//
//	Return Value:
//		None.
//
//--
/////////////////////////////////////////////////////////////////////////////
BOOL CCluParamsPage::BAllRequiredFieldsPresent(void) const
{
	BOOL	bPresent;

	if (m_editSERVERDB.GetWindowTextLength() == 0)
		bPresent = FALSE;
	else
		bPresent = TRUE;

	return bPresent;

}  //*** CCluParamsPage::BAllRequiredFieldsPresent()


/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/