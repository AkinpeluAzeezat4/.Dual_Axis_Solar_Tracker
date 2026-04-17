/*#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "mesh_comm.h"

namespace mesh_comm
{

  static Message incomingMessage;
  static bool alarmTriggered = false;

  void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
  {
  }

  void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
  {
    if (len == sizeof(Message))
    {
      memcpy(&incomingMessage, incomingData, sizeof(Message));

      if (incomingMessage.alarmTrigger)
      {
        alarmTriggered = true;
      }
    }
  }

  void beginAsMaster()
  {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK)
    {
      Serial.println("ESP-NOW init failed");
      return;
    }

    esp_now_register_send_cb(onDataSent);

    esp_now_peer_info_t peerInfo = {};
    memset(peerInfo.peer_addr, 0xFF, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.println("Failed to add broadcast peer");
      return;
    }

    Serial.println("ESP-NOW master ready");
  }

  void beginAsSlave()
  {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK)
    {
      Serial.println("ESP-NOW init failed");
      return;
    }

    esp_now_register_recv_cb(onDataRecv);

    Serial.println("ESP-NOW slave ready");
  }

  void update()
  {
    // Serial.begin(115200);
  }

  void sendAlarmTrigger()
  {
    Message msg;
    msg.alarmTrigger = true;
    Serial.println("sending messages");
    esp_now_send(NULL, (uint8_t *)&msg, sizeof(msg));
  }

  bool isAlarmTriggered()
  {
    if (alarmTriggered)
    {
      Serial.println("intruder detected");
      alarmTriggered = false;
      return true;
    }

    return false;
  }
}

*/

/*

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "mesh_comm.h"

namespace mesh_comm
{
  static Message incomingMessage;
  static bool alarmTriggered = false;

  static uint8_t targetMac[6] = {0};
  static bool peerReady = false;

  void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
  {
    Serial.print("Send status: ");
    if (status == ESP_NOW_SEND_SUCCESS)
      Serial.println("Success");
    else
      Serial.println("Fail");
  }

  void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
  {
    Serial.print("Received bytes: ");
    Serial.println(len);

    if (len == sizeof(Message))
    {
      memcpy(&incomingMessage, incomingData, sizeof(Message));

      if (incomingMessage.alarmTrigger)
      {
        alarmTriggered = true;
        Serial.println("Alarm trigger received");
      }
    }
  }

  void beginAsMaster(const uint8_t *peerMac, uint8_t wifiChannel)
  {
    memcpy(targetMac, peerMac, 6);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(wifiChannel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    Serial.print("Master MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.print("Master channel: ");
    Serial.println(wifiChannel);

    if (esp_now_init() != ESP_OK)
    {
      Serial.println("ESP-NOW init failed");
      return;
    }

    esp_now_register_send_cb(onDataSent);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, targetMac, 6);
    peerInfo.channel = wifiChannel;
    peerInfo.encrypt = false;

    esp_err_t addStatus = esp_now_add_peer(&peerInfo);
    if (addStatus != ESP_OK)
    {
      Serial.print("Add peer failed: ");
      Serial.println(addStatus);
      return;
    }

    peerReady = true;
    Serial.println("ESP-NOW master ready");
  }

  void beginAsSlave(uint8_t wifiChannel)
  {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(wifiChannel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    Serial.print("Slave MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.print("Slave channel: ");
    Serial.println(wifiChannel);

    if (esp_now_init() != ESP_OK)
    {
      Serial.println("ESP-NOW init failed");
      return;
    }

    esp_now_register_recv_cb(onDataRecv);

    Serial.println("ESP-NOW slave ready");
  }

  void update()
  {
  }

  void sendAlarmTrigger()
  {
    if (!peerReady)
    {
      Serial.println("Peer not ready");
      return;
    }

    Message msg;
    msg.alarmTrigger = true;

    esp_err_t result = esp_now_send(targetMac, (uint8_t *)&msg, sizeof(msg));

    Serial.print("esp_now_send result: ");
    Serial.println(result);
  }

  bool isAlarmTriggered()
  {
    if (alarmTriggered)
    {
      alarmTriggered = false;
      return true;
    }

    return false;
  }
}
*/

/*

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "mesh_comm.h"

namespace mesh_comm
{
  static Message incomingMessage;
  static bool alarmTriggered = false;

  static uint8_t receiverMac[6] = {0};

  void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
  {
    Serial.print("Send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  }

  void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
  {
    Serial.println("Message received");

    if (len == sizeof(Message))
    {
      memcpy(&incomingMessage, incomingData, sizeof(Message));

      if (incomingMessage.alarmTrigger)
      {
        alarmTriggered = true;
        Serial.println("Alarm trigger received");
      }
    }
  }

  void beginAsMaster(const uint8_t *peerMac)
  {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.print("Master MAC: ");
    Serial.println(WiFi.macAddress());

    Serial.print("Master channel: ");
    Serial.println(WiFi.channel());

    if (esp_now_init() != ESP_OK)
    {
      Serial.println("ESP-NOW init failed");
      return;
    }

    esp_now_register_send_cb(onDataSent);

    memcpy(receiverMac, peerMac, 6);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, receiverMac, 6);
    peerInfo.channel = WiFi.channel();
    peerInfo.encrypt = false;

    esp_err_t result = esp_now_add_peer(&peerInfo);

    if (result != ESP_OK)
    {
      Serial.print("Failed to add peer: ");
      Serial.println(result);
      return;
    }

    Serial.println("ESP-NOW master ready");
  }

  void beginAsSlave()
  {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.print("Slave MAC: ");
    Serial.println(WiFi.macAddress());

    Serial.print("Slave channel: ");
    Serial.println(WiFi.channel());

    if (esp_now_init() != ESP_OK)
    {
      Serial.println("ESP-NOW init failed");
      return;
    }

    esp_now_register_recv_cb(onDataRecv);

    Serial.println("ESP-NOW slave ready");
  }

  void update()
  {
  }

  void sendAlarmTrigger()
  {
    Message msg;
    msg.alarmTrigger = true;

    esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&msg, sizeof(msg));

    Serial.print("esp_now_send result: ");
    Serial.println(result);
  }

  bool isAlarmTriggered()
  {
    if (alarmTriggered)
    {
      alarmTriggered = false;
      return true;
    }

    return false;
  }
}
*/

