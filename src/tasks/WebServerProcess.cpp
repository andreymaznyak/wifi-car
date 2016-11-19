#ifndef _WEB_SERVER_PROCESS_
#define _WEB_SERVER_PROCESS_

#include <ProcessScheduler.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

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

      server.on("/inline", [this](){
         server.send(200, "text/plain", "this works as well");
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
    server.send(200, "text/plain", "homepage");
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
