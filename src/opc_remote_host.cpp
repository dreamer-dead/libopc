#include <atlbase.h>

#include "OPC/OpcEnum.h"
#include "OPC/opccomn.h"
#include "OPC/opcda.h"
#include "opc_exception.hpp"
#include "opc_log.hpp"
#include "opc_remote_host.hpp"
#include "opc_server.hpp"

namespace opc
{
	namespace detail
	{
		static void make_remote_object(
			LPCOLESTR host_name,
			const IID requested_class,
			const IID requested_interface,
			void** interface_ptr)
		{
			/*
			COAUTHINFO athn;
			ZeroMemory(&athn, sizeof(COAUTHINFO));
			// Set up the NULL security information
			athn.dwAuthnLevel = RPC_C_AUTHN_LEVEL_CONNECT;
			//athn.dwAuthnLevel = RPC_C_AUTHN_LEVEL_NONE;
			athn.dwAuthnSvc = RPC_C_AUTHN_WINNT;
			athn.dwAuthzSvc = RPC_C_AUTHZ_NONE;
			athn.dwCapabilities = EOAC_NONE;
			athn.dwImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE;
			athn.pAuthIdentityData = NULL;
			athn.pwszServerPrincName = NULL;
			//*/

			WCHAR buffer[MAX_PATH] = {0};

			lstrcpynW( buffer, host_name, MAX_PATH - 1 );
			COSERVERINFO remoteServerInfo;
			ZeroMemory( &remoteServerInfo, sizeof(COSERVERINFO) );
			//remoteServerInfo.pAuthInfo = &athn;
			remoteServerInfo.pAuthInfo = NULL;
			remoteServerInfo.pwszName = buffer;

			DWPRINT( L"make_remote_object( %s )\n", host_name);

			MULTI_QI reqInterface;
			reqInterface.pIID = &requested_interface;
			reqInterface.pItf = NULL;
			reqInterface.hr = S_OK;

			HRESULT result = ::CoCreateInstanceEx(
				requested_class,
				NULL, CLSCTX_REMOTE_SERVER,
				&remoteServerInfo, 1, &reqInterface);

			if (FAILED(result))
			{
				DPRINT( "Error %x\n", result);
				throw opc_exception( result, OLESTR("Failed to get remote interface") );
			}

			*interface_ptr = reqInterface.pItf; // avoid ref counter getting incremented again
		}
	}

	remote_host::remote_host( LPCOLESTR host_name )
		: host_name_( host_name )
	{
		DWPRINT( L"remote_host::remote_host( %s )\n", host_name_.c_str() );

		detail::make_remote_object( host_name_.c_str(), CLSID_OpcServerList, IID_IOPCServerList, (void**)&root_);
	}

	void remote_host::get_clsid( const CATID& cat_id, LPCOLESTR server_name, CLSID& server_id )
	{
		DWPRINT( L"get_clsid( %s )\n", server_name );

		CATID Implist[1] = { cat_id };
		ATL::CComPtr<IEnumCLSID> iEnum;
		HRESULT result = root_->EnumClassesOfCategories( 1, Implist, 0, NULL, &iEnum );
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("Failed to get enum for categeories") );
		}

		GUID glist;
		ULONG actual;
		while((result = iEnum->Next( 1, &glist, &actual )) == S_OK)
		{
			cotask_holder<OLECHAR> progID;
			cotask_holder<OLECHAR> userType;
			HRESULT res = root_->GetClassDetails( glist, progID.addr(), userType.addr() );

			if(FAILED(res))
			{
				throw opc_exception( res, OLESTR("Failed to get ProgId from ClassId") );
			}
			else
			{
				DWPRINT( L"progId = %s\n", progID );
				if ( lstrcmpW( progID.get(), server_name ) == 0 )
				{
					res = root_->CLSIDFromProgID( progID.get(), &server_id );

					if ( FAILED(res) )
					{
						throw opc_exception( res, OLESTR("IOPCServerList::CLSIDFromProgID - Failed to get ClassId from ProgId") );
					}

					return;
				}
			}
		}
	}

	da_server * remote_host::connect_to( const CLSID& clsid )
	{
		ATL::CComPtr<IUnknown> unknown_ptr;
		detail::make_remote_object( host_name_.c_str(), clsid, IID_IUnknown, (void **)&unknown_ptr );

		ATL::CComPtr<IOPCServer> server_ptr;
		HRESULT result = unknown_ptr->QueryInterface( IID_IOPCServer, (void**)&server_ptr );
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("Failed to obtain IID_IOPCServer interface from server") );
		}

		return new opc::da_server( server_ptr );
	}
}
