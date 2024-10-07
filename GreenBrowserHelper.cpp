// GreenBrowserHelper.cpp: implementation of the CGreenBrowserHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GreenBrowser.h"
#include "GreenBrowserHelper.h"
#include "GreenBrowserView.h"
#include "ChildFrm.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGreenBrowserView * CreateNewView()
{
    CWinApp* pApp = AfxGetApp();
 
	CChildFrame *pWnd;
	pWnd = ((CMainFrame*)pApp->m_pMainWnd)->NewChildWindow(0);

	return pWnd->m_pView;
}

CGreenBrowserView* GetCurrentView(int nFlag, BOOL bInNewWindow)
{
	CChildFrame* tcf = (CChildFrame*)((CMDIFrameWnd*)(AfxGetApp()->m_pMainWnd))->MDIGetActive();
	if(tcf!=NULL && !bInNewWindow && !PRESS_CTRL)
	{
		return (CGreenBrowserView*)tcf->m_pView;
	}
	else
	{
		if(nFlag == 1 || bInNewWindow || PRESS_CTRL) //create new 
			return CreateNewView();
		return NULL;
	}
}
