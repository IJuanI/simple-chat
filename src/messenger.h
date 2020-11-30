#ifndef CHAT_MESSENGER_H_
#define CHAT_MESSENGER_H_

#include "internal/messenger.h"

#define COLOR_CHAR '&'

namespace chat {

class messenger : internal::messenger {
  using parent = internal::messenger;

 public:
  messenger(const std::string&& username) : messenger(username) {}
  messenger(const std::string& username) : parent(username) {}

  inline std::string getUsername() const { return parent::getUsername(); }
  inline unsigned int getMemberCount() const {
    return parent::getMemberCount();
  }

  void prefix() const { parent::prefix(); }
  void update() { parent::update(); }
  void exit() { parent::exit(); }

  void clearLine() const { parent::clearLine(); }
};

}  // namespace chat

#endif