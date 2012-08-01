/*
*
*/

#if defined _MSC_VER && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef OPC_DATA_CALLBACK_HPP
#define OPC_DATA_CALLBACK_HPP

#include <atlbase.h>
#include <atlcom.h>

#include "OPC/opccomn.h"
#include "OPC/OpcEnum.h"
#include "OPC/opcda.h"

#include "opc_group.hpp"

namespace opc 
{
	// {54DECFF2-2ADE-49a0-BEE0-EF3D8E616AE2}
	//const GUID CLSID_VistDataCallback;
	DEFINE_GUID(CLSID_VistDataCallback, 0x54decff2, 0x2ade, 0x49a0, 0xbe, 0xe0, 0xef, 0x3d, 0x8e, 0x61, 0x6a, 0xe2 );

	class /* ATL_NO_VTABLE */ data_callback :
		public CComObjectRootEx< CComMultiThreadModel >,		
		public IOPCDataCallback
	{
	private:				

		DWORD m_dwAdvise;
		ATL::CComPtr< IOPCItemMgt > group_ptr_;
		group& owner_;

	public:
		data_callback( group& grp  );
		~data_callback();

		void advise();
		void unadvise();

		STDMETHOD(QueryInterface)( REFIID iid, LPVOID* ppInterface);
		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();

		// методы для обратной связи с OPC сервером (IOPCDataCallback)
		STDMETHOD(OnDataChange)( DWORD dwTransid, OPCHANDLE hGroup, HRESULT hrMasterquality, HRESULT hrMastererror, DWORD dwCount, OPCHANDLE *phClientItems, VARIANT *pvValues, WORD *pwQualities, FILETIME *pftTimeStamps, HRESULT *pErrors);
		STDMETHOD(OnReadComplete)( DWORD dwTransid, OPCHANDLE hGroup, HRESULT hrMasterquality, HRESULT hrMastererror, DWORD dwCount, OPCHANDLE *phClientItems, VARIANT *pvValues, WORD *pwQualities, FILETIME *pftTimeStamps, HRESULT *pErrors);
		STDMETHOD(OnWriteComplete)( DWORD dwTransid, OPCHANDLE hGroup, HRESULT hrMastererr, DWORD dwCount, OPCHANDLE *pClienthandles, HRESULT *pErrors);
		STDMETHOD(OnCancelComplete)( DWORD dwTransid, OPCHANDLE hGroup);
	};	
}

#endif //OPC_DATACALLBACK_HPP
