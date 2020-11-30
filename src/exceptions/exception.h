#ifndef CHAT_EXCEPTIONS_EXCEPTION_H_
#define CHAT_EXCEPTIONS_EXCEPTION_H_

#include <stdio.h>

#include <exception>

#include "../internal/base.h"

namespace chat {
// namespace exceptions {
class exception : public std::exception {
  const char *errCode, *description;

 public:
  exception(const char* errCode, const char* description)
      : errCode(errCode), description(description) {
    log("Creando una excepcion.", 1);
  }

  virtual void print() const {
    fprintf(stderr, "\n%s\nCódigo: %s\n%s\n%s\n",
            "/* ** Oops, hubo un error ** */", errCode, description,
            " -- --------------------------------------- -- ");
  }

  virtual char const* what() const noexcept {
    return "/* ** Ocurrió un error ** */";
  }
};
// }  // namespace exceptions
}  // namespace chat

#endif