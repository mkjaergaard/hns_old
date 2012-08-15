#pragma once

#include <stdint.h>

namespace llog
{

typedef uint32_t SeverityType;

class Severity
{

public:
  const static uint32_t Fatal = 0;
  const static uint32_t Error = 10;
  const static uint32_t Warning = 20;
  const static uint32_t Info = 30;
  const static uint32_t Debug = 40;
  const static uint32_t Trace = 50;
  const static uint32_t All = 9999;

  static const char * name(const SeverityType severity)
  {
    switch(severity)
    {
    case Fatal:
      return "FTL";
    case Error:
      return "ERR";
    case Warning:
      return "WRN";
    case Info:
      return "INF";
    case Debug:
      return "DBG";
    case Trace:
      return "TRC";
    default:
      return "???";
    }
  }

};

}
