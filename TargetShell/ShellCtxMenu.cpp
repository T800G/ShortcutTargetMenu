// ShellCtxMenu.cpp : Implementation of CShellCtxMenu
#include "stdafx.h"
#include <strsafe.h>
#include "ShellCtxMenu.h"
#include <Shlobj.h>
//#include <Shlwapi.h>
//#pragma comment(lib, "Shlwapi.lib")

//dynamic load / XP compatible
#pragma comment(lib, "shell32.lib")
typedef HRESULT (WINAPI *pfnSHGetKnownFolderPath)(
  _In_      REFKNOWNFOLDERID rfid,
  _In_      DWORD dwFlags,
  _In_opt_  HANDLE hToken,
  _Out_     PWSTR *ppszPath
 );

//extern "C" const IMAGE_DOS_HEADER __ImageBase; //defined in atlbase.h, uncomment if needed

#define MENU_CMD_STRINGA "Displays the properties of the shortcut target."
#define MENU_CMD_STRINGW L"Displays the properties of the shortcut target."
#define MENU_ITEM_STRING _T("&Shortcut target properties")


HRESULT CShellCtxMenu::FinalConstruct()
{
	ATLTRACE("CCopyPathMenu::FinalConstruct\n");
	m_pszNtfsPath=(TCHAR*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TCHAR)*_MAX_NTFS_PATH);
	if (NULL==m_pszNtfsPath) return E_OUTOFMEMORY;

	//?switch on dpi/sizes (13,15 | 17,19 | 21,25 | 27,31 px) /select immediate smaller
	m_hMenuBmp=(HBITMAP)LoadImage((HINSTANCE)&__ImageBase,
								MAKEINTRESOURCE((GetSystemMetrics(SM_CXMENUCHECK)>13) ? IDB_MENUBITMAP120 : IDB_MENUBITMAP),
									IMAGE_BITMAP, 0,0, LR_CREATEDIBSECTION);
	if (m_hMenuBmp)
		ReplaceDIBColor(&m_hMenuBmp, RGB(255,0,255), GetSysColor(COLOR_MENU));
return S_OK;
}

void CShellCtxMenu::FinalRelease()
{
	ATLTRACE("CCopyPathMenu::FinalRelease\n");
	if (m_pszNtfsPath) HeapFree(GetProcessHeap(), 0, m_pszNtfsPath);
	if (m_pPidl) ILFree(m_pPidl);
	if (m_hMenuBmp) DeleteObject(m_hMenuBmp);
}

