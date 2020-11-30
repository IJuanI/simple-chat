#ifndef CHAT_EXCEPTIONS_MESSAGES_H_
#define CHAT_EXCEPTIONS_MESSAGES_H_

#include <cstdarg>
#include <cstdio>
#include <memory>

#include "exception.h"

namespace chat {
// 0XX = Errores graves
// 1XX = Errores de Permisos
// 2XX = Errores de Sistema
// 6XX = Errores Lógicos

// X8X = Errores de Hilos
// X9X = Errores de Memoria Compartida

namespace exceptions {

/// Errores
extern const char* ERR000;

extern const char* ERR180;
extern const char* ERR190;
extern const char* ERR191;
extern const char* ERR192;

extern const char* ERR280;
extern const char* ERR281;
extern const char* ERR290;
extern const char* ERR291;
extern const char* ERR292;

extern const char* ERR680;
extern const char* ERR681;
extern const char* ERR690;
extern const char* ERR691;

/// Funcion Auxiliar
const char* format(const char* const format, ...);

}  // namespace exceptions
}  // namespace chat

#endif