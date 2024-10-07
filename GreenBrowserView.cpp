// GreenBrowserView.cpp : implementation of the CGreenBrowserView class
//
#include "stdafx.h"
#include "GreenBrowser.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include <wininet.h>

#include <Mshtmdid.h>
#include "GreenBrowserView.h"
#include "mshtml.h"
#include "SetRefIntervaldlg.h"

#include <mmsystem.h> //for sndplaysound
#include "SortSaveDlg.h"

//####################################################
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//####################################################
#define FN_LEN		220 //file name length
#define POP_DELAY	pmf->m_nPopDelay
#define MG_LEN		9 //mouse gesture len

//####################################################
#define CAN_ACTIVE(tcf)		(!(tcf->m_nActive&INW_INACTIVE) && (pmf->m_bActiveNewWindow || tcf->m_bForceActive))

//####################################################
char g_cGestureStack[MG_LEN], g_cGestureString[MG_LEN];
int g_nGestureHead, g_nGestureTail;

//####################################################
/////////////////////////////////////////////////////////////////////////////
// CGreenBrowserView

IMPLEMENT_DYNCREATE(CGreenBrowserView, CFixedHtmlView)

BEGIN_MESSAGE_MAP(CGreenBrowserView, CFixedHtmlView)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_OPTIONS_LOADIMAGE, OnOptionsLoadimage)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_LOADIMAGE, OnUpdateOptions)
	ON_COMMAND(ID_OPTIONS_ALLOWSCRIPTS, OnOptionsAllowscripts)
	ON_COMMAND(ID_OPTIONS_LOADSOUNDS, OnOptionsLoadsounds)
	ON_COMMAND(ID_OPTIONS_LOADVIDEOS, OnOptionsLoadvideos)
	ON_COMMAND(ID_OPTIONS_USE_WEBP, OnOptionsUseWebp)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_USE_WEBP, OnUpdateOptionsUseWebp)
	ON_COMMAND(ID_OPTIONS_ALLOWACTIVEX, OnOptionsAllowactivex)
	ON_COMMAND(ID_OPTIONS_ALLOWJAVAAPPLET, OnOptionsAllowjavaapplet)
	ON_UPDATE_COMMAND_UI(ID_GO_BACK, OnUpdateGoBack)
	ON_UPDATE_COMMAND_UI(ID_GO_FORWARD, OnUpdateGoForward)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_ALLOWSCRIPTS, OnUpdateOptions)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_LOADSOUNDS, OnUpdateOptions)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_LOADVIDEOS, OnUpdateOptions)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_ALLOWACTIVEX, OnUpdateOptions)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_ALLOWJAVAAPPLET, OnUpdateOptions)
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_VIEW_FONTS_LARGEST, OnUpdateViewFonts)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FONTS_LARGE, OnUpdateViewFonts)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FONTS_MEDIUM, OnUpdateViewFonts)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FONTS_SMALL, OnUpdateViewFonts)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FONTS_SMALLEST, OnUpdateViewFonts)
	//{{AFX_MSG_MAP(CGreenBrowserView)
	ON_WM_PARENTNOTIFY()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_COMMAND(ID_TOOLS_AUTOREF, OnToolsAutoref)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_AUTOREF, OnUpdateToolsAutoref)
	ON_COMMAND(ID_TOOLS_REFFRE, OnToolsReffre)
	ON_COMMAND(ID_FILE_SAVEHTML, OnFileSavehtml)
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
	ON_COMMAND(ID_FILE_PROPERTY, OnFileProperty)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_FILE_AUTOSAVE, OnFileAutoSave)
	ON_WM_LBUTTONUP()
	ON_UPDATE_COMMAND_UI(ID_TOOLS_AUTOSCROLL_PAGE0, OnUpdateToolsAutoscrollPage)
	ON_COMMAND(ID_VIEW_PAGESIZE_ADD10, OnViewPagesizeAdd10)
	ON_COMMAND(ID_VIEW_PAGESIZE_100, OnViewPagesize100)
	ON_COMMAND(ID_VIEW_PAGESIZE_200, OnViewPagesize200)
	ON_COMMAND(ID_VIEW_PAGESIZE_50, OnViewPagesize50)
	ON_COMMAND(ID_VIEW_PAGESIZE_DEC10, OnViewPagesizeDec10)
	ON_COMMAND(ID_FILE_IMPORT_EXPORT, OnFileImportExport)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_AUTOSCROLL_PAGE1, OnUpdateToolsAutoscrollPage)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_AUTOSCROLL_PAGE2, OnUpdateToolsAutoscrollPage)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_AUTOSCROLL_PAGE4, OnUpdateToolsAutoscrollPage)
	ON_COMMAND(ID_FILE_AUTOSAVE_SORT, OnFileAutoSaveSort)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_GO_BACK, OnGoBack)
	ON_COMMAND(ID_GO_FORWARD, OnGoForward)
	ON_COMMAND(ID_TOOLS_AUTOSCROLL_AUTOSCROLLINGAFTERDOWNLOADING, OnToolsAutoscrollAutoscrollingafterdownloading)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_AUTOSCROLL_AUTOSCROLLINGAFTERDOWNLOADING, OnUpdateToolsAutoscrollAutoscrollingafterdownloading)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_AUTOSCROLL_FAST, OnUpdateToolsAutoscrollSpeed)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_AUTOSCROLL_MEDIUM, OnUpdateToolsAutoscrollSpeed)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_AUTOSCROLL_SLOW, OnUpdateToolsAutoscrollSpeed)
	ON_COMMAND(ID_TOOLS_AUTOSCROLL_USEAUTOSCROLLING, OnToolsAutoscrollUseautoscrolling)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_AUTOSCROLL_USEAUTOSCROLLING, OnUpdateToolsAutoscrollUseautoscrolling)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGreenBrowserView construction/destruction
void CGreenBrowserView::SetSpecialProperty(DWORD dwNewProperty)
{
	m_dwProperty = dwNewProperty;
	ChangeProperty();
	m_bSpecialProperty = true;
}

void CGreenBrowserView::SetDefaultProperty()
{
	m_dwProperty = m_dwPropertyDefault;
}

void CGreenBrowserView::SetFullProperty()
{
	m_dwProperty = 0;
	m_dwProperty |= DLCTL_DLIMAGES|DLCTL_VIDEOS|DLCTL_BGSOUNDS;
	m_dwProperty &= ~DLCTL_NO_SCRIPTS|~DLCTL_NO_JAVA|~DLCTL_NO_RUNACTIVEXCTLS;
}

CGreenBrowserView::CGreenBrowserView()
{
	// TODO: add construction code here
	m_bAutoPopWin = FALSE;
	//
	m_bIsPopUp = FALSE;
	m_nFilterDup = 0;
	m_bEnWebProxy = FALSE;
	m_bIsCapture = FALSE;
	m_bIsActive = FALSE;
	m_bReUsed = FALSE;
	m_bBack= FALSE;
	m_bForward = FALSE;
	m_nProgress = -1;
	m_bIsNewPopup = FALSE;
	m_bDelayAdd = TRUE;
	m_nDupID = -1;
	m_bToClose = FALSE;
	m_bIsSecure = FALSE;
	m_nFontSize = g_nDefFontSize; //4, smallest, 3, small, 2, medium, 1, large, 0, largest

	m_dwPropertyDefault = m_dwProperty = pmf->m_dwDefaultProperty;
	m_lpszUrl = "";
	m_bForceRefresh = FALSE;
	m_bFirst = TRUE;
	m_nSrSpeed = 1; //1-slow, 2-medium, 3-fast
	m_bScrollAfterDL = TRUE;
	m_bUseAutoScroll = FALSE;
	m_nScrollTimer = 0;
	m_nOldIcon = -1;
	m_nCurHistory = -1;
	m_nHistoryLength = 0;
	m_ptLClick.x=0;
	m_ptLClick.y = 0;
	m_ptRClick.x=0;
	m_ptRClick.y=0;

	g_nGestureHead = 0;
	g_nGestureTail = 0;

	m_nAutoRefreshTimer = 0;
	m_bUseAutoRef = FALSE;
	m_nRefreshInterval = pmf->m_nDefRefreshInterval;
	//
	m_nScrollSize = 0;
	m_bSpecialProperty = false;
	m_strFindKey = "";
	m_bFindPrev = 0;
	m_bHighLighted = 0;
	m_strHighLightKey = "";
	m_bSortSave = false;
	m_bForbidAutoNavigate = false;
	m_nInNewWindow = 0;
	//
	m_strOpenNewUrl.Empty();
	m_nAutoRunCmd = 0;
}

CGreenBrowserView::~CGreenBrowserView()
{
	try{
	if(m_nScrollTimer)
		KillTimer(m_nScrollTimer);
	if(m_nAutoRefreshTimer)
		KillTimer(m_nAutoRefreshTimer);
	}catch(...){}
}

void CGreenBrowserView::SetUrl(const CString &url) {
	this->m_lpszUrl = url;
}

CString CGreenBrowserView::GetUrl() {
	return this->m_lpszUrl;
}

BOOL CGreenBrowserView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	try{
    cs.lpszClass = AfxRegisterWndClass(
       CS_DBLCLKS,                       // if you need double-clicks
       NULL,                             // no cursor (use default)
       NULL,                             // no background brush
       NULL); // app icon
    ASSERT(cs.lpszClass);
	}catch(...){}

	return CFixedHtmlView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGreenBrowserView drawing

void CGreenBrowserView::OnInitialUpdate(int nOpenType, LPCSTR strUrl)
{
	try
	{
		m_bFirst = TRUE;
		CFixedHtmlView::OnInitialUpdate();
		if (nOpenType == 1)// && pmf->m_strHomePage.GetLength())
			GoHome();
		else if (nOpenType == 2)
		{
			if(strUrl!=NULL && strUrl!="")
			{
				Navigate2(strUrl, 0, NULL);
				m_lpszUrl = strUrl;
			}
			else
			{
				m_lpszUrl = "about:blank";
				Navigate2(m_lpszUrl, 0, NULL);
			}
		}
		else
		{
			m_lpszUrl = "about:blank";
			Navigate2(m_lpszUrl, 0, NULL);
		}
		m_strWebProxy = pmf->m_strDefaultWebProxy;
	}catch(...){}
}

/////////////////////////////////////////////////////////////////////////////
// CGreenBrowserView diagnostics

#ifdef _DEBUG
void CGreenBrowserView::AssertValid() const
{
	CFixedHtmlView::AssertValid();
}