HRESULT CShellCtxMenu::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{
	//ATLTRACE("IShellExtInit::Initialize\n");
	HRESULT hr=E_FAIL;
//http://msdn.microsoft.com/en-us/library/windows/desktop/bb775094%28v=vs.85%29.aspx
ATLTRACE("pidlFolder=%x\n", pidlFolder);

	//http://msdn.microsoft.com/en-us/library/windows/desktop/bb762589%28v=vs.85%29.aspx
	SFGAOF sfgaoIn=SFGAO_CANMOVE|SFGAO_CANCOPY|SFGAO_CANDELETE|SFGAO_HASPROPSHEET|SFGAO_FOLDER|SFGAO_FILESYSTEM;

	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };//class->autorelease?
	if (S_OK!=pDataObj->GetData(&fmt, &stg)) return E_INVALIDARG;

	LPDROPFILES lpDropFiles = (LPDROPFILES)GlobalLock(stg.hGlobal);

	ATLASSERT(lpDropFiles->fWide);//not fWide E_INVALIDARG?
	if (lpDropFiles && lpDropFiles->fWide)
	{
		//just first file
		LPCWSTR lpFiles=(LPWSTR)((LPBYTE)lpDropFiles + lpDropFiles->pFiles);
		ATLTRACE(L"path=%s\n", lpFiles);

		//don't use dangerous operations (cut, delete) in navigation pane context menu
		//(re)use m_pszNtfsPath
		if (S_OK==SHGetFolderPath(NULL, CSIDL_FAVORITES, NULL, SHGFP_TYPE_CURRENT, m_pszNtfsPath))
		{
			ATLTRACE(L"CSIDL_FAVORITES=%s\n", m_pszNtfsPath);
			if (StrStr(lpFiles, m_pszNtfsPath)==lpFiles)
			{
				ATLTRACE("XP_Win7::PathIsSameRoot\n");
				sfgaoIn=SFGAO_CANCOPY|SFGAO_HASPROPSHEET|SFGAO_FOLDER|SFGAO_FILESYSTEM;
			}
		}
		*(LPDWORD)(&m_pszNtfsPath[0])=(DWORD)0;//m_pszNtfsPath[0]=L'\0\0';

		pfnSHGetKnownFolderPath pfn=(pfnSHGetKnownFolderPath)GetProcAddress(GetModuleHandle(_T("shell32.dll")), "SHGetKnownFolderPath");
		if (pfn)
		{
			PWSTR pwLinksPath;
			if (S_OK==pfn(FOLDERID_Links, 0, NULL, &pwLinksPath))
			{
				ATLTRACE(L"FOLDERID_Links=%s\n", pwLinksPath);
				if (StrStr(lpFiles, pwLinksPath)==lpFiles)
				{
					ATLTRACE("Win7::PathIsSameRoot\n");
					sfgaoIn=SFGAO_CANCOPY|SFGAO_HASPROPSHEET|SFGAO_FOLDER|SFGAO_FILESYSTEM;
				}
				CoTaskMemFree((LPVOID)pwLinksPath);
			}
		}

		hr=ResolveShellShortcut(lpFiles, m_pszNtfsPath, _MAX_NTFS_PATH);//msi(office) advertised lnk??
		ATLASSERT(SUCCEEDED(hr));
		if SUCCEEDED(hr)
		{
			if (StrChr(m_pszNtfsPath, L'%'))//check if target path contains env vars (%*%)
			{
				TCHAR* pNew=(TCHAR*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TCHAR)*_MAX_NTFS_PATH);
				if (pNew)
				{
					if (ExpandEnvironmentStrings(m_pszNtfsPath, pNew, _MAX_NTFS_PATH)==0)
						hr=HRESULT_FROM_WIN32(GetLastError());
					else
					{
						TCHAR* pTemp=m_pszNtfsPath;
						m_pszNtfsPath=pNew;
						pNew=pTemp;
					}
					HeapFree(GetProcessHeap(), 0, pNew);
				}
				else hr=E_OUTOFMEMORY;
			}
		}
		//set double-null file list buffer terminator for SHFileOperation
		*(LPDWORD)(&m_pszNtfsPath[_MAX_NTFS_PATH-2])=(DWORD)0;//2 wchars == 1 DWORD

		ATLASSERT(SUCCEEDED(hr));
		if SUCCEEDED(hr)
		{
			if (PathFileExists(m_pszNtfsPath)) ///????slow on network path
			{
				ATLTRACE(L"m_pszNtfsPath=%s\n",m_pszNtfsPath);
				hr=SHParseDisplayName(m_pszNtfsPath, NULL, &m_pPidl, sfgaoIn, &m_sfgao);
				if SUCCEEDED(hr)
				{
					//ATLTRACE("m_sfgao=");dbgTraceSFGAOFlags(m_sfgao);
					if (m_sfgao & SFGAO_FILESYSTEM)
					{
						ATLTRACE("m_sfgao=");dbgTraceSFGAOFlags(m_sfgao);
						if (m_sfgao & SFGAO_CANDELETE)  m_sfgao|=(SFGAO_CANMOVE|SFGAO_CANCOPY);
						if (PathIsRoot(m_pszNtfsPath))   m_sfgao &= ~(SFGAO_CANMOVE|SFGAO_CANDELETE);
						if (PathIsUNC(m_pszNtfsPath))    m_sfgao &= ~(SFGAO_CANMOVE|SFGAO_CANDELETE);
						if (IsFixedMedia(m_pszNtfsPath)) m_sfgao &= ~(SFGAO_CANMOVE|SFGAO_CANDELETE);//WinXP fix
						ATLTRACE("m_sfgao(fixed)=");dbgTraceSFGAOFlags(m_sfgao);
					}
					else hr=E_INVALIDARG;
				}
			}
			else hr=HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);//?ERROR_PATH_NOT_FOUND //GetLastError?
		}
		GlobalUnlock(stg.hGlobal);
	}
	else hr=HRESULT_FROM_WIN32(GetLastError());

	ReleaseStgMedium(&stg);//???class CStgMedium.Release in dtor? //ctor(tagTYMED)
return hr;
}


