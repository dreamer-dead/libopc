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
#include "opc_memory.hpp"

namespace opc 
{
	struct group;

	struct da_server //: protected memory_manager
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

		da_server( /* IMalloc * manager, */ATL::CComPtr<IOPCServer>& srv )
			: opc_server_( srv )
			//, memory_manager( manager )
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
