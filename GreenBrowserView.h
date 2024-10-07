// GreenBrowserView.h : interface of the CGreenBrowserView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GreenBrowserVIEW_H__19E497C7_4ECF_11D3_9B1D_0000E85300AE__INCLUDED_)
#define AFX_GreenBrowserVIEW_H__19E497C7_4ECF_11D3_9B1D_0000E85300AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FixedHtmlView.h"
#include "AddressBarEx.h"

//##############################################################
#define STOP_TIMER 950

class CGreenBrowserView : public CFixedHtmlView
{
protected: // create from serialization only
	DECLARE_DYNCREATE(CGreenBrowserView)

// Attributes
public:
	BOOL MoveDirection(CPoint& point, char* Direction);

	int  MouseAction(int kind);
	int  OpenLinkUnderMouse(int nType=0);

	void SetSpecialProperty(DWORD dwNewProperty);
	void SetDefaultProperty();
	void SetFullProperty();
	void PowerRefresh();
	void VScrollPageUp();
	void VScrollPageDown();
	void WBScrollBy(long xpos,long ypos);
	void WBScroll();
	void WBScrollHPage(int bLeft);
	void ExecCommand(DWORD nCmdID);

	CString GetPointUrl(POINT pt);
	CString GetSortSaveFileName(CString &ext);
	
	CGreenBrowserView();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGreenBrowserView)
	public:
	virtual void OnInitialUpdate(int nOpenType, LPCSTR strUrl); // called first time after construct
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, BOOL baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel);
	virtual void OnTitleChange(LPCTSTR lpszText);
	virtual void OnDocumentComplete(LPCTSTR lpszUrl);
	virtual void OnProgressChange(long nProgress, long nProgressMax);
	virtual void OnStatusTextChange(LPCTSTR lpszText);
	virtual BOOL OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar);
	virtual void OnCommandStateChange(long nCommand, BOOL bEnable);
	virtual void OnNavigateComplete2(LPCTSTR lpszURL);
	//}}AFX_VIRTUAL

// Implementation
private:
	CString m_lpszUrl;

public:
	BOOL m_bExternalRunOK;
	BOOL m_bForbidAutoNavigate;
	BOOL m_bSortSave;
	BOOL m_bHighLighted;
	BOOL m_bFindPrev;
	BOOL m_bAutoPopWin;
	BOOL m_bIsCapture;
	BOOL m_bIsSecure;
	BOOL m_bReUsed;
	BOOL m_bScrollAfterDL;
	BOOL m_bUseAutoScroll, m_bUseAutoRef;
	BOOL m_bFirst;
	BOOL m_bForceRefresh;
	BOOL m_bDelayAdd;
	BOOL m_bIsNewPopup;
	BOOL m_bEnWebProxy;
	BOOL m_bIsPopUp;
	BOOL m_bIsActive;

	int  m_nAutoRunCmd;
	int	 m_nInNewWindow;//force open in new window
	int	 m_nFindIndex;
	int  m_nFindMax;
	int  m_nFilterDup;
	int  m_nDupID;
	int  m_nOldIcon;
	int  m_nOlderIcon;//old icon before lock and protect
	int  m_nSrSpeed; //1-slow, 2-medium, 3-fast
	int  m_nScrollSize;

	UINT m_nFontSize;
	UINT m_nLastScrSpd;
	UINT m_nScrollTimer, m_nAutoRefreshTimer;
	UINT m_nRefreshInterval;

	DWORD m_bSpecialProperty;
	DWORD m_dwProperty,m_dwPropertyDefault;

	CString m_strOpenNewUrl;//last open in new window url
	CString m_strHighLightKey;
	CString	m_strFindKey;
	CString m_strPossibleURL;
	CString m_strWebProxy;

	void SetUrl(const CString &url);
	CString GetUrl();
	void ChangeSpeed(int nSpeed);
	void ChangeRefresh();
	void ChangeProperty();
	void ToNavigate(LPCTSTR URL, DWORD dwFlags = 0, LPCTSTR lpszTargetFrameName = NULL,  LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,  DWORD dwPostDataLen = 0, BOOL bOnlyRefresh = FALSE);

	virtual ~CGreenBrowserView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	afx_msg void OnEditCopy();

// Generated message map functions
protected:
	afx_msg void OnEditCut();
	afx_msg void OnEditFind();
	afx_msg void OnEditPaste();
	afx_msg void OnEditSelectAll();
	afx_msg void OnOptionsLoadimage();
	afx_msg void OnUpdateOptions(CCmdUI* pCmdUI);
	afx_msg void OnOptionsAllowscripts();
	afx_msg void OnOptionsLoadsounds();
	afx_msg void OnOptionsLoadvideos();
	afx_msg void OnOptionsUseWebp();
	afx_msg void OnUpdateOptionsUseWebp(CCmdUI* pCmdUI);
	afx_msg void OnOptionsAllowactivex();
	afx_msg void OnOptionsAllowjavaapplet();
	afx_msg void OnUpdateGoBack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGoForward(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateViewFonts(CCmdUI* pCmdUI);
	//{{AFX_MSG(CGreenBrowserView)
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnToolsAutoref();
	afx_msg void OnUpdateToolsAutoref(CCmdUI* pCmdUI);
	afx_msg void OnToolsReffre();
	afx_msg void OnFileSavehtml();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnFilePrintSetup();
	afx_msg void OnFileProperty();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnFileAutoSave();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnUpdateToolsAutoscrollPage(CCmdUI* pCmdUI);
	afx_msg void OnViewPagesizeAdd10();
	afx_msg void OnViewPagesize100();
	afx_msg void OnViewPagesize200();
	afx_msg void OnViewPagesize50();
	afx_msg void OnViewPagesizeDec10();
	afx_msg void OnFileImportExport();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileAutoSaveSort();
	//}}AFX_MSG
	afx_msg void OnGoBack();
	afx_msg void OnGoForward();
	afx_msg void OnToolsAutoscrollAutoscrollingafterdownloading();
	afx_msg void OnUpdateToolsAutoscrollAutoscrollingafterdownloading(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolsAutoscrollSpeed(CCmdUI* pCmdUI);
	afx_msg void OnToolsAutoscrollUseautoscrolling();
	afx_msg void OnUpdateToolsAutoscrollUseautoscrolling(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
private:
	void PushMouseGesture(char cDirection);
	POINT m_ptLClick, m_ptRClick;
	POINT m_ptPrev;
};

#ifndef _DEBUG  // debug version in GreenBrowserView.cpp
/*inline CGreenBrowserDoc* CGreenBrowserView::GetDocument()
   { return (CGreenBrowserDoc*)m_pDocument; }*/
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GreenBrowserVIEW_H__19E497C7_4ECF_11D3_9B1D_0000E85300AE__INCLUDED_)
