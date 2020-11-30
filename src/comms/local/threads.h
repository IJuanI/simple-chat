#ifndef CHAT_COMMS_LOCAL_THREADS_H_
#define CHAT_COMMS_LOCAL_THREADS_H_

#include <exceptions/messages.h>
#include <internal/base.h>

#ifdef __unix__
#include <pthread.h>
typedef void *(*My_THREAD_FUNC)(void *);
#define My_THREAD_HEAD void *
typedef void *My_THREAD_ARGS_TYPE;
typedef pthread_t My_THREAD_HANDLE;
#else
typedef LPTHREAD_START_ROUTINE My_THREAD_FUNC;
#define My_THREAD_HEAD DWORD WINAPI
typedef LPVOID My_THREAD_ARGS_TYPE;
typedef HANDLE My_THREAD_HANDLE;
#endif

namespace chat {
namespace comms {
namespace local {
class thread {
 public:
  static const thread empty;

 private:
  My_THREAD_FUNC func;
  My_THREAD_ARGS_TYPE args;

  My_THREAD_HANDLE handle = 0;
  thread_id_t id;

  thread(My_THREAD_HANDLE handle) : handle(handle) {}

 public:
  static thread getCurrThread() {
    My_THREAD_HANDLE handle;
#ifdef __unix__
    handle = pthread_self();
#else
    handle = GetCurrentThread();
#endif

    return thread(handle);
  }

  thread(My_THREAD_FUNC func, My_THREAD_ARGS_TYPE args = NULL)
      : func(func), args(args) {}

  void start() {
    if (handle) throw exception("ERR680", exceptions::ERR680);

    My_HANDLE errorCheck;
#ifdef __unix__
    errorCheck = pthread_create(&handle, NULL, func, args);
#else
    DWORD thread_id;
    errorCheck = handle = CreateThread(NULL, 0, func, args, 0, &thread_id);
#endif

    if (os::isError(errorCheck)) {
      My_ERROR_CODE error = os::getLastError();
      if (os::isPermisionError(error))
        throw exception("ERR180", exceptions::ERR180);
      throw exception("ERR280", exceptions::format(exceptions::ERR280, error));
    }

    id = os::getCurrThreadId();
  }

  // La clase usuario queda
  // encargada de cerrar el proceso
  void join() {
    if (!handle) throw exception("ERR681", exceptions::ERR681);

    My_ERROR_CODE errorCode;
#ifdef __unix__
    void *retval;
    errorCode = pthread_join(handle, &retval);
#else
    errorCode = WaitForSingleObject(handle, INFINITE);
#endif

    if (os::isError(errorCode)) {
      My_ERROR_CODE error = os::getLastError();
      if (error != 0)
        throw exception("ERR281",
                        exceptions::format(exceptions::ERR281, error));
    }

    handle = 0;
  }

  inline bool operator==(const thread &other) const {
#ifdef __unix__
    return pthread_equal(handle, other.handle);
#else
    return id == other.id;
#endif
  }

  inline bool operator!=(const thread &other) const {
    return !operator==(std::forward<const thread &>(other));
  }
};
}  // namespace local
}  // namespace comms
}  // namespace chat

#endif