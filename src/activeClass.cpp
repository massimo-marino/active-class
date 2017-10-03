/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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
