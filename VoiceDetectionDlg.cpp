
// VoiceDetectionDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "VoiceDetection.h"
#include "VoiceDetectionDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CVoiceDetectionDlg dialog



CVoiceDetectionDlg::CVoiceDetectionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VOICEDETECTION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVoiceDetectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVoiceDetectionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_GETPATH, &CVoiceDetectionDlg::OnBnClickedGetpath)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CVoiceDetectionDlg::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CVoiceDetectionDlg::OnCbnSelchangeCombo2)
	ON_CBN_SELCHANGE(IDC_COMBO3, &CVoiceDetectionDlg::OnCbnSelchangeCombo3)
	ON_BN_CLICKED(IDC_SHOWBORDERS, &CVoiceDetectionDlg::OnBnClickedShowborders)
	ON_BN_CLICKED(IDC_SHOW_NEXT_2, &CVoiceDetectionDlg::OnBnClickedShowNext2)
	ON_WM_RBUTTONUP()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ORDER, &CVoiceDetectionDlg::OnNMCustomdrawSliderOrder)
	ON_BN_CLICKED(IDC_APPLY_CHANGES, &CVoiceDetectionDlg::OnBnClickedApplyChanges)
	ON_BN_CLICKED(IDC_BUTTON1, &CVoiceDetectionDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CVoiceDetectionDlg message handlers

bool CVoiceDetectionDlg::SetDoubleInEdit(size_t IDC_EDIT, double DoubleVal)
{
	CString CstrDouble;
	CstrDouble.Format("%.*f", 4, DoubleVal);
	GetDlgItem(IDC_EDIT)->SetWindowTextA(CstrDouble);
	return false;
}

void CVoiceDetectionDlg::UpdateMinMax()
{
	double min = (m_VDetect[0].m_rec.m_MinAmpl[0]);
	double max = (m_VDetect[0].m_rec.m_MaxAmpl[0]);
	SetDoubleInEdit(IDC_MIN1, min);
	SetDoubleInEdit(IDC_MAX1, max);

	min = (m_VDetect[2].m_rec.m_MinAmpl[0]);
	max = (m_VDetect[2].m_rec.m_MaxAmpl[0]);
	SetDoubleInEdit(IDC_MIN2, min);
	SetDoubleInEdit(IDC_MAX2, max);

}

BOOL CVoiceDetectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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
	GetDlgItem(IDC_DIRPATHEDIT)->SetWindowTextA("C:\\TestBackup");
	// TODO: Add extra initialization here
	for (size_t CurCombo = 0; CurCombo < 3; CurCombo++) {
		CComboBox *MyCombo = ((CComboBox*)(GetDlgItem(IDC_COMBO1+ CurCombo)));
		MyCombo->AddString("averaged amplitudes");
		MyCombo->AddString("averaged samples");
		MyCombo->AddString("amlitudes of averages");
		MyCombo->AddString("Frequency");
		MyCombo->AddString("FFT of whole file");
		MyCombo->AddString("Simple Band Filter");
		MyCombo->AddString("Next Band Filter");
		MyCombo->AddString("Disperssion Band filter");
		MyCombo->SetCurSel(CurCombo);
	}
	((CComboBox*)(GetDlgItem(IDC_COMBO1)))->SetCurSel(1);
	((CComboBox*)(GetDlgItem(IDC_COMBO1 + 1)))->SetCurSel(0);
	((CComboBox*)(GetDlgItem(IDC_COMBO1+2)))->SetCurSel(7);
	//((CComboBox*)(GetDlgItem(IDC_COMBO1+1)))->SetCurSel(2);
	char* buff = new char[10];
	int CountOfWavFiles = 10;
	_itoa_s(CountOfWavFiles, buff,10,10);
	GetDlgItem(IDC_COUNTTOSHOW)->SetWindowTextA(buff);

	unsigned int LowBorder = this->m_VDetect[0].m_rec.m_FilterParams.BandPass.nLowBorder;
	_itoa_s(LowBorder, buff, 10, 10);
	GetDlgItem(IDC_LOWBORDER)->SetWindowTextA(buff);

	unsigned int HighBorder = this->m_VDetect[0].m_rec.m_FilterParams.BandPass.nHighBorder;
	_itoa_s(HighBorder, buff, 10, 10);
	GetDlgItem(IDC_HIGHBORDER)->SetWindowTextA(buff);

	((CButton*)(GetDlgItem(IDC_SHOWBORDERS)))->SetCheck(true);
	
	CString BackupPath="C:\\TestBackup";
	BackupPath = "C:\\Backup";
	GetDlgItem(IDC_DIRPATHEDIT)->SetWindowTextA(BackupPath);

	auto m_sliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ORDER);
	m_sliderCtrl->SetRange(1, 5, TRUE);
	size_t Result = this->m_VDetect[0].m_rec.m_FilterParams.BandPass.nOrder;
	m_sliderCtrl->SetPos(Result/4);


	//Result = (Result / 20 + 1) *4;
	_itoa_s(Result, buff, 10, 10);
	GetDlgItem(IDC_ORDERVIS)->SetWindowTextA(buff);
	delete[] buff;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVoiceDetectionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVoiceDetectionDlg::OnPaint()
{
	m_dc = this->GetDC();
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
		CDialogEx::OnPaint();
	}
	DrawCharts();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVoiceDetectionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BandPassParams CVoiceDetectionDlg::InitBpParams()
{
	size_t HighBorder = 0;
	size_t LowBorder = 0;
	size_t OrderOfFilter = 0;
	char* buff = new char[10];
	GetDlgItem(IDC_LOWBORDER)->GetWindowTextA(buff, 10);
	LowBorder = atoi(buff);

	GetDlgItem(IDC_HIGHBORDER)->GetWindowTextA(buff, 10);
	HighBorder = atoi(buff);

	GetDlgItem(IDC_ORDERVIS)->GetWindowTextA(buff, 10);
	OrderOfFilter = atoi(buff);

	delete[]buff;
	return BandPassParams(LowBorder,HighBorder,OrderOfFilter);
}

