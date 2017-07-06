/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../activeClass.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

activeClass::prologueFun<int> pfun = [](int arg) -> bool
{
  arg++;
  return false;
};

activeClass::epilogueFun<int> efun = [](int arg) -> bool
{
  arg++;
  return false;
};

activeClass::bodyFun<int,int> bodyfun = [](int arg) -> int
{
  arg++;
  return arg;
};

const std::string expectedVersion {"1.0.0"};

TEST(activeClass, activeClassVersion)
{
  std::string v = activeClass::baseActiveClass::activeClassVersion();

  ASSERT_THAT(v, Eq(expectedVersion));
}

TEST(activeClass, makeActiveClassObject)
{
  std::unique_ptr<activeClass::activeClass<int,int>> acptr = activeClass::makeActiveClass<int,int>(pfun, bodyfun, efun);
  std::string v = acptr.get()->activeClassVersion();

  ASSERT_THAT(v, Eq(expectedVersion));
}

TEST(activeClass, runNoThread_1)
{
  std::unique_ptr<activeClass::activeClass<int,int>> acptr = activeClass::makeActiveClass<int,int>(pfun, bodyfun, efun);
  std::string v = acptr.get()->activeClassVersion();

  acptr.get()->activeClass<int,int>::run(5);

  ASSERT_THAT(5, Eq(acptr.get()->getThreadData()));
}

TEST(activeClass, runNoThread_2)
{
  std::unique_ptr<activeClass::activeClass<int,int>> acptr = activeClass::makeActiveClass<int,int>(pfun, bodyfun, efun);
  std::string v = acptr.get()->activeClassVersion();
  int arg{5};

  acptr.get()->activeClass<int,int>::run(arg);

  ASSERT_THAT(arg, Eq(acptr.get()->getThreadData()));
}

TEST(activeClass, runNoThread_3)
{
  using threadData_t = struct {
    int x{};
  };

  activeClass::prologueFun<threadData_t> pfun = [](threadData_t& arg) -> bool
  {
    arg.x = 24;
    return false;
  };

  activeClass::epilogueFun<threadData_t> efun = [](threadData_t& arg) -> bool
  {
    arg.x = 56;
    return false;
  };

  activeClass::bodyFun<int,threadData_t> bodyfun = [](threadData_t& arg) -> int
  {
    arg.x = 89;
    return arg.x;
  };
  
  std::unique_ptr<activeClass::activeClass<int,threadData_t>> acptr = activeClass::makeActiveClass<int,threadData_t>(pfun, bodyfun, efun);
  std::string v = acptr.get()->activeClassVersion();

  threadData_t arg{89};

  acptr.get()->activeClass<int,threadData_t>::run(arg);

  ASSERT_THAT(56, Eq(acptr.get()->getThreadData().x));
}

TEST(activeClass, runNoThreadWaitThreadEndsError)
{
  using threadData_t = struct {
    int x{};
  };

  activeClass::prologueFun<threadData_t> pfun = [](threadData_t& arg) -> bool
  {
    arg.x = 24;
    return false;
  };

  activeClass::epilogueFun<threadData_t> efun = [](threadData_t& arg) -> bool
  {
    arg.x++;
    return false;
  };

  activeClass::bodyFun<int,threadData_t> bodyfun = [](threadData_t& arg) -> int
  {
    for ( unsigned int i = 0; i < 20; ++i)
    {
      arg.x = i;
      usleep(100000);
    }
    return arg.x;
  };
  
  std::unique_ptr<activeClass::activeClass<int,threadData_t>> acptr = activeClass::makeActiveClass<int,threadData_t>(pfun, bodyfun, efun);

  threadData_t arg{89};

  ASSERT_THAT(89, Eq(arg.x));
  ASSERT_THAT(0, Eq(acptr.get()->getThreadData().x));
   
  // since no thread was run waitThreadEnds() returns -1
  int threadResult = acptr.get()->activeClass<int,threadData_t>::waitThreadEnds();

  ASSERT_THAT(-1, Eq(threadResult));
  ASSERT_THAT(89, Eq(arg.x));
  ASSERT_THAT(0, Eq(acptr.get()->getThreadData().x));
}

TEST(activeClass, runThreadOK)
{
  using threadData_t = struct {
    int x{};
  };

  activeClass::prologueFun<threadData_t> pfun = [](threadData_t& arg) -> bool
  {
    arg.x = 24;
    return false;
  };

  activeClass::epilogueFun<threadData_t> efun = [](threadData_t& arg) -> bool
  {
    arg.x++;
    return false;
  };

  activeClass::bodyFun<int,threadData_t> bodyfun = [](threadData_t& arg) -> int
  {
    for ( unsigned int i = 0; i < 20; ++i)
    {
      arg.x = i;
      usleep(100000);
    }
    return arg.x;
  };

  // create the active class object with the functions 
  std::unique_ptr<activeClass::activeClass<int,threadData_t>> acptr = activeClass::makeActiveClass<int,threadData_t>(pfun, bodyfun, efun);

  threadData_t arg{89};

  ASSERT_THAT(89, Eq(arg.x));
  ASSERT_THAT(0, Eq(acptr.get()->getThreadData().x));
 
  // run the thread passing the thread data: arg is copied
  acptr.get()->activeClass<int,threadData_t>::runThread(arg);
  
  // blocked here until the end of the thread
  int threadResult = acptr.get()->activeClass<int,threadData_t>::waitThreadEnds();

  // check that the thread returned the expected value
  ASSERT_THAT(0, Eq(threadResult));
  // check that the thread data in this scope are not changed
  ASSERT_THAT(89, Eq(arg.x));
  // chech that the thread data were modified and contains the expected value
  ASSERT_THAT(20, Eq(acptr.get()->getThreadData().x));
}

TEST(activeClass, runThreadPrologueFailure)
{
  using threadData_t = struct {
    int x{};
  };

  activeClass::prologueFun<threadData_t> pfun = [](threadData_t& arg) -> bool
  {
    arg.x = 24;
    // return true to simulate a failure in the prologue
    return true;
  };

  activeClass::epilogueFun<threadData_t> efun = [](threadData_t& arg) -> bool
  {
    arg.x++;
    return false;
  };

  activeClass::bodyFun<int,threadData_t> bodyfun = [](threadData_t& arg) -> int
  {
    for (unsigned int i = 0; i < 20; ++i)
    {
      arg.x = i;
      usleep(100000);
    }
    return arg.x;
  };

  // create the active class object with the functions 
  std::unique_ptr<activeClass::activeClass<int,threadData_t>> acptr = activeClass::makeActiveClass<int,threadData_t>(pfun, bodyfun, efun);

  threadData_t arg{89};

  ASSERT_THAT(89, Eq(arg.x));
  ASSERT_THAT(0, Eq(acptr.get()->getThreadData().x));
 
  // run the thread passing the thread data: arg is copied
  acptr.get()->activeClass<int,threadData_t>::runThread(arg);
  
  // blocked here until the end of the thread
  int threadResult = acptr.get()->activeClass<int,threadData_t>::waitThreadEnds();

  // check that the thread returned the expected value
  ASSERT_THAT(0, Eq(threadResult));
  // check that the thread data in this scope are not changed
  ASSERT_THAT(89, Eq(arg.x));
  // check that the thread data were modified and contains the expected value
  ASSERT_THAT(24, Eq(acptr.get()->getThreadData().x));
}

//int main(int argc, char **argv) {
//  ::testing::InitGoogleTest(&argc, argv);
//  return RUN_ALL_TESTS();
//}
