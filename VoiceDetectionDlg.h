
// VoiceDetectionDlg.h : header file
//

#pragma once
#include "VoiceDetector.h"

// CVoiceDetectionDlg dialog
class CVoiceDetectionDlg : public CDialogEx
{
// Construction
public:
	CVoiceDetectionDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VOICEDETECTION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	bool SetDoubleInEdit(size_t IDC_EDIT, double DoubleVal); //Устонавливаем значение по указанном IDC
	void UpdateMinMax();
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	VoiceDetector m_VDetect[3];
	POINT m_UpLeft[3]{ {30,30}, {30,250}, {30,470 } };
	POINT m_RightBot[3]{ {1020,230}, {1020,450}, {1020,670 } };
	size_t m_OderOfFilter = 12;
	BandPassParams InitBpParams();

	CDC* m_dc;
	bool m_bShowBordersOfFiles;
	bool DrawSimpleChart(POINT UpLeft, POINT RightBottom, int NumberOfRec, int NumbOfChannel);
	bool DrawCharts();
	void DrawRectangle(POINT UpLeft, POINT DownRight);
	void DrawNext(int NumberOfRec, int NumbOfChannel);
	std::string GetPath(POINT pressed);
	void OpenWav(std::string sbuFile);
	void DrawFirstly(size_t NumbOfChart, size_t channel);
public:
	afx_msg void OnBnClickedGetpath();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnCbnSelchangeCombo3();
	afx_msg void OnBnClickedShowborders();
	afx_msg void OnBnClickedShowNext2();
	void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNMCustomdrawSliderOrder(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedApplyChanges();
	afx_msg void OnBnClickedButton1();
};
