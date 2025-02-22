// StudentDetectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StudentDetection.h"
#include "StudentDetectionDlg.h"
#include "TrainDataDlg.h"
#include "AboutDlg.h"
#include "InputDlg.h"
#include "ImageProcessor.h"
#include "afxwin.h"
#include "BtnST.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	CButtonST m_btnOK;
	afx_msg void OnBnClickedOk();
	BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_btnOK.SetBitmaps(IDB_BMP_OK, RGB(255,0,0));
	m_btnOK.SetFlat();
	return TRUE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnOK);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CStudentDetectionDlg dialog

SnakeDetector * CStudentDetectionDlg::detector = NULL;
WindowParams * CStudentDetectionDlg::m_windowParam = NULL;
GaussFilterColor * CStudentDetectionDlg::m_gauss = NULL;
HoGProcessor * CStudentDetectionDlg::hog = NULL;
CvSVM * CStudentDetectionDlg::svm = NULL;

CStudentDetectionDlg::CStudentDetectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStudentDetectionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	video_thread = NULL;

	CStudentDetectionDlg::m_windowParam = new WindowParams();
	CStudentDetectionDlg::m_windowParam->LoadParamsFromXML("config.xml");

	CStudentDetectionDlg::detector = new SnakeDetector( "shape.xml" );
	CStudentDetectionDlg::m_gauss = new GaussFilterColor();
	CStudentDetectionDlg::hog = new HoGProcessor();
	CStudentDetectionDlg::svm = new CvSVM();
}

CStudentDetectionDlg::~CStudentDetectionDlg() {
	if (CStudentDetectionDlg::detector != NULL)
		delete CStudentDetectionDlg::detector;

	if(!m_windowParam->m_isStopVideo)	
		m_windowParam->m_isStopVideo = true;

	if (CStudentDetectionDlg::m_windowParam != NULL)
		delete CStudentDetectionDlg::m_windowParam;

	if (CStudentDetectionDlg::m_gauss != NULL)
		delete CStudentDetectionDlg::m_gauss;

	if (CStudentDetectionDlg::hog != NULL)
		delete CStudentDetectionDlg::hog;

	if (CStudentDetectionDlg::svm != NULL)
		delete CStudentDetectionDlg::svm;
}

void CStudentDetectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_TAB_PARAM, m_tabParams);
	DDX_Control(pDX, IDC_BTN_PLAY, m_btnPlay);
	DDX_Control(pDX, IDC_BTN_STOP, m_btnStop);
	DDX_Control(pDX, IDC_CHECK_VIEW_HAIR, m_checkViewHair);
	DDX_Control(pDX, IDC_CHECK_VIEW_SVM, m_checkViewSVM);
	DDX_Control(pDX, IDC_CHECK_VIEW_SHAPE, m_checkViewShape);	
	DDX_Control(pDX, IDC_BTN_APPLY_PARAMS, m_btnApplyParams);	
	DDX_Control(pDX, IDC_PLAY_VIDEO, m_videoPlayer);
	DDX_Control(pDX, IDC_EDIT_STUDENT_COUNT, m_editStudentCount);
	DDX_Control(pDX, IDC_BTN_PAUSE, m_btnPause);
}

