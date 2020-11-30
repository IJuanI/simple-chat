#ifndef CHAT_INTERNAL_MESSENGER_H_
#define CHAT_INTERNAL_MESSENGER_H_

#include <internal/base.h>
#include <exceptions/exception.h>

#include <cstdint>
#include <iostream>

namespace chat {

int print(const char* format, ...);

::std::string readLine();

inline void clearScreen() { os::clearScreen(); }

extern bool shouldRun;

namespace comms {
class communication;
}

namespace internal {
class messenger {
  comms::communication* comms;
  ::std::string username;
  ::std::string input;

  void clearChar();

 public:
  messenger(const messenger&) = delete;
  messenger& operator=(const messenger&) = delete;

  messenger(const ::std::string& username);
  ~messenger() noexcept;
  void exit();

  ::std::string getUsername() const { return username; }
  unsigned int getMemberCount() const;

  void prefix() const;
  void update();

  void receiveMessage(const char* message, const char* sender) const;

  void clearLine() const;
 private:
  void restoreLine(bool printCurrent) const;
};

}  // namespace internal
}  // namespace chat

#endif