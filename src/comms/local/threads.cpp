#include "threads.h"

namespace chat {
namespace comms {
namespace local {
const thread thread::empty = thread((My_THREAD_HANDLE)NULL);
}
}  // namespace comms
}  // namespace chat