BEGIN_MESSAGE_MAP(CStudentDetectionDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP	
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PARAM, &CStudentDetectionDlg::OnTcnSelchangeTabParam)
	ON_COMMAND(ID_MAIN_VIDEO, &CStudentDetectionDlg::OnMainVideo)
	ON_COMMAND(ID_MAIN_EXIT, &CStudentDetectionDlg::OnMainExit)
	ON_COMMAND(ID_MAIN_TRAINDATA, &CStudentDetectionDlg::OnMainTraindata)
	ON_COMMAND(ID_MAIN_ABOUT, &CStudentDetectionDlg::OnMainAbout)
	ON_BN_CLICKED(IDC_BTN_PLAY, &CStudentDetectionDlg::OnBnClickedBtnPlay)
	ON_BN_CLICKED(IDC_BTN_STOP, &CStudentDetectionDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_CHECK_VIEW_HAIR, &CStudentDetectionDlg::OnBnClickedCheckViewHair)
	ON_BN_CLICKED(IDC_CHECK_VIEW_SVM, &CStudentDetectionDlg::OnBnClickedCheckViewSvm)
	ON_BN_CLICKED(IDC_CHECK_VIEW_SHAPE, &CStudentDetectionDlg::OnBnClickedCheckViewShape)
	ON_MESSAGE(WM_USER_THREAD_FINISHED, OnThreadFinished)
	ON_MESSAGE(WM_USER_THREAD_UPDATE_PROGRESS, OnThreadUpdateProgress)
	ON_MESSAGE(WM_USER_THREAD_UPDATE_INFO, OnThreadUpdateInfo)	
	ON_BN_CLICKED(IDC_BTN_APPLY_PARAMS, &CStudentDetectionDlg::OnBnClickedBtnApplyParams)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_STN_CLICKED(IDC_PLAY_VIDEO, &CStudentDetectionDlg::OnStnClickedPlayVideo)
	ON_BN_CLICKED(IDC_BTN_PAUSE, &CStudentDetectionDlg::OnBnClickedBtnPause)
END_MESSAGE_MAP()


// CStudentDetectionDlg message handlers


BOOL CStudentDetectionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CStudentDetectionDlg::m_windowParam->m_hWnd = this->m_hWnd;

	//create tab
	TC_ITEM tci;
	tci.mask = TCIF_TEXT;
	tci.iImage = -1;
	tci.pszText = _T("Head Params");
	m_tabParams.InsertItem(0, &tci);

	tci.pszText = _T("Shape Params");
	m_tabParams.InsertItem(1, &tci);
	
	// MG: Create the page dialogs, and set the initial Dialog
	RECT Rect;
	m_tabParams.GetItemRect( 0, &Rect );

	m_tabHeadParams = new HeadParamDlg(CStudentDetectionDlg::m_windowParam->m_DetectionParams.m_Head_Params);

	m_tabHeadParams->Create( IDD_DLG_HEAD_PARAMS, &m_tabParams );
	m_tabHeadParams->SetWindowPos( 0, Rect.left + 2, Rect.bottom + 2, 0, 0, SWP_NOSIZE|SWP_NOZORDER );	
	m_tabHeadParams->ShowWindow( SW_SHOWNA ); // MG: Sets the initial dialog

	m_tabParams.GetItemRect( 0, &Rect );
	m_tabShapeParams.Create( IDD_DLG_SHAPE_PARAMS, &m_tabParams );
	m_tabShapeParams.SetWindowPos( 0, Rect.left + 2, Rect.bottom + 2, 0, 0, SWP_NOSIZE|SWP_NOZORDER );
		
	m_tabShapeParams.ShowWindow( SW_HIDE ); // MG: Sets the initial dialog
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//init checkboxes
	m_checkViewHair.SetCheck(1);
	CStudentDetectionDlg::m_windowParam->m_isViewHairDetection = 1;
	m_checkViewShape.SetCheck(1);
	CStudentDetectionDlg::m_windowParam->m_isViewShapeDetection = 1;
	m_checkViewSVM.SetCheck(1);
	CStudentDetectionDlg::m_windowParam->m_isViewSVMDetection = 1;

	//init image for buttons
	m_btnPlay.SetBitmaps(IDB_BMP_PLAY, RGB(255, 255, 255));
	m_btnPlay.SetFlat();
	m_btnStop.SetBitmaps(IDB_BMP_STOP, RGB(255, 255, 255));
	m_btnStop.SetFlat();
	m_btnPause.SetBitmaps(IDB_BMP_PAUSE, RGB(255, 255, 255));
	m_btnPause.SetFlat();

	m_btnApplyParams.SetBitmaps(IDB_BMP_OK, RGB(255, 0, 0));
	m_btnApplyParams.SetFlat();

	m_windowParam->m_isStopVideo = true;

	m_editStudentCount.SetWindowTextW(_T("0"));

	m_videoPlayer.GetClientRect(m_rectPlayVideo);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CStudentDetectionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CStudentDetectionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CStudentDetectionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CStudentDetectionDlg::OnTcnSelchangeTabParam(NMHDR *pNMHDR, LRESULT *pResult)
{	
	if( m_tabParams.GetCurSel()==0)
	{
		m_tabHeadParams->ShowWindow( SW_SHOWNA );
		m_tabShapeParams.ShowWindow( SW_HIDE );
	}
	else if( m_tabParams.GetCurSel()==1)
	{
		m_tabHeadParams->ShowWindow( SW_HIDE );
		m_tabShapeParams.ShowWindow( SW_SHOWNA );
	}
	
	*pResult = 0;
}

