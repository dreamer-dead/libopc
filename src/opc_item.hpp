/*
*
*/

#if defined _MSC_VER && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef OPC_ITEM_HPP
#define OPC_ITEM_HPP

#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atlbase.h>

#include "OPC/opcda.h"
#include "opc_group.hpp"

namespace opc
{
	struct item
	{
		struct data
		{
			FILETIME ftTimeStamp;
			WORD wQuality;
			VARIANT vDataValue;
			HRESULT error;

			explicit data( HRESULT err ) : error( err )
			{
				vDataValue.vt = VT_EMPTY;
			}

			data()
			{
				vDataValue.vt = VT_EMPTY;
			}

			data( const FILETIME& time, WORD qual, VARIANT val, HRESULT err );

			~data()
			{
				VariantClear( &vDataValue );
			}

			void set(OPCITEMSTATE &itemState);
			void set(FILETIME time, WORD qual, VARIANT & val);
		};

		item( LPCWSTR name, bool active, group& g );

		void read( data& v, OPCDATASOURCE source );

		OPCHANDLE client() const
		{
			return client_;
		}

		LPCWSTR name() const { return &name_[0]; }

	protected:
		group& owner_;

		// OPC
		OPCHANDLE handle_;
		OPCHANDLE client_;
		VARTYPE data_type_;
		DWORD access_rights_;
		std::vector< WCHAR > name_;

	private:
		item(const item&) = delete;
		item& operator=(const item&) = delete;
	};

	struct item_data_changed
	{
		virtual ~item_data_changed() {}

		virtual void data_changed( item * it, const item::data& update ) = 0;
	};

}

#endif //OPC_ITEM_HPP
