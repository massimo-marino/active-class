/* 
 * File:   activeClass.cpp
 * Author: massimo
 *
 * Created on June 19, 2017, 10:43 AM
 */

#include "activeClass.h"
#include <exception>
////////////////////////////////////////////////////////////////////////////////
namespace activeClass
{
const std::string baseActiveClass::version_ {"1.0.0"};

//
// activeClassVersion
//
// Purpose: Return the library version
//
const std::string& baseActiveClass::activeClassVersion () noexcept
{
  return version_;

}  // activeClassVersion

baseActiveClass::~baseActiveClass()
{}
////////////////////////////////////////////////////////////////////////////////
std::thread::id baseActiveClass::getThreadId () const noexcept
{
  return threadId_;
}
  
bool baseActiveClass::getPrologueResult() const noexcept
{
  return prologueResult_;
}

void baseActiveClass::setPrologueResult(const bool result) const noexcept
{
  prologueResult_ = result;
}

bool baseActiveClass::getEpilogueResult() const noexcept
{
  return epilogueResult_;
}

void baseActiveClass::setEpilogueResult(const bool result) const noexcept
{
  epilogueResult_ = result;
}
}  // namespace activeClass
