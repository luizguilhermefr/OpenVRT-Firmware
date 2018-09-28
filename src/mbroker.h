#ifndef OPENVRT_FIRMWARE_MBROKER_H
#define OPENVRT_FIRMWARE_MBROKER_H

unsigned int next_id()
{
  static unsigned int id = 0;
  id++;
  return id;
}

#endif //OPENVRT_FIRMWARE_MBROKER_H
