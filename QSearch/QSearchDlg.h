// QSearchDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CQSearchDlg 对话框
class CQSearchDlg : public CDialog
{
// 构造
public:
	CQSearchDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_QSEARCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_str;
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnDestroy();
	afx_msg void OnClose();

public:
	CListBox m_FileName;


public:
	afx_msg void OnLbnDblclkList1();
public:
	afx_msg void OnUplow();

	// 菜单设置
public:
	static UINT initThread(LPVOID pParam);
	UINT realThread(LPVOID pParam);
	int horizontalLen;	// 滚动条
//	CProgressCtrl m_pro;	// 进度条
	void showProsess(int i);
	CMenu menu;
	bool m_bIsUpLow;
	bool m_bUnOrder;
	int num;
public:
	afx_msg void OnUnorder();
public:
	afx_msg void OnAbout();
public:
	CBitmapButton m_btnBitmap; // 按钮绘图
public:
	CWinThread* pThread;
public:
	afx_msg void OnBnClickedFind();
public:
	afx_msg void OnTip();
public:
	CProgressCtrl m_pro;
public:
	afx_msg void OnIgnore();
public:
	NOTIFYICONDATA m_ntIcon;
public:
	bool OnInitalIcon(void);


public:
	afx_msg LRESULT OnNotifyIcon(WPARAM, LPARAM); // 最小化图标
public:
	bool m_bMin;
public:
	afx_msg void OnPopupShow();
public:
	afx_msg void OnPopupAbout();
public:
	afx_msg void OnPopupQuit();
};