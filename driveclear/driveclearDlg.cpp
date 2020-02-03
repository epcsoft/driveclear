// driveclearDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "driveclear.h"
#include "driveclearDlg.h"
#include "SM3Tool.h"
#define BUFFERLEN 16777216
bool isdoing=false;
ULONGLONG  spacecanuse;
LPCTSTR localdrive=new TCHAR[16];

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CdriveclearDlg 对话框




CdriveclearDlg::CdriveclearDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CdriveclearDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CdriveclearDlg::appendInfo(const char* str)
{
	struct tm *pTime;
	time_t ctTime; time(&ctTime);
	pTime = localtime(&ctTime);
	char tmpTime[128];
	strftime(tmpTime, 128, "[%H:%M:%S]", pTime);
	CString instr;
	GetDlgItemText(IDC_EDIT5, instr);
	instr = CString(tmpTime)+str +CString("\r\n")+ instr;
	SetDlgItemText(IDC_EDIT5, instr);
}

void CdriveclearDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_capcheck);
}

void CdriveclearDlg::showInfo()
{
	SetDlgItemText(IDC_EDIT5, _T("原理:\r\n  本软件通过覆盖磁盘可用空间来达到彻底清除曾经删除的文件.\r\n\r\n作用:\r\n  可以彻底清除磁盘曾经储存但被移动、修改、删除的痕迹,这样就可以不必担心以前删除了的文件被他人使用 磁盘恢复软件 恢复了.\r\n\r\n常识:\r\n  一般常规方式删除(比如清空回收站或SHIFT+DELETE,以及在windows下的格式化)只是清除文件在文件系统中的索引位置,而文件的实体还是保存在磁盘扇区里的,所以所谓 磁盘恢复软件 依然能够恢复你已删除了的文件,只有完全清除文件在磁盘物理扇区上所占用的每一个区块才算彻底删除,所谓的 文件粉碎软件 就是这个原理."));

}

BEGIN_MESSAGE_MAP(CdriveclearDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_DEVICECHANGE()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO1, &CdriveclearDlg::OnCbnSelchangeCombo1)
	//ON_BN_CLICKED(IDCANCEL, &CdriveclearDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CdriveclearDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CdriveclearDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDSTART, &CdriveclearDlg::OnBnClickedStart)
END_MESSAGE_MAP()
CString   zxcstr(float   zvalue)  
{  
	CString   zxstr;  
	zxstr.Format(_T("%.2f MB"),zvalue);   
	return   zxstr;  
}   


ULONGLONG CdriveclearDlg::getDiskSpaceRemain(const char *path)
{
	BOOL flag;
	ULARGE_INTEGER clu, sec, freeclu;
	char myvolname[128] = { 0 };
	char myfilesys[128] = { 0 };
	char txtbuf[256] = { 0 };
	char rootPath[MAX_PATH] = {0};
	strcpy(rootPath,path);
	rootPath[3] = 0;
	flag = GetVolumeInformation(rootPath, myvolname, 128, NULL, NULL, NULL, myfilesys, 128);
	if (flag != 0)
	{
		wsprintf(txtbuf, _T("%s, %s"), myvolname, myfilesys);
		//MessageBox(hwnd,txtbuf,rootpath,MB_OK);
		SetDlgItemText(IDC_EDIT1, txtbuf);
		//CEdit *pxl=(CEdit   *)   GetDlgItem(hwnd,IDC_EDIT1);  
		//pxl->SetWindowText(txtbuf);

		GetDiskFreeSpaceEx(rootPath, (PULARGE_INTEGER)&clu, (PULARGE_INTEGER)&sec, (PULARGE_INTEGER)&freeclu);
		return freeclu.QuadPart;
	}
	return 0;
}



