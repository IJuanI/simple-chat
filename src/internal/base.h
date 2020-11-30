#ifndef CHAT_INTERNAL_BASE_H_
#define CHAT_INTERNAL_BASE_H_

#include <errno.h>

#include <cstdio>
#include <cstdlib>
#include <type_traits>
#include <utility>
#include <iostream>
#include <fstream>

#define CHAT_LOG

#ifdef __unix__
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#include <cstring>
typedef int My_HANDLE;
typedef int My_ERROR_CODE;
#define M_CTRL_HEAD void
#define M_CTRL_ARGS int
typedef pthread_t thread_id_t;
#else
#include <windows.h>
typedef HANDLE My_HANDLE;
typedef DWORD My_ERROR_CODE;
#define M_CTRL_HEAD BOOL WINAPI
#define M_CTRL_ARGS DWORD
typedef DWORD thread_id_t;
#endif

namespace chat {
namespace os {

// Recibe algún puntero
template <typename T>
inline bool isErrorPointer(T* value) {
#ifdef __unix__
  return value == (void*) -1;
#else
  return value == 0;
#endif
}

// Recibe algún número
template <typename T>
inline bool isError(T value) {
#ifdef __unix__
  return value == -1;
#else
  return value == 0;
#endif
}

inline bool isPermisionError(My_ERROR_CODE error) {
#ifdef __unix
  return error == EACCES;
#else
  return error == 5;
#endif
}

inline My_ERROR_CODE getLastError() {
#ifdef __unix__
  return errno;
#else
  return GetLastError();
#endif
}
}  // namespace os

namespace os {
inline void sleep(unsigned long milliseconds) {
#ifdef __unix__
  usleep(milliseconds * 1000);
#else
  Sleep(milliseconds);
#endif
}
}  // namespace os

namespace os {
template <typename T>
inline void copy(T*& target, T*& source, ::std::size_t size) {
#ifdef __unix__
  std::memcpy(target, source, size);
#else
  CopyMemory(target, source, size);
#endif
}

template <typename T>
inline void copy(T*& target, const T*&& source, ::std::size_t size) {
#ifdef __unix__
  std::memcpy(target, source, size);
#else
  CopyMemory(target, source, size);
#endif
}

template <typename T>
inline void copy(T* target, const T*& source, ::std::size_t size) {
#ifdef __unix__
  std::memcpy(target, source, size);
#else
  CopyMemory(target, source, size);
#endif
}
}  // namespace os

namespace os {
// Adopoto la codificacion de colores de Minecraft.
// Ver https://minecraft.gamepedia.com/Formatting_codes
inline bool isColor(char colorChar) {
  return ('0' <= colorChar && colorChar <= '9') ||
         ('a' <= colorChar && colorChar <= 'f') || colorChar == 'r' ||
         colorChar == 'l';
}

#ifndef __unix__
extern HANDLE hConsole;
extern WORD defaultAttributes;
#endif

inline void setColor(char colorChar) {
  if (colorChar == 'r') {
#ifdef __unix__
    printf("\033[0m");
#else
    if (hConsole != NULL) {
      SetConsoleTextAttribute(hConsole, defaultAttributes);
    }
#endif
  } else if (colorChar == 'l') {
#ifdef __unix__
    printf("\033[1m");
#endif  // Windows no soporta negrita
  } else {
    char colorIdx =
        colorChar <= '9' ? (colorChar - '0') : (colorChar - 'a' + 10);
#ifdef __unix__
    // No se ponen de acuerdo ni en el orden de colores...
    switch (colorIdx % 8) {
      case 1:
        colorIdx += 3;
        break;
      case 3:
        colorIdx += 3;
        break;
      case 4:
        colorIdx -= 3;
        break;
      case 6:
        colorIdx -= 3;
        break;
    }
    if (colorIdx > 7)
      printf("\033[3%d;1m", colorIdx % 8);
    else
      printf("\033[3%dm", colorIdx % 8);
#else
    if (hConsole == NULL) {
      SetConsoleOutputCP(65001);
      hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
      CONSOLE_SCREEN_BUFFER_INFO info;
      GetConsoleScreenBufferInfo(hConsole, &info);
      defaultAttributes = info.wAttributes;
    }
    SetConsoleTextAttribute(hConsole, static_cast<WORD>(colorIdx));
#endif
  }
}

inline void clearScreen() {
#ifdef __unix__
  ::std::system("clear");
#else
  ::std::system("cls");
#endif
}
}  // namespace os

namespace os {
#ifdef __unix__
typedef void (*ctrl_handler_t)(M_CTRL_ARGS);
#else
typedef BOOL(WINAPI* ctrl_handler_t)(M_CTRL_ARGS);
#endif
inline void registerHandler(ctrl_handler_t ctrlHandler) {
#ifdef __unix__
  struct sigaction sigHandler;

  sigHandler.sa_handler = ctrlHandler;
  sigemptyset(&sigHandler.sa_mask);
  sigHandler.sa_flags = 0;

  sigaction(SIGINT, &sigHandler, NULL);
  sigaction(SIGTERM, &sigHandler, NULL);
  sigaction(SIGHUP, &sigHandler, NULL);
#else
  SetConsoleCtrlHandler(ctrlHandler, TRUE);
#endif
}
}  // namespace os

#ifdef __unix__
namespace os {
inline void disableInputEcho() {
  struct termios t;
  tcgetattr(STDIN_FILENO, &t);
  t.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

inline void enableInputEcho() {
  struct termios t;
  tcgetattr(STDIN_FILENO, &t);
  t.c_lflag |= ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &t);
}
}  // namespace os

inline int _kbhit(void) {
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}
#endif

#ifdef CHAT_LOG
extern int log_level;
#endif

namespace os {
inline thread_id_t getCurrThreadId() {
#ifdef __unix__
  return pthread_self();
#else
  return GetCurrentThreadId();
#endif
}
}  // namespace os

#ifdef CHAT_LOG
extern std::streambuf* oldLogBuf;
extern std::ofstream logStream;

inline void initLog() {
  chat::logStream.open("chat.log", std::ofstream::out | std::ofstream::app);
  oldLogBuf = std::clog.rdbuf();
  std::clog.rdbuf(chat::logStream.rdbuf());
}

inline void retoreSystemLog() {
  std::clog.rdbuf(oldLogBuf);
  chat::logStream.close();
}
#endif

inline void log(const char* info, int level = 4) {
#ifdef CHAT_LOG
  if (log_level >= level) {
    std::clog << '[' << os::getCurrThreadId() << "] " << info << std::endl;
    chat::logStream.flush();
  }
#endif
}
}  // namespace chat

#endif