#define M_CHAT_VERSION "1.0.2"

#if defined(__unix__) || defined(_WIN32) || defined(WIN32)
#include <messenger.h>

// @DONE Manejar errores
// @DONE Acomodar consola para que borre '> ' antes de recibir un mensaje
// @DONE Agregar compatibilidad con wchar para arreglar formato en windows
// @DONE Agregar colores
// @DONE Agregar mensaje al salir
// @DONE Anunciar entradas y salidas por el chat
// @TODO Agregar comandos...
// @ALMOST Agregar registros
// @TODO Cola de mensajería

// Para compilar, pararse en la carpeta src y ejecutar el siguiente comando:
// Windows: g++ chat.cpp internal/messenger.cpp exceptions/messages.cpp comms/communication.cpp comms/local/comms.cpp comms/local/threads.cpp internal/base.cpp -I. -static-libstdc++ -static-libgcc -o ../chat.exe
// Linux: g++ chat.cpp internal/messenger.cpp exceptions/messages.cpp comms/communication.cpp comms/local/comms.cpp comms/local/threads.cpp internal/base.cpp -I. -static-libstdc++ -static-libgcc -lpthread -o ../chat

// Comentar #define CHAT_LOG en internal/base.h para desactivar registros
int chat::log_level = 6;

void run();

int main() {
  chat::initLog();
  try {
    chat::print("Bienvenido a &e&lCHAT&r, versión &c%s&r.\n", M_CHAT_VERSION);

    chat::print("&7A continuación, escoja un nombre de usuario.&r\n");
    const std::string& username = chat::readLine();

    chat::clearScreen();

    chat::messenger chat(username);
    int members = chat.getMemberCount();
    if (members == 1) {
      chat::print("&7Esta sala está muy vacía...&r\n");
      chat::print(
          "&7Siempre podes abrir de nuevo el programa y hablar solo!&r\n\n");
    } else {
      chat::print("&7Hay &a%d &7miembros en la sala.&r\n\n", members);
    }

    chat.prefix();
    while (chat::shouldRun) {
      chat.update();
    }
    chat.exit();

    chat.clearLine();
    chat.prefix();
    chat::print("&6&lGracias vuelva prontos.&r\n");
  } catch (const chat::exception& ex) {
    ex.print();
#ifdef __unix__
    chat::os::enableInputEcho();
#endif
  } catch (const std::exception& ex) {
    std::cerr << "\n/* ** Ocurrió un error inesperado ** */\n"
              << "Error: " << ex.what() << '\n'
              << " -- --------------------------------------- -- \n";
  }
  chat::retoreSystemLog();
}

#else
#include <stdio.h>

int main() {
  printf(
      "Su sistema operativo no es compatible con esta versión de la "
      "aplicación.\n");
  return 1;
}

#endif
