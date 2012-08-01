#include <string>
#include <algorithm>

#include "opc_host.hpp"
#include "opc_remote_host.hpp"
#include "opc_local_host.hpp"

namespace opc 
{
	host * make_host( LPCOLESTR address )
	{
		std::basic_string< OLECHAR > addr( address );

		std::transform( addr.begin(), addr.end(), addr.begin(), &::tolower );
		if ( addr == OLESTR("localhost") || addr == OLESTR("127.0.0.1") )
		{
			// localhost

			return make_local_host();
		}
		else 
		{
			// remote host 

			return new remote_host( /* memManager, */ address );
		}
	}

	host * make_local_host()
	{
		return new local_host();
	}
}
