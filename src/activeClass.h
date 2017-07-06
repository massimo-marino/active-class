/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   activeClass.h
 * Author: massimo
 *
 * Created on June 19, 2017, 10:43 AM
 */

#ifndef ACTIVECLASS_H
#define ACTIVECLASS_H

#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <functional>
////////////////////////////////////////////////////////////////////////////////
namespace activeClass
{
// create a Thing of type T and return a std::unique_ptr to it
template <typename T, typename... Args>
std::unique_ptr<T> create_thing(Args... args)
{
  std::unique_ptr<T> local_ptr(new T(args...));
  return local_ptr; // local_ptr will surrender ownership;
                    // the compiler should optimize the return as if it was:
                    // return std::move(local_ptr);
}

class baseActiveClass
{
public:
  baseActiveClass() = default;
  virtual ~baseActiveClass() = default;

  static std::string activeClassVersion () noexcept;

private:
  // copy forbidden
  baseActiveClass(const baseActiveClass& rhs);
  // copy assignment forbidden
  baseActiveClass& operator=(const baseActiveClass& rhs);

  static std::string version_;
};  // class baseActiveClass

template <typename U>
using prologueFun = std::function<bool(U&)>;

template <typename U>
using epilogueFun = std::function<bool(U&)>;

template <typename T, typename U>
using bodyFun = std::function<T(U&)>;

template <typename T, typename U>
class activeClass :public baseActiveClass
{
public:
  activeClass() = default;
  ~activeClass() = default;

  activeClass(const prologueFun<U>& pfun,
              const bodyFun<T,U>& bodyfun,
              const epilogueFun<U>& efun)
  :
  pfun_(pfun),
  efun_(efun),
  bodyfun_(bodyfun)
  {};

  bool getPrologueResult() const noexcept
  {
    return prologueResult_;
  }

  void setPrologueResult(const bool result) noexcept
  {
    prologueResult_ = result;
  }

  bool getEpilogueResult() const noexcept
  {
    return epilogueResult_;
  }

  void setEpilogueResult(const bool result) noexcept
  {
    epilogueResult_ = result;
  }

  T getBodyResult () const noexcept
  {
    return bodyResult_;
  }

  void setBodyResult(const T result) noexcept
  {
    bodyResult_ = result;
  }

  U& getThreadData () noexcept
  {
    return threadData_;
  }

  void setThreadData(const U& threadData) noexcept
  {
    threadData_ = std::move(threadData);
  }

  std::future<int>& getThreadFuture () noexcept
  {
    return threadFuture_;
  }

  void setThreadFuture(std::future<int> threadFuture) noexcept
  {
    threadFuture_ = std::move(threadFuture);
  }

  void run(const U& threadData) noexcept
  {
    setThreadData(threadData);
    activeClassBody();
  }

  void runThread(const U& threadData) noexcept
  {
    // thread body is a lambda
//    auto threadLambda = [&]() -> int {
//      setThreadData(threadData);
//      activeClassBody();
//      
//      return 0;
//    };
//    setThreadFuture(std::async(std::launch::async,threadLambda));

    setThreadFuture(std::async(std::launch::async,
                               [&]() -> int {
                                 setThreadData(threadData);
                                 activeClassBody();
                                 return 0;
                               } ) );
  }

  int waitThreadEnds()
  {
    int threadResult{-1};

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

private:
  prologueFun<U> pfun_{};
  epilogueFun<U> efun_{};
  bodyFun<T,U> bodyfun_{};

  bool prologueResult_{};
  bool epilogueResult_{};
  T bodyResult_{};
  U threadData_{};
  
  std::future<int> threadFuture_{};

  void activeClassBody() noexcept
  {
    setPrologueResult(pfun_(getThreadData()) );

    if ( false == getPrologueResult() )
    {
      setBodyResult(bodyfun_(getThreadData()) );
      setEpilogueResult(efun_(getThreadData()) );
    }
  }
};  // class activeClass

template <typename T, typename U>
std::unique_ptr<activeClass<T,U>> makeActiveClass (const prologueFun<U>& pfun,
                                                   const bodyFun<T,U>& bodyfun,
                                                   const epilogueFun<U>& efun) noexcept
{
  return create_thing<activeClass<T,U>>(pfun, bodyfun, efun);
}
}  // namespace activeClass
#endif /* ACTIVECLASS_H */
