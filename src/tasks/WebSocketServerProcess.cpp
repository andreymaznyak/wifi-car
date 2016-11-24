#ifndef _WEB_SOCKET_SERVER_PROCESS_
#define _WEB_SOCKET_SERVER_PROCESS_

#include <ProcessScheduler.h>
#include <WebSocketsServer.h>
#include <Hash.h>
enum MOTOR_COMMANDS {
  MOTOR_LEFT_START = 1,
  MOTOR_LEFT_STOP  = 2,
  MOTOR_RIGHT_START = 3,
  MOTOR_RIGHT_STOP = 4,
  MOTORS_START = 5,
  MOTORS_STOP = 6 };
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
      pinMode(D0,OUTPUT);
      pinMode(D1,OUTPUT);
      pinMode(D2,OUTPUT);

      pinMode(D5,OUTPUT);
      pinMode(D6,OUTPUT);
      pinMode(D7,OUTPUT);

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
              Serial.printf("[%u] get Text: %s Size: %d Int: %d\n", num, payload, lenght, atoi((char*)payload));
              //Расщифровываем команду
              if(lenght == 1){
                int command = atoi((char*)payload);
                switch(command){
                  case MOTORS_START:
                    Serial.printf("MOTORS_START");
                    digitalWrite(D0,HIGH);
                    digitalWrite(D1,HIGH);
                    digitalWrite(D2,HIGH);

                    digitalWrite(D5,HIGH);
                    digitalWrite(D6,HIGH);
                    digitalWrite(D7,HIGH);
                  break;
                  case MOTORS_STOP:
                    Serial.printf("MOTORS_STOP");
                    digitalWrite(D0,LOW);
                    digitalWrite(D1,LOW);
                    digitalWrite(D2,LOW);

                    digitalWrite(D5,LOW);
                    digitalWrite(D6,LOW);
                    digitalWrite(D7,LOW);
                  break;
                  case MOTOR_LEFT_START:
                    Serial.printf("MOTOR_LEFT_START");
                    digitalWrite(D0,HIGH);
                    digitalWrite(D1,HIGH);
                    digitalWrite(D2,HIGH);
                  break;
                  case MOTOR_LEFT_STOP:
                    Serial.printf("MOTOR_LEFT_STOP");
                    digitalWrite(D0,LOW);
                    digitalWrite(D1,LOW);
                    digitalWrite(D2,LOW);

                  break;
                  case MOTOR_RIGHT_START:
                    Serial.printf("MOTOR_RIGHT_START");
                    digitalWrite(D5,HIGH);
                    digitalWrite(D6,HIGH);
                    digitalWrite(D7,HIGH);
                  break;
                  case MOTOR_RIGHT_STOP:
                    Serial.printf("MOTOR_RIGHT_STOP");
                    digitalWrite(D5,LOW);
                    digitalWrite(D6,LOW);
                    digitalWrite(D7,LOW);
                  break;
                }
              }
              // send message to client
              //webSocket->sendTXT(num, payload);

              // send data to all connected clients
              webSocket->broadcastTXT(payload);
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
