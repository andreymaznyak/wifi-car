#ifndef _WEB_SERVER_PROCESS_
#define _WEB_SERVER_PROCESS_

#include <ProcessScheduler.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include "../config.h"

typedef std::function<void(void)> THandlerFunction;
// Create my custom Blink Process
class WebServerProcess : public Process
{
public:
    // Call the Process constructor
    WebServerProcess(Scheduler &manager, ProcPriority pr, unsigned int period, const int port = 80)
        :  Process(manager, pr, period)
        {
          // Port defaults to 80
          server = ESP8266WebServer(port);
        }
    ~WebServerProcess(){

    }

protected:
    virtual void setup()
    {
      server.on("/",[this](){ handleRoot();});

      server.on("/app.js", [this](){
         server.send(200, "text/javascript",
         "function socketOnOpen(a){showMessage(\"onopen\"),console.log(a),socket.onclose=function(a){showMessage(\"Соединение разорвано\"),setTimeout(function(){try{var a=new WebSocket(\"ws://192.168.4.1:81\");a.onopen=socketOnOpen}catch(a){}},3e3)},socket.onmessage=function(a){var b=a.data;showMessage(b)},socket.onerror=function(a){var b=a.data;showMessage(b)}}function startRight(){socket.send(MOTOR_RIGHT_START)}function stopRight(){socket.send(MOTOR_RIGHT_STOP)}function startLeft(){socket.send(MOTOR_LEFT_START)}function stopLeft(){socket.send(MOTOR_LEFT_STOP)}function startAll(){socket.send(MOTORS_START)}function stopAll(){socket.send(MOTORS_STOP)}function sendMessage(){var a=document.getElementById(\"text\").value;return console.log(a),socket.send(a),!1}function showMessage(a){var b=document.createElement(\"div\");b.appendChild(document.createTextNode(a)),document.getElementById(\"subscribe\").appendChild(b)}function hotkey(a){var b=a.keyCode;document.getElementById(\"key\").innerHTML=b}MOTOR_LEFT_START=1,MOTOR_LEFT_STOP=2,MOTOR_RIGHT_START=3,MOTOR_RIGHT_STOP=4,MOTORS_START=5,MOTORS_STOP=6,document.onkeydown=function(a){if(!currentDown)switch(currentDown=a.keyCode,a.key){case\"ArrowUp\":startAll();break;case\"ArrowDown\":console.log(\"nope\");break;case\"ArrowRight\":startRight();break;case\"ArrowLeft\":startLeft();break;default:console.log(\"nope\")}},document.onkeyup=function(a){switch(currentDown=void 0,a.key){case\"ArrowUp\":stopAll();break;case\"ArrowDown\":console.log(\"nope\");break;case\"ArrowRight\":stopRight();break;case\"ArrowLeft\":stopLeft();break;default:console.log(\"nope\")}};var socket=new WebSocket(\"ws://192.168.4.1:81\");socket.onopen=socketOnOpen;var currentDown=void 0;");
      });

      server.onNotFound( [this](){handleNotFound();} );

      server.begin();
      Serial.println("WebServerProcess started");
    }

     // Undo setup()
    virtual void cleanup()
    {

    }

    // Create our service routine
    virtual void service()
    {
      server.handleClient();
    }
private:
  ESP8266WebServer server;
  void handleRoot() {
    // char* a;
    // itoa(head,a,10);
    // for(uint i=0; a[i] != '\0' && i < 10; i++)
    //   add_char_to_buffer(a[i]);response_buffer
    server.send(200, "text/html", "<!DOCTYPE html><html><head> <meta charset=\"UTF-8\"> <title></title> <script src=\"app.js\"></script></head><body><button onclick=\"startRight()\"> startRight</button><button onclick=\"stopRight()\"> stopRight</button><button onclick=\"startLeft()\"> startLeft</button><button onclick=\"stopLeft()\"> stopLeft</button><button onclick=\"startAll()\">startAll</button><button onclick=\"stopAll()\">stopAll</button><input id=\"text\" type=\"text\" name=\"message\"><button onclick=\"sendMessage()\">Отправить</button><div id=\"subscribe\"></div></body></html>");
  }

  void handleNotFound(){
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i=0; i<server.args(); i++){
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
  }
};

#endif
