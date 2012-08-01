/*
*
*/

#if defined _MSC_VER && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef OPC_MEMORY_HPP
#define OPC_MEMORY_HPP

#include <ObjBase.h>

namespace opc 
{
	struct memory_manager 
	{
		memory_manager( IMalloc * manager )
			: memory_manager_(manager)
		{}

		void free( void * p )
		{
			memory_manager_->Free( p );
		}

		IMalloc * memory_manager_;

		virtual ~memory_manager(){}
	};

	template < typename T >
	struct memory_holder
	{
		T * value;
		memory_manager& manager;

		memory_holder( memory_manager& m, T * const v )
			: manager(m), value( v )
		{}

		memory_holder( memory_manager& m )
			: manager(m), value( NULL )
		{}

		T* get() const 
		{
			return value;
		}

		T** addr() 
		{
			return &value;
		}

		T& operator [] ( unsigned int idx )
		{
			return value[idx];
		}

		const T& operator [] ( unsigned int idx ) const
		{
			return value[idx];
		}

		~memory_holder() 
		{
			if ( NULL != value )
				manager.free( value );
		}
	};

	template < typename T >
	struct cotask_holder
	{
		T * value;

		cotask_holder( T * const v )
			: value( v )
		{}

		cotask_holder()
			: value( NULL )
		{}

		T* get() const 
		{
			return value;
		}

		T** addr() 
		{
			return &value;
		}

		T& operator [] ( unsigned int idx )
		{
			return value[idx];
		}

		const T& operator [] ( unsigned int idx ) const
		{
			return value[idx];
		}

		T* operator -> ()
		{
			return value;
		}

		~cotask_holder() 
		{
			if ( NULL != value )
			{
				::CoTaskMemFree( value );
				value = NULL;
			}
		}
	};
}

#endif //OPC_MEMORY_HPP