void CStudentDetectionDlg::OnMainVideo()
{
	// TODO: Add your command handler code here
	InputDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		Utils utils;
		if(CStudentDetectionDlg::m_windowParam->m_videoPath != NULL)
			delete[] CStudentDetectionDlg::m_windowParam->m_videoPath;
		CStudentDetectionDlg::m_windowParam->m_videoPath = utils.ConvertToChar(dlg.m_videoPath);

		if(CStudentDetectionDlg::m_windowParam->m_maskPath != NULL)
			delete[] CStudentDetectionDlg::m_windowParam->m_maskPath;
		CStudentDetectionDlg::m_windowParam->m_maskPath = utils.ConvertToChar(dlg.m_maskPath);
	}
}

void CStudentDetectionDlg::OnMainExit()
{
	OnOK();
}

void CStudentDetectionDlg::OnMainTraindata()
{
	TrainDataDlg dlg;
	dlg.DoModal();
}

void CStudentDetectionDlg::OnMainAbout()
{
	AboutDlg dlg;
	dlg.DoModal();
}

UINT playVideoThread(LPVOID lParam)
{	
	WindowParams* param = CStudentDetectionDlg::m_windowParam;
	Utils utils;
	
	CvCapture *capture = cvCaptureFromFile(param->m_videoPath);	
	if (capture == NULL) {
		return EXIT_FAILURE;
	}
	

	int student_count = 0;

	IplImage *frame = cvQueryFrame(capture);
	IplImage *mask = cvLoadImage(param->m_maskPath, CV_LOAD_IMAGE_GRAYSCALE);	
	IplImage *result = cvCloneImage(frame);
	IplImage *hair_canny = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
	IplImage *subtract;

	int fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);

	CvMemStorage *storage = cvCreateMemStorage();		
	CvSeq *contours = 0;

	GaussFilterColor *m_gauss = CStudentDetectionDlg::m_gauss;
	m_gauss->LoadData(param->m_modelGaussPath);
	m_gauss->SetThreshold(param->m_DetectionParams.m_Gaussian_Params.m_fThreshold);

	HoGProcessor *hog = CStudentDetectionDlg::hog;
	hog->SetParams(param->m_DetectionParams.m_HOG_Params.m_cell, param->m_DetectionParams.m_HOG_Params.m_block, param->m_DetectionParams.m_HOG_Params.m_fStepOverlap);

	CvSVM *svm = CStudentDetectionDlg::svm;
	svm->load(param->m_modelSVMPath);

	CvRect window = cvRect(0,0,48,48);
				
	vector<CvRect> vectorRect;
	while (1) {
		if(!param->m_isPauseVideo)
		{
			frame = cvQueryFrame(capture);
			
			if (frame == NULL) {
				break;
			}				
			
			if(param->m_isStopVideo)		
				break;
			
			student_count = 0;
			vectorRect.clear();	

			subtract = m_gauss->Classify(
				frame, 
				mask, 
				param->m_DetectionParams.m_Gaussian_Params.m_fThreshold);

			//cvSmooth(subtract, hair_canny, CV_MEDIAN);
			//cvCanny(hair_canny, hair_canny, 10, 100);
			cvCanny(subtract, hair_canny, 10, 100);
			
			cvFindContours(subtract, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL);

			cvCopyImage(frame, result);

			if (contours) {								
				while (contours != NULL) {				
					CvRect rectHead = cvBoundingRect(contours);					
					if(!utils.CheckRectHead(
						rectHead, 
						frame->height, 
						param->m_DetectionParams.m_Head_Params))
					{										
						if(param->m_isViewHairDetection)
							cvRectangle(result, cvPoint(rectHead.x, rectHead.y), cvPoint(rectHead.x + rectHead.width, rectHead.y + rectHead.height), CV_RGB(255,255,0));
						contours = contours->h_next;
						continue;
					}					
						
						
					//CvRect detectedRect = hog->detectObject(
					//	svm,
					//	frame, 
					//	result, 
					//	rectHead, 
					//	param->m_DetectionParams.m_SVM_Params.m_fConfidenceScore);
					//if(detectedRect.width > 0){
					//	vectorRect.push_back(detectedRect);
					//}

					int scaleWidth = 2;
					int scaleHeight = 2;

					//loai bo truong hop toc dai
					if(rectHead.height > rectHead.width)
						rectHead.height = rectHead.width;		

					CvRect rectHuman = cvRect(rectHead.x + rectHead.width/2 - rectHead.width*scaleWidth/2, 
						rectHead.y - 6, 
						rectHead.width*scaleWidth, 
						rectHead.height*scaleHeight);


					if(param->m_isViewHairDetection)
						cvRectangle(result, cvPoint(rectHuman.x, rectHuman.y), cvPoint(rectHuman.x + rectHuman.width, rectHuman.y + rectHuman.height), CV_RGB(255,255,255));


					CvPoint location = cvPoint(rectHuman.x, rectHuman.y);
					Snake *fit_snake;
					int current_y = rectHuman.y;
					int frame_height_step = frame->height*1.0/3;
					int dis = 1;
					if (current_y >= 0 && current_y < frame_height_step) {
						// far -> small shape
						dis = 3;
					}
					else if (current_y >= frame_height_step && current_y < frame_height_step*2) {
						// medium shape
						dis = 2;
					}
					else if (current_y >= frame_height_step*2 && current_y < frame->height) {
						// near -> big shape
						dis = 1;
					}
					fit_snake = CStudentDetectionDlg::detector->GetSnake(hair_canny, dis, location, rectHuman);

					// rect co shape dau nguoi trong if
					if (fit_snake != NULL)
					{
						student_count++;
						if(param->m_isViewShapeDetection) {
							// ve shape
							fit_snake->DrawCurve(result, location);			
						}
					}
			
					contours = contours->h_next;					
				}
			}
			
			//vectorRect = utils.ConnectOverlapRects(vectorRect);

			//for (unsigned int i = 0; i < vectorRect.size(); i++) {
			//	// kiem tra rect co shape
			//	CvRect rect = vectorRect.at(i);
			//	CvPoint location = cvPoint(rect.x+rect.width*1.0f/3, rect.y+rect.height*1.0f/3);

			//	Snake *fit_snake;
			//	int current_y = rect.y+rect.height*1.0f/3;
			//	int frame_height_step = frame->height*1.0/3;
			//	int dis = 1;
			//	if (current_y >= 0 && current_y < frame_height_step) {
			//		// far -> small shape
			//		dis = 3;
			//	}
			//	else if (current_y >= frame_height_step && current_y < frame_height_step*2) {
			//		// medium shape
			//		dis = 2;
			//	}
			//	else if (current_y >= frame_height_step*2 && current_y < frame->height) {
			//		// near -> big shape
			//		dis = 1;
			//	}
			//	fit_snake = CStudentDetectionDlg::detector->GetSnake(hair_canny, dis, location, rect);

			//	// rect co shape dau nguoi trong if
			//	if (fit_snake != NULL)
			//	{
			//		student_count++;
			//		if(param->m_isViewShapeDetection) {
			//			// ve shape
			//			fit_snake->DrawCurve(result, location);			
			//		}
			//	}			
			//}
			
			if(param->m_isViewSVMDetection)
				utils.OutputResult(result, vectorRect, CV_RGB(255,0,0));
						

			PostMessage(param->m_hWnd,WM_USER_THREAD_UPDATE_PROGRESS,(WPARAM)result,0);
			// chinh lai cho nay, doi lai bien dem count (thoa ca 2 SVM + shape)
			PostMessage(param->m_hWnd,WM_USER_THREAD_UPDATE_INFO,(WPARAM)student_count,0);
			//PostMessage(param->m_hWnd,WM_USER_THREAD_UPDATE_INFO,(WPARAM)vectorRect.size(),0);
		}
	}


	svm->clear();
	cvReleaseImage(&result);
	cvReleaseImage(&mask);
	cvReleaseImage(&hair_canny);
	cvReleaseImage(&subtract);
	cvReleaseCapture(&capture);
	cvReleaseMemStorage(&storage);
	PostMessage(param->m_hWnd,WM_USER_THREAD_FINISHED,0,0);
}
void CStudentDetectionDlg::OnBnClickedBtnPlay()
{
	if(m_btnPlay.IsWindowEnabled())
	{
		if(CStudentDetectionDlg::m_windowParam->m_videoPath == NULL)
		{
			MessageBox(_T("Please select video"), _T("Information"));
			return;
		}

		m_btnPlay.EnableWindow(0);
		video_thread = AfxBeginThread(playVideoThread, CStudentDetectionDlg::m_windowParam, THREAD_PRIORITY_NORMAL, 0, 0);
		m_windowParam->m_isStopVideo = false;
	}
}

