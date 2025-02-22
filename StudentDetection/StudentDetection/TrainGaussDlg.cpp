// TrainGaussDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StudentDetection.h"
#include "TrainGaussDlg.h"
#include "Utils.h"
#include "GaussFilterColor.h"

// TrainGaussDlg dialog

IMPLEMENT_DYNAMIC(TrainGaussDlg, CDialog)

TrainGaussDlg::TrainGaussDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TrainGaussDlg::IDD, pParent)
{

}

TrainGaussDlg::~TrainGaussDlg()
{
}

void TrainGaussDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PREFIX_GAUSS, m_editPrefixGauss);
	DDX_Control(pDX, IDC_EDIT_START_INDEX_GAUSS, m_editStartIndxGauss);
	DDX_Control(pDX, IDC_EDIT_POSFIX_GAUSS, m_editPosfixGauss);
	DDX_Control(pDX, IDC_EDIT_END_INDEX_GAUSS, m_editEndIndxGauss);
	DDX_Control(pDX, IDC_EDIT_OUT_GAUSS, m_editOutPath);
	DDX_Control(pDX, IDC_BTN_BWS_OUT_GAUSS, m_btnBrowseOutPath);
	DDX_Control(pDX, IDC_BTN_TRAIN_GAUSS, m_btnTrainGauss);
	DDX_Control(pDX, IDC_EDIT_PROGRESS, m_editProgress);
}


BEGIN_MESSAGE_MAP(TrainGaussDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_BWS_OUT_GAUSS, &TrainGaussDlg::OnBnClickedBtnBwsOutGauss)
	ON_BN_CLICKED(IDC_BTN_TRAIN_GAUSS, &TrainGaussDlg::OnBnClickedBtnTrainGauss)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// TrainGaussDlg message handlers
HBRUSH TrainGaussDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{

	HBRUSH hbr;

	if( nCtlColor == CTLCOLOR_STATIC )
	{
		pDC->SetBkMode(TRANSPARENT);
		//pDC->SetTextColor(RGB(255, 0, 0));
		hbr = (HBRUSH)GetStockObject( NULL_BRUSH );
	}

	else
	{
		hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
	
	return hbr;
	
}
void TrainGaussDlg::OnBnClickedBtnBwsOutGauss()
{
	// TODO: Add your control notification handler code here
	CFileDialog m_fdlg(false,NULL,NULL,OFN_HIDEREADONLY,_T("Text Files (*.txt)|*.txt||"));
	m_fdlg.m_ofn.lpstrInitialDir = _T("C:\\");
	if(m_fdlg.DoModal()==IDOK)
	{
		m_editOutPath.SetWindowText(m_fdlg.GetPathName());
		UpdateData(false);
	}
}

void TrainGaussDlg::OnBnClickedBtnTrainGauss()
{
	// TODO: Add your control notification handler code here
	CString tmp, prefix, posfix;
	Utils utils;
	GaussFilterColor gauss;

	m_editStartIndxGauss.GetWindowTextW(tmp);
	int startIndx = utils.ConvertToInt(tmp);

	m_editEndIndxGauss.GetWindowTextW(tmp);
	int endIndx = utils.ConvertToInt(tmp);

	m_editPrefixGauss.GetWindowTextW(prefix);
	m_editPosfixGauss.GetWindowTextW(posfix);
	m_editOutPath.GetWindowTextW(tmp);

	m_editProgress.SetWindowTextW(_T("Begin training gaussian..."));
	char *preFix = utils.ConvertToChar(prefix);
	char *posFix = utils.ConvertToChar(posfix);
	char *fileOutput = utils.ConvertToChar(tmp);
	
	gauss.TrainData(preFix, posFix , startIndx - endIndx + 1, startIndx, endIndx, fileOutput);
	m_editProgress.SetWindowTextW(_T("Train Gaussian completed"));

	MessageBox(_T("Train Gaussian completed"), _T("Information"));	

	delete[] preFix;
	delete[] posFix;
	delete[] fileOutput;
}

BOOL TrainGaussDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnEraseBkgnd(pDC);
	SBitdraw(pDC,IDB_BMP_BACKGROUND1);
	return true;
	
}
bool TrainGaussDlg::SBitdraw(CDC *pDC, UINT nIDResource)
{
	CBitmap* m_bitmap;
	m_bitmap=new CBitmap();
	m_bitmap->LoadBitmap(nIDResource);
	if(!m_bitmap->m_hObject)
		return true;
	CRect rect;
	GetClientRect(&rect);
	CDC dc;
	dc.CreateCompatibleDC(pDC);	
	dc.SelectObject(m_bitmap);

	int xo=0, yo=0;
	pDC->BitBlt(xo, yo, rect.Width(),rect.Height(), &dc, 0, 0, SRCCOPY);
	return true;
}