void CGreenBrowserView::Dump(CDumpContext& dc) const
{
	CFixedHtmlView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGreenBrowserView message handlers
void CGreenBrowserView::OnTitleChange(LPCTSTR lpszText)
{
	try{
	CChildFrame* tcf = ((CChildFrame*)GetParentFrame());
	if(tcf!=NULL)
	{
		if (!pmf->m_bForbidMainTitle)
		{
			if(tcf->m_strStickName != "")
			{
				tcf->SetWindowText(tcf->m_strStickName);
			}
			else
			{
				tcf->SetWindowText(lpszText);
			}
		}
		tcf->m_strTitle = lpszText;
	}

	CString url("");
	//need pay attention!
	try{
	GetLocationURL(url);
	}catch(...){}

	BOOL bSetAddress = TRUE;
	if(!url.IsEmpty())
	{
		if(m_lpszUrl == url)
			bSetAddress = FALSE;
		else
			m_lpszUrl = url;
		//for history
		if(m_pITravelLog == NULL)
			SetHistory(url);
	}
	else
		bSetAddress = FALSE;
	if(m_pITravelLog == NULL && strlen(lpszText)>0)
		m_strLastTitle = lpszText;

	if(pmf!=NULL)
	{
		//set the new title of tab
		if(tcf->m_strStickName=="")
		{
			int nTabID = pmf->FindTab(tcf);
			if (g_bShowIndex)
			{
				CString strTitle;
				strTitle.Format("%d-%s",nTabID+1,lpszText);
				pmf->SetTabTitle( strTitle, nTabID );
			}
			else
				pmf->SetTabTitle( lpszText, nTabID );
		}
		if(bSetAddress && m_bIsActive)
		{
			pmf->SetAddress(url);
		}
		//update tab bar
		if(!pmf->m_bAutoTab)
			pmf->PostMessage(WM_UPDATE_TAB);
	}
	}catch(...){}
}

void WBShowScrollBar(CGreenBrowserView* pvw)
{
	try
	{
		LPDISPATCH pDisp = NULL;
		IHTMLDocument2 *pHTMLDoc = NULL;
		IHTMLElement * pHTMLElem = NULL;
		IHTMLBodyElement *pHTMLBody = NULL;
		try
		{
			pDisp = pvw->GetHtmlDocument();
			if( pDisp )
			{
				if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDoc)))
				{
					if(SUCCEEDED(pHTMLDoc->get_body( &pHTMLElem )))
					{
						if (SUCCEEDED(pHTMLElem->QueryInterface(IID_IHTMLBodyElement, (void**)&pHTMLBody)))
						{
							pHTMLBody->put_scroll(g_bstrYes);
						}
					}
				}
			}
		}catch(...){}

		try
		{
			if(pHTMLBody!=NULL)
				pHTMLBody->Release();
			if(pHTMLElem!=NULL)
				pHTMLElem->Release();
			if(pHTMLDoc!=NULL)
				pHTMLDoc->Release();
			if(pDisp!=NULL)
				pDisp->Release();
		}catch(...){}
	}catch(...){}
}

void CGreenBrowserView::OnDocumentComplete(LPCTSTR lpszUrl)
{
	// make sure the main frame has the new URL.  This call also stops the animation
	try
	{
		CString strUrl;
		GetLocationURL(strUrl);
		if(strUrl.IsEmpty() && lpszUrl!=NULL)
			strUrl = lpszUrl;
		//the strUrl maybe empty
		if (strUrl.GetLength())
			m_lpszUrl = strUrl;
		CString strUrlNew = pmf->GetStandUrl(strUrl);
		
		//filter popup, //strUrlNew==m_lpszUrl => the url maybe the frame in the page
		if(g_bKillPop && !m_bToClose && !m_bIsPopUp && pmf!=NULL && g_bKillListWin && !strUrl.IsEmpty() && strUrlNew==m_lpszUrl) 
		{
			if (pmf->NeedPopFilter(strUrlNew))
			{
				if (pmf->ConfirmPopupFilter(strUrlNew, FALSE))
				{
					m_bIsPopUp = TRUE;
					m_strPossibleURL = strUrlNew;
					m_nProgress = -10;
					CChildFrame* tcf = ((CChildFrame*)GetParentFrame());
					if(tcf!=NULL)
					{
						m_bToClose = TRUE;
						tcf->PostMessage(WM_CLOSE);
						//add counter
						pmf->AddToPopList(strUrlNew, 2);
						return;
					}
				}
			}
		}
		
		if(m_pDisp == NULL) //means document complete
		{
			m_nProgress = -10;			
			if(m_bIsActive && pmf!=NULL && !strUrl.IsEmpty())
			{
				//((CMainFrame*)((CChildFrame*)GetParentFrame()->GetParentFrame()))->SetAddress(strUrl);
				BOOL bModified=FALSE;
				if (pmf->m_bBlankMonitorClip)
				{
					if (strUrl == "about:blank")
					{
						CString str = _GetClipString();
						if (str.GetLength())
						{
							//1:only url 2:url and text
							if (pmf->m_bBlankMonitorClip==2 || IS_URL(str))
							{
								pmf->m_wndAddress->SetWindowText(str);
								bModified = TRUE;
							}
						}
					}
				}
				if (!bModified)
					pmf->SetAddress(strUrl);
				g_nPercent= -1;	
			}
			//set icon
			if((m_nProgress>95 || m_nProgress<0))//3.2 && !strUrl.IsEmpty()
			{
				CChildFrame* tcf = (CChildFrame*)GetParentFrame();
				int nTabID = pmf->FindTab(tcf);
				int oi = m_nOldIcon;
				if(m_bIsActive)
				{
					if(tcf->m_bLock)
						pmf->SetTabIcon( TI_LOCK, nTabID, this);
					else if(tcf->m_bProtect)
						pmf->SetTabIcon( TI_PROTECT, nTabID, this);
					else if (m_nOldIcon!=TI_0)
						pmf->SetTabIcon( TI_NO, nTabID, this);
					//start to scroll
					if(!m_nScrollTimer && (m_bUseAutoScroll || pmf->m_bAllScroll) && m_nSrSpeed!=0)
					{
						if(m_nProgress==100 || m_nProgress<0 || (!m_bScrollAfterDL && m_nProgress>=0))
						{
							//						m_nScrollTimer = SetTimer(2132, (UINT)(1000*g_fSeed/m_nSrSpeed), NULL);
							if(pmf->m_bMouseCtrlScrSpeed)
								m_nLastScrSpd = STOP_TIMER;
							else
								m_nLastScrSpd = (UINT)((g_fSeed*100)/(m_nSrSpeed*m_nSrSpeed));
							m_nScrollTimer = SetTimer(2132, m_nLastScrSpd, NULL);
						}
					}
				}
				else if (m_lpszUrl != "about:blank")//3.3.6
					pmf->SetTabIcon( TI_100, nTabID, this );
				//update tab bar tab0
				if(pmf->m_nTabStyle==2)
					pmf->PostMessage(WM_UPDATE_TAB);
			}
			//restore homepage
			if(g_bLockHomepage)
			{
				CString strHomePage = pmf->GetHomePage();
				if (strHomePage != pmf->m_strHomePage)
				{
					if (strHomePage == "about:blank")//user may set blank in other place
						pmf->m_strHomePage = strHomePage;
					else
						pmf->SetHomePage(pmf->m_strHomePage);
				}
			}
			//better place here, here means the main page complete
			//need run plugin on blank page,for example close right page
			if (pmf->m_bAutoRunExternal && !m_bExternalRunOK)
			{
				pmf->RunSelectedExternal((CChildFrame*)GetParentFrame(),TRUE);
				m_bExternalRunOK = TRUE;
			}
			//better place here, some page reload image will cause OnProgrssChange
			if (m_lpszUrl!="about:blank")
			{
				if (pmf->m_bReloadUnShownImage)
					pmf->RunReloadUnloadImage((CChildFrame*)GetParentFrame());
			}
			if (m_nAutoRunCmd)
			{
				//must active the page, some command only run for active page
				pmf->MDIActivate(GetParentFrame());
				//if use post, user maybe active other page when reveive msg
				pmf->SendMessage(WM_COMMAND, m_nAutoRunCmd, 0);
				m_nAutoRunCmd = 0;
			}
		}
		//scroll bar and background
		if(m_bIsNewPopup &&(m_nProgress==100 || m_nProgress<0))
			WBShowScrollBar(this);
		if(g_bAutoSetBG)
		{
			VARIANT color;
			color.vt = VT_BSTR;
			color.bstrVal = pmf->m_strBGColor.AllocSysString();
			pmf->SetBackground(this, color);
			SysFreeString(color.bstrVal);
		}
	}catch(...){}
}