bool CVoiceDetectionDlg::DrawSimpleChart(POINT UpLeft, POINT RightBottom, int NumberOfRec, int NumbOfChannel)
{
	//m_VDetect.m_Channel[0].samples;
	std::vector<POINT> ResPoints;
	HPEN myPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	m_dc->SelectObject(myPen);
	int SizeOfVec = m_VDetect[NumberOfRec].m_rec.m_CurSampleCount;
	if (!SizeOfVec)
		return false;
	int NumberOfFile = 1;
	auto StartValue = 0; //m_VDetect.m_rec.m_wavFiles[NumberOfFile].StartPoint;
	//SizeOfVec = m_VDetect.m_rec.m_wavFiles[NumberOfFile].m_Channel[0].size;
	RightBottom.y -= 1;

	int SamplesPerPixel = double(SizeOfVec) / double(RightBottom.x - UpLeft.x);
	double MultiplyConst = 1;
	if (!SamplesPerPixel) {
		MultiplyConst = (RightBottom.x - UpLeft.x) / SizeOfVec;
		RightBottom.x = UpLeft.x + SizeOfVec;
		SamplesPerPixel = 1;
	}
	double MaxAmplit = -1;//m_AviMaker.m_maxAmplitude;
	double MinAmplit = MAXINT;
	bool IsFirst = true;

	

	for (int x = UpLeft.x; x < (RightBottom.x); x ++) { //Бежим по каждому пикселю поочерёдно
		double Medvalue = 0;
		int CurX = x - UpLeft.x;
		double MaxValue = -1;
		double MinValue = 120;
		for (int NumSample = SamplesPerPixel * CurX; (NumSample < int(SamplesPerPixel * (CurX + 1))) && (NumSample < SizeOfVec); NumSample++) {
			double CurSampleAmplitude=(m_VDetect[NumberOfRec].m_rec.GetAmplit((StartValue+ NumSample), NumbOfChannel));
			if ((CurSampleAmplitude > 100) || (CurSampleAmplitude < 0)) {
				CurSampleAmplitude--;
			}
		//	CurSampleAmplitude = (CurSampleAmplitude - 55);
			Medvalue += CurSampleAmplitude;
			if (CurSampleAmplitude > MaxValue) {
				MaxValue = CurSampleAmplitude;
			}
			if ((CurSampleAmplitude) < MinValue) {
				MinValue = CurSampleAmplitude;
			}

		}
		Medvalue = (Medvalue) / (SamplesPerPixel); //Нашли среднее значение для пикселя x
		/*Medvalue = abs(127.5 - Medvalue);*/
		Medvalue = abs(Medvalue);
		if (Medvalue < 0) {
			MessageBox("Value less than zero", "Do something");

		}


		//Medvalue = MaxValue;
		//Medvalue = MinValue;
		if (Medvalue > MaxAmplit)
			MaxAmplit = Medvalue;
		if (Medvalue < MinAmplit)
			MinAmplit = Medvalue;
		POINT forPB; //for pushback
		forPB.x = x;
		forPB.y = Medvalue;
		ResPoints.push_back(forPB);
	}


	double AmplitPerPixel = (MaxAmplit - MinAmplit) / (double(RightBottom.y - UpLeft.y));
	if (!AmplitPerPixel) {
		//MessageBox("No Amplitude", "Do something");
		return false;
	}

	for (auto it = ResPoints.begin(); it != ResPoints.end(); it++) {
		int YDelta = ((*it).y - MinAmplit)/ AmplitPerPixel; //Нашли амплитуду в пределах одного пикселя
		//if (YDelta < 0) {
		//	YDelta=0;
		//}
		 
		(*it).x = MultiplyConst * ((*it).x - 30) + 30;
		//int y = UpLeft.y + YDelta;
		int y = RightBottom.y - YDelta;
 {
			(*it).y = y;
			POINT resPoint = { (*it).x , (*it).y };
			if (false) {
				m_dc->MoveTo({ LONG((*it).x)  ,RightBottom.y });
				m_dc->LineTo(resPoint);
			}
			else {
				if (!IsFirst)
					m_dc->LineTo(resPoint);
				else {
					m_dc->MoveTo(resPoint);
					IsFirst = false;
				}
			}

		}
	}
	if (m_bShowBordersOfFiles) {
		
		HPEN FileBorders[2] = { CreatePen(PS_SOLID, 2, RGB(0, 0, 255)),CreatePen(PS_SOLID, 2, RGB(255, 0, 0)) };
		for (auto ItWav = m_VDetect[NumberOfRec].m_rec.m_wavFiles.begin(); ItWav != m_VDetect[NumberOfRec].m_rec.m_wavFiles.end(); ItWav++) {
			bool IsVoice = (*ItWav).m_bExistVoice;
			m_dc->SelectObject(FileBorders[IsVoice]);
			int begin = ((*ItWav).StartPoint / SamplesPerPixel) * MultiplyConst;
			int end = ((*ItWav).FinishPoint / SamplesPerPixel) * MultiplyConst;
			m_dc->MoveTo({ (begin + UpLeft.x)  ,RightBottom.y });
			m_dc->LineTo({ begin + UpLeft.x }, UpLeft.y);

		}
	}
}