/*
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "mesh_comm.h"

namespace mesh_comm
{
  static Message incomingMessage;
  static bool alarmTriggered = false;
  static uint8_t receiverMac[6] = {0};

  void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
  {
    Serial.print("Send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  }

  void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
  {
    Serial.println("Message received");

    if (len == sizeof(Message))
    {
      memcpy(&incomingMessage, incomingData, sizeof(Message));

      if (incomingMessage.alarmTrigger)
      {
        alarmTriggered = true;
        Serial.println("Alarm trigger received");
      }
    }
  }

  void beginAsMaster(const uint8_t *peerMac)
  {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.print("Master MAC: ");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK)
    {
      Serial.println("ESP-NOW init failed");
      return;
    }

    esp_now_register_send_cb(onDataSent);

    memcpy(receiverMac, peerMac, 6);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, receiverMac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    esp_err_t result = esp_now_add_peer(&peerInfo);

    if (result != ESP_OK)
    {
      Serial.print("Failed to add peer: ");
      Serial.println(result);
      return;
    }

    Serial.println("ESP-NOW master ready");
  }

  void beginAsSlave()
  {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.print("Slave MAC: ");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK)
    {
      Serial.println("ESP-NOW init failed");
      return;
    }

    esp_now_register_recv_cb(onDataRecv);

    Serial.println("ESP-NOW slave ready");
  }

  void update()
  {
  }

  void sendAlarmTrigger()
  {
    Message msg;
    msg.alarmTrigger = true;

    esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&msg, sizeof(msg));

    Serial.print("esp_now_send result: ");
    Serial.println(result);
  }

  bool isAlarmTriggered()
  {
    if (alarmTriggered)
    {
      alarmTriggered = false;
      return true;
    }

    return false;
  }
}

*/

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "mesh_comm.h"

namespace mesh_comm
{
  static Message incomingMessage;
  static bool alarmTriggered = false;

  static uint8_t peerMacs[10][6];
  static uint8_t totalPeers = 0;

  void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
  {
    Serial.print("Send to ");
    if (mac_addr != nullptr)
    {
      for (int i = 0; i < 6; i++)
      {
        if (mac_addr[i] < 16)
          Serial.print("0");
        Serial.print(mac_addr[i], HEX);
        if (i < 5)
          Serial.print(":");
      }
    }
    else
    {
      Serial.print("unknown");
    }

    Serial.print(" -> ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  }

  void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
  {
    Serial.println("Message received");

    if (len == sizeof(Message))
    {
      memcpy(&incomingMessage, incomingData, sizeof(Message));

      if (incomingMessage.alarmTrigger)
      {
        alarmTriggered = true;
        Serial.println("Alarm trigger received");
      }
    }
  }

  void beginAsMaster(const uint8_t peers[][6], uint8_t peerCount)
  {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.print("Master MAC: ");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK)
    {
      Serial.println("ESP-NOW init failed");
      return;
    }

    esp_now_register_send_cb(onDataSent);

    totalPeers = peerCount;

    for (uint8_t i = 0; i < totalPeers; i++)
    {
      memcpy(peerMacs[i], peers[i], 6);

      esp_now_peer_info_t peerInfo = {};
      memcpy(peerInfo.peer_addr, peerMacs[i], 6);
      peerInfo.channel = 0;
      peerInfo.encrypt = false;

      esp_err_t result = esp_now_add_peer(&peerInfo);

      Serial.print("Adding peer ");
      Serial.print(i + 1);
      Serial.print(": ");

      if (result == ESP_OK)
      {
        Serial.println("OK");
      }
      else
      {
        Serial.print("Failed -> ");
        Serial.println(result);
      }
    }

    Serial.println("ESP-NOW master ready");
  }

  void beginAsSlave()
  {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(200);

    Serial.print("Slave MAC: ");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK)
    {
      Serial.println("ESP-NOW init failed");
      return;
    }

    esp_now_register_recv_cb(onDataRecv);

    Serial.println("ESP-NOW slave ready");
  }

  void update()
  {
  }

  void sendAlarmTrigger()
  {
    Message msg;
    msg.alarmTrigger = true;

    for (uint8_t i = 0; i < totalPeers; i++)
    {
      esp_err_t result = esp_now_send(peerMacs[i], (uint8_t *)&msg, sizeof(msg));

      Serial.print("Sending to peer ");
      Serial.print(i + 1);
      Serial.print(" result: ");
      Serial.println(result);
    }
  }

  bool isAlarmTriggered()
  {
    if (alarmTriggered)
    {
      alarmTriggered = false;
      return true;
    }

    return false;
  }
}
