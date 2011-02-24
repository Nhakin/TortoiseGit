// CreateBranchTagDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Git.h"
#include "TortoiseProc.h"
#include "CreateBranchTagDlg.h"

#include "Messagebox.h"

// CCreateBranchTagDlg dialog

IMPLEMENT_DYNAMIC(CCreateBranchTagDlg, CResizableStandAloneDialog)

CCreateBranchTagDlg::CCreateBranchTagDlg(CWnd* pParent /*=NULL*/)
	: CResizableStandAloneDialog(CCreateBranchTagDlg::IDD, pParent),
	CChooseVersion(this)
{
	m_bIsTag=0;
	m_bSwitch = 0;	// default switch to checkbox not selected
	m_bTrack=0;
	m_bSign=0;
}

CCreateBranchTagDlg::~CCreateBranchTagDlg()
{
}

void CCreateBranchTagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	CHOOSE_VERSION_DDX;

	DDX_Text(pDX, IDC_BRANCH_TAG, this->m_BranchTagName);
	DDX_Check(pDX,IDC_CHECK_FORCE,this->m_bForce);
	DDX_Check(pDX,IDC_CHECK_TRACK,this->m_bTrack);
	DDX_Check(pDX,IDC_CHECK_SWITCH,this->m_bSwitch);
	DDX_Check(pDX,IDC_CHECK_SIGN,this->m_bSign);
	DDX_Text(pDX, IDC_EDIT_MESSAGE,this->m_Message);
}


BEGIN_MESSAGE_MAP(CCreateBranchTagDlg, CResizableStandAloneDialog)
	CHOOSE_VERSION_EVENT
	ON_BN_CLICKED(IDOK, &CCreateBranchTagDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX_BRANCH, &CCreateBranchTagDlg::OnCbnSelchangeComboboxexBranch)
//	ON_BN_CLICKED(IDC_BUTTON_BROWSE_REF, &CCreateBranchTagDlg::OnBnClickedButtonBrowseRef)
ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CCreateBranchTagDlg::OnInitDialog()
{
	CResizableStandAloneDialog::OnInitDialog();

	CHOOSE_VERSION_ADDANCHOR;

	AddAnchor(IDC_GROUP_BRANCH, TOP_LEFT, TOP_RIGHT);
	
	AddAnchor(IDC_GROUP_OPTION, TOP_LEFT, TOP_RIGHT);

	AddAnchor(IDOK,BOTTOM_RIGHT);
	AddAnchor(IDCANCEL,BOTTOM_RIGHT);
	AddAnchor(IDHELP, BOTTOM_RIGHT);
	AddAnchor(IDC_GROUP_MESSAGE,TOP_LEFT,BOTTOM_RIGHT);
	AddAnchor(IDC_EDIT_MESSAGE,TOP_LEFT,BOTTOM_RIGHT);

	this->AddOthersToAnchor();

	if(m_Base.IsEmpty())
	{
		this->SetDefaultChoose(IDC_RADIO_HEAD);
	
	}else
	{
		this->SetDefaultChoose(IDC_RADIO_VERSION);
		this->GetDlgItem(IDC_COMBOBOXEX_VERSION)->SetWindowTextW(m_Base);
	}

	Init();

	this->GetDlgItem(IDC_CHECK_TRACK)->EnableWindow(FALSE);

	if(this->m_bIsTag)
	{
		this->SetWindowTextW(_T("Create Tag"));
		this->GetDlgItem(IDC_LABEL_BRANCH)->SetWindowTextW(_T("Tag"));
		this->GetDlgItem(IDC_CHECK_SIGN)->EnableWindow(!g_Git.GetConfigValue(_T("user.signingkey")).IsEmpty());
	}
	else
	{
		this->SetWindowTextW(_T("Create Branch"));
		this->GetDlgItem(IDC_LABEL_BRANCH)->SetWindowTextW(_T("Branch"));
		this->GetDlgItem(IDC_EDIT_MESSAGE)->EnableWindow(false);
		this->GetDlgItem(IDC_CHECK_SIGN)->EnableWindow(false);
	}
	// show the switch checkbox if we are a create branch dialog
	this->GetDlgItem(IDC_CHECK_SWITCH)->ShowWindow( !m_bIsTag );
	CWnd* pHead = GetDlgItem(IDC_RADIO_HEAD);
	CString HeadText;
	pHead->GetWindowText( HeadText ); 
	pHead->SetWindowText( HeadText + " (" + g_Git.GetCurrentBranch() + ")");
	EnableSaveRestore(_T("BranchTagDlg"));

	OnCbnSelchangeComboboxexBranch();
	return TRUE;


}
// CCreateBranchTagDlg message handlers

void CCreateBranchTagDlg::OnBnClickedOk()
{
	this->UpdateData(TRUE);

	this->m_BranchTagName.Trim();
	if(this->m_BranchTagName.IsEmpty()  ||  this->m_BranchTagName.Find(' ') >= 0 )
	{
		CMessageBox::Show(NULL, IDS_B_T_NOTEMPTY, IDS_TORTOISEGIT, MB_OK);
		return;
	}
	this->UpdateRevsionName();
	OnOK();
}

void CCreateBranchTagDlg::OnCbnSelchangeComboboxexBranch()
{
	if(this->m_ChooseVersioinBranch.GetString().Left(8)==_T("remotes/"))
	{
		bool isDefault = false;
		this->UpdateData();
		
		CString str = this->m_OldSelectBranch;
		int start = str.ReverseFind(_T('/'));
		if(start>=0)
			str=str.Mid(start+1);
		if(str == m_BranchTagName)
			isDefault =true;
	
		if( m_BranchTagName.IsEmpty() ||  isDefault)
		{
			this->GetDlgItem(IDC_CHECK_TRACK)->EnableWindow(TRUE);

			m_BranchTagName= m_ChooseVersioinBranch.GetString();
			int start =0;
			start = m_BranchTagName.ReverseFind(_T('/'));
			if(start>=0)
				m_BranchTagName = m_BranchTagName.Mid(start+1);

			UpdateData(FALSE);
		}
	}
	else
		this->GetDlgItem(IDC_CHECK_TRACK)->EnableWindow(FALSE);

	if(this->m_bIsTag)
		this->GetDlgItem(IDC_CHECK_TRACK)->EnableWindow(FALSE);

	m_OldSelectBranch = m_ChooseVersioinBranch.GetString();
}

void CCreateBranchTagDlg::OnVersionChanged()
{
	int radio=GetCheckedRadioButton(IDC_RADIO_HEAD,IDC_RADIO_VERSION);
	if (radio == IDC_RADIO_BRANCH)
		OnCbnSelchangeComboboxexBranch();
	else
		GetDlgItem(IDC_CHECK_TRACK)->EnableWindow(FALSE);
}

void CCreateBranchTagDlg::OnDestroy()
{
	WaitForFinishLoading();
	__super::OnDestroy();
}
