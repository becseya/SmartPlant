#pragma once

namespace SmartPlant {

class SleepInterrupter
{
  public:
    virtual bool isInterrupted()   = 0;
    virtual void handleInterrupt() = 0;
};

} // namespace SmartPlant