void CStudentDetectionDlg::OnBnClickedBtnStop()
{	
	if(video_thread != NULL)
	{
		m_windowParam->m_isStopVideo = true;
		m_btnPlay.EnableWindow();
	}
}

void CStudentDetectionDlg::OnBnClickedCheckViewHair()
{
	CStudentDetectionDlg::m_windowParam->m_isViewHairDetection = m_checkViewHair.GetCheck();
}

void CStudentDetectionDlg::OnBnClickedCheckViewSvm()
{
	CStudentDetectionDlg::m_windowParam->m_isViewSVMDetection = m_checkViewSVM.GetCheck();
}

void CStudentDetectionDlg::OnBnClickedCheckViewShape()
{
	CStudentDetectionDlg::m_windowParam->m_isViewShapeDetection = m_checkViewShape.GetCheck();
}

LRESULT CStudentDetectionDlg::OnThreadFinished(WPARAM wParam,LPARAM lParam)
{
	m_btnPlay.EnableWindow();
	m_windowParam->m_isStopVideo = true;

	Invalidate();
	
	return 0;
}

LRESULT CStudentDetectionDlg::OnThreadUpdateProgress(WPARAM wParam,LPARAM lParam)
{
	IplImage* frame = (IplImage*)wParam;
	IplImage* displayImg;
	ImageProcessor imgProcess;	

	CvvImage cvv;
	displayImg = imgProcess.getSubImageAndResize(frame, cvRect(0,0,frame->width, frame->height), m_rectPlayVideo.Width(), m_rectPlayVideo.Height());
	cvv.CopyOf(displayImg);
	cvv.Show(m_videoPlayer.GetDC()->m_hDC, 0, 0, m_rectPlayVideo.Width(), m_rectPlayVideo.Height());
	
	cvv.Destroy();
	cvReleaseImage(&displayImg);
	return 0;
}