void CGreenBrowserView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD /*nFlags*/,
	LPCTSTR lpszTargetFrameName, BOOL baPostedData,
	LPCTSTR /*lpszHeaders*/, BOOL* pbCancel)
{
	LARGE_INTEGER liFinish;
	LARGE_INTEGER liFreq;
	BOOL bCanNew = FALSE;
	TCITEM TabCtrlItem;
	int nFilterType = -1;

	try{
	if (m_bForbidAutoNavigate)
	{
		*pbCancel=TRUE;
		return;
	}
	CString strUrl=lpszURL;
	//down manager before ftp, for exam: ftp://123/asd.zip
	if (pmf->m_nManagerIndex)
	{
		if(pmf->IsDownloadAble(strUrl))
		{
			*pbCancel=TRUE;
			pmf->RunDownManagerScript(strUrl, (CChildFrame*)GetParentFrame());
			return;
		}
	}
	//must after down manager
	if (pmf->m_nFtpOpenType && !PRESS_CTRL && IS_FTP(strUrl))
	{
		_OpenFtpUrl(strUrl);
		*pbCancel=TRUE;
		return;
	}
	//open link in new window
	if (pmf->m_bOpenFolderOut)
	{
		if (IS_LOCAL_FOLDER(strUrl))
		{
			ShellExecute(NULL, "open", lpszURL , "", NULL, SW_SHOW );
			*pbCancel = TRUE;
			return;
		}
	}
	//init some var
	m_bExternalRunOK = FALSE;
	m_strFindKey = "";
	m_strHighLightKey = "";
	pmf->m_strLastNavUrl = lpszURL;

	//set prop
	CString strUrlNew = pmf->GetStandUrl(lpszURL);
	if ( pmf->m_nDownAll && m_bSpecialProperty==FALSE)
	{
		int oldType = pmf->GetUrlType(m_lpszUrl);
		int type = pmf->GetUrlType(lpszURL);
		//open other url in same window
		BOOL bSameUrl=false;
		if (m_lpszUrl==strUrlNew)
			bSameUrl = true;
		else
		{
			//!! http://abc and http://abc/
			int nLen1 = m_lpszUrl.GetLength();
			int nLen2 = strUrlNew.GetLength();
			if (nLen1 != nLen2)
			{
				nLen1 = min(nLen1,nLen2);
				if (m_lpszUrl.Left(nLen1) == strUrlNew.Left(nLen1) )
					bSameUrl = true;
			}
		}
		if ( !bSameUrl && (type!=0))//difficult dowith local file (%20 problem)
		{
			if (type == oldType)
				;
			else if (type==2)//internet file
			{
				if (pmf->m_nDownAll==1 && oldType==0)
					SetDefaultProperty();
				else if (pmf->m_nDownAll==2 && oldType<2)
					SetDefaultProperty();
				else
					;//do nothing
			}
			else if (type==1)//local net file
			{
				if (pmf->m_nDownAll==1 && oldType==0)
					SetDefaultProperty();
				else
					;//do nothing
			}
		}
		else if (type==3)//"about:blank"
			SetDefaultProperty();
		else if (pmf->m_nDownAll==1)
		{
			if (type==0)
				SetFullProperty();
			else
				SetDefaultProperty();
		}
		else if (pmf->m_nDownAll==2)
		{
			if (type==0 || type==1)
				SetFullProperty();
			else
				SetDefaultProperty();
		}
	}
	//
	CChildFrame* tcf = (CChildFrame*)GetParentFrame();
	BOOL m_bLock = tcf->m_bLock;
	//??
	if (m_strOpenNewUrl == lpszURL)
	{
		QueryPerformanceCounter(&liFinish);
		QueryPerformanceFrequency(&liFreq);
		double secs = (double)(liFinish.LowPart - m_tLastClick.LowPart)/(double)liFreq.LowPart;
		if(secs<POP_DELAY && secs>0)
		{
			m_strOpenNewUrl.Empty();
			*pbCancel = TRUE;
			return;
		}
	}
	m_strOpenNewUrl.Empty();
	//
	if(g_bLinkInNewWindow || m_bLock || m_nInNewWindow)
	{
		if(m_tLastClick.LowPart != 0 && !baPostedData)
		{
			QueryPerformanceCounter(&liFinish);
			QueryPerformanceFrequency(&liFreq);
			if(liFreq.LowPart!=0)
			{
				double secs = (double)(liFinish.LowPart - m_tLastClick.LowPart)/(double)liFreq.LowPart;
				if(secs<POP_DELAY && secs>0)
				{
					if (g_bKillPop && g_bKillDupWin)
					{
						int dup = pmf->FindDupURL(lpszURL, tcf);
						if(dup<0)
							bCanNew = TRUE;
						else
						{
							*pbCancel = TRUE;

							TabCtrlItem.mask = TCIF_PARAM;
							pmf->m_wndTab.GetItem(dup, &TabCtrlItem);
							CChildFrame* tcf2 = (CChildFrame*)TabCtrlItem.lParam;
							if(tcf2!=NULL && tcf2!=tcf)
								pmf->MDIActivate(tcf2);
						}
					}
					else
						bCanNew = TRUE;
				}
				else
					m_tLastClick.LowPart = 0;
			}
		}
		//
		if(!m_bFirst && pmf!=NULL && bCanNew)
		{
			const char* pdest = strchr(lpszURL, '#');
			if(pdest!=NULL)
			{
				if(strncmp(lpszURL, m_lpszUrl, pdest - lpszURL)!=0)
					pdest = NULL;
			}
			BOOL bScript = IS_SCRIPT_STR(lpszURL);
			if ( !bScript && !IS_MAILTO_STR(lpszURL) && pdest==NULL )
			{
				CChildFrame *tcf = NULL;
				tcf = pmf->NewChildWindow(0,1, NULL, this);
				if(tcf != NULL)
				{
					tcf->m_pView->m_dwProperty = m_dwProperty;
					tcf->m_pView->m_bIsNewPopup = FALSE; //TRUE;
					tcf->m_pView->m_bFirst = TRUE;
					tcf->m_pView->m_nProgress = 0;
					//must before native
					tcf->m_nActive = m_nInNewWindow;
					tcf->m_bForceActive = m_nInNewWindow & INW_ACTIVE;
					//for that OnCreateClient will modify m_bIsActive, modify back at here
					tcf->m_pView->m_bIsActive = (CAN_ACTIVE(tcf) ? TRUE : FALSE);
					m_nInNewWindow = 0;
					//
					tcf->m_pView->ToNavigate(lpszURL);
					if(pbCancel!=NULL)
						*pbCancel = TRUE;
					m_strOpenNewUrl = lpszURL;
					return;
				}
			}
			//not clear script link, it will open in OnNewWindow2
			if (!bScript)
				m_nInNewWindow = 0;
		}
	}//end lock and open new

	m_bFirst = FALSE;
	m_bForceRefresh =  FALSE;

	//filter popup
	if(g_bKillPop && g_bKillListWin && lpszURL!=NULL && strcmp(lpszURL, "about:blank")!=0 && strUrlNew==m_lpszUrl)
	{
		if (pmf->NeedPopFilter(strUrlNew))
		{
			if (pmf->ConfirmPopupFilter(strUrlNew, FALSE))
			{
				if(pbCancel!=NULL)
					*pbCancel = TRUE;
				m_bIsPopUp = TRUE;
				m_strPossibleURL = strUrlNew;
				m_nProgress = -1;
				//add counter
				nFilterType = 2;
				pmf->AddToPopList(strUrlNew, nFilterType);
			}
		}
		else
		{
			m_nProgress = 0;
			if(m_bIsActive)
			{	
				//if "mailto:" then not chang g_nPercent
				if (strUrl.Left(5)=="http:" || strUrl.Left(5)=="file:" || strUrl.Find(':')==1)
				{
					g_nPercent = m_nProgress;
					//
					int nIcon = m_nProgress/34+1;
					int nTabID = pmf->FindTab((CChildFrame*)GetParentFrame());
					if(m_nProgress>=0)
						pmf->SetTabIcon( nIcon, nTabID, this );
				}
			}
		}
	}
	//filter duplicate url
	if(g_bKillPop && g_bKillDupWin && m_nFilterDup && !m_bIsPopUp && m_bIsNewPopup && pmf!=NULL && lpszURL!=NULL && strcmp(lpszURL, "about:blank")!=0 )//&& strUrlNew==m_lpszUrl)
	{
		//only filter once the window popup
		m_nFilterDup --;
		int dup = pmf->FindDupURL(strUrlNew, tcf);
		if(dup>=0)
		{
			//if (pmf->ConfirmPopupFilter(strUrlNew,1))
			{
				if(pbCancel!=NULL)
					*pbCancel = TRUE;
				m_bIsPopUp = TRUE;
				m_strPossibleURL = strUrlNew;
				m_nDupID = dup;
				m_nProgress = -1;
				m_nFilterDup = 0;

				//active that dup window
				if(tcf && pmf)
				{
					TabCtrlItem.mask = TCIF_PARAM;
					pmf->m_wndTab.GetItem(m_nDupID, &TabCtrlItem);
					CChildFrame* tcf2 = (CChildFrame*)TabCtrlItem.lParam;
					if(tcf2!=NULL && tcf2!=tcf)
					{
						pmf->MDIActivate(tcf2);
						//pmf->m_wndTab.SetCurSel(m_nDupID);
					}
					m_nDupID = -1;
				}
			}
		}
		else
		{
			m_nProgress = 0;
			if(m_bIsActive)
			{	
				//if "mailto:" then not chang g_nPercent
				if (strUrl.Left(5)=="http:" || strUrl.Left(5)=="file:" || strUrl.Find(':')==1)
				{
					g_nPercent = m_nProgress;
					//
					int nIcon = m_nProgress/34+1;
					int nTabID = pmf->FindTab((CChildFrame*)GetParentFrame());
					if(m_nProgress>=0)
						pmf->SetTabIcon( nIcon, nTabID, this );
				}
			}
		}
	}
	//if have filter not auto pop filter
	if (*pbCancel==TRUE)
	{
		m_bAutoPopWin = FALSE;
		//when not have page, list filter this, will make a error window
		//but not close dup window
		if (nFilterType==0 || nFilterType==2)
		{
			CChildFrame* tcf = ((CChildFrame*)GetParentFrame());
			if(tcf!=NULL)
			{
				m_bToClose = TRUE;
				tcf->PostMessage(WM_CLOSE);
			}
		}
	}
	//
	if (m_bAutoPopWin)
	{
		m_bAutoPopWin = FALSE;
		BOOL bNeedFilter=pmf->NeedUnPopFilter(strUrlNew);
		if (bNeedFilter && pmf->ConfirmPopupFilter(strUrlNew, TRUE))
		{
			if(pbCancel!=NULL)
				*pbCancel = TRUE;
			m_bIsPopUp = TRUE;
			m_strPossibleURL = strUrlNew;
			m_nProgress = -1;
			//must close the tcf
			CChildFrame* tcf = ((CChildFrame*)GetParentFrame());
			if(tcf!=NULL)
			{
				m_bToClose = TRUE;
				tcf->PostMessage(WM_CLOSE);
			}
			//
			pmf->AddToPopList(strUrlNew, 0);
			return;
		}
	}
	//secure info
	if(strncmp(lpszURL, "https", 5)!=0)
		m_bIsSecure = FALSE;
	if(m_bIsActive && pmf!=NULL)
		pmf->m_bIsSecure = m_bIsSecure;

	//reused window
	if(!m_bIsPopUp && m_bReUsed && tcf!=NULL && pmf!=NULL && CAN_ACTIVE(tcf))
	{
		int nTabID = pmf->FindTab(tcf);
		//active window
		pmf->MDIActivate(tcf);
		//select current tab
		pmf->m_wndTab.SetCurSel( nTabID );
		tcf->m_bForceActive = FALSE;
	}
	m_bReUsed = FALSE;

	//Add a tab to tabbar
	if(!m_bIsPopUp && m_bDelayAdd && tcf!=NULL && pmf!=NULL)
	{
		tcf->ShowWindow(SW_SHOW);
		m_bDelayAdd = FALSE;
		//add tab
		pmf->AddNewTab(tcf);
		int nTabID = pmf->FindTab(tcf);
		//active window
		if (CAN_ACTIVE(tcf))
		{
			pmf->MDIActivate(tcf);
			//select current tab
			pmf->m_wndTab.SetCurSel( nTabID );
			tcf->m_bForceActive = FALSE;
		}
		//after use m_nActive, set back to 0, BeforeNavigate2 TargetFrameName will use this
		tcf->m_nActive = 0;
		//set icon
		pmf->SetTabIcon( TI_0, nTabID, this );
		//update tab bar
		if(pmf->m_nTabStyle==2)
			pmf->PostMessage(WM_UPDATE_TAB);
		pmf->PostMessage(WM_UPDATE_TAB_TIP);
	}
	}catch(...){}
}

// these are all simple one-liners to do simple controlling of the browser
void CGreenBrowserView::OnGoBack()
{
	if(m_bBack)
		GoBack();
}

void CGreenBrowserView::OnGoForward()
{
	if(m_bForward)
		GoForward();
}

void CGreenBrowserView::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel) 
{
	// TODO: Add your specialized code here and/or call the base class
	CChildFrame *tcf = NULL;
	if(ppDisp == NULL || Cancel == NULL)
		return;

	try{
	*Cancel = TRUE;
	*ppDisp = NULL;
	//auto filter popup window not opened by user
	if(g_bKillPop && g_bKillAutoWin && !PRESS_CTRL)
	{
		//this window is to be closed, so don't open any other windows from it.
		if(m_bToClose)
			return;
		LARGE_INTEGER liFinish;
		LARGE_INTEGER liFreq;
		QueryPerformanceCounter(&liFinish);
		QueryPerformanceFrequency(&liFreq);
		if(liFreq.LowPart!=0)
		{
			CString strUrl;
			double secs = (double)(liFinish.LowPart - m_tLastClick.LowPart)/(double)liFreq.LowPart;
			if((secs>POP_DELAY && secs<4.0 && m_bLClick) || m_nProgress==-1)
			{
				if(m_nProgress>=0)
					strUrl = GetPointUrl(m_ptLClick);
				else if(GetBusy())//whether browser is busy when m_mPorgress <0
					m_nProgress=0;
			}
			//
			try{
			if	(
					!(	m_bLClick
						&&
						( (secs>=0 && secs<=POP_DELAY) || IS_URL(strUrl) || m_nProgress==-1 )
					 ) 
				)
			{
				int bFilter = TRUE;
				//may click open in new window of pop menu
				if (m_bRClick)
				{
					if (m_bRClick == RC_NOT_FILTER)
					{
						bFilter = FALSE;//not filter
						m_bRClick = 0;
					}
					else if (!pmf->m_bFilterRefresh)
						bFilter = FALSE;
					else
					{
						CString strUrl = GetPointUrl(pmf->m_ptMouseDown);
						if (IS_URL(strUrl) || IS_SCRIPT(strUrl))
							bFilter = FALSE;//not filter
					}
				}
				//for that here not know the url, have to get the url at OnBeforeNavigate2
				if (bFilter)
				{
					//5.5.27, use may use unfilter list
					/*if (!pmf->m_bOutputSaveFilter)
					{
						pmf->m_nPopFiltered++;
						if (pmf->m_bPlayPopupFilterSound)
							::sndPlaySound(theApp.m_strRoot+"Resource\\PopFilter.wav", SND_ASYNC);
						return;
					}
					else*/
						pmf->m_bAutoPopWin = TRUE;
					//return; will get the url in onbeforenavigate
				}
			}
			}catch(...){}
		}
	}
	m_bLClick = FALSE;
	
	//new child window from this
	tcf = pmf->NewChildWindow(0,1, NULL, this);
	//must do this, for pop in exist blank window
	if (pmf->m_bAutoPopWin)
	{
		tcf->m_pView->m_bAutoPopWin = TRUE;
		pmf->m_bAutoPopWin = FALSE;
	}
	if (m_nInNewWindow)
	{
		tcf->m_nActive = m_nInNewWindow;
		tcf->m_bForceActive = m_nInNewWindow & INW_ACTIVE;
		tcf->m_pView->m_bIsActive = (CAN_ACTIVE(tcf) ? TRUE : FALSE);
		m_nInNewWindow = 0;
	}
	//this maybe a blank window.
	if(((CChildFrame*)GetParentFrame())->m_pBlankChild != tcf)
	{
		((CChildFrame*)GetParentFrame())->m_pBlankChild = tcf;
		QueryPerformanceCounter(&((CChildFrame*)GetParentFrame())->m_tBlankChild);
	}
	else
		((CChildFrame*)GetParentFrame())->m_pBlankChild = NULL;
	tcf->m_pView->m_dwProperty = m_dwProperty;
	tcf->m_pView->m_bIsNewPopup = TRUE;
	tcf->m_pView->m_bIsPopUp = FALSE;
	tcf->m_pView->m_bToClose = FALSE;
	tcf->m_pView->m_nFilterDup = 1;
	//tcf->m_pView->m_bForceResize = TRUE;
	tcf->m_pView->m_nProgress = 0;
	//tcf->m_pView->g_bKillAutoWin = g_bKillAutoWin;
	*ppDisp = tcf->m_pView->GetApplication();
	if(*ppDisp != NULL)
		*Cancel = FALSE;

	}catch(...){}

 	//CFixedHtmlView::OnNewWindow2(ppDisp, Cancel);
}

