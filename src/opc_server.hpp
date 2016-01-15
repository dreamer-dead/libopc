/*
*
*/

#if defined _MSC_VER && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef OPC_SERVER_HPP
#define OPC_SERVER_HPP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atlbase.h>

#include "OPC/opcda.h"

namespace opc
{
	class group;

	struct da_server
	{
		struct status
		{
			FILETIME ftStartTime;
			FILETIME ftCurrentTime;
			FILETIME ftLastUpdateTime;
			OPCSERVERSTATE dwServerState;
			DWORD dwGroupCount;
			DWORD dwBandWidth;
			WORD wMajorVersion;
			WORD wMinorVersion;
			WORD wBuildNumber;
			WCHAR vendorInfo[128];
		};

		ATL::CComPtr<IOPCServer> opc_server_;

		explicit da_server( ATL::CComPtr<IOPCServer>& srv )
			: opc_server_( srv )
		{
		}

		IOPCServer * get_server() const
		{
			return opc_server_;
		}

		void get_status( status& stat );

		group * add_group( LPCWSTR group_name, bool active, DWORD update_ms, FLOAT deadBand );
	};
}

#endif //OPC_SERVER_HPP
