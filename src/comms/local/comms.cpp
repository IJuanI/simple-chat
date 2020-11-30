#include "comms.h"

namespace chat {
namespace comms {
namespace local {

struct shared_data_t {
  unsigned int attendees = 0;
  unsigned int readers = 0;
  thread sourceThread = thread::empty;
  char sender[20] = "\0";
  char message[255] = "\0";
};

bool ready = false;
shared_data_t* shared_data;

My_THREAD_HEAD listenComms(My_THREAD_ARGS_TYPE args) {
  log("Hilo de eventos creado correctamente.", 3);
  ((comms*)args)->hookEvents();
  log("El hilo de eventos finalizó su ejecución correctamente.", 3);
  return 0;
}

comms::comms(const communication* client)
    : client(client),
      commsThread(listenComms, this),
      sharedMemory(M_CHAT_CHANNEL) {
  log("Iniciando comunicaciones locales.", 3);
  bool initMem;
  sharedMemory.attach((void**)&shared_data, sizeof(shared_data_t), initMem);
  if (initMem) {
    log("Inicializando memoria compartida a valores por defecto.", 6);
    shared_data_t default_data;
    os::copy(shared_data, &default_data, sizeof(shared_data_t));
    log("Inicializada memoria compartida a valores por defecto.", 5);
  }
  log("Creando hilo de eventos...", 4);
  commsThread.start();

  log("Esperando inicialización del hilo de eventos.", 5);
  while (!ready) os::sleep(2);

  log("Comunicaciones locales iniciadas correctamente.", 4);
}

comms::~comms() noexcept {
  log("Destruyendo comunicaciones locales.", 7);
  try {
    exit();
  } catch (const chat::exception& ex) {
    ex.print();
  }
  log("Comunicaciones locales destruídas con éxito.", 6);
}

void comms::exit() {
  if (running) {
    log("Destruyendo comunicaciones locales.", 4);
    running = false;
    commsThread.join();
    bool clear = --(shared_data->attendees) == 0;
    log(exceptions::format("Participantes restantes: %u.",
                           shared_data->attendees),
        5);
    sharedMemory.detach((void**)&shared_data, clear);
    log("Comunicaciones locales detenidas.", 3);
  }
}

unsigned int comms::getMemberCount() const { return shared_data->attendees; }

void comms::sendMessage(const char* message, const char* sender) const {
  log("Enviando mensaje.", 1);
  log(exceptions::format("Lectores: %u.", shared_data->attendees), 4);
  shared_data->sourceThread = commsThread;
  strncpy((char*)shared_data->sender, sender, sizeof(char) * 20);
  strncpy((char*)shared_data->message, message, sizeof(char) * 255);
  shared_data->readers = shared_data->attendees;
  log("Mensaje enviado.", 2);
}

void comms::hookEvents() const {
  log("Delegada ejecución del hilo de eventos a la comunicación local.", 5);
  ++(shared_data->attendees);
  ready = true;
  eventsLoop();
}

void comms::eventsLoop() const {
  log("Iniciado el ciclo de eventos.", 4);
  while (running) {
    while (shared_data->readers != 0 && running) os::sleep(10);
    log("Esperando nuevo mensaje.", 6);
    while (shared_data->readers == 0 && running) os::sleep(10);
    if (shared_data->readers != 0) {
      log("Se detectó un nuevo mensaje.", 4);
      if (shared_data->sourceThread != commsThread) {
        log("Recibiendo un mensaje.", 1);
        client->receiveMessage(shared_data->message, shared_data->sender);
        log("Mensaje recibido con éxito.", 4);
      }

      --(shared_data->readers);
      log("Mensaje leído.", 3);
    } else if (shared_data->readers != 0 &&
               shared_data->sourceThread != commsThread) {
      --(shared_data->readers);
      log("Mensaje leído.", 3);
    }
  }

  log("Fin del mensaje.", 5);
}

}  // namespace local
}  // namespace comms
}  // namespace chat