void CdriveclearDlg::getdiskinfo(LPCTSTR rootpath)
{
	BOOL flag;
	ULARGE_INTEGER clu,sec,freeclu;  
	char myvolname[128] = {0};
	char myfilesys[128] = {0};
	char txtbuf[256] = {0};
	flag= GetVolumeInformation(rootpath,myvolname,128,NULL,NULL,NULL,myfilesys,128);
	if(flag!=0)
	{
		wsprintf(txtbuf,_T("%s, %s"),myvolname,myfilesys);
		//MessageBox(hwnd,txtbuf,rootpath,MB_OK);
		SetDlgItemText(IDC_EDIT1,txtbuf); 
		//CEdit *pxl=(CEdit   *)   GetDlgItem(hwnd,IDC_EDIT1);  
		//pxl->SetWindowText(txtbuf);

		GetDiskFreeSpaceEx(rootpath,(PULARGE_INTEGER)&clu,(PULARGE_INTEGER)&sec,(PULARGE_INTEGER)&freeclu);
		//wsprintf(txtbuf,_T("%s/%s"),(long)freeclu,(long)sec);
		SetDlgItemText(IDC_EDIT2,zxcstr((float)freeclu.QuadPart/1024/1024)); 
		//localdrive=rootpath;
		StringCbCopy((STRSAFE_LPSTR)localdrive,16,rootpath);
		spacecanuse=freeclu.QuadPart;
	}
	else refreshdskinfo();
}
void CdriveclearDlg::refreshdskinfo(){
	bool first=true;
	CComboBox   *pxl   =   (CComboBox   *)   GetDlgItem(IDC_COMBO1);  
	unsigned char r;
	//LPTSTR szBuffer=new TCHAR[1024]; 
	char testbuf[16] = {0};
	pxl->ResetContent();
	for(r='A';r<='Z';r++){
		//testbuf[0]=r;
		sprintf(testbuf,"%c:\\",r);

		if(GetVolumeInformation((LPCTSTR)testbuf,NULL,NULL,NULL,NULL,NULL,NULL,NULL)!=0){
			//MessageBox((LPCTSTR)testbuf,NULL,MB_OK);
			pxl->InsertString(-1,(LPCTSTR )testbuf);  
			if(first){getdiskinfo(testbuf);first=false;}
		}

	}
	pxl->SetCurSel(0);

}
// CdriveclearDlg 消息处理程序

BOOL CdriveclearDlg::OnInitDialog()
{

	CDialog::OnInitDialog();

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
	HANDLE hMutex = CreateMutex(NULL, FALSE, _T("driverclear"));
	if (hMutex != NULL)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			MessageBox("磁盘填充器已经在运行了","提示",MB_OK | MB_ICONINFORMATION);
			//关闭互斥对象，退出程序
			CloseHandle(hMutex);
			exit (0);
		} else
		{
			//MessageBox(NULL,TEXT("第一次运行这个程序。"),TEXT("OnlyOne"),MB_OK | MB_ICONINFORMATION);
		}
	} 
	//ShowWindow(SW_SHOW);
	// TODO: 在此添加额外的初始化代码
	
	showInfo();
	//创建互斥对象
	srand(time(NULL));
	m_capcheck.SetCheck(BST_CHECKED);
	refreshdskinfo();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CdriveclearDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CdriveclearDlg::OnPaint()
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
HCURSOR CdriveclearDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CdriveclearDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
	if (!isdoing)
	{
		refreshdskinfo();
	}
	return CDialog::OnDeviceChange(nEventType, dwData);
}

void randomBuff(char* buffer,int len)
{
	int blocksize = 23333;
	if (blocksize > len)
	{
		blocksize = len;
	}
	for (int r = 0; r < blocksize; r++)
	{
		buffer[r] = rand() % 255;
	}
	for (int r = 1; r < len/ blocksize; r++)
	{
		memcpy(buffer+r*blocksize,buffer,blocksize);
	}
}
unsigned int getCurrentTime()
{
#ifndef WIN32
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#else
	unsigned int current = (unsigned int)(timeGetTime());
	return current;
#endif
}

HANDLE dealFileCreation(std::vector<GM_TestFile>& vecTestFile, CdriveclearDlg *dlg,CString &path,GM_TestFile &singlefile)
{
	char txtbuf[MAX_PATH] = {0};
	sprintf(txtbuf, "temp_%u.tmp", vecTestFile.size());
	path = localdrive + CString(txtbuf);
	dlg->appendInfo(CString("创建文件:" + path));
	HANDLE hfile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_NO_BUFFERING, NULL);
	if (hfile == INVALID_HANDLE_VALUE)
	{
		dlg->appendInfo("无法创建文件,介质只读或没有权限");
		CloseHandle(hfile);
		return NULL;
	}
	singlefile.filename = path;
	singlefile.vecSM3Hash.clear();
	return hfile;
}

