/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "slib/core/definition.h"

#if defined(SLIB_UI_IS_WIN32)

#include "ui_core_win32.h"
#include "ui_core_common.h"
#include "view_win32.h"

#include "slib/ui/screen.h"
#include "slib/ui/app.h"
#include "slib/core/queue.h"
#include "slib/core/dispatch.h"
#include "slib/core/safe_static.h"

#include <commctrl.h>

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "comctl32.lib")

namespace slib
{

	DWORD _g_thread_ui = 0;

	class _priv_Win32_UI
	{
	public:
		Ref<Screen> m_screenPrimary;

		_priv_Win32_UI();

	};

	SLIB_SAFE_STATIC_GETTER(_priv_Win32_UI, _priv_Win32_getUI)

	class _priv_Win32_Screen : public Screen
	{
	public:
		_priv_Win32_Screen()
		{
		}

		UIRect getRegion()
		{
			UIRect ret;
			ret.left = 0;
			ret.top = 0;
			ret.right = (sl_ui_pos)(GetSystemMetrics(SM_CXSCREEN));
			ret.bottom = (sl_ui_pos)(GetSystemMetrics(SM_CYSCREEN));
			return ret;
		}
	};

	List< Ref<Screen> > UI::getScreens()
	{
		_priv_Win32_UI* ui = _priv_Win32_getUI();
		if (!ui) {
			return sl_null;
		}
		List< Ref<Screen> > ret;
		ret.add_NoLock(ui->m_screenPrimary);
		return ret;
	}

	Ref<Screen> UI::getPrimaryScreen()
	{
		_priv_Win32_UI* ui = _priv_Win32_getUI();
		if (!ui) {
			return sl_null;
		}
		return ui->m_screenPrimary;
	}

	Ref<Screen> UI::getFocusedScreen()
	{
		_priv_Win32_UI* ui = _priv_Win32_getUI();
		if (!ui) {
			return sl_null;
		}
		return ui->m_screenPrimary;
	}

	sl_bool UI::isUiThread()
	{
		return (_g_thread_ui == ::GetCurrentThreadId());
	}

