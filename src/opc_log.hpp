#if defined _MSC_VER && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef OPC_LOG_HPP
#define OPC_LOG_HPP

#if defined _DEBUG && defined _CONSOLE
#define DEBUG_PRINT 1
#define DPRINT printf
#define DWPRINT wprintf
#else
#define DEBUG_PRINT 0
#define DPRINT (void)
#define DWPRINT (void)
#endif

#endif //OPC_LOG_HPP