void CGreenBrowserView::OnProgressChange(long nProgress, long nProgressMax) 
{
	// TODO: Add your specialized code here and/or call the base class
	try{
	int oldp = m_nProgress;
	if(nProgressMax>0 && nProgress>=0 && nProgressMax/100>0)
	{
		m_nProgress  = nProgress/(nProgressMax/100);
		if(m_nProgress>100)
			m_nProgress=100;
	}
	else
	{
		m_nProgress = -10;
	}
	if(m_bIsActive)
		g_nPercent = m_nProgress;

	if(m_bIsActive && (m_bUseAutoScroll || pmf->m_bAllScroll) )
	{
		//start to scroll
		if(m_nScrollTimer==0 && (m_bUseAutoScroll || pmf->m_bAllScroll) && m_nSrSpeed!=0)
		{
			if(m_nProgress==100 || m_nProgress<0 || (!m_bScrollAfterDL && m_nProgress>=0))
			{
				if(pmf->m_bMouseCtrlScrSpeed)
					m_nLastScrSpd = STOP_TIMER;
				else
					m_nLastScrSpd = (UINT)((g_fSeed*100)/(m_nSrSpeed*m_nSrSpeed));
				m_nScrollTimer = SetTimer(2132, m_nLastScrSpd, NULL);
			}
		}
		else if(m_nScrollTimer && m_nProgress<20 && m_nProgress>0)
		{
			//stop the timer;
			if(m_nScrollTimer)
				KillTimer(m_nScrollTimer);
			m_nScrollTimer = 0;
		}
	}
	//set icon
	if(pmf!=NULL)
	{
		int nIcon = m_nProgress/34+1;
		int oi = m_nOldIcon;
		if(m_nProgress<0)
			nIcon=-1;
		if((m_nProgress>=0 && nIcon != oi) || m_nProgress<0)
		{
			int nTabID;
			nTabID = pmf->FindTab((CChildFrame*)GetParentFrame());
			if(m_nProgress>=0 && nIcon != oi)
				pmf->SetTabIcon( nIcon, nTabID, this );
			else if(m_nProgress<0)
			{
				if(((CChildFrame*)GetParentFrame())->m_bLock)
					pmf->SetTabIcon( TI_LOCK, nTabID, this );
				else if(((CChildFrame*)GetParentFrame())->m_bProtect)
					pmf->SetTabIcon( TI_PROTECT, nTabID, this );
				else
				{
					if(!m_bIsActive)
					{
						if (m_nOldIcon!=TI_0 && nProgress==0 && nProgressMax==0 && m_lpszUrl != "about:blank")//3.3.6
							pmf->SetTabIcon( TI_100, nTabID, this );
					}
					else
					{
						if (m_nOldIcon!=TI_0)
							pmf->SetTabIcon( TI_NO, nTabID, this );
					}
				}
			}
			//update tab bar
			if(m_nProgress*oldp<=0 && oi*m_nOldIcon<=0 && m_nOldIcon != oi && pmf->m_nTabStyle==2)
				pmf->PostMessage(WM_UPDATE_TAB);
			//complete
			if (nProgress==0 && nProgressMax==0)
			{
				CChildFrame* tcf = (CChildFrame*)pmf->MDIGetActive();
				if (tcf && pmf->m_nState)
				{
					if (tcf->m_pView->GetUrl() == "about:blank" &&
						tcf == (CChildFrame*)GetParentFrame() )
					{
						pmf->m_wndAddress->SetFocus();
					}
					else
					{
						CWnd* pWnd = GetFocus();
						if ( pWnd == (CWnd*)(pmf->m_wndAddress->GetEditCtrl()) || pWnd == (CWnd*)(pmf->m_wndSearch->GetEditCtrl())	)
							;//do nothing
						else if (IsTopParentActive())
							tcf->ViewSetFocus();
					}
				}
				//run internal plugin,must place here, for that refresh not get ondocumentcomplete
				if (m_nOldIcon!=TI_0)// && m_lpszUrl!="about:blank")
				{
					if (pmf->m_bUseFlyingFilter)
						pmf->RunKillFlying((CChildFrame*)GetParentFrame());
					if (pmf->m_bKillBlank && pmf->m_bUseUrlFilter)
						pmf->RunKillBlank((CChildFrame*)GetParentFrame());
				}
			}
		}
	}
	}catch(...){}

	//CFixedHtmlView::OnProgressChange(nProgress, nProgressMax);
}

