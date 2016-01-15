/*
*
*/

#if defined _MSC_VER && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef OPC_REMOTE_HOST_HPP
#define OPC_REMOTE_HOST_HPP

#include <string>
#include <comcat.h>

#include "OPC/OpcEnum.h"

#include "opc_host.hpp"

namespace opc
{
	struct remote_host final : public host_base< IOPCServerList >
	{
		explicit remote_host( LPCOLESTR host_name );
		~remote_host() final {}

	protected:
		void get_clsid( const CATID& cat_id, LPCOLESTR server_name, CLSID& server_id ) final;
		da_server * connect_to( const CLSID& clsid ) final;

	private:
		remote_host(const remote_host&) = delete;
		remote_host& operator=(const remote_host&) = delete;

		const std::basic_string< OLECHAR > host_name_;
	};
}

#endif //OPC_REMOTE_HOST_HPP
