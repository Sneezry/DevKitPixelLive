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
    Screen.print("Connecting...");

    if (WiFi.begin() == WL_CONNECTED)
    {
        wifiConnected = true;
        IPAddress ip = WiFi.localIP();

        Serial.print("SSID: ");
        Serial.println(WiFi.SSID());
        Serial.print("IP address: ");
        Serial.println(ip.get_address());
        Screen.clean();
        Screen.print(ip.get_address());
    }
    else
    {
        Serial.println("No internet connection.");
        Screen.clean();
        Screen.print("No WiFi.");
    }
}

void loop()
{
    if (wifiConnected)
    {
        if (!startServer && digitalRead(USER_BUTTON_B) == LOW)
        {
            startServer = true;
            httpd_server_start();
        }
    }
}
