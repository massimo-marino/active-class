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
#include <future>
////////////////////////////////////////////////////////////////////////////////
namespace activeClass
{
template <typename T, typename U>
using threadResult = std::tuple<const bool, T, const bool, U&>;

using fut = std::future<int>;

class baseActiveClass
{
public:
  baseActiveClass() = default;
  ~baseActiveClass() = default;
  // copy forbidden
  baseActiveClass(const baseActiveClass& rhs) = delete;
  // copy assignment forbidden
  baseActiveClass& operator=(const baseActiveClass& rhs) = delete;

  static const std::string& activeClassVersion () noexcept;
  int waitThreadEnds();
  std::future_status checkThread(const std::chrono::milliseconds& delay);

 protected:
  static const std::string version_;
  bool prologueResult_ {};
  bool epilogueResult_ {};
  fut threadFuture_ {};

  bool getPrologueResult() const noexcept;
  void setPrologueResult(const bool result) noexcept;
  bool getEpilogueResult() const noexcept;
  void setEpilogueResult(const bool result) noexcept;
  fut& getThreadFuture () noexcept;
  void setThreadFuture(fut threadFuture) noexcept;
};  // class baseActiveClass

template <typename U>
using prologueFun = std::function<const bool&&(U&)>;

template <typename U>
using epilogueFun = std::function<const bool&&(U&)>;

template <typename T, typename U>
using bodyFun = std::function<T(U&)>;

template <typename T, typename U>
class activeClass final : public baseActiveClass
{
public:
  activeClass() = default;
  ~activeClass() = default;
  // copy forbidden
  activeClass(const activeClass& rhs) = delete;
  // copy assignment forbidden
  activeClass& operator=(const activeClass& rhs) = delete;

  explicit activeClass(const prologueFun<U>& pfun,
                       const bodyFun<T,U>& bodyfun,
                       const epilogueFun<U>& efun)
  :
  pfun_(pfun),
  efun_(efun),
  bodyfun_(bodyfun)
  {}

  U& getThreadData () noexcept
  {
    return threadData_;
  }

  void run(const U& threadData) noexcept
  {
    setThreadData(threadData);
    activeClassBody();
  }

  void runThread(const U& threadData) noexcept
  {
    // the thread gets a lambda; it cannot be a class method because it should be
    // declared static, and this would break the class design, so that's why we
    // use a lambda here capturing by reference the thread data and this
    setThreadFuture(std::async(std::launch::async,
                               [&]() -> int {
                                 run(threadData);
                                 return 0;
                               } ) );
  }

private:
  prologueFun<U> pfun_ {};
  epilogueFun<U> efun_ {};
  bodyFun<T,U> bodyfun_ {};

  T bodyResult_ {};
  U threadData_ {};

  void setBodyResult(const T result) noexcept
  {
    bodyResult_ = result;
  }

  T getBodyResult () const noexcept
  {
    return bodyResult_;
  }

  void setThreadData(const U& threadData) noexcept
  {
    threadData_ = std::move(threadData);
  }

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

////////////////////////////////////////////////////////////////////////////////
// active class factory

template <typename T, typename U>
using activeClassPtr = std::unique_ptr<activeClass<T,U>>;

// create an object of type T and return a std::unique_ptr to it
template <typename T, typename... Args>
auto create_unique_ptr(Args&&... args) -> std::unique_ptr<T>
{
  // since C++14
  return std::make_unique<T>(args...);

  // before C++14:  
  //std::unique_ptr<T> local_ptr(new T(args...));
  //return local_ptr; // local_ptr will surrender ownership;
                    // the compiler should optimize the return as if it was:
                    // return std::move(local_ptr);
}

template <typename T, typename U>
activeClassPtr<T,U>
makeActiveClass (const prologueFun<U>& pfun,
                 const bodyFun<T,U>& bodyfun,
                 const epilogueFun<U>& efun) noexcept
{
  return create_unique_ptr<activeClass<T,U>>(pfun, bodyfun, efun);
}
}  // namespace activeClass
#endif /* ACTIVECLASS_H */
