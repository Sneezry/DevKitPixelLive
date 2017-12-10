/*
*  Connect WiFi
*  Reference API: https://microsoft.github.io/azure-iot-developer-kit/docs/apis/wifi/
*
*  Copyright (c) Microsoft Corporation
*  All rights reserved.
*  MIT License
*/

#include "Arduino.h";
#include "AZ3166WiFi.h";
#include "httpServer.h";

bool wifiConnected = false;
bool startServer = false;

void setup()
{
    pinMode(USER_BUTTON_B, INPUT);

    Screen.clean();
    Screen.print(0, "Pixel Live\r\n");
    Screen.print(1, "  Setting up\r\n");
    Screen.print(2, "  network...\r\n");

    if (WiFi.begin() == WL_CONNECTED)
    {
        wifiConnected = true;
        IPAddress ip = WiFi.localIP();

        Screen.clean();
        Screen.print(0, "Pixel Live\r\n");
        Screen.print(1, "  Setting up\r\n");
        Screen.print(2, "  server...\r\n");
        Screen.print(3, ip.get_address());
        httpd_server_start();
    }
    else
    {
        Serial.println("No internet connection.");
        Screen.clean();
        Screen.print(0, "Pixel Live\r\n");
        Screen.print(2, "  No network.\r\n");
    }
}

void loop()
{

}
