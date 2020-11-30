#include "messages.h"

namespace chat {
namespace exceptions {

const char* ERR000 = "Error al formatear otro error...";

const char* ERR180 = "La aplicación no tiene permisos suficientes para crear un hilo!";
const char* ERR190 = "La aplicación no tiene permisos suficientes para crear memoria compartida.";
const char* ERR191 = "La aplicación no tiene permisos suficientes para leer memoria compartida creada por otro proceso.";
const char* ERR192 = "La aplicación no tiene permisos suficientes para vincular memoria compartida por el proceso que inicio la sala de chat.";

const char* ERR280 = "Error al intentar crear un hilo (%d).";
const char* ERR281 = "Error al intentar unirse a un hilo (%d).";
const char* ERR290 = "Error al intentar crear memoria compartida (%d).";
const char* ERR291 = "Error al intentar leer memoria compartida (%d).";
const char* ERR292 = "Error al intentar vincular la memoria compartida al proceso actual (%d).";

const char* ERR680 = "Se intentó iniciar un hilo que ya se está ejecutando.";
const char* ERR681 = "Se intentó unir a un hilo que aún no se había creado.";
const char* ERR690 = "Se intentó vincular una dirección de memoria compartida ya vinculada al proceso.";
const char* ERR691 = "Se intentó desvincular una dirección de memoria que aún no había sido vinculada al proceso actual.";

const char* format(const char* const format, ...) {
  return format;
  /*FILE* stream = tmpfile();

  if (stream == NULL) {
    perror("Failed: ");
    throw exception("ERR000", "Error al formatear otro error...");
  }

  // Aplico el mismo formato que printf
  va_list args;
  va_start(args, format);
  int result = vfprintf(stream, format, args);
  va_end(args);

  if (result < 0) {
    throw exception("ERR000", "Error al formatear otro error...");
  }

  fseek(stream, 0L, SEEK_END);
  ::std::size_t length = static_cast<::std::size_t>(ftell(stream));
  char* buffer = ::std::unique_ptr<char>(new char[length]).get();
  rewind(stream);

  if (!fgets(buffer, length, stream)) {
    throw exception("ERR000", ERR000);
  }

  fclose(stream);

  return buffer;*/
}

}  // namespace exceptions
}  // namespace chat