LRESULT CStudentDetectionDlg::OnThreadUpdateInfo(WPARAM wParam,LPARAM lParam)
{
	Utils utils;
	int count = (int)wParam;	
	m_editStudentCount.SetWindowTextW(utils.ConvertToCString(count));
	
	return 0;
}

void CStudentDetectionDlg::OnBnClickedBtnApplyParams()
{
	// TODO: Add your control notification handler code here
	//apply head params
	CString tmp;
	Utils utils;
	
	m_tabHeadParams->m_editMaxHeadArea.GetWindowTextW(tmp);
	CStudentDetectionDlg::m_windowParam->m_DetectionParams.m_Head_Params.m_iMaxHeadArea = utils.ConvertToInt(tmp);

	m_tabHeadParams->m_editMaxHeadAreaAtTop.GetWindowTextW(tmp);
	CStudentDetectionDlg::m_windowParam->m_DetectionParams.m_Head_Params.m_iMaxHeadAreaTop = utils.ConvertToInt(tmp);
		
	m_tabHeadParams->m_editMaxWidthHead.GetWindowTextW(tmp);
	CStudentDetectionDlg::m_windowParam->m_DetectionParams.m_Head_Params.m_iMaxWidth  = utils.ConvertToInt(tmp);

	m_tabHeadParams->m_editMinAreaAtBottom.GetWindowTextW(tmp);
	CStudentDetectionDlg::m_windowParam->m_DetectionParams.m_Head_Params.m_iMinHeadAreaBottom = utils.ConvertToInt(tmp);

	m_tabHeadParams->m_editMinHeadArea.GetWindowTextW(tmp);
	CStudentDetectionDlg::m_windowParam->m_DetectionParams.m_Head_Params.m_iMinHeadArea = utils.ConvertToInt(tmp);

	m_tabHeadParams->m_editMinWidthHead.GetWindowTextW(tmp);
	CStudentDetectionDlg::m_windowParam->m_DetectionParams.m_Head_Params.m_iMinWidth  = utils.ConvertToInt(tmp);

	m_tabHeadParams->m_editRelativeHeightWidth.GetWindowTextW(tmp);
	CStudentDetectionDlg::m_windowParam->m_DetectionParams.m_Head_Params.m_iRelative_Height_Width  = utils.ConvertToInt(tmp);

	m_tabHeadParams->m_editRelativeWidthHeight.GetWindowTextW(tmp);
	CStudentDetectionDlg::m_windowParam->m_DetectionParams.m_Head_Params.m_iRelative_Width_Height  = utils.ConvertToInt(tmp);
}

