// GreenBrowserHelper.h: interface for the CGreenBrowserHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GreenBrowserHELPER_H__74460E23_1598_11D4_9BCE_0000E85300AE__INCLUDED_)
#define AFX_GreenBrowserHELPER_H__74460E23_1598_11D4_9BCE_0000E85300AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GreenBrowserView.h"

CGreenBrowserView* GetCurrentView(int nFlag = 0, BOOL bInNewWindow = FALSE);
CGreenBrowserView * CreateNewView();

#endif // !defined(AFX_GreenBrowserHELPER_H__74460E23_1598_11D4_9BCE_0000E85300AE__INCLUDED_)
