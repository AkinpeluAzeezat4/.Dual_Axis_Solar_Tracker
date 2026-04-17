/*#ifndef MESH_COMM_H
#define MESH_COMM_H

#include <Arduino.h>

namespace mesh_comm
{
  struct Message
  {
    bool alarmTrigger;
  };

  void beginAsMaster();
  void beginAsSlave();
  void update();

  void sendAlarmTrigger();

  bool isAlarmTriggered();
}

#endif
*/

/*
#ifndef MESH_COMM_H
#define MESH_COMM_H

#include <Arduino.h>

namespace mesh_comm
{
  struct Message
  {
    bool alarmTrigger;
  };

  void beginAsMaster(const uint8_t *peerMac, uint8_t wifiChannel = 1);
  void beginAsSlave(uint8_t wifiChannel = 1);
  void update();

  void sendAlarmTrigger();

  bool isAlarmTriggered();
}

#endif
*/

/*
#ifndef MESH_COMM_H
#define MESH_COMM_H

#include <Arduino.h>

namespace mesh_comm
{
  struct Message
  {
    bool alarmTrigger;
  };

  void beginAsMaster(const uint8_t *peerMac);
  void beginAsSlave();
  void update();

  void sendAlarmTrigger();

  bool isAlarmTriggered();
}

#endif

*/

#ifndef MESH_COMM_H
#define MESH_COMM_H

#include <Arduino.h>

namespace mesh_comm
{
  struct Message
  {
    bool alarmTrigger;
  };

  void beginAsMaster(const uint8_t peers[][6], uint8_t peerCount);
  void beginAsSlave();
  void update();

  void sendAlarmTrigger();

  bool isAlarmTriggered();
}

#endif
