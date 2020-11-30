#ifndef CHAT_COMMS_LOCAL_SHMEMORY_H_
#define CHAT_COMMS_LOCAL_SHMEMORY_H_

#include <exceptions/messages.h>
#include <internal/base.h>

#ifdef __unix__
#include <sys/shm.h>
typedef int My_CHANNEL_ID_TYPE;
typedef void* My_SHM_TARGET_TYPE;
#else
#include <windows.h>
#ifdef UNICODE
typedef LPCWSTR My_CHANNEL_ID_TYPE;
#else
typedef LPCSTR My_CHANNEL_ID_TYPE;
#endif
typedef LPVOID My_SHM_TARGET_TYPE;
#endif

namespace chat {
namespace comms {
namespace local {

class shmemory {
  My_HANDLE handle = 0;
  My_CHANNEL_ID_TYPE channel_id;

 public:
  shmemory(My_CHANNEL_ID_TYPE channel_id) : channel_id(channel_id) {}

  void attach(My_SHM_TARGET_TYPE* target, std::size_t size, bool& created) {
    if (handle) throw exception("ERR690", exceptions::ERR690);

#ifdef __unix__
    handle = shmget(channel_id, size, 0666);
#else
    handle = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, channel_id);
#endif

    if (os::isError(handle)) {
      if (os::getLastError() == ENOENT) {
#ifdef __unix__
        handle = shmget(channel_id, size, 0666 | IPC_CREAT);
#else
        handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
                                   0, size, channel_id);
#endif
        if (os::isError(handle)) {
          My_ERROR_CODE error = os::getLastError();
          if (os::isPermisionError(error))
            throw exception("ERR190", exceptions::ERR190);
          throw exception("ERR290",
                          exceptions::format(exceptions::ERR290, error));
        }

        created = true;
      } else {
        My_ERROR_CODE error = os::getLastError();
        if (os::isPermisionError(error))
          throw exception("ERR191", exceptions::ERR191);
        throw exception("ERR291",
                        exceptions::format(exceptions::ERR291, error));
      }
    } else {
      created = false;
    }

#ifdef __unix__
    (*target) = shmat(handle, 0, 0);
#else
    (*target) = MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, size);
#endif

    if (os::isErrorPointer(target)) {
      My_ERROR_CODE error = os::getLastError();
      if (os::isPermisionError(error))
        throw exception("ERR192", exceptions::ERR192);
      throw exception("ERR292", exceptions::format(exceptions::ERR292, error));
    }
  }

  void detach(My_SHM_TARGET_TYPE* target, bool clear) {
    if (!handle) throw exception("ERR691", exceptions::ERR691);

#ifdef __unix__
    shmdt(target);
    if (clear) {
      shmctl(handle, IPC_RMID, 0);
    }
#else
    UnmapViewOfFile(*target);
    CloseHandle(handle);
#endif
  }
};

}  // namespace local
}  // namespace comms
}  // namespace chat
#endif