bool CVoiceDetectionDlg::DrawCharts()
{
	
	CBrush mybrush;
	mybrush.CreateSolidBrush(RGB(37, 150, 190));                           // Must initialize!
	POINT UpLeft = { m_UpLeft[0].x - 20,m_UpLeft[0].y - 20 };
	POINT DownRight = { m_RightBot[2].x + 20,m_RightBot[2].y + 20 };
	CRect MyRect = { UpLeft, DownRight };
	m_dc->FillRect(MyRect, &mybrush);

	OnBnClickedShowborders();

	OnCbnSelchangeCombo1();
	OnCbnSelchangeCombo2();
	OnCbnSelchangeCombo3();

	return false;
}

void CVoiceDetectionDlg::DrawRectangle(POINT UpLeft, POINT DownRight)
{
	CBrush mybrush;
	mybrush.CreateSolidBrush(RGB(255, 255, 255));                           // Must initialize!
	CRect MyRect = { UpLeft, DownRight };
	m_dc->FillRect(MyRect, &mybrush);
}

void CVoiceDetectionDlg::DrawNext(int NumberOfRec, int NumbOfChannel)
{
	m_VDetect[NumberOfRec].m_rec.reset();

	char* buff = new char[10];
	GetDlgItem(IDC_COUNTTOSHOW)->GetWindowTextA(buff, 10);
	int CountOfWavFiles = atoi(buff);
	delete[]buff;

	m_VDetect[NumberOfRec].ReadNextFiles(CountOfWavFiles);
	m_VDetect[NumberOfRec].DeleteNoise();
	DrawRectangle(m_UpLeft[NumberOfRec], m_RightBot[NumberOfRec]);
	DrawSimpleChart(m_UpLeft[NumberOfRec], m_RightBot[NumberOfRec], NumberOfRec, NumbOfChannel);

}

