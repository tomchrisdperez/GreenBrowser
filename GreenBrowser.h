// GreenBrowser.h : main header file for the GreenBrowser application
//

#if !defined(AFX_GreenBrowser_H__19E497BD_4ECF_11D3_9B1D_0000E85300AE__INCLUDED_)
#define AFX_GreenBrowser_H__19E497BD_4ECF_11D3_9B1D_0000E85300AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"
#include "MainFrm.h"
#include "GreenBrowser_i.h"

//##############################################################
#define WM_USER_SHELL_OPEN (WM_USER + 0x1000) 
#define WM_UPDATE_FAV	(WM_USER+0x1001)
#define WM_UPDATE_TAB	(WM_USER + 0x1002)
#define WM_UPDATE_TAB_TIP (WM_USER + 0x1003)
#define WM_UPDATE_TOOLBAR	(WM_USER + 0x1004)
#define WM_ACTIVATE_WINDOW	(WM_USER + 0x1005)
#define WM_DELAYLOAD_CONFIG	(WM_USER + 0x1006)
#define WM_SEL_TAB	(WM_USER + 0x1007)

#ifndef WM_APPCOMMAND
	#define WM_APPCOMMAND					0x0319
	#define FAPPCOMMAND_MASK				0xF000
	#define APPCOMMAND_BROWSER_BACKWARD		1
	#define APPCOMMAND_BROWSER_FORWARD		2
	#define APPCOMMAND_BROWSER_REFRESH		3
	#define APPCOMMAND_BROWSER_STOP			4
	#define APPCOMMAND_BROWSER_HOME			7
	#define APPCOMMAND_FIND		28
	#define APPCOMMAND_NEW		29
	#define APPCOMMAND_OPEN		30
	#define APPCOMMAND_CLOSE	31
	#define APPCOMMAND_SAVE		32
	#define APPCOMMAND_PRINT	33
	#define APPCOMMAND_COPY		36
	#define APPCOMMAND_CUT		37
	#define APPCOMMAND_PASTE	38

	#define GET_APPCOMMAND_LPARAM(lParam)	((short)(HIWORD(lParam) & ~FAPPCOMMAND_MASK))
#endif

#define WM_MENURBUTTONUP                0x0122
#define WM_MENUDRAG                     0x0123
#define WM_MENUGETOBJECT                0x0124
#define WM_UNINITMENUPOPUP              0x0125
#define WM_MENUCOMMAND                  0x0126

/////////////////////////////////////////////////////////////////////////////
// CGreenBrowserApp:
// See GreenBrowser.cpp for the implementation of this class
//

class CGreenBrowserApp : public CWinApp
{
public:
	BOOL	m_bUseLngFile;//use lng file
	BOOL	m_bAutoStart;

	int		m_nDelay;

	CString m_strLngFile;//lng file name
	CString m_strRoot;
	CString m_strUser;
	CString m_strFormDataPath;
	CString m_strGroupPath;
	CString m_strImagePath;
	CString m_strDefaultDirSort;
	CString m_strDefaultDir;
	CString m_strSkinPath;
	CString m_strProfile;

	//function
	CGreenBrowserApp();
	BOOL IsDonated();
	void SaveConfg();
	void LoadConfg();

	class CImpIDispatch* m_pDispOM; 
	class CImpIDropTarget* m_pDropTarget;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGreenBrowserApp)
	public:
	virtual BOOL InitInstance();
	void CopyDefaultFiles();
	void InitializeLanguage();
	bool HandlePreviousInstance(const CCommandLineInfo& cmdInfo);
	bool InitializeWinsock();
	void InitializeMainFrame(CMainFrame* mf, const CCommandLineInfo& cmdInfo);
	void HandleStartupMode(CMainFrame* mf, const CCommandLineInfo& cmdInfo, BOOL run);
	void HandleLastVisit(CMainFrame* mf, BOOL run);
	void LoadLastVisit(CMainFrame* mf);
	void CopyFileIfNotExists(const CString& source, const CString& destination);
	void HandleExistingInstance(HWND hWndPrevious, const CCommandLineInfo& cmdInfo);
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CGreenBrowserApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bATLInited;
	BOOL InitATL();
	BOOL ThreadOnIdle(LONG lCount);
};

extern CGreenBrowserApp theApp; 

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDonate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GreenBrowser_H__19E497BD_4ECF_11D3_9B1D_0000E85300AE__INCLUDED_)
