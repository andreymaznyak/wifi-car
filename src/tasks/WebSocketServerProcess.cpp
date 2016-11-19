#ifndef _WEB_SOCKET_SERVER_PROCESS_
#define _WEB_SOCKET_SERVER_PROCESS_

#include <ProcessScheduler.h>
#include <WebSocketsServer.h>
#include <Hash.h>

class WebSocketServerProcess : public Process
{
public:
    // Call the Process constructor
    WebSocketServerProcess(Scheduler &manager, ProcPriority pr, unsigned int period, const int port = 81)
        :  Process(manager, pr, period)
        {
          webSocket = new WebSocketsServer(port);
        }
    ~WebSocketServerProcess(){
        delete webSocket;
    }

protected:
    virtual void setup()
    {
      webSocket->begin();
      webSocket->onEvent([this](uint8_t num, WStype_t type, uint8_t * payload, size_t lenght){webSocketEvent(num,type,payload,lenght);});

      Serial.println("WebSocketServerProcess started");
    }

     // Undo setup()
    virtual void cleanup()
    {

    }

    // Create our service routine
    virtual void service()
    {
       webSocket->loop();
    }
private:
    WebSocketsServer * webSocket;
    void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

      switch(type) {
          case WStype_DISCONNECTED:
              Serial.printf("[%u] Disconnected!\n", num);
              break;
          case WStype_CONNECTED:
              {
                  IPAddress ip = webSocket->remoteIP(num);
                  Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

  				// send message to client
  				webSocket->sendTXT(num, "Connected");
              }
              break;
          case WStype_TEXT:
              Serial.printf("[%u] get Text: %s\n", num, payload);

              // send message to client
              // webSocket.sendTXT(num, "message here");

              // send data to all connected clients
              // webSocket.broadcastTXT("message here");
              break;
          case WStype_BIN:
              Serial.printf("[%u] get binary lenght: %u\n", num, lenght);
              hexdump(payload, lenght);

              // send message to client
              // webSocket.sendBIN(num, payload, lenght);
              break;
      }

  }
};


#endif