std::string CVoiceDetectionDlg::GetPath(POINT pressedPoint)
{
	std::string ResString;
	for (int CurChart = 0; CurChart < 3; CurChart++) {

		if ((pressedPoint.x > m_UpLeft[CurChart].x) && (pressedPoint.y > m_UpLeft[CurChart].y)) {
			if ((pressedPoint.x < m_RightBot[CurChart].x) && (pressedPoint.y < m_RightBot[CurChart].y)) {
				//Если попадает в наш график
				int SizeOfVec = m_VDetect[CurChart].m_rec.m_CurSampleCount;
				if (!SizeOfVec)
					break;
				int SamplesPerPixel = double(SizeOfVec) / double(m_RightBot[CurChart].x - m_UpLeft[CurChart].x);
				double MultiplyConst = 1;
				if (!SamplesPerPixel) {
					MultiplyConst = (m_RightBot[CurChart].x - m_UpLeft[CurChart].x) / SizeOfVec;
					SamplesPerPixel = 1;
				}
				for (auto ItWav = m_VDetect[CurChart].m_rec.m_wavFiles.begin(); ItWav != m_VDetect[CurChart].m_rec.m_wavFiles.end(); ItWav++) {
					int begin = ((*ItWav).StartPoint / SamplesPerPixel) * MultiplyConst+ m_UpLeft[CurChart].x;
					int end = ((*ItWav).FinishPoint / SamplesPerPixel) * MultiplyConst+ m_UpLeft[CurChart].x;
					if ((pressedPoint.x > begin) && (pressedPoint.x < end)) {
						ResString = (*ItWav).FilePath;
						return ResString;
					}
				}
			}
		}
	}
	return ResString;
}

void CVoiceDetectionDlg::OpenWav(std::string sbuFile)
{
	if (sbuFile.empty())
		return;
	int SizeOfFile = sbuFile.size();
	size_t SizeOfName = 41;
	std::string NameOfFile = sbuFile.substr(SizeOfFile - SizeOfName);
	SizeOfFile = NameOfFile.size();
	NameOfFile[SizeOfFile-1] = 'v';
	NameOfFile[SizeOfFile-2] = 'a';
	NameOfFile[SizeOfFile-3] = 'w';
	std::string DirPath = "C:\\TestBackup\\buff";
	std::string FullPath = DirPath + "\\" + NameOfFile;
	ShellExecute(NULL, "open", FullPath.c_str(), NULL, NULL, SW_SHOWDEFAULT);

}

