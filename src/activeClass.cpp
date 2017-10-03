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
////////////////////////////////////////////////////////////////////////////////
bool baseActiveClass::getPrologueResult() const noexcept
{
  return prologueResult_;
}

void baseActiveClass::setPrologueResult(const bool result) noexcept
{
  prologueResult_ = result;
}

bool baseActiveClass::getEpilogueResult() const noexcept
{
  return epilogueResult_;
}

void baseActiveClass::setEpilogueResult(const bool result) noexcept
{
  epilogueResult_ = result;
}

std::future<int>& baseActiveClass::getThreadFuture () noexcept
{
  return threadFuture_;
}

void baseActiveClass::setThreadFuture(std::future<int> threadFuture) noexcept
{
  threadFuture_ = std::move(threadFuture);
}

int baseActiveClass::waitThreadEnds()
{
  int threadResult {-1};

  try
  {
    threadResult = getThreadFuture().get();
  }
    catch (const std::exception& e)
  {
    std::cerr << __func__
              << ": EXCEPTION: "
              << e.what()
              << std::endl;
  }
  return threadResult;
}

std::future_status baseActiveClass::checkThread(const std::chrono::milliseconds& delay)
{
  // blocks for delay milliseconds
  return getThreadFuture().wait_for(delay);
}

//
// activeClassVersion
//
// Purpose: Return the library version
//
const std::string& baseActiveClass::activeClassVersion () noexcept
{
  return version_;

}  // activeClassVersion
}  // namespace activeClass
