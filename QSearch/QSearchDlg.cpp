// QSearchDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "QSearch.h"
#include "QSearchDlg.h"
#include "IgnoreDlg.h"
#include "Volume.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define MY_WM_NOTIFYICON (WM_USER+1001)

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CQSearchDlg 对话框




CQSearchDlg::CQSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQSearchDlg::IDD, pParent)
	, m_bMin(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STR, m_str);
	DDX_Control(pDX, IDC_LIST1, m_FileName);
	DDX_Control(pDX, IDC_PROGRESS1, m_pro);
}

BEGIN_MESSAGE_MAP(CQSearchDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CQSearchDlg::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_LIST1, &CQSearchDlg::OnLbnSelchangeList1)
	ON_LBN_DBLCLK(IDC_LIST1, &CQSearchDlg::OnLbnDblclkList1)
	ON_COMMAND(ID_UPLOW, &CQSearchDlg::OnUplow)
	ON_COMMAND(ID_UNORDER, &CQSearchDlg::OnUnorder)
	ON_COMMAND(ID_ABOUT, &CQSearchDlg::OnAbout)
	ON_BN_CLICKED(ID_FIND, &CQSearchDlg::OnBnClickedFind)
	ON_COMMAND(ID_TIP, &CQSearchDlg::OnTip)

	//	ON_COMMAND(ID_32783, &CQSearchDlg::OnIgnore)
	ON_COMMAND(ID_IGNORE, &CQSearchDlg::OnIgnore)

	// OnNotifyIcon 就是消息处理函数的名称
	ON_MESSAGE(MY_WM_NOTIFYICON, &CQSearchDlg::OnNotifyIcon)   

	ON_COMMAND(ID_32788, &CQSearchDlg::OnPopupShow)
	ON_COMMAND(ID_32786, &CQSearchDlg::OnPopupAbout)
	ON_COMMAND(ID_32787, &CQSearchDlg::OnPopupQuit)

	// 退出
	ON_WM_CLOSE() // 这个地方一定要写

	ON_WM_DESTROY()

END_MESSAGE_MAP()


// CQSearchDlg 消息处理程序

BOOL CQSearchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_str.SetWindowText(_T("请输入要查找的文件名："));

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	// 初始化系统托盘图标
	OnInitalIcon();

	// 按钮绘图
	HICON m_hicn1=AfxGetApp()->LoadIcon(IDI_ICON1);
	CWnd *pWnd = GetDlgItem(ID_FIND);
	CButton *Button= (CButton *) pWnd;
	Button->SetIcon(m_hicn1);

	// 水平滚动条
	::SendMessage(m_FileName, LB_SETHORIZONTALEXTENT, 2000, 0);

	// 菜单
	m_bUnOrder = true;
	m_bIsUpLow = false;
	menu.LoadMenu(IDR_MENU);
	SetMenu(&menu);

	// 线程
//	pThread = AfxBeginThread(initdata.initThread, &initdata);
	initdata.init(NULL);
	num = initdata.getJ();
	m_pro.SetRange(0,num*100);
	pThread = AfxBeginThread(initThread, (LPVOID)this);
	


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

UINT CQSearchDlg::initThread(LPVOID pParam) {
	CQSearchDlg * pObj = (CQSearchDlg*)pParam;
	if ( pObj ) {
		return pObj->realThread(NULL);
	}
	return false;
}

UINT CQSearchDlg::realThread(LPVOID pParam) {

	char *pvol = initdata.getVol();
	for ( int i=0; i<num; ++i ) {
		initdata.initvolumelist((LPVOID)pvol[i]);
		CString showpro(_T("正在统计"));
		showpro += pvol[i];
		showpro += _T("盘文件...");
		GetDlgItem(IDC_SHOWPRO)->SetWindowText(showpro);
		//m_pro.SetPos(i+1);
		showProsess((i+1)*100);
	}

	GetDlgItem(IDC_SHOWPRO)->SetWindowText(_T("统计完毕^_^"));
	m_pro.ShowWindow(SW_HIDE);

	return 0;
}

void CQSearchDlg::showProsess(int end) {
	for ( int i=0; i <= end; i+=5 ) {
		m_pro.SetPos(i);
	}
}

void CQSearchDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CQSearchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CQSearchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CQSearchDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	// OnOK();

}

void CQSearchDlg::OnLbnSelchangeList1()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CQSearchDlg::OnLbnDblclkList1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString CSpath;
	m_FileName.GetText(m_FileName.GetCurSel(), CSpath);
	string path(CW2A( (LPCTSTR)CSpath) );
	ShellExecute(NULL,   NULL, CSpath,   NULL,   NULL,   SW_SHOWNORMAL);
}

void CQSearchDlg::OnUplow()
{
	// TODO: 在此添加命令处理程序代码
	CMenu* pUplowMenu = menu.GetSubMenu(0);
	if ( m_bIsUpLow ) {
		pUplowMenu->CheckMenuItem(ID_UPLOW, MF_BYCOMMAND|MF_UNCHECKED );
	} else {
		pUplowMenu->CheckMenuItem(ID_UPLOW, MF_BYCOMMAND|MF_CHECKED );
	}
	m_bIsUpLow = !m_bIsUpLow;
}

void CQSearchDlg::OnUnorder()
{
	// TODO: 在此添加命令处理程序代码
	CMenu* pUnorder = menu.GetSubMenu(0);
	if ( m_bUnOrder ) {
		pUnorder->CheckMenuItem(ID_UNORDER, MF_BYCOMMAND|MF_UNCHECKED );
	} else {
		pUnorder->CheckMenuItem(ID_UNORDER, MF_BYCOMMAND|MF_CHECKED );
	}
	m_bUnOrder = !m_bUnOrder;
}

