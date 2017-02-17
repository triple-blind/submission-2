/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#pragma once 

#ifndef _WINDOWS_
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif

//------------------------------------------------------------------------
// undefine the macros defined in the winuser.h file in order to avoid
// warnings because of multiple defines 
//------------------------------------------------------------------------

#ifdef SendMessageW
#undef SendMessageW
#endif

#ifdef CreateWindowExW
#undef CreateWindowExW
#endif

#ifdef RegisterClassExW
#undef RegisterClassExW
#endif

#ifdef UnregisterClassW
#undef UnregisterClassW
#endif

#ifdef RegisterClipboardFormatW
#undef RegisterClipboardFormatW
#endif

#ifdef GetClipboardFormatNameW
#undef GetClipboardFormatNameW
#endif

#ifdef SetWindowTextW
#undef SetWindowTextW
#endif

#ifdef GetWindowTextW
#undef GetWindowTextW
#endif

#ifdef InsertMenuItemW
#undef InsertMenuItemW
#endif

#ifndef DrawTextW
#undef DrawTextW
#endif

//------------------------------------------------------------------------
// defines
//------------------------------------------------------------------------

#define USER9X_API extern

//------------------------------------------------------------------------
// declare function pointers to the appropriate user32 functions
//------------------------------------------------------------------------

USER9X_API LRESULT ( WINAPI * lpfnSendMessageW) (
	HWND hWnd,		// handle to the destination window
	UINT Msg,		// message
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
);

USER9X_API HWND ( WINAPI * lpfnCreateWindowExW ) (
	DWORD dwExStyle,      // extended window style
	LPCWSTR lpClassName,  // registered class name
	LPCWSTR lpWindowName, // window name
	DWORD dwStyle,        // window style
	int x,                // horizontal position of window
	int y,                // vertical position of window
	int nWidth,           // window width
	int nHeight,          // window height
	HWND hWndParent,      // handle to parent or owner window
	HMENU hMenu,          // menu handle or child identifier
	HINSTANCE hInstance,  // handle to application instance
	LPVOID lpParam        // window-creation data
);

USER9X_API ATOM ( WINAPI * lpfnRegisterClassExW ) ( 
	CONST WNDCLASSEXW* lpwcx // class data
);

USER9X_API BOOL ( WINAPI * lpfnUnregisterClassW ) (
	LPCWSTR lpClassName, // class name
	HINSTANCE hInstance	 // handle to application instance
);

USER9X_API UINT (WINAPI * lpfnRegisterClipboardFormatW) (
	LPCWSTR lpszFormat // name of new format
);

USER9X_API int ( WINAPI * lpfnGetClipboardFormatNameW ) (
	UINT   format,			// clipboard format to retrieve
	LPWSTR lpszFormatName,	// format name
	int    cchMaxCount		// length of format name buffer
);

USER9X_API BOOL ( WINAPI * lpfnSetWindowTextW ) (
	HWND hWnd,
    LPCWSTR lpString
);

USER9X_API int ( WINAPI * lpfnGetWindowTextW ) (
    HWND hWnd,			// handle to the window or control
	LPWSTR lpString,	// text buffer
	int    nMaxCount		// length of text buffer
);

USER9X_API BOOL ( WINAPI * lpfnInsertMenuItemW ) (
    HMENU hMenu,            // handle to menu
    UINT uItem,             // identifier or position
    BOOL fByPosition,       // meaning of uItem
    LPCMENUITEMINFOW lpmii  // menu item information
);

USER9X_API int ( WINAPI * lpfnDrawTextW ) (
  HDC hDC,          // handle to DC
  LPCWSTR lpString, // text to draw
  int nCount,       // text length
  LPRECT lpRect,    // formatting dimensions
  UINT uFormat      // text-drawing options
);

USER9X_API BOOL ( WINAPI * lpfnDrawStateW ) (
  HDC hdc,                     // handle to device context
  HBRUSH hbr,                  // handle to brush
  DRAWSTATEPROC lpOutputFunc,  // callback function
  LPARAM lData,                // image information
  WPARAM wData,                // more image information
  int x,                       // horizontal location
  int y,                       // vertical location
  int cx,                      // image width
  int cy,                      // image height
  UINT fuFlags                 // image type and state
);

//------------------------------------------------------------------------
// redefine the above undefined macros so that the preprocessor replaces
// all occurrences of this macros with our function pointer
//------------------------------------------------------------------------

#define SendMessageW				lpfnSendMessageW
#define CreateWindowExW				lpfnCreateWindowExW
#define RegisterClassExW			lpfnRegisterClassExW
#define UnregisterClassW			lpfnUnregisterClassW
#define RegisterClipboardFormatW	lpfnRegisterClipboardFormatW
#define GetClipboardFormatNameW		lpfnGetClipboardFormatNameW
#define SetWindowTextW				lpfnSetWindowTextW
#define GetWindowTextW              lpfnGetWindowTextW
#define InsertMenuItemW				lpfnInsertMenuItemW
#define DrawTextW                   lpfnDrawTextW
#define DrawStateW                  lpfnDrawStateW

#ifdef __cplusplus
}
#endif
