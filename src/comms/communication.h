#ifndef CHAT_COMMS_COMMUNICATION_H_
#define CHAT_COMMS_COMMUNICATION_H_

#include <internal/messenger.h>

#include <iostream>

namespace chat {
namespace comms {
namespace local {
class comms;
}

class communication {
  local::comms* localComms;
  const internal::messenger* client;
  bool enabled = true;

 public:
  communication(const communication&) = delete;
  communication& operator=(const communication&) = delete;

  communication(const internal::messenger* client);
  ~communication() noexcept;
  void exit();

  unsigned int getMemberCount() const;

  // Transformo los rtype a ltype para simplificar la implementacion
  inline void sendMessage(const ::std::string&& message) const {
    sendMessage(message);
  }
  void sendMessage(const ::std::string& message) const;

  void receiveMessage(const char* message, const char* sender) const;
};

}  // namespace comms
}  // namespace chat

#endif