void CQSearchDlg::OnAbout()
{
	// TODO: 在此添加命令处理程序代码
	CAboutDlg m_AboutDlg;
	m_AboutDlg.DoModal();
}

void CQSearchDlg::OnBnClickedFind()
{
	// TODO: 在此添加控件通知处理程序代码
	//	WaitForSingleObject(pThread, INFINITE);
	//	delete pThread;

	// 清空之前的结果
	m_FileName.ResetContent();

	CString lpszStringBuf;
	// 得到输入的文件名称
	m_str.GetWindowText(lpszStringBuf);
	lpszStringBuf.TrimLeft();	// 以防全部是空格
	lpszStringBuf.TrimRight();
	if ( lpszStringBuf.GetLength() == 0 ) {
		return;
	} else if ( lpszStringBuf.GetLength() < 2 ) {
		MessageBox(_T("有效字符小于2,可能会有数十W的文件，请重新输入：^_^"));
		return;
	}

	// CString -> string
	// string str(CW2A( (LPCTSTR)lpszStringBuf) );


	// TODO: 在此添加 
	// 遍历c d e盘
	cmpStrStr cmpstrstr(m_bIsUpLow, m_bUnOrder);
	vector<string>* pignorepath = initdata.getIgnorePath();
	
	for ( list<Volume>::iterator lvolit = initdata.volumelist.begin();
		lvolit != initdata.volumelist.end(); ++lvolit ) {
			// c d e volumelist
			lvolit->findFile(lpszStringBuf, cmpstrstr, pignorepath);

			// 在ListBox中显示
			for ( vector<CString>::iterator vstrit = lvolit->rightFile.begin();
				vstrit != lvolit->rightFile.end(); ++vstrit) {
					m_FileName.AddString( *vstrit );
			}
			lvolit->rightFile.clear();	

	}
}

void CQSearchDlg::OnTip()
{
	// TODO: 在此添加命令处理程序代码
	MessageBox(_T("若是盘符显示不完整，耐心等待5s，再次搜索即可^_^"));
}

void CQSearchDlg::OnIgnore()
{
	// TODO: 在此添加命令处理程序代码
	CIgnoreDlg  Dlg;
	Dlg.DoModal();
	OnBnClickedFind();
}

bool CQSearchDlg::OnInitalIcon(void)
{
	m_bMin = false;

	m_ntIcon.cbSize = sizeof(NOTIFYICONDATA); // 该结构体变量的大小
	m_ntIcon.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); // 图标，通过资源ID得到
	m_ntIcon.hWnd = this->m_hWnd;	 // 接收托盘图标通知消息的窗口句柄
	wstring atip = L"QSearch";
	wcscpy_s(m_ntIcon.szTip,128, atip.c_str());	// 鼠标设上面时显示的提示
	m_ntIcon.uCallbackMessage = MY_WM_NOTIFYICON; // 应用程序定义的消息ID号
	m_ntIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;// 图标的属性：设置成员uCallbackMessage、hIcon、szTip有效
	::Shell_NotifyIcon(NIM_ADD, &m_ntIcon); // 在系统通知区域增加这个图标

	return TRUE;
}

LRESULT CQSearchDlg::OnNotifyIcon(WPARAM wparam, LPARAM lparam)
{
	if(lparam == WM_LBUTTONDOWN) {
		//这里添加对鼠标左键点击的处理，具体处理见（4）
		if(m_bMin == true) {
			AfxGetMainWnd()->ShowWindow(SW_SHOW);
			AfxGetMainWnd()->ShowWindow(SW_RESTORE);
			//这里貌似只有写这样两句才能保证恢复窗口后，该窗口处于活动状态（在最前面）
			m_bMin = false;
		} else {
			AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
			m_bMin = true;
		}  
	} else if(lparam == WM_RBUTTONDOWN)	{
		//这里添加对鼠标右键点击的处理，具体处理见（5）
		//弹出左键菜单
		CMenu popMenu;

		//IDR_MENU_POPUP是在ResourceView中创建并编辑的一个菜单
		popMenu.LoadMenu(IDR_MENU_POPUP); 
		
		//弹出的菜单实际上是IDR_MENU_POPUP菜单的某项的子菜单，这里是第一项
		CMenu* pmenu = popMenu.GetSubMenu(0);
		
		CPoint pos;
		GetCursorPos(&pos);            //弹出菜单的位置，这里就是鼠标的当前位置
		//显示该菜单，第一个参数的两个值分别表示在鼠标的右边显示、响应鼠标右击
		pmenu->TrackPopupMenu(TPM_RIGHTALIGN|TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd(), 0);  
	}
	return 0;
}

void CQSearchDlg::OnDestroy() 
{
	// 删除该图标
	::Shell_NotifyIcon(NIM_DELETE, &m_ntIcon);
}
void CQSearchDlg::OnPopupShow()
{
	// TODO: 在此添加命令处理程序代码
	AfxGetMainWnd()->ShowWindow(SW_SHOWNORMAL);
}

void CQSearchDlg::OnPopupAbout()
{
	// TODO: 在此添加命令处理程序代码
	CAboutDlg abdlg;
	abdlg.DoModal();
}

void CQSearchDlg::OnPopupQuit()
{
	// TODO: 在此添加命令处理程序代码
	// 发送一个WM_CLOSE消息，关闭窗口
	// SendMessage(WM_CLOSE);
	this->DestroyWindow();
}

void CQSearchDlg::OnClose() {
	//this->DestroyWindow();

	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	m_bMin = true;
}
