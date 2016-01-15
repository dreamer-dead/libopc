#include <ObjBase.h>

#include "opc_client.hpp"
#include "opc_exception.hpp"

namespace opc
{
	void initialize()
	{
		HRESULT	result = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("CoInitialize failed") );
		}

		result = ::CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("CoInitializeSecurity failed") );
		}

#if 0
		if ( memManager )
		{
			result = ::CoGetMalloc(MEMCTX_TASK, memManager);
			if (FAILED(result))
			{
				throw opc_exception( result, OLESTR("CoGetMalloc failed") );
			}
		}
#endif
	}

	void shutdown()
	{
		::CoUninitialize();
	}
}
