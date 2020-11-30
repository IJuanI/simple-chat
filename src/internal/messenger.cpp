#include "messenger.h"

#include <comms/communication.h>
#include <messenger.h>

#include "../exceptions/messages.h"
#ifdef __unix__
#include <stdarg.h>
#else
#include <conio.h>
#endif

#include <string>

namespace chat {

int print(const char* format, ...) {
  log("Imprimiendo mensaje colorido en pantalla.", 4);
  // Uso un stream para no limitar la cantidad de caracteres
  FILE* stream = tmpfile();

  // Aplico el mismo formato que printf
  int result;
  va_list args;
  va_start(args, format);
  result = vfprintf(stream, format, args);
  va_end(args);

  // Notifico un error
  if (result < 0) return result;

  // Reservo el espacio suficiente
  ::std::string output;
  fseek(stream, 0L, SEEK_END);
  output.reserve(static_cast<::std::size_t>(ftell(stream)));
  rewind(stream);

  // Aplico reemplazo de colores
  char buffer[256];
  bool color_mode = false;
  while (!feof(stream)) {
    if (fgets(buffer, 256, stream) == NULL) break;
    for (int i = 0; i < 256; ++i) {
      char c = buffer[i];
      if (c == '\0') break;
      if (color_mode) {
        color_mode = false;
        if (os::isColor(c)) {
          printf("%s", output.c_str());
          // El color cambia aca
          os::setColor(c);
          output.clear();
          continue;
        }
        output.push_back(COLOR_CHAR);
      } else if (c == COLOR_CHAR) {
        color_mode = true;
        continue;
      }
      output.push_back(c);
    }
  }

  // Ya lei todo el stream
  fclose(stream);

  // Pudo quedar un caracter de color al final.
  if (color_mode) output.push_back(COLOR_CHAR);

  result = printf("%s", output.c_str());
  fflush(stdout);

  log("Impreso mensaje colorido en pantalla.", 3);
  return result;
}

::std::string readLine() {
  print("&3> &f");

  ::std::string result;
  ::std::getline(::std::cin, result);

  return result;
}

bool shouldRun = true;

namespace internal {

M_CTRL_HEAD ctrlHandler(M_CTRL_ARGS ctrlType) {
  log("Solicitando cierre de aplicación.", 4);
#ifndef __unix__
  switch (ctrlType) {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
#endif
      chat::shouldRun = false;
#ifndef __unix__
      return TRUE;
    default:
      return FALSE;
  }
#endif
  log("Solicitado el cierre de la aplicación.", 3);
}

messenger::messenger(const ::std::string& username) : username(username) {
  log("Iniciando el chat.", 1);
  print("Un gusto &b%s&f.\n", username.c_str());

  log("Registrando eventos de cierre.", 3);
  os::registerHandler(ctrlHandler);
  log("Eventos de cierre registrados.", 4);

#ifdef __unix__
  os::disableInputEcho();
#endif

  comms = new comms::communication(this);
  log("Chat iniciado.", 2);
}

messenger::~messenger() noexcept {
  log("Cerrando el chat.", 2);
#ifdef __unix__
  os::enableInputEcho();
#endif

  if (comms) delete comms;
  log("Chat cerrado.", 1);
}

void messenger::exit() {
  log("Finalizando el chat.", 2);
#ifdef __unix__
  os::enableInputEcho();
#endif

  if (comms) {
    comms->exit();
    delete comms;
  }

  log("Chat finalizado.", 1);
}

unsigned int messenger::getMemberCount() const {
  log("Buscando cantidad de miembros.", 6);
  return comms->getMemberCount();
}

void messenger::clearChar() {
  input.pop_back();
  fputc('\b', stdout);
  fputc(' ', stdout);
  fputc('\b', stdout);
}

void messenger::prefix() const { print("&3> &f"); }

void messenger::update() {
  while (_kbhit()) {
#ifdef __unix__
    int c = getchar();
#else
    int c = _getch();
#endif
    log(exceptions::format("Tecla detectada: %d.", c), 7);
    if (c == '\n' || c == '\r') {
      log("Ingresando mensaje local.", 5);
      comms->sendMessage(input);
      receiveMessage(input.c_str(), (const char*)-1);
      input.clear();
      log("Mensaje local ingresado.", 6);
#ifdef __unix__
    } else if (c == 127) {  // borrar
#else
    } else if (c == 8) {   // borrar
#endif
      log("Borrando último caracter.", 4);
      if (!input.empty()) clearChar();
#ifdef __unix__
    } else if (c == 8) {  // shift + borrar
      log("Borrando bloque de caracteres (Linux).", 4);
      std::size_t pos = input.find_last_of(' ');
      std::size_t erased =
          pos == std::string::npos ? input.size() : input.size() - pos;
      for (std::size_t n = 0; n < erased; ++n) clearChar();
      log("Bloque borrado (Linux).", 6);
    } else if (c == 27) {
      log("Ignorando evento especial (Linux).", 4);
      if (!_kbhit()) continue;  // escape
      c = getchar();
      log(exceptions::format("Evento ignorado: %d.", c), 8);
      if (c == 91) {  // Flechas
        c = getchar();
        log(exceptions::format("Evento ignorado: %d.", c), 8);
        continue;
      } else if (c == 127) {  // alt + borrar
#else
    } else if (c == 23 || c == 127) {  // shift + borrar
#endif
        log("Borrando bloque de caracteres.", 4);
        ::std::size_t pos = input.find_last_of(' ');
        ::std::size_t erased =
            pos == ::std::string::npos ? input.size() : input.size() - pos;
        for (::std::size_t n = 0; n < erased; ++n) clearChar();
        log("Bloque borrado.", 6);
#ifdef __unix__
      } else
        continue;
#else
    } else if (c == 0) {
      log("Ignorando evento especial (Windows).", 4);
      c = _getch();
      log(exceptions::format("Evento ignorado: %d.", c), 8);
#endif
    } else {
      log(exceptions::format("Imprimiendo caracter %d.", c), 6);
      fputc(c, stdout);
      input.push_back(c);
      log("Caracter impreso.", 7);
    }
  }

  os::sleep(20);
}

void messenger::receiveMessage(const char* message, const char* sender) const {
  log("Imprimiendo mensaje.", 5);
  clearLine();
  if ((std::intptr_t)sender == -1) {
    log("Imprimiendo mensaje enviado por el mismo cliente.", 6);
    print("&eYo: &f%s&r\n", message);
    restoreLine(false);
  } else {
    if (*sender == '\0') {
      log("Imprimiendo alerta del sistema.", 6);
      print("&a%s&r\n", message);
    } else {
      log("Imprimiendo mensaje enviado por otro cliente.", 6);
      print("&7%s: &f%s&r\n", sender, message);
    }
    restoreLine(true);
  }
  log("Mensaje impreso en pantalla.", 4);
}

void messenger::clearLine() const {
  fputc('\r', stdout);
  for (::std::size_t i = 0; i < input.size() + 3; ++i) fputc(' ', stdout);
  fputc('\r', stdout);
}

void messenger::restoreLine(bool printCurrent) const {
  prefix();
  if (printCurrent) printf("%s", input.c_str());
}
}  // namespace internal
}  // namespace chat
