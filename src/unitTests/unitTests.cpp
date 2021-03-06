/* 
 * File:   unitTests.cpp
 * Author: massimo
 *
 * Created on June 19, 2017, 10:43 AM
 */
#include "../activeClass.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// BEGIN: ignore the warnings listed below when compiled with clang from here
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
////////////////////////////////////////////////////////////////////////////////
using namespace ::testing;

const std::string expectedVersion {"1.0.0"};

TEST(activeClass, activeClassVersion)
{
  std::string v = activeClass::baseActiveClass::activeClassVersion();

  ASSERT_EQ(v, expectedVersion);
}

TEST(activeClass, makeActiveClassObject)
{
  activeClass::prologueFun<int> pfun = [](int arg)
  {
    arg++;
    return false;
  };

  activeClass::bodyFun<int,int> bodyfun = [](int arg)
  {
    arg++;
    return arg;
  };

  activeClass::epilogueFun<int> efun = [](int arg)
  {
    arg++;
    return false;
  };

  // create the active class object with the functions
  activeClass::activeClassPtr<int,int> acptr = activeClass::makeActiveClass<int,int>(pfun, bodyfun, efun);
  std::string v = acptr.get()->activeClassVersion();

  ASSERT_EQ(v, expectedVersion);

  std::thread::id tid = std::this_thread::get_id();
  ASSERT_NE(tid, acptr.get()->getThreadId());
}

TEST(activeClass, runNoThread_1)
{
activeClass::prologueFun<int> pfun = [](int arg)
  {
    arg++;
    return false;
  };

  activeClass::bodyFun<int,int> bodyfun = [](int arg)
  {
    arg++;
    return arg;
  };

  activeClass::epilogueFun<int> efun = [](int arg)
  {
    arg++;
    return false;
  };

  // create the active class object with the functions
  activeClass::activeClassPtr<int,int> acptr = activeClass::makeActiveClass<int,int>(pfun, bodyfun, efun);
  std::string v = acptr.get()->activeClassVersion();

  // run all the provided functions without generating a new thread; run synch
  // in this thread
  auto [prologueResult, bodyResult, epilogueResult, thrData] = acptr.get()->activeClass<int,int>::run(5);

  std::thread::id tid = std::this_thread::get_id();
  ASSERT_EQ(tid, acptr.get()->getThreadId());
  ASSERT_EQ(6, bodyResult);
  ASSERT_EQ(5, acptr.get()->getThreadData());
  ASSERT_EQ(5, thrData);
}

TEST(activeClass, runNoThread_2)
{
activeClass::prologueFun<int> pfun = [](int arg)
  {
    arg++;
    return false;
  };

  activeClass::bodyFun<int,int> bodyfun = [](int arg)
  {
    arg++;
    return arg;
  };

  activeClass::epilogueFun<int> efun = [](int arg)
  {
    arg++;
    return false;
  };

  // create the active class object with the functions
  activeClass::activeClassPtr<int,int> acptr = activeClass::makeActiveClass<int,int>(pfun, bodyfun, efun);
  std::string v = acptr.get()->activeClassVersion();

  int arg {5};

  // run all the provided functions without generating a new thread; run synch
  // in this thread
  auto [prologueResult, bodyResult, epilogueResult, thrData] = acptr.get()->activeClass<int,int>::run(arg);

  std::thread::id tid = std::this_thread::get_id();
  ASSERT_EQ(tid, acptr.get()->getThreadId());
  ASSERT_EQ(6, bodyResult);
  ASSERT_EQ(arg, acptr.get()->getThreadData());
  ASSERT_EQ(arg, thrData);
}

TEST(activeClass, runNoThread_3)
{
  using threadData_t = struct {
    int x {};
  };

  activeClass::prologueFun<threadData_t> pfun = [](threadData_t& arg)
  {
    arg.x = 24;
    return false;
  };

  activeClass::bodyFun<int,threadData_t> bodyfun = [](threadData_t& arg)
  {
    arg.x = 78;
    return arg.x;
  };

  activeClass::epilogueFun<threadData_t> efun = [](threadData_t& arg)
  {
    arg.x = 56;
    return false;
  };
  
  // create the active class object with the functions
  activeClass::activeClassPtr<int,threadData_t> acptr = activeClass::makeActiveClass<int,threadData_t>(pfun, bodyfun, efun);
  std::string v = acptr.get()->activeClassVersion();

  threadData_t arg {89};

  // run all the provided functions without generating a new thread; run synch
  // in this thread
  auto [prologueResult, bodyResult, epilogueResult, thrData] = acptr.get()->run(arg);

  std::thread::id tid = std::this_thread::get_id();
  ASSERT_EQ(tid, acptr.get()->getThreadId());
  ASSERT_EQ(78, bodyResult);
  ASSERT_EQ(56, acptr.get()->getThreadData().x);
  ASSERT_EQ(56, thrData.x);
}