void CGreenBrowserView::OnEditCopy() 
{
	// TODO: Add your command handler code here
	ExecWB(OLECMDID_COPY, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
}

void CGreenBrowserView::OnEditCut() 
{
	// TODO: Add your command handler code here
	ExecWB(OLECMDID_CUT, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
}

void CGreenBrowserView::OnEditFind() 
{
	// TODO: Add your command handler code here
	pmf->ExecCmdTarget(HTMLID_FIND, this);
}

void CGreenBrowserView::OnEditPaste() 
{
	// TODO: Add your command handler code here
	ExecWB(OLECMDID_PASTE, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
}

void CGreenBrowserView::OnEditSelectAll() 
{
	// TODO: Add your command handler code here
	ExecWB(OLECMDID_SELECTALL, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
}

void CGreenBrowserView::OnStatusTextChange(LPCTSTR lpszText) 
{
    // TODO: Add your specialized code here and/or call the base class
    try {
        if (!m_bIsActive || pmf == NULL || lpszText == NULL || strlen(lpszText) == 0)
            return;

        if (g_bDisScrollText) {
            const char* pdes = strstr(lpszText, "about:");
            if (!pdes) pdes = strstr(lpszText, "://");
            if (!pdes) pdes = strstr(lpszText, "mailto:");
            if (!pdes) pdes = strstr(lpszText, "javascript:");

            int pos = (pdes) ? pdes - lpszText : 0;

            if (strncmp(lpszText, "Shortcut", 8) != 0 && strcmp(lpszText, "Done") != 0 &&
                (!pdes || pos > 10) && strncmp(lpszText, "Wait", 4) != 0 &&
                strncmp(lpszText, "Start", 5) != 0 && strncmp(lpszText, "Open", 4) != 0 &&
                strncmp(lpszText, "Connect", 7) != 0 && strncmp(lpszText, "Find", 4) != 0 &&
                strncmp(lpszText, "download", 8) != 0 && strncmp(lpszText, "Web", 3) != 0 &&
                strstr(lpszText, "remain") == NULL)
            {
                return;
            }
        }

        if (!(pmf->m_bStatusBlank && strlen(lpszText) == 0)) {
            pmf->SetMessageText(lpszText);
        }

        pmf->m_bStatusBlank = (strlen(lpszText) == 0);
        
    } catch (...) {}
}

BOOL CGreenBrowserView::OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (dispid == DISPID_AMBIENT_DLCONTROL)
	{
		pvar->vt = VT_I4;
		pvar->lVal = m_dwProperty; 
		if (pmf->m_bDisDownActivex)
			pvar->lVal |= DLCTL_NO_DLACTIVEXCTLS;
		return TRUE;
	}
	return CFixedHtmlView::OnAmbientProperty(pSite, dispid, pvar);
}

void CGreenBrowserView::OnOptionsLoadimage() 
{
	// TODO: Add your command handler code here
	m_dwProperty ^= DLCTL_DLIMAGES;
	ChangeProperty();
	if (!PRESS_CTRL)
		PowerRefresh();
}

void CGreenBrowserView::OnUpdateOptions(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	try{
	switch (pCmdUI->m_nID)
	{
	case ID_OPTIONS_LOADIMAGE:
		pmf->ToolBarSetCheck(pCmdUI, (m_dwProperty&DLCTL_DLIMAGES)!=0);
		break;
	case ID_OPTIONS_LOADVIDEOS:
		if(m_dwProperty&DLCTL_VIDEOS)
			pCmdUI->SetCheck(1);
		else
			pCmdUI->SetCheck(0);
		break;
	case ID_OPTIONS_LOADSOUNDS:
		if(m_dwProperty&DLCTL_BGSOUNDS)
			pCmdUI->SetCheck(1);
		else
			pCmdUI->SetCheck(0);
		break;
	case ID_OPTIONS_ALLOWSCRIPTS:
		if(m_dwProperty&DLCTL_NO_SCRIPTS)
			pCmdUI->SetCheck(0);
		else
			pCmdUI->SetCheck(1);
		break;
	case ID_OPTIONS_ALLOWJAVAAPPLET:
		if(m_dwProperty&DLCTL_NO_JAVA)
			pCmdUI->SetCheck(0);
		else
			pCmdUI->SetCheck(1);
		break;
	case ID_OPTIONS_ALLOWACTIVEX:
		if(m_dwProperty&DLCTL_NO_RUNACTIVEXCTLS)
			pCmdUI->SetCheck(0);
		else
			pCmdUI->SetCheck(1);
		break;
	}
	}catch(...){}
}

void CGreenBrowserView::OnOptionsAllowscripts() 
{
	// TODO: Add your command handler code here
	m_dwProperty ^= DLCTL_NO_SCRIPTS;
	ChangeProperty();
	if (!PRESS_CTRL)
		PowerRefresh();
}

void CGreenBrowserView::OnOptionsLoadsounds() 
{
	// TODO: Add your command handler code here
	m_dwProperty ^= DLCTL_BGSOUNDS;
	ChangeProperty();
	if (!PRESS_CTRL)
		PowerRefresh();
}

void CGreenBrowserView::OnOptionsLoadvideos() 
{
	// TODO: Add your command handler code here
	m_dwProperty ^= DLCTL_VIDEOS;
	ChangeProperty();
	if (!PRESS_CTRL)
		PowerRefresh();
}

void CGreenBrowserView::OnOptionsUseWebp() 
{
	// TODO: Add your command handler code here
	if(m_bEnWebProxy)
		m_bEnWebProxy = FALSE;
	else
	{
		m_bEnWebProxy = TRUE;
		//auto refresh
		if (m_strWebProxy.IsEmpty())
		{
			if (!pmf->m_strDefaultWebProxy.IsEmpty())
				m_strWebProxy = pmf->m_strDefaultWebProxy;
			else
			{
				m_bEnWebProxy = FALSE;
				MSGBOX(IDS_NO_WEB_PROXY);
				return;
			}
		}
		if(!m_lpszUrl.IsEmpty() && m_lpszUrl!="about:blank")
		{
			ToNavigate(m_lpszUrl, 0, NULL, NULL, NULL, 0, TRUE);
		}
	}
}

void CGreenBrowserView::OnUpdateOptionsUseWebp(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	try
	{
		pmf->ToolBarSetCheck(pCmdUI, m_bEnWebProxy);
	}catch(...){}
}

void CGreenBrowserView::OnOptionsAllowactivex() 
{
	// TODO: Add your command handler code here
	m_dwProperty ^= DLCTL_NO_RUNACTIVEXCTLS;
	ChangeProperty();
	if (!PRESS_CTRL)
		PowerRefresh();
}

void CGreenBrowserView::OnOptionsAllowjavaapplet() 
{
	// TODO: Add your command handler code here
	m_dwProperty ^= DLCTL_NO_JAVA;
	ChangeProperty();
	if (!PRESS_CTRL)
		PowerRefresh();
}

void CGreenBrowserView::OnCommandStateChange(long nCommand, BOOL bEnable) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(nCommand == 2)
		m_bBack = bEnable;
	else if(nCommand == 1)
		m_bForward = bEnable;

	//	CFixedHtmlView::OnCommandStateChange(nCommand, bEnable);
}

void CGreenBrowserView::OnUpdateGoBack(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	try{
	pCmdUI->Enable(m_bBack);
	}catch(...){}
}

void CGreenBrowserView::OnUpdateGoForward(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	try{
	pCmdUI->Enable(m_bForward);
	}catch(...){}
}

void ResizeSibling(CGreenBrowserView* pvw)
{
	try{
	g_bMax = 0;
	CChildFrame* tcf;
	CRect oldrect,rect;
	if(pmf!=NULL)
	{
		int n = pmf->m_wndTab.GetItemCount();
		TCITEM TabCtrlItem;
		TabCtrlItem.mask = TCIF_PARAM;
		for(int i=n-1;i>=0;i--)
		{
			pmf->m_wndTab.GetItem(i, &TabCtrlItem);
			tcf = ((CChildFrame*)TabCtrlItem.lParam);
			if(tcf != NULL && tcf->m_pView!=NULL)
			{
				if(tcf->m_pView != pvw)
				{
					tcf->GetClientRect(&rect);
					tcf->m_pView->m_wndBrowser.GetWindowRect(&oldrect);
					tcf->m_pView->ScreenToClient(&oldrect);
					if(oldrect.left != -BORDER && oldrect.top != -BORDER)
					{
						try{
						tcf->m_pView->m_wndBrowser.MoveWindow(-2,-2, rect.Width()+BORDER+BORDER, rect.Height()+BORDER+BORDER);
						}catch(...){}
					}
					else
					{
						try{
						if(oldrect.Width()!=rect.Width()+BORDER+BORDER && rect.Width()!=0)
							tcf->m_pView->SetWidth (rect.Width()+BORDER+BORDER);
						if(oldrect.Height()!=rect.Height()+BORDER+BORDER && rect.Height()!=0)
							tcf->m_pView->SetHeight (rect.Height()+BORDER+BORDER);
						}catch(...){}
					}
				}
			}
		}
	}
	}catch(...){}
}

void CGreenBrowserView::OnSize(UINT nType, int cx, int cy) 
{
	try
	{
		if(g_bMax==1)
		{
			if(GetParentFrame()->IsZoomed())
			{
				CRect oldrect;
				m_wndBrowser.GetWindowRect(&oldrect);
				ScreenToClient(&oldrect);
				if(oldrect.left != -BORDER && oldrect.top != -BORDER)
				{
					try{
					m_wndBrowser.MoveWindow(-2,-2, cx+BORDER+BORDER, cy+BORDER+BORDER);
					}catch(...){}
				}
				else
				{
					try{
					if(oldrect.Width()!=cx+BORDER+BORDER )
						SetWidth (cx+BORDER+BORDER);
					if(oldrect.Height()!=cy+BORDER+BORDER)
						SetHeight (cy+BORDER+BORDER);
					}catch(...){}
				}
			}
		}
		else
		{
			CRect oldrect;
			m_wndBrowser.GetWindowRect(&oldrect);
			ScreenToClient(&oldrect);
			if(oldrect.left != -BORDER && oldrect.top != -BORDER)
			{
				try{
				m_wndBrowser.MoveWindow(-2,-2, cx+BORDER+BORDER, cy+BORDER+BORDER);
				}catch(...){}
			}
			else
			{
				try{
				if(oldrect.Width()!=cx+BORDER+BORDER && cx!=0)
					SetWidth (cx+BORDER+BORDER);
				if(oldrect.Height()!=cy+BORDER+BORDER && cy!=0)
					SetHeight (cy+BORDER+BORDER);
				}catch(...){}
			}
			if(GetParentFrame()->GetStyle() & WS_MAXIMIZE)
				g_bMax = 1;
		}
		//size other views
		if(g_bMax == 3)
			ResizeSibling(this);
	}catch(...){}
}

void CGreenBrowserView::OnUpdateViewFonts(CCmdUI* pCmdUI) 
{ 
	// TODO: Add your command update UI handler code here
	if(pCmdUI->m_nID==ID_VIEW_FONTS_LARGEST+m_nFontSize)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CGreenBrowserView::ToNavigate(LPCTSTR URL, DWORD dwFlags, LPCTSTR lpszTargetFrameName, LPCTSTR lpszHeaders, LPVOID lpvPostData, DWORD dwPostDataLen, BOOL bOnlyRefresh)
{
	try {
		CString str = URL, str2 = URL;
		m_lpszUrl = CString(str);
		m_bFirst = TRUE;

		if (m_bEnWebProxy) {
			if (m_strWebProxy.IsEmpty() && pmf != NULL) {
				m_strWebProxy = pmf->m_strDefaultWebProxy;
			}

			if ((str.Left(4) == "http" || str.Left(4) == "www.") && str.Find(m_strWebProxy) < 0) {
				if (str.Left(4) == "www.") {
					str2 = "http://" + str;
				}

				if (m_strWebProxy.Find("%s") > 0) {
					str2.Replace(":", "%3A");
					str2.Replace("/", "%2F");
					str = m_strWebProxy;
					str.Replace("%s", str2);
				} else if (m_strWebProxy.Find("%S") > 0) {
					str = m_strWebProxy;
					str.Replace("%S", str2);
				} else {
					str = m_strWebProxy + str2;
				}

				Navigate2(str, dwFlags, lpszTargetFrameName, lpszHeaders, lpvPostData, dwPostDataLen);
				return;
			}
		}


		if (!bOnlyRefresh) {
			Navigate2(str, dwFlags, lpszTargetFrameName, lpszHeaders, lpvPostData, dwPostDataLen);
		}
	} catch (...) {}

	m_nProgress = -1;
}


void CGreenBrowserView::OnParentNotify(UINT message, LPARAM lParam) 
{
	CFixedHtmlView::OnParentNotify(message, lParam);
	
	// TODO: Add your message handler code here
	try{
	CChildFrame* tcf = (CChildFrame*)GetParentFrame();
	//
	pmf->m_ptMouseDown.x = LOWORD((DWORD)lParam);
	pmf->m_ptMouseDown.y = HIWORD((DWORD)lParam);
	//
	if (message==WM_LBUTTONDOWN && pmf->m_nLButtonDown)
	{
		if (MouseAction(pmf->m_nLButtonDown))
		{
			if (pmf->m_nMButtonDown>=ID_OPEN_LINK && pmf->m_nMButtonDown<=ID_OPEN_LINK_DEACTIVE)
			{
				//for that simu as shift+click 
				QueryPerformanceCounter(&m_tLastClick);
				m_bLClick = TRUE;
			}
			return;
		}
	}
	else if (message==WM_RBUTTONDOWN && pmf->m_nRButtonDown)
	{
		if (MouseAction(pmf->m_nRButtonDown))
		{
			if (pmf->m_nMButtonDown>=ID_OPEN_LINK && pmf->m_nMButtonDown<=ID_OPEN_LINK_DEACTIVE)
			{
				//for that simu as shift+click 
				QueryPerformanceCounter(&m_tLastClick);
				m_bLClick = TRUE;
			}
			SetCapture();
			m_bIsCapture = TRUE;
			pmf->m_bPopRightMenu=FALSE;
			return;
		}
	}
	//
	if((message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN) && (g_bLinkInNewWindow || tcf->m_bLock || g_bKillAutoWin) )
	{
		if(message == WM_LBUTTONDOWN )
		{
			QueryPerformanceCounter(&m_tLastClick);
			m_bRClick = FALSE;
			m_bLClick = TRUE;
			m_ptLClick.x = LOWORD((DWORD)lParam);
			m_ptLClick.y = HIWORD((DWORD)lParam);
		}
		else if(message == WM_RBUTTONDOWN )
		{
			QueryPerformanceCounter(&m_tLastRClick);
			m_bRClick = TRUE;
			m_bLClick = FALSE;
		}
		else
			m_bRClick = FALSE;
	}
	if  ( (message==WM_RBUTTONDOWN) && 
		  (pmf->m_bRMouseDrag || pmf->m_bRMouseGesture)
		)
	{
		SetCapture();
		m_bIsCapture = TRUE;
		//m_bRClick = FALSE;
		m_ptRClick.x = LOWORD((DWORD)lParam);
		m_ptRClick.y = HIWORD((DWORD)lParam);
		m_ptPrev = m_ptRClick;
		g_nGestureHead = 0;
		g_nGestureTail = 0;
		pmf->m_bPopRightMenu=TRUE;
	}
	}catch(...){}
}


void CGreenBrowserView::OnToolsAutoscrollAutoscrollingafterdownloading() 
{
	// TODO: Add your command handler code here
	m_bScrollAfterDL = !m_bScrollAfterDL;

	if(pmf!=NULL)
		pmf->m_bDefScrollAfterDL = m_bScrollAfterDL;
}

void CGreenBrowserView::OnUpdateToolsAutoscrollAutoscrollingafterdownloading(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bScrollAfterDL);
}

void CGreenBrowserView::OnUpdateToolsAutoscrollSpeed(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(pmf->m_bMouseCtrlScrSpeed)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable();

	if(pCmdUI->m_nID == ID_TOOLS_AUTOSCROLL_FAST)
		pCmdUI->SetCheck(m_nSrSpeed==3);
	else if(pCmdUI->m_nID == ID_TOOLS_AUTOSCROLL_MEDIUM)
		pCmdUI->SetCheck(m_nSrSpeed == 2);
	else
		pCmdUI->SetCheck(m_nSrSpeed == 1);
}

void CGreenBrowserView::OnToolsAutoscrollUseautoscrolling() 
{
	// TODO: Add your command handler code here
	m_bUseAutoScroll = !m_bUseAutoScroll;
	if(pmf!=NULL)
		pmf->m_bDefUseAutoScroll = m_bUseAutoScroll;

	//start to scroll
	if(!m_nScrollTimer && (m_bUseAutoScroll || pmf->m_bAllScroll) && m_nSrSpeed!=0)
	{
		if(m_nProgress==100 || m_nProgress<0 || (!m_bScrollAfterDL && m_nProgress>=0))
		{
			if(pmf->m_bMouseCtrlScrSpeed)
				m_nLastScrSpd = STOP_TIMER;
			else
				m_nLastScrSpd = (UINT)((g_fSeed*100)/(m_nSrSpeed*m_nSrSpeed));
			m_nScrollTimer = SetTimer(2132, m_nLastScrSpd, NULL);
		}
	}
	else if(!(m_bUseAutoScroll || pmf->m_bAllScroll) && m_nScrollTimer)
	{
		if(m_nScrollTimer)
			KillTimer(m_nScrollTimer);
		m_nScrollTimer = 0;
	}
}

void CGreenBrowserView::OnUpdateToolsAutoscrollUseautoscrolling(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	try{
		pmf->ToolBarSetCheck(pCmdUI, m_bUseAutoScroll);
	}catch(...){}
}

void CGreenBrowserView::ChangeSpeed(int nSpeed)
{
	if(m_nScrollTimer)
	{
		KillTimer(m_nScrollTimer);
		m_nScrollTimer = 0;
	}

	//start to scroll
	if(!m_nScrollTimer && (m_bUseAutoScroll || pmf->m_bAllScroll) && nSpeed!=0)
	{
		if(m_nProgress==100 || m_nProgress<0 || (!m_bScrollAfterDL && m_nProgress>=0))
		{
			if(pmf->m_bMouseCtrlScrSpeed)
				m_nLastScrSpd = nSpeed;
			else
				m_nLastScrSpd = (UINT)((g_fSeed*100)/(m_nSrSpeed*m_nSrSpeed));
			m_nScrollTimer = SetTimer(2132, m_nLastScrSpd, NULL);
		}
	}
}

void CGreenBrowserView::WBScroll()
{
	CGreenBrowserView* pvw = this;
	if(pvw->m_bToClose)
		return;

	LPDISPATCH pDisp = NULL;
	IHTMLDocument2 *pHTMLDoc = NULL;
	IHTMLWindow2 *pHTMLWnd = NULL;

	try{
	pDisp = pvw->GetHtmlDocument();
	if( pDisp )
	{
		if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDoc)))
		{
			if(SUCCEEDED(pHTMLDoc->get_parentWindow( &pHTMLWnd )))
			{
				if (pmf->m_nAutoScrollPage)
				{
					CRect	rect;
					int	blockHeight;
					pvw->m_wndBrowser.GetWindowRect(&rect);
					blockHeight = (rect.bottom - rect.top) / pmf->m_nAutoScrollPage;
					m_nScrollSize++;
					if (m_nScrollSize>=blockHeight)
					{
						while(m_nScrollSize>0)
						{
							pHTMLWnd->scrollBy(0, 2);
							Sleep(2);
							m_nScrollSize -= 2;
						}
						m_nScrollSize=0;
					}
				}
				else
					pHTMLWnd->scrollBy(0, 1);
			}
		}
	}
	}catch(...){}

	try{
	if(pHTMLWnd!=NULL)	pHTMLWnd->Release();
	if(pHTMLDoc!=NULL)	pHTMLDoc->Release();
	if(pDisp!=NULL)		pDisp->Release();
	}catch(...){}
}

