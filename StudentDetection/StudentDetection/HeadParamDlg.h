#pragma once
#include "afxwin.h"
#include "WindowParams.h"
// HeadParamDlg dialog
class HeadParamDlg : public CDialog
{
	DECLARE_DYNAMIC(HeadParamDlg)

public:
	HeadParamDlg(CWnd* pParent = NULL);   // standard constructor	
	HeadParamDlg(Head_Params headParam, CWnd* pParent = NULL);   // standard constructor	
	virtual ~HeadParamDlg();	
	BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_DLG_HEAD_PARAMS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL OnEraseBkgnd(CDC* pDC);
	BOOL SBitdraw(CDC *pDC, UINT nIDResource);
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	Head_Params m_params;
	CEdit m_editMinWidthHead;
	CEdit m_editMaxWidthHead;
	CEdit m_editMinHeadArea;
	CEdit m_editMaxHeadArea;
	CEdit m_editMaxHeadAreaAtTop;
	CEdit m_editMinAreaAtBottom;
	CEdit m_editRelativeWidthHeight;
	CEdit m_editRelativeHeightWidth;
};