TEST(activeClass, runNoThreadWaitThreadEndsError)
{
  using threadData_t = struct {
    int x {};
  };

  activeClass::prologueFun<threadData_t> pfun = [](threadData_t& arg)
  {
    arg.x = 24;
    return false;
  };

  activeClass::bodyFun<int,threadData_t> bodyfun = [](threadData_t& arg)
  {
    for (int i {0}; i < 20; ++i)
    {
      arg.x = i;
      usleep(100000);
    }
    return arg.x;
  };

  activeClass::epilogueFun<threadData_t> efun = [](threadData_t& arg)
  {
    arg.x++;
    return false;
  };

  // create the active class object with the functions
  activeClass::activeClassPtr<int,threadData_t> acptr = activeClass::makeActiveClass<int,threadData_t>(pfun, bodyfun, efun);

  threadData_t arg {89};

  ASSERT_EQ(89, arg.x);
  ASSERT_EQ(0, acptr.get()->getThreadData().x);

  // since no thread is run bodyResult is 0
  auto [prologueResult, bodyResult, epilogueResult, threadData] = acptr.get()->waitThreadEndsAndGetResults();

  std::thread::id tid = std::this_thread::get_id();
  ASSERT_NE(tid, acptr.get()->getThreadId());
  ASSERT_EQ(89, arg.x);
  ASSERT_EQ(0, acptr.get()->getThreadData().x);
  ASSERT_EQ(false, prologueResult);
  ASSERT_EQ(0, bodyResult);
  ASSERT_EQ(false, epilogueResult);
  ASSERT_EQ(0, threadData.x);
}

TEST(activeClass, runThreadOK_1)
{
  using threadData_t = struct {
    int x {};
  };

  activeClass::prologueFun<threadData_t> pfun = [](threadData_t& arg)
  {
    arg.x = 24;
    return false;
  };

  activeClass::bodyFun<int,threadData_t> bodyfun = [](threadData_t& arg)
  {
    for (int i {0}; i < 20; ++i)
    {
      arg.x = i;
      usleep(100000);
    }
    return arg.x;
  };

  activeClass::epilogueFun<threadData_t> efun = [](threadData_t& arg)
  {
    arg.x++;
    return false;
  };

  // create the active class object with the functions
  activeClass::activeClassPtr<int,threadData_t> acptr = activeClass::makeActiveClass<int,threadData_t>(pfun, bodyfun, efun);

  threadData_t arg {89};

  ASSERT_EQ(89, arg.x);
  ASSERT_EQ(0, acptr.get()->getThreadData().x);
 
  // run the thread passing the thread data: arg is copied
  // blocked here until the end of the thread
  auto [prologueResult, bodyResult, epilogueResult, threadData] = acptr.get()->runThreadAndWaitTermination(arg);

  std::thread::id tid = std::this_thread::get_id();
  ASSERT_NE(tid, acptr.get()->getThreadId());

  // check that the thread body returned the expected value
  ASSERT_EQ(19, bodyResult);
  // check that the thread data in this scope are not changed
  ASSERT_EQ(89, arg.x);
  // check that the thread data were modified by the thread and contains the expected value
  ASSERT_EQ(20, acptr.get()->getThreadData().x);
}