//scroll horizontal page
void CGreenBrowserView::WBScrollHPage(int bLeft)
{
	CGreenBrowserView* pvw = this;
	if(pvw->m_bToClose)
		return;

	LPDISPATCH pDisp = NULL;
	IHTMLDocument2 *pHTMLDoc = NULL;
	IHTMLWindow2 *pHTMLWnd = NULL;

	try{
	pDisp = pvw->GetHtmlDocument();
	if( pDisp )
	{
		if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDoc)))
		{
			if(SUCCEEDED(pHTMLDoc->get_parentWindow( &pHTMLWnd )))
			{
				CRect	rect;
				pvw->m_wndBrowser.GetWindowRect(&rect);
				if (bLeft)
					pHTMLWnd->scrollBy(-(long)(rect.Width()*0.87),0);
				else
					pHTMLWnd->scrollBy((long)(rect.Width()*0.87),0);
			}
		}
	}
	}catch(...){}

	try{
	if(pHTMLWnd!=NULL)	pHTMLWnd->Release();
	if(pHTMLDoc!=NULL)	pHTMLDoc->Release();
	if(pDisp!=NULL)		pDisp->Release();
	}catch(...){}
}

void CGreenBrowserView::WBScrollBy(long xpos,long ypos)
{
	CGreenBrowserView* pvw = this;
	if(pvw->m_bToClose)
		return;

	LPDISPATCH pDisp = NULL;
	IHTMLDocument2 *pHTMLDoc = NULL;
	IHTMLWindow2 *pHTMLWnd = NULL;

	try{
	pDisp = pvw->GetHtmlDocument();
	if( pDisp )
	{
		if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDoc)))
		{
			if(SUCCEEDED(pHTMLDoc->get_parentWindow( &pHTMLWnd )))
				pHTMLWnd->scrollBy(xpos,ypos);
		}
	}
	}catch(...){}

	try{
	if(pHTMLWnd!=NULL)	pHTMLWnd->Release();
	if(pHTMLDoc!=NULL)	pHTMLDoc->Release();
	if(pDisp!=NULL)		pDisp->Release();
	}catch(...){}
}

void CGreenBrowserView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	try{
	if(nIDEvent == m_nScrollTimer && (m_bUseAutoScroll || pmf->m_bAllScroll))
	{
		if(m_nLastScrSpd<STOP_TIMER)
			WBScroll();
		//update scroll speed
		if(pmf->m_bMouseCtrlScrSpeed)
		{
			CPoint mousept;
			GetCursorPos(&mousept);
			ScreenToClient(&mousept);
			CRect rect;
			GetWindowRect(&rect);
			if(mousept.x<=rect.Width() && mousept.x>=rect.Width()-pmf->m_nScrollWidth)
			{
				if(mousept.y<=rect.Height()-pmf->m_nScrollArrowH && mousept.y>0)
				{
					int sp = 100 - mousept.y*100/(rect.Height()-pmf->m_nScrollArrowH)+1;
					if(abs(sp - (int)m_nLastScrSpd) > 5)
						ChangeSpeed(sp);
				}
				else
					ChangeSpeed(STOP_TIMER);
			}
			else
				ChangeSpeed(STOP_TIMER);
		}

	}
	else if(nIDEvent == m_nAutoRefreshTimer && m_bUseAutoRef)
	{
		if (!m_bIsActive || pmf->m_bAutoRefreshActive)
			PowerRefresh();
	}
	CFixedHtmlView::OnTimer(nIDEvent);
	}catch(...){}
}

void CGreenBrowserView::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	try{
	CFixedHtmlView::OnClose();
	}catch(...){}
}

void CGreenBrowserView::ChangeProperty()
{
	try{
	m_dwPropertyDefault = m_dwProperty;
	IOleControl* poc;
	LPDISPATCH pDisp = GetHtmlDocument();
	if(pDisp != NULL)
	{
		if(SUCCEEDED(pDisp->QueryInterface(IID_IOleControl, (void**)&poc)))
		{
			poc->OnAmbientPropertyChange(DISPID_AMBIENT_DLCONTROL);
			poc->Release();
		}
		pDisp->Release();
	}
	}catch(...){}
}

void CGreenBrowserView::OnToolsAutoref() 
{
	// TODO: Add your command handler code here
	m_bUseAutoRef = !m_bUseAutoRef;

	//start to scroll
	if(!m_nAutoRefreshTimer && m_bUseAutoRef && m_nRefreshInterval!=0)
	{
		m_nAutoRefreshTimer = SetTimer(3132+rand()/500, (UINT)(m_nRefreshInterval*1000), NULL);
	}
	else if(!m_bUseAutoRef && m_nAutoRefreshTimer)
	{
		if(m_nAutoRefreshTimer)
			KillTimer(m_nAutoRefreshTimer);
		m_nAutoRefreshTimer = 0;
	}	
}

void CGreenBrowserView::OnUpdateToolsAutoref(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bUseAutoRef);
}

void CGreenBrowserView::OnToolsReffre() 
{
	// TODO: Add your command handler code here
	CSetRefIntervalDlg dlg;
	dlg.m_nRefInt = m_nRefreshInterval;
	if(dlg.DoModal() == IDOK)
	{
		m_nRefreshInterval = dlg.m_nRefInt;
		pmf->m_nDefRefreshInterval = m_nRefreshInterval;
		//change timer
		if(m_bUseAutoRef)
			ChangeRefresh();
	}
}

void CGreenBrowserView::ChangeRefresh()
{
	if(m_nAutoRefreshTimer)
	{		//stop the timer;
		KillTimer(m_nAutoRefreshTimer);
		m_nAutoRefreshTimer = 0;
	}
	//start to scroll
	if(!m_nAutoRefreshTimer && m_bUseAutoRef && m_nRefreshInterval!=0)
	{
		m_nAutoRefreshTimer = SetTimer(3132, (UINT)(m_nRefreshInterval*1000), NULL);
	}
}

void CGreenBrowserView::OnNavigateComplete2(LPCTSTR lpszURL)
{
	try
	{
		CString strUrl;
		CChildFrame* tcf = ((CChildFrame*)GetParentFrame());
		GetLocationURL(strUrl);
		
		if(!m_bToClose && ((m_bIsPopUp && m_bIsNewPopup) || (tcf!=NULL && !tcf->IsWindowVisible())) && !strUrl.IsEmpty())
		{
			CString url2 = strUrl.Right(m_strPossibleURL.GetLength()+13);
			if((pmf->IsWindowVisible() && tcf!=NULL && !tcf->IsWindowVisible()) || url2 == "navcancl.htm#" + m_strPossibleURL /*for IE 5*/|| strUrl == "about:NavigationCanceled" /*for IE4*/ || strUrl==m_strPossibleURL)
			{
				if(tcf!=NULL) 
				{
					tcf->PostMessage(WM_CLOSE);
					m_bToClose = TRUE;
				}
				m_bIsPopUp = FALSE;
			}
		}
		//
		try
		{	
			//security flag
			if(!m_bToClose)
			{
				g_bVerbose = TRUE;
				if(strUrl.Left(5)=="https")
				{
					LPDISPATCH pDisp = NULL;
					IHTMLDocument2 *pHTMLDoc = NULL;
					BSTR p;
					
					try
					{
						pDisp = GetHtmlDocument();
						if( pDisp )
							pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDoc);
						else
							m_bIsSecure = FALSE;
						if(pHTMLDoc)
						{
							if(SUCCEEDED(pHTMLDoc->get_protocol( &p )))
							{
								CString s(p);
								if(s=="HyperText Transfer Protocol with Privacy")
									m_bIsSecure = TRUE;
								else
									m_bIsSecure = FALSE;
								SysFreeString(p);
							}
						}
						if(m_bIsActive && pmf!=NULL)
							pmf->m_bIsSecure = m_bIsSecure;
					}catch(...){}
					
					try
					{
						if(pHTMLDoc!=NULL)
							pHTMLDoc->Release();
						if(pDisp!=NULL)
							pDisp->Release();
					}catch(...){}
				}
			}
		}catch(...){}
	}catch(...){}
}

void CGreenBrowserView::OnFileSaveAs()
{
	// TODO: Add your command handler code here
	ExecWB(OLECMDID_SAVEAS, OLECMDEXECOPT_DODEFAULT,
		   NULL, NULL);
}

void CGreenBrowserView::OnFileSavehtml() 
{
	// TODO: Add your command handler code here
	ExecWB(OLECMDID_SAVEAS, OLECMDEXECOPT_DONTPROMPTUSER,
		   NULL, NULL);
}

void CGreenBrowserView::OnFilePrintSetup() 
{
	// TODO: Add your command handler code here
	ExecCommand(OLECMDID_PAGESETUP);
}