	void _priv_Win32_PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		Win32_UI_Shared* shared = Win32_UI_Shared::get();
		if (!shared) {
			return;
		}
		::PostMessageW(shared->hWndMessage, uMsg, wParam, lParam);
	}

	void UI::dispatchToUiThread(const Function<void()>& callback, sl_uint32 delayMillis)
	{
		if (callback.isNull()) {
			return;
		}
		if (delayMillis == 0) {
			Win32_UI_Shared* shared = Win32_UI_Shared::get();
			if (!shared) {
				return;
			}
			if (_priv_UIDispatcher::addCallback(callback)) {
				::PostMessageW(shared->hWndMessage, SLIB_UI_MESSAGE_DISPATCH, 0, 0);
			}
		} else {
			sl_reg ptr;
			if (_priv_UIDispatcher::addDelayedCallback(callback, ptr)) {
				Dispatch::setTimeout(Function<void()>::bind(&_priv_Win32_PostMessage, SLIB_UI_MESSAGE_DISPATCH_DELAYED, 0, (LPARAM)ptr), delayMillis);
			}
		}
	}

	void _priv_Win32_processMenuCommand(WPARAM wParam, LPARAM lParam);
	sl_bool _priv_Win32_processMenuShortcutKey(MSG& msg);
	void _priv_Win32_processCustomMsgBox(WPARAM wParam, LPARAM lParam);
	sl_bool _priv_Win32_captureChildInstanceEvents(View* view, MSG& msg);

	LRESULT CALLBACK _priv_Win32_MessageProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == SLIB_UI_MESSAGE_DISPATCH) {
			_priv_UIDispatcher::processCallbacks();
		} else if (uMsg == SLIB_UI_MESSAGE_DISPATCH_DELAYED) {
			_priv_UIDispatcher::processDelayedCallback((sl_reg)lParam);
		} else if (uMsg == SLIB_UI_MESSAGE_CUSTOM_MSGBOX) {
			_priv_Win32_processCustomMsgBox(wParam, lParam);
			return 0;
		} else if (uMsg == WM_MENUCOMMAND) {
			_priv_Win32_processMenuCommand(wParam, lParam);
			return 0;
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}


	void UIPlatform::runLoop(sl_uint32 level)
	{
		MSG msg;
		while (::GetMessageW(&msg, NULL, 0, 0)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			if (msg.message == SLIB_UI_MESSAGE_DISPATCH) {
				_priv_UIDispatcher::processCallbacks();
			} else if (msg.message == SLIB_UI_MESSAGE_DISPATCH_DELAYED) {
				_priv_UIDispatcher::processDelayedCallback((sl_reg)(msg.lParam));
			} else if (msg.message == SLIB_UI_MESSAGE_CLOSE) {
				::DestroyWindow(msg.hwnd);
			} else if (msg.message == WM_MENUCOMMAND) {
				_priv_Win32_processMenuCommand(msg.wParam, msg.lParam);
			} else {
				do {
					HWND hWnd = ::GetActiveWindow();
					if (hWnd) {
						if (::GetWindowLongW(hWnd, GWL_STYLE) & WS_POPUP) {
							if (::IsDialogMessageW(hWnd, &msg)) {
								break;
							}
						}
					}
					if (_priv_Win32_processMenuShortcutKey(msg)) {
						break;
					}
					Ref<Win32_ViewInstance> instance = Ref<Win32_ViewInstance>::from(UIPlatform::getViewInstance(msg.hwnd));
					if (instance.isNotNull()) {
						Ref<View> view = instance->getView();
						if (view.isNotNull()) {
							if (_priv_Win32_captureChildInstanceEvents(view.get(), msg)) {
								break;
							}
							if (instance->preprocessWindowMessage(msg)) {
								break;
							}
						}
					}
					::TranslateMessage(&msg);
					::DispatchMessageW(&msg);
				} while (0);
			}
		}

		_priv_UIDispatcher::removeAllCallbacks();

	}

	void UIPlatform::quitLoop()
	{
		::PostQuitMessage(0);
	}

	void UIPlatform::runApp()
	{
		Win32_UI_Shared::get();

		_g_thread_ui = ::GetCurrentThreadId();

		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		::OleInitialize(NULL);

		INITCOMMONCONTROLSEX icex;
		icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_TAB_CLASSES;
		InitCommonControlsEx(&icex);

		UIApp::dispatchStartToApp();

		runLoop(0);

		UIApp::dispatchExitToApp();

		//Gdiplus::GdiplusShutdown(gdiplusToken);
	}

	void UIPlatform::quitApp()
	{
		::PostQuitMessage(0);
	}

	_priv_Win32_UI::_priv_Win32_UI()
	{
		m_screenPrimary = new _priv_Win32_Screen();
	}

	LRESULT CALLBACK _priv_Win32_ViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK _priv_Win32_WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	Win32_UI_Shared::Win32_UI_Shared()
	{
		hInstance = ::GetModuleHandleW(NULL);
		
		// register view class
		{
			WNDCLASSEXW wc;
			::ZeroMemory(&wc, sizeof(wc));
			wc.cbSize = sizeof(wc);
			wc.style = CS_DBLCLKS | CS_PARENTDC;
			wc.lpfnWndProc = _priv_Win32_ViewProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = hInstance;
			wc.hIcon = NULL;
			wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)(COLOR_MENU + 1);
			wc.lpszMenuName = NULL;
			wc.lpszClassName = L"SLIBUIVIEW";
			wc.hIconSm = NULL;
			wndClassForView = ::RegisterClassExW(&wc);
		}

		// register window class
		{
			WNDCLASSEXW wc;
			::ZeroMemory(&wc, sizeof(wc));
			wc.cbSize = sizeof(wc);
			wc.style = CS_DBLCLKS;
			wc.lpfnWndProc = _priv_Win32_WindowProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = hInstance;
			wc.hIcon = ::LoadIcon(hInstance, IDI_APPLICATION);
			wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)(COLOR_MENU + 1);
			wc.lpszMenuName = NULL;
			wc.lpszClassName = L"SLIBUIWINDOW";
			wc.hIconSm = NULL;
			wndClassForWindow = ::RegisterClassExW(&wc);
		}

		// Mesage Window
		{
			WNDCLASSW wc;
			::ZeroMemory(&wc, sizeof(wc));
			wc.hInstance = hInstance;
			wc.lpfnWndProc = _priv_Win32_MessageProc;
			wc.lpszClassName = L"SLIBMESSAGEHANDLER";
			m_wndClassForMessage = ::RegisterClassW(&wc);
			hWndMessage = ::CreateWindowExW(0, (LPCWSTR)((LONG_PTR)m_wndClassForMessage), L"", 0, 0, 0, 0, 0, HWND_MESSAGE, 0, hInstance, 0);
		}

	}

	Win32_UI_Shared::~Win32_UI_Shared()
	{
		if (hWndMessage) {
			::DestroyWindow(hWndMessage);
		}
	}

	Win32_UI_Shared* Win32_UI_Shared::get()
	{
		SLIB_SAFE_STATIC(Win32_UI_Shared, ret);
		if (SLIB_SAFE_STATIC_CHECK_FREED(ret)) {
			return sl_null;
		}
		return &ret;
	}

}

#endif