TEST(activeClass, runThreadOK_2)
{
  using threadData_t = struct {
    int x {};
  };

  activeClass::prologueFun<threadData_t> pfun = [](threadData_t& arg)
  {
    arg.x = 24;
    return false;
  };

  activeClass::bodyFun<int,threadData_t> bodyfun = [](threadData_t& arg)
  {
    for (int i {0}; i < 20; ++i)
    {
      arg.x = i;
      usleep(1000);
    }
    return arg.x;
  };

  activeClass::epilogueFun<threadData_t> efun = [](threadData_t& arg)
  {
    arg.x++;
    return false;
  };

  // create the active class object with the functions
  activeClass::activeClassPtr<int,threadData_t> acptr = activeClass::makeActiveClass<int,threadData_t>(pfun, bodyfun, efun);

  threadData_t arg {89};

  ASSERT_EQ(89, arg.x);
  ASSERT_EQ(0, acptr.get()->getThreadData().x);
 
  // run the thread passing the thread data: arg is copied
  // blocked here until the end of the thread
  auto [prologueResult, bodyResult, epilogueResult, threadData] = acptr.get()->runThreadAndWaitTermination(arg);

  std::thread::id tid = std::this_thread::get_id();
  ASSERT_NE(tid, acptr.get()->getThreadId());

  // check that the thread body returned the expected value
  ASSERT_EQ(19, bodyResult);
  // check that the thread data in this scope are not changed
  ASSERT_EQ(89, arg.x);
  // check that the thread data were modified by the thread and contains the expected value
  ASSERT_EQ(20, acptr.get()->getThreadData().x);
}

TEST(activeClass, runThreadPrologueFailure)
{
  using threadData_t = struct {
    int x {};
  };

  activeClass::prologueFun<threadData_t> pfun = [](threadData_t& arg)
  {
    arg.x = 24;
    // return true to simulate a failure in the prologue
    return true;
  };

  activeClass::bodyFun<int,threadData_t> bodyfun = [](threadData_t& arg)
  {
    for (int i {0}; i < 20; ++i)
    {
      arg.x = i;
      usleep(100000);
    }
    return arg.x;
  };

  activeClass::epilogueFun<threadData_t> efun = [](threadData_t& arg)
  {
    arg.x++;
    return false;
  };

  // create the active class object with the functions
  activeClass::activeClassPtr<int,threadData_t> acptr = activeClass::makeActiveClass<int,threadData_t>(pfun, bodyfun, efun);

  threadData_t arg {89};

  ASSERT_EQ(89, arg.x);
  ASSERT_EQ(0,acptr.get()->getThreadData().x);

  // run the thread passing the thread data: arg is copied
  acptr.get()->activeClass<int,threadData_t>::runThread(arg);

  // blocked here until the end of the thread
  auto [prologueResult, bodyResult, epilogueResult, threadData] = acptr.get()->waitThreadEndsAndGetResults();

   std::thread::id tid = std::this_thread::get_id();
  ASSERT_NE(tid, acptr.get()->getThreadId());

  // check that the thread body returned the expected value
  ASSERT_EQ(19, bodyResult);
  // check that the thread data in this scope are not changed
  ASSERT_EQ(89, arg.x);
  // check that the thread data were modified by the thread and contains the expected value
  ASSERT_EQ(20, acptr.get()->getThreadData().x);
}

TEST(activeClass, runThreadOK_3)
{
  using threadData_t = struct {
    std::string s_ {};
  };

  activeClass::prologueFun<threadData_t> pfun = []([[maybe_unused]] threadData_t& arg)
  {
    return false;
  };

  activeClass::bodyFun<std::string,threadData_t> bodyfun = [](threadData_t& arg)
  {
    arg.s_ = "this is a test";
    return arg.s_;
  };

  activeClass::epilogueFun<threadData_t> efun = []([[maybe_unused]] threadData_t& arg)
  {
    return false;
  };

  // create the active class object with the functions
  activeClass::activeClassPtr<std::string,threadData_t> acptr = activeClass::makeActiveClass<std::string,threadData_t>(pfun, bodyfun, efun);

  threadData_t arg {};

  ASSERT_EQ("", arg.s_);
  ASSERT_EQ("", acptr.get()->getThreadData().s_);

  // run the thread passing the thread data: arg is copied
  acptr.get()->activeClass<std::string,threadData_t>::runThread(arg);

  // blocked here until the end of the thread
  auto [prologueResult, bodyResult, epilogueResult, thrData] = acptr.get()->waitThreadEndsAndGetResults();

  std::thread::id tid = std::this_thread::get_id();
  ASSERT_NE(tid, acptr.get()->getThreadId());
  
  // check that the thread body returned the expected value
  ASSERT_EQ(bodyResult, "this is a test");
  // check that the thread data in this scope are not changed
  ASSERT_EQ("", arg.s_);
  // check that the thread data were modified by the thread and contains the expected value
  ASSERT_EQ(bodyResult, acptr.get()->getThreadData().s_);
  ASSERT_EQ(bodyResult, thrData.s_);
}

#pragma clang diagnostic pop
// END: ignore the warnings when compiled with clang up to here
