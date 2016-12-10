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
         server.send(200, "text/javascript",   "const MOTOR_LEFT_START = 1,\
                                                MOTOR_LEFT_STOP  = 2,\
                                                MOTOR_RIGHT_START = 3,\
                                                MOTOR_RIGHT_STOP = 4,\
                                                MOTORS_START = 5,\
                                                MOTORS_STOP = 6,\
                                                MOTOR_CHANGE_VECTOR = 7,\
                                                MOTOR_LEFT_START_BACKWARD = 8,\
                                                MOTOR_RIGHT_START_BACKWARD = 9,\
                                                MOTOR_LEFT_START_FORWARD = 10,\
                                                MOTOR_RIGHT_START_FORWARD = 11;\
                                            \
                                            document.onkeydown = function(event) {\
                                                if(!currentDown){\
                                                    currentDown = event.keyCode;\
                                                    switch (event.key) {\
                                                        case 'ArrowUp':\
                                                            startAll();\
                                                            break;\
                                                        case 'ArrowDown':\
                                                            socket.send(MOTOR_CHANGE_VECTOR);\
                                                            break;\
                                                        case 'ArrowRight':\
                                                            startRight();\
                                                            break;\
                                                        case 'ArrowLeft':\
                                                            startLeft();\
                                                            break;\
                                                        default:\
                                                            console.log('nope');\
                                                            break;\
                                                    }\
                                                }\
                                            };\
                                            document.onkeyup = function(event){\
                                                currentDown = undefined;\
                                                switch (event.key) {\
                                                    case 'ArrowUp':\
                                                        stopAll();\
                                                        break;\
                                                    case 'ArrowDown':\
                                                        console.log('nope');\
                                                        break;\
                                                    case 'ArrowRight':\
                                                        stopRight();\
                                                        break;\
                                                    case 'ArrowLeft':\
                                                        stopLeft();\
                                                        break;\
                                                    default:\
                                                        console.log('nope');\
                                                        break;\
                                                }\
                                            };\
                                            var socket = new WebSocket('ws://192.168.4.1:81');\
                                            socket.onopen = socketOnOpen;\
                                            window.onload = function(){\
                                                document.getElementById('SRF').addEventListener('touchstart', startRightForward, false);\
                                                document.getElementById('SRF').addEventListener('touchend', stopRight, false);\
                                                document.getElementById('SLF').addEventListener('touchstart', startLeftForward, false);\
                                                document.getElementById('SLF').addEventListener('touchend', stopLeft, false);\
                                                document.getElementById('SRB').addEventListener('touchstart', startRightBackward, false);\
                                                document.getElementById('SRB').addEventListener('touchend', stopRight, false);\
                                                document.getElementById('SLB').addEventListener('touchstart', startLeftBackward, false);\
                                                document.getElementById('SLB').addEventListener('touchend', stopLeft, false);\
                                            };\
                                            function socketOnOpen(event){\
                                                showMessage('onopen');\
                                                console.log(event);\
                                                socket.onclose = function(event) {\
                                                    showMessage('Соединение разорвано');\
                                                    setTimeout(function () {\
                                                        try{\
                                                            var socket = new WebSocket('ws://192.168.4.1:81');\
                                                            socket.onopen = socketOnOpen;\
                                                        }catch(e){\
                                                        }\
                                                    },3000);\
                                                };\
                                                socket.onmessage = function(event) {\
                                                    var incomingMessage = event.data;\
                                                    showMessage(incomingMessage);\
                                                };\
                                                socket.onerror = function(event) {\
                                                    var incomingMessage = event.data;\
                                                    showMessage(incomingMessage);\
                                                };\
                                            }\
                                            var currentDown = undefined;\
                                            function startRight(){\
                                                socket.send(MOTOR_RIGHT_START);\
                                            }\
                                            function stopRight(){\
                                                socket.send(MOTOR_RIGHT_STOP);\
                                            }\
                                            function startLeft(){\
                                                socket.send(MOTOR_LEFT_START);\
                                            }\
                                            function stopLeft(){\
                                                socket.send(MOTOR_LEFT_STOP);\
                                            }\
                                            function startAll(){\
                                                socket.send(MOTORS_START);\
                                            }\
                                            function stopAll(){\
                                                socket.send(MOTORS_STOP);\
                                            }\
                                            function startLeftForward() {\
                                                socket.send(MOTOR_LEFT_START_FORWARD);\
                                            }\
                                            function startLeftBackward() {\
                                                socket.send(MOTOR_LEFT_START_BACKWARD);\
                                            }\
                                            function startRightForward() {\
                                                socket.send(MOTOR_RIGHT_START_FORWARD);\
                                            }\
                                            function startRightBackward() {\
                                                socket.send(MOTOR_RIGHT_START_BACKWARD);\
                                            }\
                                            function sendMessage() {\
                                                var outgoingMessage = document.getElementById('text').value;\
                                                console.log(outgoingMessage);\
                                                socket.send(outgoingMessage);\
                                                return false;\
                                            }\
                                            function showMessage(message) {\
                                                var messageElem = document.createElement('div');\
                                                messageElem.appendChild(document.createTextNode(message));\
                                                document.getElementById('subscribe').appendChild(messageElem);\
                                            }\
                                            function hotkey(event) {\
                                                var id = event.keyCode;\
                                                document.getElementById('key').innerHTML = id;\
                                            }");
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
    server.send(200, "text/html", "<!DOCTYPE html>\
    <html lang=\"en\">\
    <head>\
        <meta charset=\"UTF-8\">\
        <title></title>\
        <script src=\"app.js\"></script>\
    </head>\
    <body>\
    <button onclick=\"startRight()\"> startRight</button>\
    <button onclick=\"stopRight()\"> stopRight</button>\
    <button onclick=\"startLeft()\"> startLeft</button>\
    <button onclick=\"stopLeft()\"> stopLeft</button>\
    <button onclick=\"startAll()\">startAll</button>\
    <button onclick=\"stopAll()\">stopAll</button>\
    <input type=\"text\" placeholder=\"Клавиши нажимать тут\" id=\"kinput\">\
    <!-- форма для отправки сообщений -->\
    <input id=\"text\" type=\"text\" name=\"message\">\
    <button onclick=\"sendMessage()\">Отправить</button>\
    <div>\
        <div>\
            <button id=\"SRF\"  style=\"height: 40vh; width: 40%\"> R FORWARD </button>\
            <button id=\"SLF\"   style=\"height: 40vh; width:  40%\"> L FORWARD </button>\
        </div>\
        <div>\
            <button id=\"SRB\"  style=\"height: 40vh; width:  40%\"> R BACKWARD </button>\
            <button id=\"SLB\"  style=\"height: 40vh; width: 40%\"> L BACKWARD </button>\
        </div>\
    </div>\
    <!-- здесь будут появляться входящие сообщения -->\
    <div id=\"subscribe\"></div>\
    </body>\
    </html>");
    //  "<!DOCTYPE html><html lang=\"en\"><head>    <meta charset=\"UTF-8\">
    //   <title></title>    <script src=\"app.js\"></script></head><body><button onclick=\"startRight()\"> startRight</button><button onclick=\"stopRight()\">
    //   stopRight</button><button onclick=\"startLeft()\"> startLeft</button><button onclick=\"stopLeft()\"> stopLeft</button><button onclick=\"startAll()\">
    //   startAll</button><button onclick=\"stopAll()\">stopAll</button><input type=\"text\" placeholder=\"Клавиши нажимать тут\" id=\"kinput\">
    //   <!-- форма для отправки сообщений --><input id=\"text\" type=\"text\" name=\"message\"><button onclick=\"sendMessage()\">Отправить</button><div>
    //      <div>        <button onclick=\"startRightForward()\" style=\"height: 40vh; width: 40%\"> R FORWARD </button>
    //              <button onclick=\"startLeftForward()\"  style=\"height: 40vh; width:  40%\"> L FORWARD </button>    </div>    <div>
    //                     <button onclick=\"startRightBackward()\" style=\"height: 40vh; width:  40%\"> R BACKWARD </button>
    //                            <button onclick=\"startLeftBackward()\" style=\"height: 40vh; width: 40%\"> L BACKWARD </button>
    //                               </div></div><!-- здесь будут появляться входящие сообщения --><div id=\"subscribe\"></div></body></html>");
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