void CVoiceDetectionDlg::DrawFirstly(size_t NumbOfChart, size_t channel)
{
	char* buff = new char[10];
	GetDlgItem(IDC_COUNTTOSHOW)->GetWindowTextA(buff, 10);
	int CountOfWavFiles = atoi(buff);
	delete[]buff;

	CString BackupPath;
	GetDlgItem(IDC_DIRPATHEDIT)->GetWindowTextA(BackupPath);
	m_VDetect[NumbOfChart].Init(BackupPath);

	BandPassParams BpParams = InitBpParams();

	int Cursel = ((CComboBox*)(GetDlgItem(IDC_COMBO1+ NumbOfChart)))->GetCurSel();
	m_VDetect[NumbOfChart].m_rec.reset();
	m_VDetect[NumbOfChart].m_rec.Init(Cursel, BpParams);
	m_VDetect[NumbOfChart].ReadNextFiles(CountOfWavFiles);
	m_VDetect[NumbOfChart].DeleteNoise();

	DrawRectangle(m_UpLeft[NumbOfChart], m_RightBot[NumbOfChart]);
	DrawSimpleChart(m_UpLeft[NumbOfChart], m_RightBot[NumbOfChart], NumbOfChart, channel);
}





void CVoiceDetectionDlg::OnBnClickedGetpath()
{
	std::string folderpath;
	CString strCaption = "Choose BackUp folder";

	// The BROWSEINFO struct tells the shell 
	// how it should display the dialog.
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(bi));

	bi.ulFlags = BIF_USENEWUI;
	bi.hwndOwner = GetSafeHwnd();
	bi.lpszTitle = strCaption;

	// must call this if using BIF_USENEWUI
	::OleInitialize(NULL);

	// Show the dialog and get the itemIDList for the 
	// selected folder.
	LPITEMIDLIST pIDL = ::SHBrowseForFolder(&bi);

	if (pIDL != NULL)
	{
		// Create a buffer to store the path, then 
		// get the path.
		char buffer[_MAX_PATH] = { '\0' };
		if (::SHGetPathFromIDList(pIDL, buffer) != 0)
		{
			// Set the string value.
			folderpath = buffer;
		}

		// free the item id list
		CoTaskMemFree(pIDL);
	}
	::OleUninitialize();
	GetDlgItem(IDC_DIRPATHEDIT)->SetWindowTextA(folderpath.c_str());
	// TODO: Add your control notification handler code here
}


void CVoiceDetectionDlg::OnCbnSelchangeCombo1()
{
	DrawFirstly(0, 0);
	UpdateMinMax();
	// TODO: Add your control notification handler code here
}


void CVoiceDetectionDlg::OnCbnSelchangeCombo2()
{
	DrawFirstly(1, 0);
	UpdateMinMax();
}


void CVoiceDetectionDlg::OnCbnSelchangeCombo3()
{
	DrawFirstly(2, 0);
	UpdateMinMax();
}


void CVoiceDetectionDlg::OnBnClickedShowborders()
{
	m_bShowBordersOfFiles = ((CButton*)(GetDlgItem(IDC_SHOWBORDERS)))->GetCheck();
	// TODO: Add your control notification handler code here
}


void CVoiceDetectionDlg::OnBnClickedShowNext2()
{

	DrawNext(0,0);
	DrawNext(1,0);
	DrawNext(2,0);
	UpdateMinMax();
	// TODO: Add your control notification handler code here
}

void CVoiceDetectionDlg::OnRButtonUp(UINT nFlags, CPoint point)

{
	// TODO: Add your message handler code here and/or call default
	std::string ResString = GetPath(point);
	OpenWav(ResString);
	

}


void CVoiceDetectionDlg::OnNMCustomdrawSliderOrder(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	auto m_sliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ORDER);
	size_t Result = m_sliderCtrl->GetPos()*4;
	//Result = (Result / 20 + 1) *4;
	char* buff = new char[10];
	_itoa_s(Result, buff, 10, 10);
	GetDlgItem(IDC_ORDERVIS)->SetWindowTextA(buff);
	delete[] buff;
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CVoiceDetectionDlg::OnBnClickedApplyChanges()
{
	DrawCharts();
	// TODO: Add your control notification handler code here
}


void CVoiceDetectionDlg::OnBnClickedButton1()
{
	bool res = m_VDetect[2].StartBackupCorrection();
	if (!res)
		res = true;
	// TODO: Add your control notification handler code here
}
