/* 
 * Copyright (C) 2011-2016 MicroSIP (http://www.microsip.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

#include "StdAfx.h"
#include "RinginDlg.h"
#include "langpack.h"
#include "mainDlg.h"

#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>

int spawnX = -1;
int spawnY = -1;

RinginDlg::RinginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(RinginDlg::IDD, pParent)
{
	Create (IDD, pParent);
}

RinginDlg::~RinginDlg(void)
{
}

int RinginDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (langPack.rtl) {
		ModifyStyleEx(0,WS_EX_LAYOUTRTL);
	}
	return 0;
}

void RinginDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

BOOL CALLBACK MyInfoEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    MONITORINFOEX iMonitor;
    iMonitor.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &iMonitor);
    if (iMonitor.dwFlags == DISPLAY_DEVICE_MIRRORING_DRIVER) {
        return true;
    } else {
        reinterpret_cast< std::vector<HMONITOR>* >(dwData)->push_back(hMonitor);
        return true;
    }
}

void WritePosition(int x, int y){
	 TCHAR buffer[MAX_PATH];
	 GetModuleFileName(NULL, buffer, _countof(buffer));
	 std::wstringstream stream;
	 stream << buffer << L"\\..\\config.ini";
	 std::wstring path = stream.str();

	 std::ofstream file(path.c_str(),std::ofstream::out);
	 file << x << " " << y;
	 file.close();
}

bool GetPosition(int out[]){
	 TCHAR buffer[MAX_PATH];
	 GetModuleFileName(NULL, buffer, _countof(buffer));
	 std::wstringstream stream;
	 stream << buffer << L"\\..\\config.ini";
	 std::wstring path = stream.str();

	 std::ifstream file(path.c_str());
	 if(file.fail()){
		return false;
	 }
	 else{
		 std::string str;
		 file >> str;
		 int x = std::stoi(str);
		 str = "";
		 file >> str;
		 int y = std::stoi(str);
		 out[0] =  x;
		 out[1] = y;

		 file.close();
		 return true;
	 }
}

BOOL RinginDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	TranslateDialog(this->m_hWnd);

	CFont* font = this->GetFont();
	LOGFONT lf;
	font->GetLogFont(&lf);

	lf.lfHeight = 12;
	m_font_ignore.CreateFontIndirect(&lf);
	GetDlgItem(IDC_IGNORE)->SetFont(&m_font_ignore);
	
	lf.lfHeight = 24;
	lf.lfWeight = FW_BOLD;
	m_font.CreateFontIndirect(&lf);
	GetDlgItem(IDC_CALLER_NAME)->SetFont(&m_font);

	GetDlgItem(IDC_IGNORE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CALLER_ADDR)->ModifyStyleEx(WS_EX_CLIENTEDGE,0,SWP_NOSIZE|SWP_FRAMECHANGED);

#if 0
	if (accountSettings.randomAnswerBox) {
		CRect ringinRect;
		GetWindowRect(&ringinRect);
		std::vector<HMONITOR> hMonitorArray;
		EnumDisplayMonitors(NULL, NULL, &MyInfoEnumProc, reinterpret_cast<LPARAM>(&hMonitorArray));
		std::random_shuffle ( hMonitorArray.begin(), hMonitorArray.end() );
		std::vector<HMONITOR>::iterator it = hMonitorArray.begin();
		HMONITOR hMonitor = *it;
		MONITORINFO mi;
		mi.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor,&mi);
		x = mi.rcWork.left + ( (mi.rcWork.right-mi.rcWork.left) -ringinRect.Width()) * rand() / RAND_MAX;
		y = mi.rcWork.top + ( (mi.rcWork.bottom-mi.rcWork.top) -ringinRect.Height()) * rand() / RAND_MAX;
	} else {
#endif
		CRect ringinRect;
		GetWindowRect(&ringinRect);
		std::vector<HMONITOR> hMonitorArray;
		EnumDisplayMonitors(NULL, NULL, &MyInfoEnumProc, reinterpret_cast<LPARAM>(&hMonitorArray));
		std::vector<HMONITOR>::reverse_iterator it = hMonitorArray.rbegin();
		HMONITOR hMonitor = *it;
		MONITORINFO mi;
		mi.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor,&mi);

		int x,y;
		if(spawnX == -1){
			spawnX = mi.rcWork.left + (mi.rcWork.right - mi.rcWork.left)/2 - ringinRect.Width()/2;
			spawnY = mi.rcWork.top + (mi.rcWork.bottom - mi.rcWork.top)/2 - ringinRect.Height()/2;				
		}		

		if (mainDlg->ringinDlgs.GetCount())
		{
			//multiple dialogs: just offset
			CRect rect;
			mainDlg->ringinDlgs.GetAt(mainDlg->ringinDlgs.GetCount()-1)->GetWindowRect(&rect);
			x=rect.left+22;
			y=rect.top+22;
		} else {
			//first dialog: place at predetermined position
			x = spawnX;
			y = spawnY;
		}

#if 0
	if (mainDlg->IsWindowVisible()) {
		if (mainDlg->IsIconic()) {
			mainDlg->ShowWindow(SW_SHOWNOACTIVATE);
		} else {
			mainDlg->ShowWindow(SW_HIDE);
			mainDlg->ShowWindow(SW_MINIMIZE);
			mainDlg->ShowWindow(SW_RESTORE);
		}
	}
#endif
	ShowWindow(SW_SHOWNOACTIVATE);
	SetWindowPos( NULL, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);

	return TRUE;
}

void RinginDlg::OnMove(int x, int y)
{
	if(spawnX != -1 && (!mainDlg->ringinDlgs.GetCount() || mainDlg->ringinDlgs.GetAt(0) == this)){
		spawnX = x;
		spawnY = y;
	}
}

BEGIN_MESSAGE_MAP(RinginDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDOK, &RinginDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &RinginDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_ANSWER, &RinginDlg::OnBnClickedAudio)
	ON_BN_CLICKED(IDC_DECLINE, &RinginDlg::OnBnClickedDecline)
	ON_BN_CLICKED(IDC_VIDEO, &RinginDlg::OnBnClickedVideo)
END_MESSAGE_MAP()

void RinginDlg::OnClose() 
{
	Close();
}

void RinginDlg::Close(BOOL accept)
{
	int count = mainDlg->ringinDlgs.GetCount();
	for (int i = 0; i < count; i++ )
	{
		if ( call_id == mainDlg->ringinDlgs.GetAt(i)->call_id)
		{
			if (!accept) {
				mainDlg->UpdateWindowText(_T("-"));
			}
			if (count==1) {
				mainDlg->PlayerStop();
			}
			mainDlg->ringinDlgs.RemoveAt(i);
			DestroyWindow();
			break;
		}
	}
}

void RinginDlg::OnBnClickedOk()
{
}

void RinginDlg::OnBnClickedCancel()
{
	Close();
}

void RinginDlg::OnBnClickedDecline()
{
	pjsua_call_info call_info;
	pjsua_call_get_info(call_id,&call_info);
	if (pjsua_call_hangup(call_id, 0, NULL, NULL) == PJ_SUCCESS) {
		mainDlg->callIdIncomingIgnore = PjToStr(&call_info.call_id);
	}
	Close();
}

void RinginDlg::OnBnClickedAudio() 
{
	CallAccept();
}

void RinginDlg::OnBnClickedVideo()
{
	CallAccept(TRUE);
}

void RinginDlg::CallAccept(BOOL hasVideo)
{
	mainDlg->onCallAnswer((WPARAM)call_id, (LPARAM)hasVideo);
	//Close(TRUE);
}

void RinginDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	//SetTimer(IDT_TIMER_INIT_RINGIN,1000,NULL);
}

void RinginDlg::OnTimer (UINT TimerVal)
{
	if (TimerVal == IDT_TIMER_INIT_RINGIN)
	{

		//SetForegroundWindow();
		//BringWindowToTop();

		//ShowWindow(SW_SHOWNORMAL);

		if (mainDlg->IsIconic()) {
//			mainDlg->ShowWindow(SW_RESTORE);
		}
		
		//::SetWindowPos(mainDlg->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		//::SetFocus(mainDlg->m_hWnd);
		
		//BringWindowToTop();
		//SetForegroundWindow();

		//mainDlg->SetForegroundWindow();


		KillTimer(IDT_TIMER_INIT_RINGIN);
	}
}