#include "base.h"

namespace chat {
namespace os {
#ifndef __unix__
HANDLE hConsole = 0;
WORD defaultAttributes = 0;
#endif
}  // namespace os

#ifdef CHAT_LOG
std::streambuf* oldLogBuf = NULL;
std::ofstream logStream = std::ofstream();
#endif
}  // namespace chat
