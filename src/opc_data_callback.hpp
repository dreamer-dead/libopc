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
	class /* ATL_NO_VTABLE */ data_callback :
		public CComObjectRootEx< CComMultiThreadModel >,
		public IOPCDataCallback
	{
	private:

		DWORD advise_hint_ = 0;
		ATL::CComPtr< IOPCItemMgt > group_ptr_;
		group& owner_;

		data_callback(const data_callback&) = delete;
		data_callback& operator=(const data_callback&) = delete;

	public:
		explicit data_callback( group& grp );
		~data_callback();

		void advise();
		void unadvise();

		// IUnknown:
		STDMETHOD(QueryInterface)( REFIID iid, LPVOID* ppInterface);
		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();

		// IOPCDataCallback:
		// Minimal set of callback methods to work with OPC server.
		STDMETHOD(OnDataChange)( DWORD dwTransid, OPCHANDLE hGroup, HRESULT hrMasterquality, HRESULT hrMastererror, DWORD dwCount, OPCHANDLE *phClientItems, VARIANT *pvValues, WORD *pwQualities, FILETIME *pftTimeStamps, HRESULT *pErrors);
		STDMETHOD(OnReadComplete)( DWORD dwTransid, OPCHANDLE hGroup, HRESULT hrMasterquality, HRESULT hrMastererror, DWORD dwCount, OPCHANDLE *phClientItems, VARIANT *pvValues, WORD *pwQualities, FILETIME *pftTimeStamps, HRESULT *pErrors);
		STDMETHOD(OnWriteComplete)( DWORD dwTransid, OPCHANDLE hGroup, HRESULT hrMastererr, DWORD dwCount, OPCHANDLE *pClienthandles, HRESULT *pErrors);
		STDMETHOD(OnCancelComplete)( DWORD dwTransid, OPCHANDLE hGroup);
	};
}

#endif //OPC_DATACALLBACK_HPP