BOOL CStudentDetectionDlg::OnEraseBkgnd(CDC* pDC) 
{
	CBitmap m_bitmap;
	BOOL rVal = FALSE;

	if( m_bitmap.LoadBitmap( IDB_BMP_BACKGROUND ) )
	{
		CRect rect;
		GetClientRect( &rect );

		CDC dc;
		dc.CreateCompatibleDC( pDC );
		CBitmap* pOldBitmap = dc.SelectObject( &m_bitmap );

		pDC->BitBlt( 0, 0, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);

		dc.SelectObject(pOldBitmap);
		rVal = TRUE;
	}

	return rVal;
}

HBRUSH CStudentDetectionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{	
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

    DWORD dwStyle = pWnd->GetStyle();
    if ( dwStyle & (BS_AUTORADIOBUTTON | BS_RADIOBUTTON | BS_AUTOCHECKBOX |BS_CHECKBOX) )
	{
        hbr = (HBRUSH) GetStockObject (WHITE_BRUSH);
    }
	else
	{
		switch(nCtlColor)
		{
		case CTLCOLOR_STATIC:
		case CTLCOLOR_EDIT:
			pDC->SetBkMode(TRANSPARENT);			
			hbr = (HBRUSH)GetStockObject( NULL_BRUSH );			
			break;	
		default:
			hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
			break;
		}
	}

	if(pWnd->GetDlgCtrlID() == IDC_EDIT_STUDENT_COUNT)
	{		
		pDC->SetBkMode(TRANSPARENT);		  
		hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);	
	}
    return hbr;
}
void CAboutDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CStudentDetectionDlg::OnStnClickedPlayVideo()
{
	// TODO: Add your control notification handler code here
	if(m_windowParam->m_isStopVideo)
	{
		InputDlg dlg;
		if(dlg.DoModal() == IDOK)
		{
			Utils utils;			
			CStudentDetectionDlg::m_windowParam->m_videoPath = utils.ConvertToChar(dlg.m_videoPath);		
			CStudentDetectionDlg::m_windowParam->m_maskPath = utils.ConvertToChar(dlg.m_maskPath);
		}
	}
}

void CStudentDetectionDlg::OnBnClickedBtnPause()
{
	// TODO: Add your control notification handler code here
	m_windowParam->m_isPauseVideo = !m_windowParam->m_isPauseVideo;
}
