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
  MOTORS_STOP = 6,
  MOTOR_CHANGE_VECTOR = 7,
  MOTOR_LEFT_START_BACKWARD = 8,
  MOTOR_RIGHT_START_BACKWARD = 9,
  MOTOR_LEFT_START_FORWARD = 10,
  MOTOR_RIGHT_START_FORWARD = 11
};
enum MOTOR_PINS {
  MOTOR_L_PIN_1 = D0,
  MOTOR_L_PIN_2 = D1,
  MOTOR_R_PIN_1 = D2,
  MOTOR_R_PIN_2 = D3,
};
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
      pinMode(MOTOR_L_PIN_1,OUTPUT);
      pinMode(MOTOR_L_PIN_2,OUTPUT);

      pinMode(MOTOR_R_PIN_1,OUTPUT);
      pinMode(MOTOR_R_PIN_2,OUTPUT);

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
    bool direction_forward = true;
    void leftMotorStartForward(){
      digitalWrite(MOTOR_L_PIN_2, LOW );
      digitalWrite(MOTOR_L_PIN_1, HIGH );
    }
    void leftMotorStartBackward(){
      digitalWrite(MOTOR_L_PIN_2, HIGH);
      digitalWrite(MOTOR_L_PIN_1, LOW);
    }
    void leftMotorStart(){
      digitalWrite(MOTOR_L_PIN_2, direction_forward ? LOW : HIGH);
      digitalWrite(MOTOR_L_PIN_1,direction_forward ? HIGH : LOW);
    }
    void leftMotorStop(){
      digitalWrite(MOTOR_L_PIN_2,LOW);
      digitalWrite(MOTOR_L_PIN_1,LOW);
    }
    void rigthMotorStartForward(){
      digitalWrite(MOTOR_R_PIN_2, LOW );
      digitalWrite(MOTOR_R_PIN_1, HIGH );
    }
    void rightMotorStartBackward(){
      digitalWrite(MOTOR_R_PIN_2, HIGH);
      digitalWrite(MOTOR_R_PIN_1, LOW);
    }
    void rightMotorStart(){
      digitalWrite(MOTOR_R_PIN_2, direction_forward ? LOW : HIGH);
      digitalWrite(MOTOR_R_PIN_1, direction_forward ? HIGH : LOW);
    }
    void rightMotorStop(){
      digitalWrite(MOTOR_R_PIN_2,LOW);
      digitalWrite(MOTOR_R_PIN_1,LOW);
    }

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
              if(lenght < 3){
                int command = atoi((char*)payload);
                switch(command){
                  case MOTORS_START:
                    Serial.printf("MOTORS_START");
                    leftMotorStart();
                    rightMotorStart();
                  break;
                  case MOTORS_STOP:
                    Serial.printf("MOTORS_STOP");
                    leftMotorStop();
                    rightMotorStop();
                  break;
                  case MOTOR_LEFT_START:
                    Serial.printf("MOTOR_LEFT_START");
                    leftMotorStart();
                  break;
                  case MOTOR_LEFT_STOP:
                    Serial.printf("MOTOR_LEFT_STOP");
                    leftMotorStop();

                  break;
                  case MOTOR_RIGHT_START:
                    Serial.printf("MOTOR_RIGHT_START");
                    rightMotorStart();
                  break;
                  case MOTOR_RIGHT_STOP:
                    Serial.printf("MOTOR_RIGHT_STOP");
                    rightMotorStop();
                  break;
                  case MOTOR_CHANGE_VECTOR:
                    Serial.printf("MOTOR_RIGHT_STOP");
                    direction_forward = !direction_forward;
                  break;
                  case MOTOR_LEFT_START_BACKWARD:
                    leftMotorStartBackward();
                    Serial.printf("MOTOR_LEFT_BACKWARD");
                  break;
                  case MOTOR_RIGHT_START_BACKWARD:
                    rightMotorStartBackward();
                    Serial.printf("MOTOR_RIGHT_BACKWARD");
                  break;

                  case MOTOR_RIGHT_START_FORWARD:
                    rigthMotorStartForward();
                    Serial.printf("MOTOR_RIGHT_FORWARD");
                  break;
                  case MOTOR_LEFT_START_FORWARD:
                    leftMotorStartForward();
                    Serial.printf("MOTOR_LEFT_FORWARD");
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
