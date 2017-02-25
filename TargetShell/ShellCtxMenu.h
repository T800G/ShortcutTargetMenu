#ifndef __SHELLCTXMENU_H
#define __SHELLCTXMENU_H

#include "resource.h"
#define STRICT_TYPED_ITEMIDS
#include <shlobj.h>
#include <Shobjidl.h>
#include <comdef.h>
#include "TargetShell_h.h"
#include "helpers.h"
#include "resload.h"

class ATL_NO_VTABLE CShellCtxMenu : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShellCtxMenu, &CLSID_ShellCtxMenu>,
	public IShellExtInit,
	public IContextMenu
{
public:
// Construction
	CShellCtxMenu(): m_pszNtfsPath(NULL), m_hMenuBmp(NULL),
					m_uInserted(0), m_pPidl(NULL), m_sfgao(0) { }
	//virtual ~CShellCtxMenu();

	BEGIN_COM_MAP(CShellCtxMenu)
		COM_INTERFACE_ENTRY(IShellExtInit)
		COM_INTERFACE_ENTRY(IContextMenu)
	END_COM_MAP()

	DECLARE_REGISTRY_RESOURCEID(IDR_SHELLCTXMENU)
	DECLARE_NOT_AGGREGATABLE(CShellCtxMenu)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

	// IShellExtInit
	STDMETHOD(Initialize)(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

	// IContextMenu
	STDMETHOD(GetCommandString)(UINT_PTR, UINT, UINT*, LPSTR, UINT);
	STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO);
	STDMETHOD(QueryContextMenu)(HMENU, UINT, UINT, UINT, UINT);

private:
	TCHAR* m_pszNtfsPath;
	PIDLIST_ABSOLUTE m_pPidl;
	SFGAOF m_sfgao;
	HBITMAP m_hMenuBmp;
	UINT m_uInserted;
};

OBJECT_ENTRY_AUTO(__uuidof(ShellCtxMenu), CShellCtxMenu)

#endif // __SHELLCTXMENU_H


