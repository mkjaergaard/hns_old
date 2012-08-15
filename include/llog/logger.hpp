#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <boost/date_time/posix_time/time_formatters.hpp>

//

#include <llog/severity_type.hpp>
#include <llog/argument.hpp>
//#include <llog/context__fwd.hpp>
#include <llog/context.hpp>

namespace bt = boost::posix_time;

namespace llog
{

class Logger
{
protected:
  typedef std::vector<boost::shared_ptr<Context> > ContextListType;
  ContextListType contexts_;
  SeverityType severity_;

  static Logger instance_;

  Logger() :
    severity_(Severity::All)
  {
  }

  boost::shared_ptr<Context> createContext_(const std::string& name)
  {
    boost::shared_ptr<Context> context = boost::make_shared<Context>(name);
    contexts_.push_back(context);
    return context;
  }

public:
  bool isSeverityLogged(const SeverityType severity)
  {
    return severity <= severity_;
  }

  static Logger& getInstance()
  {
    return instance_;
  }

  static boost::shared_ptr<Context> createContext(const std::string& name)
  {
    return Logger::getInstance().createContext_(name);
  }

};

#include <stdio.h>

static void log(const SeverityType severity,
		const char* event)
{
  if(Logger::getInstance().isSeverityLogged(severity))
  {
    char buffer[512];
    sprintf(buffer,
	    "[%s %s] %s",
	    llog::Severity::name(severity),
	    bt::to_simple_string(bt::microsec_clock::universal_time().time_of_day()).c_str(),
	    event);

    std::cout << buffer << std::endl;
  }
}

template<typename T1>
static void log(const SeverityType severity,
		const char* event,
		const char* name1, const Argument<T1>& argument1)
{
  if(Logger::getInstance().isSeverityLogged(severity))
  {
    log(severity, event);
    std::cout << " - " << name1 << ": " << llog::to_string(argument1) << std::endl;
  }
}

template<typename T1, typename T2>
static void log(const SeverityType severity,
		const char* event,
		const char* name1, const Argument<T1>& argument1,
		const char* name2, const Argument<T2>& argument2)
{
  if(Logger::getInstance().isSeverityLogged(severity))
  {
    log<T1>(severity, event,
	    name1, argument1);
    std::cout << " - " << name2 << ": " << llog::to_string(argument2) << std::endl;
  }
}

}