void CGreenBrowserView::OnFileProperty() 
{
	// TODO: Add your command handler code here
	ExecCommand(OLECMDID_PROPERTIES);
}

void CGreenBrowserView::ExecCommand(DWORD nCmdID)
{
	LPOLECOMMANDTARGET lpTarget = NULL;
	LPDISPATCH lpDisp = GetHtmlDocument();

	if (lpDisp != NULL)
	{
		// the control will handle all printing UI
		if (SUCCEEDED(lpDisp->QueryInterface(IID_IOleCommandTarget,
				(LPVOID*) &lpTarget)))
		{
			lpTarget->Exec(NULL, nCmdID, 0, NULL, NULL);
			lpTarget->Release();
		}
		lpDisp->Release();
	}	
}

void CGreenBrowserView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	try{

	if(m_bIsCapture)
	{
		int i =0;
		m_bIsCapture = FALSE;
		ReleaseCapture();
		//if have right button down action, then not pop menu
		if (pmf->m_nRButtonDown!=0)
			goto _PopRightMenu;
		//if mouse drag and not move then need pop menu
		if (pmf->m_bRMouseDrag)
			goto _PopRightMenu;

		g_cGestureString[0]= '\0';
		while(g_nGestureTail != g_nGestureHead)
		{
			g_cGestureString[i] = g_cGestureStack[g_nGestureHead];
			i++;
			g_cGestureString[i] = '\0';
			g_nGestureHead = (g_nGestureHead +1)%MG_LEN;		
		}

		if(i>0)
		{
			int command = pmf->m_GestureList.GetCmdFromName(g_cGestureString);
			if (command)
				MouseAction(command);
		}

_PopRightMenu:		
		if(i==0 && pmf->m_bPopRightMenu)
		{
			//popup right menu;
			POINT p2 = point;
			ClientToScreen(&p2);
			CWnd *pWnd = WindowFromPoint(p2);
			if(pWnd)
			{
				pWnd->ScreenToClient(&p2);
				pWnd->PostMessage(WM_RBUTTONUP, nFlags, MAKELONG(p2.x, p2.y));
			}
		}
	}	
	}catch(...){}

	CFixedHtmlView::OnRButtonUp(nFlags, point);
}

void CGreenBrowserView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	try{
	//if right button down, then do nothing
	if(m_bIsCapture && pmf->m_nRButtonDown==0)
	{
		//if drag page then not mouse action
		if (pmf->m_bRMouseDrag)//drag page
		{
			if (m_ptPrev.x != point.x || m_ptPrev.y != point.y)
			{
				WBScrollBy(m_ptPrev.x - point.x, m_ptPrev.y - point.y);
				m_ptPrev = point;
				pmf->m_bPopRightMenu = FALSE;
				return;
			}
		}
		if (pmf->m_bRMouseGesture && nFlags == MK_RBUTTON )
		{
			char cDirection;
			if(MoveDirection(point, &cDirection))
			{
				PushMouseGesture(cDirection);
				m_ptRClick = point;
			}
		}
	}
	}catch(...){}

	CFixedHtmlView::OnMouseMove(nFlags, point);
}

void CGreenBrowserView::PushMouseGesture(char cDirection)
{
	if(g_nGestureTail!=0 || g_nGestureHead !=0)
	{
		int pre = (g_nGestureTail -1 + MG_LEN)%MG_LEN;		
		if(g_cGestureStack[pre] == cDirection)
			return;
	}

	g_cGestureStack[g_nGestureTail] = cDirection;
	g_nGestureTail = (g_nGestureTail+1)%MG_LEN;
	if(g_nGestureHead == g_nGestureTail)
		g_nGestureHead = (g_nGestureHead + 1)%MG_LEN;

	//show gesture 
	if (pmf->m_bShowGesture)
	{
		int i=0;
		g_cGestureString[0]= '\0';
		int iMouseGS = g_nGestureHead;//not change g_nGestureHead
		while(g_nGestureTail != iMouseGS)
		{
			g_cGestureString[i] = g_cGestureStack[iMouseGS];
			i++;
			g_cGestureString[i] = '\0';
			iMouseGS = (iMouseGS +1)%MG_LEN;		
		}
		if(i>0)
		{
			CString strName;
			CString strMsg;
			LOADSTR(strMsg,IDS_GESTURE);
			strMsg += ": ";
			strMsg += g_cGestureString;
			if (theApp.m_bUseLngFile)
			{
				CString strUp,strDown,strLeft,strRight;
				//
				LOADSTR(strUp,IDS_UP);
				LOADSTR(strDown,IDS_DOWN);
				LOADSTR(strLeft,IDS_LEFT);
				LOADSTR(strRight,IDS_RIGHT);
				//
				strMsg.Replace("U",strUp);
				strMsg.Replace("D",strDown);
				strMsg.Replace("L",strLeft);
				strMsg.Replace("R",strRight);
			}
			strMsg += " => ";
			int command = pmf->m_GestureList.GetCmdFromName(g_cGestureString);
			_GetMenuItemString(strName,command);
			strMsg += strName;
			pmf->SetMessageText(strMsg);
		}
	}
}

BOOL CGreenBrowserView::MoveDirection(CPoint &point, char *Direction)
{
	int x = point.x - m_ptRClick.x;
	int y = point.y - m_ptRClick.y;
	int dist = x*x+y*y;
	if(dist>64)
	{
		if(x>abs(y) && x>0)
			*Direction = 'R';//right
		else if(abs(x)>abs(y) && x<0)
			*Direction = 'L';//left
		else if(y>abs(x) && y>0)
			*Direction = 'D';//down
		else if(abs(y)>abs(x) && y<0)
			*Direction = 'U';//up
		else
			return FALSE;
		return TRUE;
	}
	else
		return FALSE;
}

CString CGreenBrowserView::GetSortSaveFileName(CString &ext)
{
	CString filename;
	int l;

	GetLocationURL(filename);
	//
	filename.Replace('/', '\\');
	if(filename.Right(1) == "\\")
		filename = filename.Left(filename.GetLength()-1);

	l = filename.ReverseFind('\\');
	filename = filename.Mid(l+1);
	filename.Replace(':', '_');
	l = filename.Find('?');
	if(l>0)
		filename = filename.Left(l);
	l = filename.ReverseFind('.');
	if(l>0)
	{
		ext = filename.Right(filename.GetLength()-l);
		ext.MakeLower();
		filename = filename.Left(l);
	}
	if(ext != ".htm" && ext != ".html" && ext !=".shtml" && !pmf->IsImageType(ext))
	{
		ext = ".htm";
	}
	if (pmf->m_bNameByTitle)
	{
		CString strName;
		CChildFrame* tcf = ((CChildFrame*)GetParentFrame());
		GET_TAB_TITLE(tcf,strName);
		GET_URL_NAME(strName)
		REPLACE_ERR_PATH_CHAR(strName)
		int len = strName.GetLength();
		if (len)
		{
			if (len>FN_LEN)//cut to <len
			{
				strName = strName.Left(FN_LEN);
				if (strName.GetAt(FN_LEN-1) & 0x80)
				{
					int count=0;
					for (int i=0;i<FN_LEN;i++)
					{
						if (strName.GetAt(i) & 0x80)
							count++;
						else
							count = 0;
					}
					if (count%2)
						strName = strName.Left(FN_LEN-1);
				}
				strName += "_";
			}
			filename = strName;
		}
		// bag.jpg => bag
		if (filename.Right(ext.GetLength()) == ext)
		{
			filename = filename.Left(filename.GetLength()-ext.GetLength());
		}
	}

	return filename + ext;
}

