/*
*
*/

#if defined _MSC_VER && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef OPC_LOCALHOST_HPP
#define OPC_LOCALHOST_HPP

#include "opc_host.hpp"

namespace opc 
{
	struct local_host : host_base< ICatInformation >
	{
		local_host();

	protected :
		virtual da_server * connect_to( const CLSID clsid );
		virtual void get_clsid( const CATID& cat_id, LPCOLESTR server_name, CLSID& server_id );

	private:
	};
}

#endif //OPC_LOCALHOST_HPP
