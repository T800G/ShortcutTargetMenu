

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat Nov 02 23:37:22 2013
 */
/* Compiler settings for .\TargetShell.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __TargetShell_h_h__
#define __TargetShell_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IShellCtxMenu_FWD_DEFINED__
#define __IShellCtxMenu_FWD_DEFINED__
typedef interface IShellCtxMenu IShellCtxMenu;
#endif 	/* __IShellCtxMenu_FWD_DEFINED__ */


#ifndef __ShellCtxMenu_FWD_DEFINED__
#define __ShellCtxMenu_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellCtxMenu ShellCtxMenu;
#else
typedef struct ShellCtxMenu ShellCtxMenu;
#endif /* __cplusplus */

#endif 	/* __ShellCtxMenu_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IShellCtxMenu_INTERFACE_DEFINED__
#define __IShellCtxMenu_INTERFACE_DEFINED__

/* interface IShellCtxMenu */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IShellCtxMenu;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("329214E1-015B-4727-8E89-8F3DE9AB68FB")
    IShellCtxMenu : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IShellCtxMenuVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellCtxMenu * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellCtxMenu * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellCtxMenu * This);
        
        END_INTERFACE
    } IShellCtxMenuVtbl;

    interface IShellCtxMenu
    {
        CONST_VTBL struct IShellCtxMenuVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellCtxMenu_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IShellCtxMenu_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IShellCtxMenu_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IShellCtxMenu_INTERFACE_DEFINED__ */



#ifndef __TargetShellLib_LIBRARY_DEFINED__
#define __TargetShellLib_LIBRARY_DEFINED__

/* library TargetShellLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_TargetShellLib;

EXTERN_C const CLSID CLSID_ShellCtxMenu;

#ifdef __cplusplus

class DECLSPEC_UUID("7FFBA480-8BC9-443D-989F-5EBCE907569C")
ShellCtxMenu;
#endif
#endif /* __TargetShellLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


