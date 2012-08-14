#if defined _MSC_VER && _MSC_VER >= 1300
#pragma once
#endif

#if !defined __HDR_UTILS_ENCODE_HPP__
#define __HDR_UTILS_ENCODE_HPP__

#include <string>
#include <locale>
#include <algorithm>

namespace utils
{
	namespace detail
	{
		struct locale_holder 
		{			
			locale_holder() : loc_( std::locale() ) {}
			locale_holder( const std::locale& loc ) : loc_(loc) {}

		protected:
			std::locale loc_;
		};

		struct narrow : locale_holder
		{
			char operator()( wchar_t c )
			{
				return std::use_facet< std::ctype<wchar_t> >(loc_).narrow(c, '?');
			}
		};

		struct widen : locale_holder
		{
			wchar_t operator()( char c )
			{
				return std::use_facet< std::ctype<wchar_t> >(loc_).widen(c);
			}
		};
	}

	template <typename TO, typename FROM > std::basic_string< TO > encode( const std::basic_string<FROM>& source );

	template <> inline std::string encode< char, char >( const std::string& source) { return source; }
	template <> inline std::wstring encode< wchar_t, wchar_t >( const std::wstring& source) { return source; }

	template <> inline std::string encode< char, wchar_t >( const std::wstring& source )
	{
		std::string result;
		result.resize( source.length() );
		std::transform(source.begin(), source.end(), result.begin(), detail::narrow());

		return result;
	}

	template <> inline std::wstring encode< wchar_t, char >( const std::string& source )
	{
		std::wstring result;
		result.resize( source.length() );
		std::transform(source.begin(), source.end(), result.begin(), detail::widen());

		return result;
	}
}

#endif /* __HDR_UTILS_ENCODE_HPP__ */
