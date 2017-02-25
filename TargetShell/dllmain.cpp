// TargetProp.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "TargetShell_h.h"
#include "resource.h"
#include "shellctxmenu.h"


class CTargetShellModule : public CAtlDllModuleT< CTargetShellModule >
{
//public :
//	DECLARE_LIBID(LIBID_TargetShellLib)
//	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_TARGETSHELL, "{54E0A0A9-5267-45FF-8F1B-7A8AEDAB5D68}")
} _AtlModule;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved); 
}

#ifdef _MANAGED
#pragma managed(pop)
#endif




// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer(FALSE);
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer(FALSE);
	return hr;
}

