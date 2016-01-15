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

		// ������������� �������� �����

		/*
		ATL::CComPtr<IConnectionPointContainer> iConnectionPointContainer;
		HRESULT result = group_ptr_->QueryInterface(IID_IConnectionPointContainer, (void**)&iConnectionPointContainer);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("Could not get IID_IConnectionPointContainer") );
		}

		ATL::CComPtr<IConnectionPoint> iAsynchDataCallbackConnectionPoint;
		result = iConnectionPointContainer->FindConnectionPoint(IID_IOPCDataCallback, &iAsynchDataCallbackConnectionPoint);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("Could not get IID_IOPCDataCallback") );
		}

		result = iAsynchDataCallbackConnectionPoint->Advise( (IUnknown *)this, &m_dwAdvise);
		if (FAILED(result))
		{
			iAsynchDataCallbackConnectionPoint = NULL;
			throw opc_exception( result, OLESTR("Failed to set DataCallbackConnectionPoint") );
		}
		//*/

		//*
		// ������ ���� ����, ����� ������ ��� ��� �������, ����� AtlAdvise
		m_dwAdvise = 0;
		HRESULT res = ATL::AtlAdvise(group_ptr_, (IUnknown*)this, IID_IOPCDataCallback, &m_dwAdvise);

		if ( FAILED(res) )
		throw opc_exception( res, OLESTR("AtlAdvise failed!") );
		//*/
	}

	void data_callback::unadvise()
	{
		HRESULT res = ATL::AtlUnadvise((IUnknown*)group_ptr_, IID_IOPCDataCallback, m_dwAdvise);

		/*
		if ( FAILED(res) )
		throw opc_exception( res, OLESTR("AtlUnadvise failed!") );
		//*/
		m_dwAdvise = 0;
	}

	data_callback::~data_callback()
	{
	}


// �������� �������� ��� �������������� ����������, �� ����� ����� � ���� 2� �������
#pragma warning( push, 4 )
#pragma warning( disable: 4100 )

	STDMETHODIMP data_callback::OnDataChange(
		DWORD dwTransid, OPCHANDLE hGroup,
		HRESULT hrMasterquality, HRESULT hrMastererror,
		DWORD dwCount, OPCHANDLE *phClientItems,
		VARIANT *pvValues, WORD *pwQualities,
		FILETIME *pftTimeStamps, HRESULT *pErrors)
	{
		// TODO: �������� ���������� �����

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
		LPOLESTR str = NULL;
		if ( SUCCEEDED( StringFromIID( iid, &str ) ) )
		{
			DWPRINT( L"data_callback::QueryInterface( %s )\n", str );
		}

		if ( str )
			::CoTaskMemFree( str );
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

// ����������� ��������
#pragma warning( pop )


	STDMETHODIMP_(ULONG) data_callback::AddRef()
	{
		DPRINT( L"data_callback::AddRef\n" );

		return CComObjectRoot::InternalAddRef();
	}


	STDMETHODIMP_(ULONG) data_callback::Release()
	{
		DPRINT( L"data_callback::Release\n" );

		// ������� �� ����������� ������ ��� �������! ��. http://msdn.microsoft.com/en-us/library/k2dfb2wa(v=vs.80).aspx
		CComObjectRoot::InternalRelease();

		if ( m_dwRef == 0){
			delete this;

			return 0;
		}

		return m_dwRef;
	}
	//*/
}
