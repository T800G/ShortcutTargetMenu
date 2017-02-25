#ifndef _HELPERS_4469818A_9E07_4F0C_BDE9_E756E6C46A56_
#define _HELPERS_4469818A_9E07_4F0C_BDE9_E756E6C46A56_

#include <sal.h>

// Replace DIB palette color
#ifndef _WINDOWS_
#include <windows.h>
#endif
#ifdef _ATL_MIN_CRT
// declare global var if you don't use crt and have more than 4k of stack (needs __chkstk())
RGBQUAD __clrtbl[256*sizeof(RGBQUAD)];
#endif
inline BOOL ReplaceDIBColor(__inout HBITMAP* pDIB, __in COLORREF oldColor, __in COLORREF newColor)
{
	_ASSERTE(pDIB!=NULL);
	if (pDIB==NULL) return FALSE;
	if (oldColor==newColor) return TRUE;//nothing to do
	BOOL bRet=FALSE;

	//get color information
	DIBSECTION ds;
	if (!::GetObject(*pDIB, sizeof(DIBSECTION), &ds)) return FALSE;
	UINT nColors = ds.dsBmih.biClrUsed ? ds.dsBmih.biClrUsed : 1<<ds.dsBmih.biBitCount;//bpp to UINT
	if (ds.dsBmih.biBitCount>8 || nColors>256) return FALSE;//8bpp max

	HDC hDC=::CreateCompatibleDC(NULL);
	if (!hDC) return FALSE;
	HBITMAP hbmpOld=(HBITMAP)::SelectObject(hDC, *pDIB);
	if (hbmpOld!=NULL)
	{
		//replace color table entries
#ifndef _ATL_MIN_CRT
		RGBQUAD __clrtbl[256*sizeof(RGBQUAD)];
#endif
		if (::GetDIBColorTable(hDC, 0, nColors, __clrtbl))
		{
			UINT i;
			for (i=0; i<nColors; i++)
			{
				if (oldColor==RGB(__clrtbl[i].rgbRed, __clrtbl[i].rgbGreen, __clrtbl[i].rgbBlue))
				{
					__clrtbl[i].rgbRed=GetRValue(newColor);
					__clrtbl[i].rgbGreen=GetGValue(newColor);
					__clrtbl[i].rgbBlue=GetBValue(newColor);
					bRet=TRUE;
				}
			}
			if (bRet)//set new table
				if (!::SetDIBColorTable(hDC, 0, nColors, __clrtbl)) bRet=FALSE;
		}
	*pDIB=(HBITMAP)::SelectObject(hDC, hbmpOld);
	}
	::DeleteDC(hDC);
return bRet;
}


#ifndef _MAX_URL
#define _MAX_URL  2048 //IE7 limit
#endif

#ifndef _MAX_NTFS_PATH
#define _MAX_NTFS_PATH   0x8000
#endif

#define INI_KBLIMIT  32768 // 64kb ini file limit?

#ifndef _UNICODE
	#error IPersistFile requires UNICODE
#endif
#include <shlguid.h>
#include <shobjidl.h>
inline HRESULT ResolveShellShortcut(__in LPCTSTR lpszShortcutPath, __out LPTSTR lpszFilePath, __in size_t cchFilePath)
{
	_ASSERTE(lpszShortcutPath);
	_ASSERTE(lpszFilePath);
	_ASSERTE(cchFilePath>0);
	IShellLink* ipShellLink;
	HRESULT hRes=::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&ipShellLink);
	if (hRes==S_OK)
	{
		IPersistFile* ipPersistFile;
		hRes=ipShellLink->QueryInterface(IID_IPersistFile, (void**)&ipPersistFile);
		if (hRes==S_OK)
		{
			hRes=ipPersistFile->Load(lpszShortcutPath, STGM_READ);
			if (hRes==S_OK)
				hRes = ipShellLink->GetPath(lpszFilePath, (int)cchFilePath, NULL, SLGP_RAWPATH | SLGP_UNCPRIORITY);
			ipPersistFile->Release();
			ipPersistFile=NULL;
		}
		ipShellLink->Release();
		ipShellLink=NULL;
	}
return hRes;
}

inline HRESULT ResolveShellShortcut(__in IShellLink* pShellLink, __in LPCTSTR lpszShortcutPath, __out LPTSTR lpszFilePath, __in size_t cchFilePath)
{
	_ASSERTE(pShellLink);
	_ASSERTE(lpszShortcutPath);
	_ASSERTE(lpszFilePath);
	_ASSERTE(cchFilePath>0);
	IPersistFile* ipPersistFile;
	HRESULT hRes=pShellLink->QueryInterface(IID_IPersistFile, (void**)&ipPersistFile);
	if (hRes==S_OK)
	{
		hRes=ipPersistFile->Load(lpszShortcutPath, STGM_READ);
		if (hRes==S_OK)
			hRes = pShellLink->GetPath(lpszFilePath, (int)cchFilePath, NULL, SLGP_RAWPATH | SLGP_UNCPRIORITY);
		ipPersistFile->Release();
		ipPersistFile=NULL;
	}
return hRes;
}


inline BOOL IsFixedMedia(LPCTSTR pszPath)
{
	TCHAR drv[4]=_T("_:\\");
	drv[0]=pszPath[0];
return (GetDriveType(drv)==DRIVE_CDROM);
}


//extract path from shortcut
inline BOOL ResolveURLFile(LPCTSTR szUrl, LPTSTR pUrl, SIZE_T psLen)
{
return (BOOL)GetPrivateProfileString(_T("InternetShortcut"), _T("URL"), NULL, pUrl, (DWORD)psLen, szUrl);
}


