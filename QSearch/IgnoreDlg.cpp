// IgnoreDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "QSearch.h"
#include "IgnoreDlg.h"

#include <fstream>
#include <string>


// CIgnoreDlg 对话框

IMPLEMENT_DYNAMIC(CIgnoreDlg, CDialog)

CIgnoreDlg::CIgnoreDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIgnoreDlg::IDD, pParent)
{
	
}

CIgnoreDlg::~CIgnoreDlg()
{

}

BOOL CIgnoreDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{

	}

	ReadConfig();
	// 水平滚动条
	::SendMessage(m_IgnoreList, LB_SETHORIZONTALEXTENT, 2000, 0);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CIgnoreDlg::DoDataExchange(CDataExchange* pDX)
{
//	ReadConfig();
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IGN_LIST, m_IgnoreList);
}


BEGIN_MESSAGE_MAP(CIgnoreDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD, &CIgnoreDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_DEL, &CIgnoreDlg::OnBnClickedDel)
	ON_BN_CLICKED(IDC_CLEAN, &CIgnoreDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_OK, &CIgnoreDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CIgnoreDlg 消息处理程序

void CIgnoreDlg::OnBnClickedAdd()
{
	// TODO: 在此添加控件通知处理程序代码


	BROWSEINFO   bInfo; 
	ZeroMemory(&bInfo,   sizeof(bInfo)); 
//	bInfo.hwndOwner   =   m_hWnd; 
	TCHAR   tchPath[255]; 
	tchPath[0] = '\0';
	bInfo.lpszTitle   =   _T( "所选择路径:   "); 
	bInfo.ulFlags   =   BIF_RETURNONLYFSDIRS;         

	LPITEMIDLIST   lpDlist; 
	//用来保存返回信息的IDList，使用SHGetPathFromIDList函数转换为字符串 
	lpDlist   =   SHBrowseForFolder(&bInfo)   ;   //显示选择对话框 
	if( lpDlist != NULL ) { 
		SHGetPathFromIDList(lpDlist,   tchPath);//把项目标识列表转化成目录 
//		TRACE(tchPath); 
	}
	
	if ( tchPath[0] != '\0' ) {
		m_IgnoreList.AddString(tchPath);
	}
}
void CIgnoreDlg::OnBnClickedDel()
{
	// TODO: 在此添加控件通知处理程序代码
	
	m_IgnoreList.DeleteString(m_IgnoreList.GetCurSel());
}

void CIgnoreDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_IgnoreList.ResetContent();
}


bool CIgnoreDlg::WriteConfig(void)
{
	// 写入配置文件 config.ini
	using namespace std;
	ofstream fout("config.ini");

	CString Ctmp;
	int listcount = m_IgnoreList.GetCount();
	for ( int i=0; i<listcount; ++i ) {
		m_IgnoreList.GetText(i, Ctmp);
		string path(CW2A( (LPCTSTR)Ctmp) );
		fout << path << "\n";
	}	

	fout.close();

	return true;
}



void CIgnoreDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	WriteConfig();
	OnOK();
}

bool CIgnoreDlg::ReadConfig(void)
{
	using namespace std;
	ifstream fin("config.ini", ios::in);
	
	string tmp;
	while ( getline(fin, tmp) ) {
		m_IgnoreList.AddString(CA2W(tmp.c_str()) );
	}
	
	fin.close();

	return true;
}
