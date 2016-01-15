/*
*
*/

#if defined _MSC_VER && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef OPC_GROUP_HPP
#define OPC_GROUP_HPP

#include <string>
#include <list>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atlbase.h>

#include "OPC/opcda.h"

namespace opc
{
	struct item;

	struct item_data_changed;

	class group
	{
	public:
		group( LPCWSTR name, bool active, DWORD updateMs, FLOAT deadBand, IOPCServer * serverPtr );
		void remove( bool force );

		item * add_item( LPCWSTR name, bool active );
		void clear();

		void enable_callback( item_data_changed * ptr );
		void enable_callback_unk( void * ptr );
		void disable_callback();

		void item_changed(
			DWORD transaction, OPCHANDLE client,
			const VARIANT& v, const FILETIME& ft, const WORD qual
			);

		ATL::CComPtr<IOPCItemMgt>& item_mgt() { return item_mgt_; }
		ATL::CComPtr<IOPCGroupStateMgt>& group_mgt() { return state_mgt_; }

		~group();

	protected:
		friend struct item;

		group( const group& ) = delete;
		group& operator=( const group& ) = delete;

		IOPCSyncIO * sync_io() const { return sync_io_; }

		std::wstring name_;
		IOPCServer * server_ptr_ = nullptr;
		std::list< item * > items_;
		item_data_changed * data_changed_ptr_ = nullptr;

		// OPC
		OPCHANDLE handle_ = 0;
		ATL::CComPtr<IOPCGroupStateMgt>	state_mgt_;
		ATL::CComPtr<IOPCSyncIO>		sync_io_;
		ATL::CComPtr<IOPCItemMgt>		item_mgt_;
		ATL::CComPtr<IOPCDataCallback>	data_callback_;
		DWORD advise_hint_ = 0;
	};
}

#endif //OPC_GROUP_HPP