HRESULT CShellCtxMenu::QueryContextMenu (HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags)
{
    if ( uFlags & CMF_DEFAULTONLY ) return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
	//shell already resolved *.lnk file?
	UINT uID = uidFirstCmd;

	MENUITEMINFO mii = {sizeof(MENUITEMINFO)};
	mii.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID;
	mii.wID = uID++;
	mii.dwTypeData = _T("&Shortcut target");//?localize
	mii.hSubMenu = CreatePopupMenu();
	if (mii.hSubMenu)
	{
		//ATLTRACE("GetUserDefaultUILanguage= %d\n", GetUserDefaultUILanguage());
		//ATLTRACE("GetUserDefaultLangID= %d\n", GetUserDefaultLangID());
		LANGID wdUIlang=GetUserDefaultUILanguage();

		//resource are compiled with /n switch (null-terminated resource strings)
		LPCWSTR pszMenuString=LoadStringLCNull((HINSTANCE)&__ImageBase, IDS_CUT, wdUIlang);
		if (pszMenuString) {if (!InsertMenu(mii.hSubMenu, 0,
											((m_sfgao & SFGAO_CANMOVE) ? MF_BYPOSITION : MF_BYPOSITION|MF_DISABLED|MF_GRAYED),
											uID++, pszMenuString))
											goto finish;}

		pszMenuString=LoadStringLCNull((HINSTANCE)&__ImageBase, IDS_COPY, wdUIlang);
		if (pszMenuString) {if (!InsertMenu(mii.hSubMenu, 1,
											((m_sfgao & SFGAO_CANCOPY) ? MF_BYPOSITION : MF_BYPOSITION|MF_DISABLED|MF_GRAYED),
											uID++, pszMenuString))
											goto finish;}
		
		if (!InsertMenu(mii.hSubMenu, 2, MF_BYPOSITION|MF_SEPARATOR, uID++, NULL)) goto finish;
		
		pszMenuString=LoadStringLCNull((HINSTANCE)&__ImageBase, IDS_PROPERTIES, wdUIlang);
		if (pszMenuString) {if (!InsertMenu(mii.hSubMenu, 3,
											((m_sfgao & SFGAO_HASPROPSHEET) ? MF_BYPOSITION : MF_BYPOSITION|MF_DISABLED|MF_GRAYED),
											uID++, pszMenuString))
											goto finish;}

////use rc.exe WITHOUT /n switch:
		//// reuse m_pszNtfsPath buffer
		//if (LoadStringLC((HINSTANCE)&__ImageBase, IDS_CUT, m_pszNtfsPath, _MAX_NTFS_PATH, wdUIlang))
		//{if (!InsertMenu(mii.hSubMenu, 0, ((m_sfgao & SFGAO_CANMOVE) ? MF_BYPOSITION : MF_BYPOSITION|MF_DISABLED|MF_GRAYED), uID++, m_pszNtfsPath)) goto finish;}
		//if (LoadStringLC((HINSTANCE)&__ImageBase, IDS_COPY, m_pszNtfsPath, _MAX_NTFS_PATH, wdUIlang))
		//{if (!InsertMenu(mii.hSubMenu, 1, ((m_sfgao & SFGAO_CANCOPY) ? MF_BYPOSITION : MF_BYPOSITION|MF_DISABLED|MF_GRAYED), uID++, m_pszNtfsPath)) goto finish;}
		//if (!InsertMenu(mii.hSubMenu, 3, MF_BYPOSITION|MF_SEPARATOR, uID++, NULL)) goto finish;
		//if (LoadStringLC((HINSTANCE)&__ImageBase, IDS_PROPERTIES, m_pszNtfsPath, _MAX_NTFS_PATH, wdUIlang))
		//{if (!InsertMenu(mii.hSubMenu, 4, ((m_sfgao & SFGAO_HASPROPSHEET) ? MF_BYPOSITION : MF_BYPOSITION|MF_DISABLED|MF_GRAYED), uID++, m_pszNtfsPath)) goto finish;}
	}
	if (!InsertMenuItem(hmenu, uMenuIndex++, TRUE, &mii)) goto finish;
	SetMenuItemBitmaps(hmenu, mii.wID, MF_BYCOMMAND, m_hMenuBmp, m_hMenuBmp);

finish:
	m_uInserted=uID-uidFirstCmd;
	ATLTRACE("uID=%d, m_uInserted=%d\n", uID, m_uInserted);
return MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_NULL, USHORT(m_uInserted+1));
}


HRESULT CShellCtxMenu::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
	//ATLTRACE("IContextMenu::GetCommandString\n");
	HRESULT hr=E_FAIL;
	if (idCmd > m_uInserted) return E_INVALIDARG;
return S_OK;
	//if (uFlags & GCS_HELPTEXT)
	//{
	//	if (idCmd==m_cmdID) hr=(uFlags & GCS_UNICODE) ? StringCchCopyW((LPWSTR)pszName, cchMax, MENU_CMD_STRINGW) :
	//													StringCchCopyA(pszName, cchMax, MENU_CMD_STRINGA);
	//}
	//only one menu item, else must set verbs???(stringA?) (uFlags & GCS_VERB)
return (SUCCEEDED(hr) ? S_OK : E_INVALIDARG);
}


HRESULT CShellCtxMenu::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	ATLTRACE("IContextMenu::InvokeCommand\n");
    if (HIWORD(pCmdInfo->lpVerb)!=0)
	{
		ATLTRACE("IContextMenu::InvokeCommand - given verb: %s\n",pCmdInfo->lpVerb);
		if (StrCmpA(pCmdInfo->lpVerb, MENU_CMD_STRINGA)!=0) return E_INVALIDARG;
		return E_FAIL;
	}
	ATLTRACE("LOWORD(pCmdInfo->lpVerb)=%d\n",LOWORD(pCmdInfo->lpVerb));
	HRESULT hr=S_FALSE;

	SHELLEXECUTEINFO sei;
	SecureZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
	sei.cbSize=sizeof(SHELLEXECUTEINFO);
	sei.hwnd=pCmdInfo->hwnd;
	sei.fMask=SEE_MASK_INVOKEIDLIST;
	sei.lpIDList=m_pPidl;
	sei.nShow=SW_SHOW;	
	switch (LOWORD(pCmdInfo->lpVerb))
	{
//0 submenu item???
		case 1: sei.lpVerb=_T("Cut") ;break;
		case 2: sei.lpVerb=_T("Copy"); break;
		//case 3: //menu separator
		case 4: sei.lpVerb=_T("Properties"); break;
		default: return E_INVALIDARG;
	}
	if (!ShellExecuteEx(&sei)) return HRESULT_FROM_WIN32(GetLastError());
return S_OK;
}
