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
	struct local_host final : public host_base< ICatInformation >
	{
		local_host();
		~local_host() final {}

	protected:
		// host_base<ICatInformation> overrides:
		da_server * connect_to( const CLSID& clsid ) final;
		void get_clsid( const CATID& cat_id, LPCOLESTR server_name, CLSID& server_id ) final;
	};
}

#endif //OPC_LOCALHOST_HPP
