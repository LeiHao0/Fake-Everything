#pragma once
#include "afxwin.h"


// CIgnoreDlg 对话框

class CIgnoreDlg : public CDialog
{
	DECLARE_DYNAMIC(CIgnoreDlg)

public:
	CIgnoreDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIgnoreDlg();

// 对话框数据
	enum { IDD = IDD_IGNORE };

protected:
	virtual BOOL OnInitDialog();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAdd();

public:
	CListBox m_IgnoreList;
public:
	afx_msg void OnBnClickedDel();
public:
	afx_msg void OnBnClickedButton2();
public:
	bool WriteConfig(void);
public:
	afx_msg void OnBnClickedOk();
public:
	bool ReadConfig(void);
};
