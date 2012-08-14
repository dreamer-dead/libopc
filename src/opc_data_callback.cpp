#include "opc_exception.hpp"
#include "opc_data_callback.hpp"

namespace opc 
{
	data_callback::data_callback( group& grp )
		: group_ptr_( grp.item_mgt() )
		, owner_(grp)
	{				
	}

	void data_callback::advise()
	{
#if defined _DEBUG && defined _CONSOLE
		printf( "data_callback::advise()\n" );
#endif

		// устанавливаем обратную связь

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
		// Вместо кода выше, можно просто вот так сделать, через AtlAdvise
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


// Отключим ворнинги про неиспользуемые переменные, их очень много в этих 2х методах
#pragma warning( push, 4 )
#pragma warning( disable: 4100 )

	STDMETHODIMP data_callback::OnDataChange( 
		DWORD dwTransid, OPCHANDLE hGroup, 
		HRESULT hrMasterquality, HRESULT hrMastererror, 
		DWORD dwCount, OPCHANDLE *phClientItems, 
		VARIANT *pvValues, WORD *pwQualities, 
		FILETIME *pftTimeStamps, HRESULT *pErrors)
	{
		// TODO: получили обновления тегов

#if defined _DEBUG && defined _CONSOLE
		wprintf( L"data_callback::OnDataChange( trans = %u, hGroup = 0x%X, count = %u )\n", dwTransid, hGroup, dwCount );
		for( DWORD ii = 0; ii < dwCount; ++ii )
		{
			if ( SUCCEEDED(pErrors[ii]) )
			{
				printf("hClient = 0x%X", phClientItems[ii]);

				switch (pvValues[ii].vt)
				{
					case VT_R4: printf("%f\n", pvValues[ii].fltVal); break;
					case VT_I4: printf("%d\n", pvValues[ii].iVal); break;
					case VT_I2: printf("%d\n", pvValues[ii].iVal); break;
					case VT_I1: {int v = pvValues[ii].bVal; printf("%d\n", v);} break;
					default: printf("\n");break;
				}
			}
		}
#endif
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
#if defined _DEBUG && defined _CONSOLE
		wprintf( L"data_callback::OnReadComplete\n" );
#endif

		return S_OK;
	}

	STDMETHODIMP data_callback::OnWriteComplete( 
		DWORD dwTransid, OPCHANDLE hGroup, 
		HRESULT hrMastererr, DWORD dwCount, 
		OPCHANDLE *pClienthandles, HRESULT *pErrors)
	{
#if defined _DEBUG && defined _CONSOLE
		wprintf( L"data_callback::OnWriteComplete\n" );
#endif

		return S_OK;
	}

	STDMETHODIMP data_callback::OnCancelComplete( DWORD dwTransid, OPCHANDLE hGroup)
	{
#if defined _DEBUG && defined _CONSOLE
		wprintf( L"data_callback::OnCancelComplete\n" );
#endif

		return S_OK;
	}

	//*
	STDMETHODIMP data_callback::QueryInterface( REFIID iid, LPVOID* ppInterface)
	{
		if ( ppInterface == NULL)
		{
			return E_INVALIDARG;
		}

#if defined _DEBUG && defined _CONSOLE
		LPOLESTR str = NULL;
		if ( SUCCEEDED( StringFromIID( iid, &str ) ) )
		{
			wprintf( L"data_callback::QueryInterface( %s )\n", str );
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

// восстановим варнинги
#pragma warning( pop )


	STDMETHODIMP_(ULONG) data_callback::AddRef()
	{
#if defined _DEBUG && defined _CONSOLE
		wprintf( L"data_callback::AddRef\n" );
#endif
		return CComObjectRoot::InternalAddRef();
	}


	STDMETHODIMP_(ULONG) data_callback::Release()
	{
#if defined _DEBUG && defined _CONSOLE
		wprintf( L"data_callback::Release\n" );
#endif
		// функция не обязательно вернет нам счетчик! см. http://msdn.microsoft.com/en-us/library/k2dfb2wa(v=vs.80).aspx
		CComObjectRoot::InternalRelease(); 

		if ( m_dwRef == 0){
			delete this;

			return 0;
		}

		return m_dwRef;
	}
	//*/
}
