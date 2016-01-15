#include "opc_exception.hpp"
#include "opc_local_host.hpp"
#include "opc_log.hpp"
#include "opc_server.hpp"

namespace opc
{
	local_host::local_host()
	{
		HRESULT result = ::CoCreateInstance (CLSID_StdComponentCategoriesMgr, NULL,CLSCTX_INPROC_SERVER, IID_ICatInformation,(void **)&root_);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("Failed to get IID_ICatInformation") );
		}
	}

	void local_host::get_clsid( const CATID& cat_id, LPCOLESTR server_name, CLSID& server_id )
	{
		DWPRINT( L"get_clsid( %s )\n", server_name );

		CATID Implist[1] = { cat_id };
		ATL::CComPtr<IEnumCLSID> iEnum;
		HRESULT result = root_->EnumClassesOfCategories(1, Implist,0, NULL, &iEnum);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("Failed to get enum for categeories") );
		}

		GUID glist;
		ULONG actual;
		while((result = iEnum->Next(1, &glist, &actual)) == S_OK)
		{
			cotask_holder<OLECHAR> progID;

			HRESULT res = ProgIDFromCLSID(glist, progID.addr());
			if(FAILED(res))
			{
				throw opc_exception( res, OLESTR("Failed to get ProgId from ClassId") );
			}
			else
			{
				DWPRINT( L"progId = %s\n", progID );
				if ( lstrcmpW( progID.get(), server_name ) == 0 )
				{
					server_id = glist;

					return;
				}
			}
		}
	}

	da_server * local_host::connect_to( const CLSID& clsid )
	{
		ATL::CComPtr<IClassFactory> class_factory_ptr;
		HRESULT result = ::CoGetClassObject(clsid, CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&class_factory_ptr);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("Failed get Class factory") );
		}

		ATL::CComPtr<IUnknown> unknown_ptr;
		result = class_factory_ptr->CreateInstance(NULL, IID_IUnknown, (void**)&unknown_ptr);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("Failed get create OPC server ref") );
		}

		ATL::CComPtr<IOPCServer> server_ptr;
		result = unknown_ptr->QueryInterface(IID_IOPCServer, (void**)&server_ptr);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("Failed obtain IID_IOPCServer interface from server") );
		}

		return new opc::da_server( server_ptr );
	}
}
