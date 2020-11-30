#ifndef CHAT_COMMS_INTERNAL_LOCAL_COMMS_H_
#define CHAT_COMMS_INTERNAL_LOCAL_COMMS_H_

#include "../communication.h"
#include "shmemory.h"
#include "threads.h"

#ifdef __unix__
#define M_CHAT_CHANNEL 109382
#else
#define M_CHAT_CHANNEL TEXT("My_CHAT_MESSAGING")
#endif

namespace chat {
namespace comms {
namespace local {
class comms {
  const communication* client;
  thread commsThread;
  shmemory sharedMemory;
  bool running = true;

 public:
  // Copiar esto desataría el caos
  comms(const comms&) = delete;
  comms& operator=(const comms&) = delete;

  comms(const communication*);
  ~comms() noexcept;
  void exit();

  unsigned int getMemberCount() const;

  // El mensaje siempre llega de 255 de largo
  // El sender siempre llega de 20 de largo
  void sendMessage(const char* message, const char* sender) const;

  void hookEvents() const;
  void eventsLoop() const;
};
}  // namespace local
}  // namespace comms
}  // namespace chat

#endif
