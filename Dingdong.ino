#include <EthernetENC.h>
#include "Dingdong.hpp"

EthernetServer mServer(80);

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup()
{
    Serial.begin(115200);
    Serial.println("Hello world!");

    delay(500);

    Ethernet.begin(mac);
    Serial.println("Connecting w/ internet...");
    delay(2000);

    Serial.println("Starting server...");
    mServer.begin();
    delay(2000);

    Serial.print("Ready in:");
    Serial.println(Ethernet.localIP());
}

void dashboard(const DINGDONG::RHandle& req)
{

    Serial.print("abc");
    return;
}

void other(const DINGDONG::RHandle& req)
{

    Serial.print("nada");
    return;
}

int linestack = 0;
void loop()
{
    linestack++;
    if(linestack > 35) {
        linestack = 0;
        Serial.println();
    }
    Serial.print(".");


    DINGDONG::conn app(mServer);
    app.get("/", dashboard);
    app.get("/a", other);


    delay(200);
}