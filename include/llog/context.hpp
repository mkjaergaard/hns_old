#pragma once

#include <vector>
#include <string>
#include <iostream> // tmp

#include <llog/severity_type.hpp>
#include <llog/argument.hpp>

namespace llog
{

class Context
{
public:

protected:
  Context * parent_;

  typedef std::vector<Context*> ChildListType;
  ChildListType childs_;

  std::string name_;
  std::string full_name_;
  SeverityType severity_;

protected:
  void registerChild(Context * child)
  {
    //todo: synchronized
    childs_.push_back(child);
  }

public:
  Context(const std::string& name, Context * parent = 0) :
    name_(name),
    full_name_(),
    parent_(parent),
    severity_(0)
  {
    if(parent_ != 0)
    {
      parent_->registerChild(this);
      full_name_ = parent->getFullName();
    }
    full_name_.append("/");
    full_name_.append(name_);
  }

  ~Context()
  {
    //todo: consider how to handle destruction
  }

  void setLoggingSeverity(const SeverityType severity)
  {
    severity_ = severity;
  }

  void setLoggingSeverityRecursive(const SeverityType severity)
  {
    severity_ = severity;
    for(ChildListType::iterator it = childs_.begin();
	it != childs_.end();
	it++)
    {
      (*it)->setLoggingSeverityRecursive(severity);
    }
  }

  bool isSeverityLogged(const SeverityType severity)
  {
    return severity <= severity_;
  }

  const std::string& getFullName()
  {
    return full_name_;
  }

  template<typename T>
  void log(const SeverityType severity,
	   const char* name1, const Argument<T>& argument1)
  {
    if(isSeverityLogged(severity))
    {
      // todo: dispatch to a backend instead
      std::cout << full_name_ << ":" << std::endl
		<< "  " << name1 << llog::to_string(argument1) << std::endl;
    }
  }

};

}