void WINAPI erasing (PVOID pParam) 
{
	HANDLE hfile=NULL;
	ULARGE_INTEGER a1,a2,a3;
	unsigned long n;
	SYSTEMTIME sys; 
	unsigned int nowtime;
	unsigned int countst,counten;
	CString path=localdrive;
	ULONGLONG validSpace = spacecanuse;
	char* buf=new char [BUFFERLEN];
	char txtbuf[256] = {0};
	int nErrcount = 0;
	BOOL userstop=false;
	CdriveclearDlg *dlg = (CdriveclearDlg*)pParam;
	if(spacecanuse<=0)
	{
		dlg->appendInfo("剩余0字节...");
		isdoing=false;
		delete[] buf;
		return;
	}
	isdoing = true;

	dlg->GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	dlg->GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
	dlg->m_capcheck.EnableWindow(FALSE);
	dlg->GetDlgItem(IDSTART)->SetWindowText("停止");
	dlg->SendDlgItemMessage(IDC_PROGRESS1,PBM_SETRANGE, 0,MAKELPARAM (0,100));
	GetSystemTime(&sys);
	nowtime=getCurrentTime();
	dlg->appendInfo("准备填充...");
	std::vector<GM_TestFile> vecTestFile;
	GM_TestFile singlefile;
	while(1)
	{			
		if (hfile == NULL)
		{
			hfile = dealFileCreation(vecTestFile, dlg, path, singlefile);
			if (hfile == NULL)
			{
				isdoing = false;
				delete[] buf;
				return;
			}
		}
		randomBuff(buf, BUFFERLEN);
		countst = getCurrentTime();		
		if(WriteFile(hfile,buf, BUFFERLEN,&n,NULL)==0 || n!= BUFFERLEN)break;
		counten = getCurrentTime() - countst;
		if (validSpace < n)
		{
			validSpace = 0;
		}
		else
		{
			validSpace -= n;
		}
		sprintf(txtbuf, "%llu MB", validSpace / 1024 / 1024);
		dlg->SetDlgItemText(IDC_EDIT3, txtbuf);
		if(isdoing==false)
		{
			dlg->appendInfo("用户终止!");
			userstop=true;
			break;
		}
		if (dlg->m_capcheck.GetCheck() == BST_CHECKED)
		{
			GM_SM3Hash sm3hash;
			SM3Tool sm3;
			sm3.sm3((unsigned char*)buf, (int)n, sm3hash.sm3hash);
			sm3hash.len = n;
			singlefile.vecSM3Hash.push_back(sm3hash);
		}
		if (singlefile.vecSM3Hash.size() >=100)
		{
			vecTestFile.push_back(singlefile);
			CloseHandle(hfile);
			hfile = NULL;
		}
		GetSystemTime(&sys);
		unsigned int delta = getCurrentTime() - nowtime;
		if(delta>1000)
		{
			nowtime = getCurrentTime();
			dlg->SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, (WPARAM)((spacecanuse- validSpace) * 100 / spacecanuse), 0);
			if(counten==0)
				sprintf(txtbuf,"-");
			else
			{
				sprintf(txtbuf, "%.2f MB/S", (float)BUFFERLEN *1000.0f / (float)counten / 1024.0f / 1024.0f);
			}
			dlg->SetDlgItemText(IDC_EDIT4,txtbuf);
		}		
	}
	dlg->appendInfo("收尾...");
	if(!userstop)
	{
		if (hfile == NULL)
		{
			hfile = dealFileCreation(vecTestFile, dlg, path, singlefile);
			if (hfile == NULL)
			{
				isdoing = false;
				delete[] buf;
				return;
			}
		}
		GetDiskFreeSpaceEx(localdrive,(PULARGE_INTEGER)&a1,(PULARGE_INTEGER)&a2,(PULARGE_INTEGER)&a3);
		WriteFile(hfile,buf,a3.QuadPart,&n,NULL);
		if (dlg->m_capcheck.GetCheck() == BST_CHECKED&&n>0)
		{
			GM_SM3Hash sm3hash;
			SM3Tool sm3;
			sm3.sm3((unsigned char*)buf, (int)n, sm3hash.sm3hash);
			sm3hash.len = n;
			singlefile.vecSM3Hash.push_back(sm3hash);
		}
	}
	CloseHandle(hfile);
	vecTestFile.push_back(singlefile);
	if (dlg->m_capcheck.GetCheck() == BST_CHECKED&&!userstop)
	{
		
			for (int i = 0; i < vecTestFile.size(); i++)
			{
				if (userstop)
				{
					break;
				}
				GM_TestFile & testFile = vecTestFile[i];
				hfile = CreateFile(testFile.filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
				if (hfile == INVALID_HANDLE_VALUE)
				{
					dlg->appendInfo(CString("无法读取文件：" + testFile.filename + ",介质损坏！"));
					CloseHandle(hfile);
				}
				else
				{
					dlg->appendInfo(CString("校验文件：" + testFile.filename));
				}
				for (int r = 0; r < testFile.vecSM3Hash.size(); r++)
				{
					countst = getCurrentTime();
					if (ReadFile(hfile, buf, BUFFERLEN, &n, NULL) == 0)
					{
						dlg->appendInfo("读取错误!数据意外终止！");
						userstop = true;
						break;
					}
					counten = getCurrentTime() - countst;
					if (isdoing == false)
					{
						dlg->appendInfo("用户终止!");
						userstop = true;
						break;
					}
					unsigned char sm3hash[32] = { 0 };
					SM3Tool sm3;
					sm3.sm3((unsigned char*)buf, (int)n, sm3hash);
					if (memcmp(sm3hash, testFile.vecSM3Hash[r].sm3hash, 32) != 0)
					{
						nErrcount++;
						sprintf(txtbuf, "%s文件检测到%d个错误，位置：%llu MB", testFile.filename.GetBuffer(),nErrcount, (ULONGLONG)r*BUFFERLEN / 1024 / 1024);
						dlg->appendInfo(txtbuf);
						if (nErrcount > 10)
						{
							dlg->appendInfo("错误过多，检测中断！");
							userstop = true;
							break;
						}
					}
					else
					{
						validSpace += testFile.vecSM3Hash[r].len;
					}
					sprintf(txtbuf, "%llu MB", (spacecanuse - validSpace) / 1024 / 1024);
					dlg->SetDlgItemText(IDC_EDIT3, txtbuf);
					dlg->SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, (WPARAM)(validSpace * 100 / spacecanuse), 0);
					GetSystemTime(&sys);
					unsigned int delta = getCurrentTime() - nowtime;
					if (delta > 1000)
					{
						nowtime = getCurrentTime();
						if (counten == 0)
							sprintf(txtbuf, "-");
						else
						{
							sprintf(txtbuf, "%.2f MB/S", (float)BUFFERLEN *1000.0f / (float)counten / 1024.0f / 1024.0f);
						}
						dlg->SetDlgItemText(IDC_EDIT4, txtbuf);
					}
				}
				CloseHandle(hfile);
				hfile = NULL;
			}
		if (hfile != NULL)
		{
				CloseHandle(hfile);
		}
	}
	
	dlg->appendInfo("清理数据中...");
	for (int r = 0; r < vecTestFile.size(); r++)
	{
		DeleteFile(vecTestFile[r].filename);
	}
	isdoing=false;
	dlg->SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS, (WPARAM)(100),0);
	dlg->SetDlgItemText(IDC_EDIT3,_T("0 MB"));
	if (userstop == false)
	{
		if (dlg->m_capcheck.GetCheck() == BST_CHECKED)
		{
			if (nErrcount == 0)
			{
				sprintf(txtbuf, "处理结束！您的磁盘有效容量：%lld MB (%.2lf GB)",validSpace/1024/1024,double(validSpace)/1024.0/1024.0/1024.0);
				dlg->appendInfo(txtbuf);
			}
			else
			{
				dlg->appendInfo("处理结束！您的磁盘有错误！");
			}
		}
		else
		{ 
			dlg->appendInfo("处理结束！您的磁盘数据已毁尸灭迹！");
		}
	}
	dlg->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	dlg->GetDlgItem(IDC_COMBO1)->EnableWindow(TRUE);
	dlg->m_capcheck.EnableWindow(TRUE);
	dlg->GetDlgItem(IDSTART)->EnableWindow(TRUE);
	dlg->GetDlgItem(IDSTART)->SetWindowText("开始");
	delete[] buf;
}
void CdriveclearDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString testbuf;
	GetDlgItemText(IDC_COMBO1,testbuf);
	getdiskinfo(testbuf);
}
void CdriveclearDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if(isdoing)MessageBox(_T("正在处理,请先停止!"),NULL,MB_OK);
	else CDialog::OnClose();
} 

void CdriveclearDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CAboutDlg   dlg;  
	//dlg.Create(IDD_ABOUTBOX,this);
	//dlg.ShowWindow(SW_SHOW);
	dlg.DoModal();
}

void CdriveclearDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	refreshdskinfo();
}

void CdriveclearDlg::OnBnClickedStart()
{
	// TODO: 在此添加控件通知处理程序代码
	unsigned long ThreadID;
	if(!isdoing)
	{
		getdiskinfo(localdrive);
		SetDlgItemText(IDC_EDIT5, "");
		CloseHandle(CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)erasing,this,0,&ThreadID));
	}
	else {
		CWnd   *pWnd;  
		pWnd=GetDlgItem(IDSTART);  
		pWnd->EnableWindow(FALSE);
		isdoing=false;
	}
}
