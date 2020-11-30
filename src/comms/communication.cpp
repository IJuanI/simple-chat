#include "communication.h"

#include "local/comms.h"

namespace chat {
namespace comms {
communication::communication(const internal::messenger* client)
    : client(client) {
  log("Iniciando comunicaciones.", 2);
  localComms = new local::comms(this);
  localComms->sendMessage(("&3" + client->getUsername() + " &aingresó a la sala.").c_str(), "\0");
  log("Comunicaciones iniciadas correctamente.", 3);
}

communication::~communication() noexcept {
  log("Destruyendo comunicaciones.", 6);
  try {
    if (enabled) {
      localComms->sendMessage(("&3" + client->getUsername() + " &cabandonó la sala.").c_str(), "\0");
      delete localComms;
    }
  } catch (const chat::exception& ex) {
    ex.print();
  }
  log("Comunicaciones destruidas.", 5);
}

void communication::exit() {
  log("Finalizando comunicaciones.", 4);
  if (localComms) {
    localComms->sendMessage(("&3" + client->getUsername() + " &cabandonó la sala.").c_str(), "\0");
    enabled = false;
    localComms->exit();
    delete localComms;
  }
  log("Comunicaciones Finalizadas.", 3);
}

unsigned int communication::getMemberCount() const {
  return localComms->getMemberCount();
}

void communication::sendMessage(const ::std::string& message) const {
  localComms->sendMessage(message.c_str(), client->getUsername().c_str());
}

void communication::receiveMessage(const char* message,
                                          const char* sender) const {
  client->receiveMessage(::std::forward<const char*>(message),
                         ::std::forward<const char*>(sender));
}
}  // namespace comms
}  // namespace chat