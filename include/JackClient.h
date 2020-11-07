#pragma once

#include <cstring>
#include <iostream>
#include <vector>

#include <jack/jack.h>
#include <jack/midiport.h>

/**
 * Wrapper around a Jack client.
 */
class JackClient {
public:
  typedef jack_default_audio_sample_t sample_t;
  typedef jack_midi_event_t midi_event_t;

  JackClient(const char *clientName)
      : clientName(clientName) {
    // Create a Jack client and connect to the Jack server but do not start
    // processing
    open();
  };

  ~JackClient() {
    close();
  }

  /**
   * Opens connection with the Jack server.
   * Also registers callback, ports, etc.
   */
  void open() {
    serverName = nullptr;
    options = JackNullOption;

    client = jack_client_open(clientName, options, &status, serverName);

    if (!client) {
      std::cerr << "jack_client_open() failed, status = 0x" << status << "\n";
      if (status & JackServerFailed) {
        std::cerr << "Unable to connect to JACK server\n";
      }
      exit(1);
    }

    if (status & JackServerStarted) {
      std::cout << "JACK server started\n";
    }

    // If clientName is not unique JACK will assign a unique name.
    // Get this name.
    if (status & JackNameNotUnique) {
      clientName = jack_get_client_name(client);
      std::cerr << "Unique name assigned: " << clientName << "\n";
    }

    // Register JACK callbacks
    jack_on_shutdown(client, jackShutdown, 0);
  }

  /**
   * Activate the client.
   */
  void run() {
    if (jack_activate (client)) {
      std::cerr << "Cannot activate client!\n";
      exit (1);
    }
  }

  /**
   * Closes JACK client.
   */
  void close() {
    jack_client_close(client);
  }


  /**
   * Return a list of all currently open ports.
   */
  std::vector<std::string> getPorts() {
    const char **ports_cstr = jack_get_ports(client,
                                             nullptr,
                                             nullptr,
                                             0);
    std::vector<std::string> ports;
    for (std::size_t i = 0; ports_cstr[i] != nullptr; ++i) {
      ports.push_back(ports_cstr[i]);
    }

    jack_free(ports_cstr);

    return ports;
  }

private:
  const char *clientName;

  jack_client_t *client;
  const char *serverName;
  jack_options_t options;
  jack_status_t status;

  /**
   * JACK calls this if server shuts down or server disconnects client.
   */
  static void jackShutdown(void *arg) {
    exit(1);
  }
};
