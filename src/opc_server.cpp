#include "opc_server.hpp"
#include "opc_group.hpp"
#include "opc_exception.hpp"
#include "opc_memory.hpp"

namespace opc 
{
	void da_server::get_status( da_server::status& stat )
	{
		cotask_holder<OPCSERVERSTATUS> serverStatus;
		HRESULT result = opc_server_->GetStatus( serverStatus.addr() );
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("Failed to get status") );
		}

		stat.ftStartTime = serverStatus->ftStartTime;
		stat.ftCurrentTime = serverStatus->ftCurrentTime;
		stat.ftLastUpdateTime = serverStatus->ftLastUpdateTime;
		stat.dwServerState = serverStatus->dwServerState;
		stat.dwGroupCount = serverStatus->dwGroupCount;
		stat.dwBandWidth = serverStatus->dwBandWidth;
		stat.wMajorVersion = serverStatus->wMajorVersion;
		stat.wMinorVersion = serverStatus->wMinorVersion;
		stat.wBuildNumber = serverStatus->wBuildNumber;
		if (serverStatus->szVendorInfo != NULL)
		{
			lstrcpynW( stat.vendorInfo, serverStatus->szVendorInfo, sizeof( stat.vendorInfo ) / sizeof(stat.vendorInfo[0]) );
			::CoTaskMemFree( serverStatus->szVendorInfo );	
		}
	}

	group * da_server::add_group( LPCWSTR group_name, bool active, DWORD update_ms, FLOAT deadBand )
	{
#if defined _DEBUG && defined _CONSOLE
		wprintf( L"da_server::add_group( %s, %s, %u, %f )\n", group_name, (active ? L"active" : L"inactive" ), update_ms, deadBand );
#endif

		return new group( group_name, active, update_ms, deadBand, opc_server_ );
	}
}