void CGreenBrowserView::OnFileAutoSave() 
{
	// TODO: Add your command handler code here
	try{		
	IHTMLDocument2 * pDoc = NULL;
	IPersistFile *pPersistFile = NULL;
	CString filename, ext, htmlname,strNewFileName;
	int l;
	BSTR bfilename;
	char tmp[4]; 
	GetLocationURL(filename);
	htmlname.Empty();
	//
	strNewFileName = GetSortSaveFileName(ext);
	CString strSortSaveFolder;
	strSortSaveFolder.Empty();
	if (m_bSortSave)
	{
		m_bSortSave = FALSE;
		//not show the dlg
		if ( pmf->m_bShowSortSaveDlg && PRESS(VK_MENU) ||
			!pmf->m_bShowSortSaveDlg && !PRESS(VK_MENU))
		{
			if (pmf->m_strSortSaveFolder.GetLength())
			{
				strSortSaveFolder = pmf->m_strSortSaveFolder;
				goto _BeforeSave;
			}
		}
		//
		CSortSaveDlg dlg;
		CChildFrame* tcf = ((CChildFrame*)GetParentFrame());
		GET_TAB_TITLE(tcf,dlg.m_strTitle);
		//
		dlg.m_bType = pmf->m_bNameByTitle;
		dlg.m_strUrl = filename;
		dlg.m_strFileName = strNewFileName;
		dlg.m_strSortSaveFolder = pmf->m_strSortSaveFolder;
		if(dlg.DoModal()==IDOK)
		{
			strNewFileName = dlg.m_strFileName;
			REPLACE_ERR_PATH_CHAR(strNewFileName)
				pmf->m_strSortSaveFolder = strSortSaveFolder = dlg.m_strSortSaveFolder;
		}
		else
			return ;
	}
	else if (pmf->m_bSavePageByDate)
	{
		CString strTime;
		SYSTEMTIME time;
		::GetLocalTime(&time);
		strTime.Format("%04d_%02d_%02d",time.wYear,time.wMonth,time.wDay);
		strSortSaveFolder = strTime;
	}
_BeforeSave:
	//############################################################
	//select save path
	if (strSortSaveFolder.GetLength())
	{
		pmf->m_strAutoSavePath = theApp.m_strDefaultDirSort;
		pmf->m_strAutoSavePath += strSortSaveFolder;
		pmf->m_strAutoSavePath += "\\";
	}
	else
		pmf->m_strAutoSavePath = theApp.m_strDefaultDir;
	//
	if(pmf->m_bCateWithSite)
	{
		CString site;
		GetLocationURL(site);
		pmf->GetSiteName(site);
		//
		pmf->m_strAutoSavePath += site;
		pmf->m_strAutoSavePath += "\\";
	}
	//#############################################################
	//make sure the dir exist
	_CreateFullDir(pmf->m_strAutoSavePath);
	
	//#############################################################
	//before save
	if(	pmf->m_nAutoSave == 0 || 
		pmf->m_nAutoSave == 1 || 
		pmf->m_nAutoSave == 4 || 
		pmf->m_nAutoSave == 5 || 
		(pmf->m_nAutoSave == 3 && filename.Left(5)=="file:")
		)
	{
		keybd_event(VK_TAB, MapVirtualKey(VK_TAB, 0), 0, 0);
		if(pmf->m_nAutoSave==1)
		{
			keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), 0, 0);
			keybd_event('A', MapVirtualKey('A', 0), 0, 0);
			keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), 0, 0);
			keybd_event('A', MapVirtualKey('A', 0), 0, 0);
			keybd_event(VK_RETURN, MapVirtualKey(VK_RETURN, 0), 0, 0);
			keybd_event('A', MapVirtualKey('A', 0), 0, 0);
		}
		else if(pmf->m_nAutoSave==4)
		{
			keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), 0, 0);
			keybd_event('A', MapVirtualKey('A', 0), 0, 0);
			keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), 0, 0);
			keybd_event('A', MapVirtualKey('A', 0), 0, 0);
			keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), 0, 0);
			keybd_event('A', MapVirtualKey('A', 0), 0, 0);
			keybd_event(VK_RETURN, MapVirtualKey(VK_RETURN, 0), 0, 0);
			keybd_event('A', MapVirtualKey('A', 0), 0, 0);
		}
		else if(pmf->m_nAutoSave==5)
		{
			keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), 0, 0);
			keybd_event('A', MapVirtualKey('A', 0), 0, 0);
			keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), 0, 0);
			keybd_event('A', MapVirtualKey('A', 0), 0, 0);
			keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), 0, 0);
			keybd_event('A', MapVirtualKey('A', 0), 0, 0);
			keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), 0, 0);
			keybd_event('A', MapVirtualKey('A', 0), 0, 0);
			keybd_event(VK_RETURN, MapVirtualKey(VK_RETURN, 0), 0, 0);
			keybd_event('A', MapVirtualKey('A', 0), 0, 0);
		}
		keybd_event(VK_RETURN, MapVirtualKey(VK_RETURN, 0), 0, 0);
		//set save directory
		HKEY hKey;
		if(RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Main"), &hKey) == ERROR_SUCCESS)
		{
			RegSetValueEx(hKey, _T("Save Directory"), 0, REG_SZ, 
				(LPBYTE)(LPCTSTR)pmf->m_strAutoSavePath,
				pmf->m_strAutoSavePath.GetLength()
				);
		}
		SendMessage(WM_COMMAND, ID_FILE_SAVEAS, 0);
	}
	else if(pmf->m_nAutoSave == 2)
	{
		filename = strNewFileName;
		l = filename.ReverseFind('.');
		if(l>0)
		{
			ext = filename.Right(filename.GetLength()-l);
			ext.MakeLower();
			filename = filename.Left(l);
		}
		filename = pmf->m_strAutoSavePath + filename;
		//
		l = 0;
		HFILE hf; OFSTRUCT of;
		htmlname = filename + ext;
		hf = OpenFile(htmlname, &of, OF_EXIST);
		while(hf!=HFILE_ERROR)
		{
			l++;
			htmlname = filename;
			htmlname += '[';
			itoa(l, tmp, 10);
			htmlname += tmp;
			htmlname += ']';
			htmlname += ext;
			hf = OpenFile(htmlname, &of, OF_EXIST);
		}
		if(!pmf->IsImageType(ext))
		{
			pDoc = (IHTMLDocument2*)GetHtmlDocument();
			bfilename = htmlname.AllocSysString();
			if (SUCCEEDED(pDoc->QueryInterface(IID_IPersistFile, (void**)&pPersistFile)))
				pPersistFile->Save(bfilename, FALSE);
			
			try{
				SysFreeString(bfilename);
				if(pPersistFile!=NULL)
					pPersistFile->Release();
				if(pDoc != NULL)
					pDoc->Release();
			}catch(...){}
		}
		else
		{
			//save image
			GetLocationURL(filename);
			DWORD dwEntrySize=0;
			LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry;
			if (!GetUrlCacheEntryInfo(filename,NULL,&dwEntrySize))
			{
				
				if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
				{
                    
					lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) 
						new char[dwEntrySize];
					
					if (GetUrlCacheEntryInfo(filename,lpCacheEntry,&dwEntrySize))
					{
						CopyFile(lpCacheEntry->lpszLocalFileName, htmlname, FALSE);
					}
					delete lpCacheEntry;
					lpCacheEntry = NULL;
					dwEntrySize = 0;
				}
				else
				{
					//local image files
					pmf->DecodeEscap(filename);
					pmf->DecodeEscap(htmlname);
					if(filename.Left(8)=="file:///")
						filename = filename.Mid(8);
					CopyFile(filename, htmlname, FALSE);
				}
			}
		}
	}
	else if(pmf->m_nAutoSave == 3)
	{
		pmf->m_bIsAutoSave = TRUE;
		pmf->m_pCurView = this;
		pmf->SendMessage(WM_COMMAND, ID_FILE_QUICKSAVE,0);
		pmf->m_bIsAutoSave = FALSE;
	}
	
	l = pmf->FindTab((CChildFrame*)GetParentFrame());
	pmf->SetTabIcon(TI_SAVE, l, this);
	if(pmf->m_nTabStyle==2)
		pmf->PostMessage(WM_UPDATE_TAB);
	
	CString strMsg;
	LOADSTR(strMsg ,IDS_PAGE_SAVED);
	if (htmlname.GetLength()>0)
		strMsg+= htmlname;
	else
		strMsg+= pmf->m_strAutoSavePath;
	//add local url
	strMsg += " <= ";
	strMsg += m_lpszUrl;
	pmf->AddToCollector(strMsg,12);
	}catch(...){}	
}

void CGreenBrowserView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bIsCapture)
	{
		m_bIsCapture = FALSE;
		ReleaseCapture();
	}
	CFixedHtmlView::OnLButtonUp(nFlags, point);
}

int CGreenBrowserView::MouseAction(int kind) 
{
	if (kind)
	{
		g_bRTab = FALSE;
		//special
		if (kind==ID_OPEN_LINK)
			return OpenLinkUnderMouse(INW_NORMAL);
		else if (kind==ID_OPEN_LINK_ACTIVE)
			return OpenLinkUnderMouse(INW_ACTIVE);
		else if (kind==ID_OPEN_LINK_DEACTIVE)
			return OpenLinkUnderMouse(INW_INACTIVE);
		else
			pmf->PostMessage(WM_COMMAND, kind, 0);
	}
	return TRUE;
}

CString CGreenBrowserView::GetPointUrl(POINT pt)
{
	CString strUrl;
	strUrl.Empty();

	LPDISPATCH pDisp; 
	IHTMLDocument2* pHTMLDoc=NULL;
	IHTMLElement* pElem = NULL;
	IHTMLElement* pElemParent = NULL;

	try{
	pDisp = GetHtmlDocument();
	if(pDisp!=NULL)
	{
		if(SUCCEEDED(pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDoc)))
		{
			if(pHTMLDoc!=NULL)
			{
				if(SUCCEEDED(pHTMLDoc->elementFromPoint(pt.x, pt.y, &pElem)))
				{
					BSTR bstrSrc;
					while(pElem)
					{
						pElem->get_tagName(&bstrSrc);
							strUrl = bstrSrc;
						SysFreeString(bstrSrc);
						if (strUrl!="A")
						{
							pElem->get_parentElement(&pElemParent);
							pElem->Release();
							pElem = pElemParent;
						}
						else
						{
							pElem->toString(&bstrSrc);
							strUrl=bstrSrc;
							SysFreeString(bstrSrc);
							break;
						}
					}
				}
			}
		}
	}
	}catch(...){}

	try{
	RELEASE(pElem)
	RELEASE(pHTMLDoc)
	RELEASE(pDisp)
	}catch(...){}

	return strUrl;
}

int CGreenBrowserView::OpenLinkUnderMouse(int nActive)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CString strUrl = GetPointUrl(pt);
	//
	if (IS_URL(strUrl) || IS_SCRIPT_MAILTO(strUrl))
	{
		pmf->m_ptMouseDown = pt;
		pmf->ClickPoint(nActive);
		return TRUE;
	}
	return FALSE;
}

void CGreenBrowserView::VScrollPageUp()
{
	this->PostMessage(WM_KEYDOWN,VK_PRIOR,1);
}

void CGreenBrowserView::VScrollPageDown()
{
	this->PostMessage(WM_KEYDOWN,VK_NEXT,1);
}

void CGreenBrowserView::OnUpdateToolsAutoscrollPage(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	switch(pCmdUI->m_nID)
	{
	case ID_TOOLS_AUTOSCROLL_PAGE0:
		pCmdUI->SetCheck(pmf->m_nAutoScrollPage==0);
		break;
	case ID_TOOLS_AUTOSCROLL_PAGE1:
		pCmdUI->SetCheck(pmf->m_nAutoScrollPage==1);
		break;
	case ID_TOOLS_AUTOSCROLL_PAGE2:
		pCmdUI->SetCheck(pmf->m_nAutoScrollPage==2);
		break;
	case ID_TOOLS_AUTOSCROLL_PAGE4:
		pCmdUI->SetCheck(pmf->m_nAutoScrollPage==4);
		break;
	}
}

void CGreenBrowserView::PowerRefresh()
{
	CChildFrame* tcf = ((CChildFrame*)GetParentFrame());
	if (m_lpszUrl == "about:blank")
		return;
	int nTabID = pmf->FindTab(tcf);
	if (m_nOldIcon==-1 && pmf->m_nTabStyle==2)
		pmf->PostMessage(WM_UPDATE_TAB);
	pmf->SetTabIcon( TI_1, nTabID, this );
	if (IS_RES_URL(m_lpszUrl))
	{
		CString strNewUrl;// not use m_lpszUrl; user may modified the url
		pmf->m_wndAddress->GetEditCtrl()->GetWindowText(strNewUrl);
		DO_RES_URL(strNewUrl);
		CChildFrame* tcf = (CChildFrame*)pmf->MDIGetActive();
		if (tcf && tcf->m_pView == this)
			pmf->m_wndAddress->GetEditCtrl()->SetWindowText(strNewUrl);
		Navigate2(strNewUrl, 0, NULL);
		return;
	}
	//for http://www.ror.cn refresh
	m_bForbidAutoNavigate = 0;
	Refresh2(REFRESH_NORMAL);//Refresh2(REFRESH_COMPLETELY);
}

void CGreenBrowserView::OnViewPagesize50() 
{
	// TODO: Add your command handler code here
	pmf->m_bShowScriptResult = false;
	pmf->RunScript("<script language=\"JavaScript\"> document.body.style.zoom=\"50%\"; </script>");
	pmf->m_bShowScriptResult = true;
}

void CGreenBrowserView::OnViewPagesize100() 
{
	// TODO: Add your command handler code here
	pmf->m_bShowScriptResult = false;
	pmf->RunScript("<script language=\"JavaScript\"> document.body.style.zoom=\"100%\"; </script>");
	pmf->m_bShowScriptResult = true;
}

void CGreenBrowserView::OnViewPagesize200() 
{
	// TODO: Add your command handler code here
	pmf->m_bShowScriptResult = false;
	pmf->RunScript("<script language=\"JavaScript\"> document.body.style.zoom=\"150%\"; </script>");
	pmf->m_bShowScriptResult = true;
}

void CGreenBrowserView::OnViewPagesizeAdd10() 
{
	// TODO: Add your command handler code here
	pmf->m_bShowScriptResult = false;
	pmf->RunScript("<script language=\"JavaScript\">	var i = parseInt(document.body.style.zoom);	if(isNaN(i))i=100; if(i<0)i=0; if(i==19)i=18; newZoom=i+10+'%'; document.body.style.zoom=newZoom; </script>");
	pmf->m_bShowScriptResult = true;
}

void CGreenBrowserView::OnViewPagesizeDec10() 
{
	// TODO: Add your command handler code here
	pmf->m_bShowScriptResult = false;
	pmf->RunScript("<script language=\"JavaScript\">	var i = parseInt(document.body.style.zoom);	if(isNaN(i))i=100; if(i<18)i=18; newZoom=i-10+'%'; document.body.style.zoom=newZoom; </script>");
	pmf->m_bShowScriptResult = true;
}

void CGreenBrowserView::OnFileImportExport() 
{
	// TODO: Add your command handler code here
	try{
	CFindWnd FindIEWnd( m_wndBrowser.m_hWnd, "Shell DocObject View");
	::SendMessage( FindIEWnd.m_hWnd, WM_COMMAND, MAKEWPARAM(LOWORD(374), 0x0), 0 );
	}catch(...){}
}

void CGreenBrowserView::OnFileAutoSaveSort() 
{
	// TODO: Add your command handler code here
	m_bSortSave = TRUE;
	OnFileAutoSave();
}
