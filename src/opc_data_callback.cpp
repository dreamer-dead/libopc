#include "opc_data_callback.hpp"
#include "opc_exception.hpp"
#include "opc_log.hpp"

namespace
{
#if DEBUG_PRINT
void dump_clients_data(
	const OPCHANDLE *phClientItems,
	const VARIANT *pvValues,
	const HRESULT *pErrors,
	DWORD dwCount )
{
	for( DWORD ii = 0; ii < dwCount; ++ii )
	{
		if ( SUCCEEDED(pErrors[ii]) )
		{
			DPRINT("hClient = 0x%X", phClientItems[ii]);

			switch (pvValues[ii].vt)
			{
				case VT_R4: DPRINT("%f\n", pvValues[ii].fltVal); break;
				case VT_I4: DPRINT("%d\n", pvValues[ii].iVal); break;
				case VT_I2: DPRINT("%d\n", pvValues[ii].iVal); break;
				case VT_I1: {int v = pvValues[ii].bVal; DPRINT("%d\n", v);} break;
				default: DPRINT("\n"); break;
			}
		}
	}
}
#endif  // DEBUG_PRINT

// It is better to use ATL::AtlAdvise instead of this code.
DWORD advise_impl( IUnknown* pUnkCP, IUnknown* pUnk, const IID& iid )
{
	ATL::CComPtr<IConnectionPointContainer> iConnectionPointContainer;
	HRESULT result = pUnkCP->QueryInterface( IID_IConnectionPointContainer, (void**)&iConnectionPointContainer );
	if (FAILED(result))
	{
		throw opc::opc_exception( result, OLESTR("Could not get IID_IConnectionPointContainer") );
	}

	ATL::CComPtr<IConnectionPoint> iAsynchDataCallbackConnectionPoint;
	result = iConnectionPointContainer->FindConnectionPoint( iid, &iAsynchDataCallbackConnectionPoint );
	if (FAILED(result))
	{
		throw opc::opc_exception( result, OLESTR("Could not get IID_IOPCDataCallback") );
	}

	DWORD advise = 0;
	result = iAsynchDataCallbackConnectionPoint->Advise( pUnk, &advise );
	if (SUCCEEDED(result))
		return advise;

	throw opc::opc_exception( result, OLESTR("Failed to set DataCallbackConnectionPoint") );
}
}

namespace opc
{
	data_callback::data_callback( group& grp )
		: group_ptr_( grp.item_mgt() )
		, owner_(grp)
	{
	}

	void data_callback::advise()
	{
		DPRINT( "data_callback::advise()\n" );

		// Connecting self as a callback.
		advise_hint_ = 0;
		HRESULT res = ATL::AtlAdvise( group_ptr_, this, IID_IOPCDataCallback, &advise_hint_ );

		if ( FAILED(res) )
			throw opc_exception( res, OLESTR("AtlAdvise failed!") );
	}

	void data_callback::unadvise()
	{
		HRESULT res = ATL::AtlUnadvise( group_ptr_, IID_IOPCDataCallback, advise_hint_ );

		/*
		if ( FAILED(res) )
		throw opc_exception( res, OLESTR("AtlUnadvise failed!") );
		//*/
		(void)res;
		advise_hint_ = 0;
	}

	data_callback::~data_callback()
	{
	}


// Disable unused parameter warnings in these methods.
// There are a LOT of unused parameters.
#pragma warning( push, 4 )
#pragma warning( disable: 4100 )

	STDMETHODIMP data_callback::OnDataChange(
		DWORD dwTransid, OPCHANDLE hGroup,
		HRESULT hrMasterquality, HRESULT hrMastererror,
		DWORD dwCount, OPCHANDLE *phClientItems,
		VARIANT *pvValues, WORD *pwQualities,
		FILETIME *pftTimeStamps, HRESULT *pErrors)
	{
#if DEBUG_PRINT
		DPRINT( "data_callback::OnDataChange( trans = %u, hGroup = 0x%X, count = %u )\n", dwTransid, hGroup, dwCount );
		dump_clients_data(phClientItems, pvValues, pErrors, dwCount);
#endif // DEBUG_PRINT

		for( DWORD ii = 0; ii < dwCount; ++ii )
		{
			owner_.item_changed( dwTransid, phClientItems[ii], pvValues[ii], pftTimeStamps[ii], pwQualities[ii] );
		}

		return S_OK;
	}

	STDMETHODIMP data_callback::OnReadComplete(
		DWORD dwTransid, OPCHANDLE hGroup,
		HRESULT hrMasterquality, HRESULT hrMastererror,
		DWORD dwCount, OPCHANDLE *phClientItems,
		VARIANT *pvValues, WORD *pwQualities,
		FILETIME *pftTimeStamps, HRESULT *pErrors)
	{
		DPRINT( L"data_callback::OnReadComplete\n" );

		return S_OK;
	}

	STDMETHODIMP data_callback::OnWriteComplete(
		DWORD dwTransid, OPCHANDLE hGroup,
		HRESULT hrMastererr, DWORD dwCount,
		OPCHANDLE *pClienthandles, HRESULT *pErrors)
	{
		DPRINT( L"data_callback::OnWriteComplete\n" );

		return S_OK;
	}

	STDMETHODIMP data_callback::OnCancelComplete( DWORD dwTransid, OPCHANDLE hGroup)
	{
		DPRINT( L"data_callback::OnCancelComplete\n" );

		return S_OK;
	}

	//*
	STDMETHODIMP data_callback::QueryInterface( REFIID iid, LPVOID* ppInterface)
	{
		if ( ppInterface == NULL)
		{
			return E_INVALIDARG;
		}

#if DEBUG_PRINT
		{
			cotask_holder str;
			if ( SUCCEEDED( StringFromIID( iid, str.addr() ) ) )
			{
				DWPRINT( L"data_callback::QueryInterface( %s )\n", str.get() );
			}
		}

#endif

		if ( iid == IID_IUnknown )
		{
			*ppInterface = (IUnknown*) this;
		} else if ( iid == IID_IOPCDataCallback)
		{
			*ppInterface = (IOPCDataCallback*) this;
		} else
		{
			*ppInterface = NULL;
			return E_NOINTERFACE;
		}

		AddRef();
		return S_OK;
	}

// Restore warnings.
#pragma warning( pop )


	STDMETHODIMP_(ULONG) data_callback::AddRef()
	{
		DPRINT( L"data_callback::AddRef\n" );

		return CComObjectRoot::InternalAddRef();
	}


	STDMETHODIMP_(ULONG) data_callback::Release()
	{
		DPRINT( L"data_callback::Release\n" );

		// InternalRelease method not always returns a reference counter.
		// See http://msdn.microsoft.com/en-us/library/k2dfb2wa(v=vs.80).aspx
		CComObjectRoot::InternalRelease();

		if ( !m_dwRef )
		{
			delete this;
			return 0;
		}

		return m_dwRef;
	}
	//*/
}