//inline HRESULT HresultFromLastError() {DWORD dwErr=::GetLastError();return HRESULT_FROM_WIN32(dwErr);}
//inline HRESULT HresultFromWin32(DWORD nError) {return( HRESULT_FROM_WIN32(nError));}


//#define INSERTEDMENUITEMS(x)  MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_NULL,USHORT((x)+1))
//inline BOOL InsertMenuSeparator(HMENU hMenu, UINT uPosition, UINT_PTR uIDNewItem)
//{return ::InsertMenu(hMenu, uPosition, MF_SEPARATOR | MF_BYPOSITION, uIDNewItem, NULL);}


inline BOOL IsMenuItemSeparator(HMENU hmenu, UINT item, BOOL bByPosition)
{
	MENUITEMINFO mii;
	mii.cbSize=sizeof(MENUITEMINFO);
	mii.fMask=MIIM_FTYPE;
	if (::GetMenuItemInfo(hmenu, item, bByPosition, &mii)) return (mii.fType==MFT_SEPARATOR);
return FALSE;
}


#ifdef _DEBUG
#define dbgTraceCMFlags(x) _dbgTraceCMFlags((x))
static void _dbgTraceCMFlags(UINT uFlags)
{
	if (uFlags & CMF_NORMAL) ATLTRACE("CMF_NORMAL");
	if (uFlags & CMF_DEFAULTONLY) ATLTRACE("|CMF_DEFAULTONLY");
	if (uFlags & CMF_VERBSONLY) ATLTRACE("|CMF_VERBSONLY");
	if (uFlags & CMF_EXPLORE) ATLTRACE("|CMF_EXPLORE");
	if (uFlags & CMF_NOVERBS) ATLTRACE("|CMF_NOVERBS");
	if (uFlags & CMF_CANRENAME) ATLTRACE("|CMF_CANRENAME");
	if (uFlags & CMF_NODEFAULT) ATLTRACE("|CMF_NODEFAULT");
#if (NTDDI_VERSION < NTDDI_VISTA)
	if (uFlags & CMF_INCLUDESTATIC) ATLTRACE("|CMF_INCLUDESTATIC");
#endif
	if (uFlags & 0x00000080) ATLTRACE("|CMF_ITEMMENU");
	if (uFlags & CMF_EXTENDEDVERBS) ATLTRACE("|CMF_EXTENDEDVERBS");
	if (uFlags & 0x00000200) ATLTRACE("|CMF_DISABLEDVERBS");
	if (uFlags & CMF_RESERVED) ATLTRACE("|CMF_RESERVED");
	ATLTRACE("\n");
}

static void dbgTraceSFGAOFlags(ULONG uFlags)
{
	if (uFlags & SFGAO_STORAGE) ATLTRACE("|SFGAO_STORAGE");
	if (uFlags & SFGAO_CANRENAME) ATLTRACE("|SFGAO_CANRENAME");
	if (uFlags & SFGAO_CANDELETE) ATLTRACE("|SFGAO_CANDELETE");
	if (uFlags & SFGAO_HASPROPSHEET) ATLTRACE("|SFGAO_HASPROPSHEET");
	if (uFlags & SFGAO_DROPTARGET) ATLTRACE("|SFGAO_DROPTARGET");
	if (uFlags & SFGAO_SYSTEM) ATLTRACE("|SFGAO_SYSTEM");
	if (uFlags & SFGAO_ENCRYPTED) ATLTRACE("|SFGAO_ENCRYPTED");
	if (uFlags & SFGAO_ISSLOW) ATLTRACE("|SFGAO_ISSLOW");
	if (uFlags & SFGAO_GHOSTED) ATLTRACE("|SFGAO_GHOSTED");
	if (uFlags & SFGAO_LINK) ATLTRACE("|SFGAO_LINK");
	if (uFlags & SFGAO_SHARE) ATLTRACE("|SFGAO_SHARE");
	if (uFlags & SFGAO_READONLY) ATLTRACE("|SFGAO_READONLY");
	if (uFlags & SFGAO_HIDDEN) ATLTRACE("|SFGAO_HIDDEN");
	if (uFlags & SFGAO_FILESYSANCESTOR) ATLTRACE("|SFGAO_FILESYSANCESTOR");
	if (uFlags & SFGAO_FOLDER) ATLTRACE("|SFGAO_FOLDER");
	if (uFlags & SFGAO_FILESYSTEM) ATLTRACE("|SFGAO_FILESYSTEM");
	if (uFlags & SFGAO_HASSUBFOLDER) ATLTRACE("|SFGAO_HASSUBFOLDER");
	if (uFlags & SFGAO_VALIDATE) ATLTRACE("|SFGAO_VALIDATE");
	if (uFlags & SFGAO_REMOVABLE) ATLTRACE("|SFGAO_REMOVABLE");
	if (uFlags & SFGAO_COMPRESSED) ATLTRACE("|SFGAO_COMPRESSED");
	if (uFlags & SFGAO_BROWSABLE) ATLTRACE("|SFGAO_BROWSABLE");
	if (uFlags & SFGAO_NONENUMERATED) ATLTRACE("|SFGAO_NONENUMERATED");
	if (uFlags & SFGAO_NEWCONTENT) ATLTRACE("|SFGAO_NEWCONTENT");
	if (uFlags & SFGAO_STREAM) ATLTRACE("|SFGAO_STREAM");
	if (uFlags & SFGAO_STORAGEANCESTOR) ATLTRACE("|SFGAO_STORAGEANCESTOR");
	ATLTRACE("\n");
}


#else
#define dbgTraceCMFlags(x)
#define dbgTraceSFGAOFlags(x)
#endif//_DEBUG




#endif//_HELPERS_4469818A_9E07_4F0C_BDE9_E756E6C46A56_
