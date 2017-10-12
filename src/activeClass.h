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
class baseActiveClass
{
 protected:
  baseActiveClass() = default;
  ~baseActiveClass() = default;
  // copy forbidden
  baseActiveClass(const baseActiveClass& rhs) = delete;
  // copy assignment forbidden
  baseActiveClass& operator=(const baseActiveClass& rhs) = delete;

  static const std::string version_;
  mutable std::thread::id threadId_ {};
  mutable bool prologueResult_ {};
  mutable bool epilogueResult_ {};

  bool getPrologueResult() const noexcept;
  void setPrologueResult(const bool result) const noexcept;
  bool getEpilogueResult() const noexcept;
  void setEpilogueResult(const bool result) const noexcept;

 public:
  static const std::string& activeClassVersion () noexcept;
  std::thread::id getThreadId () const noexcept;
};  // class baseActiveClass

template <typename T, typename U>
using threadResult = std::tuple<const bool, T, const bool, U>;

template <typename T, typename U>
using fut = std::future<threadResult<T,U>>;

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

  U& getThreadData () const noexcept
  {
    return threadData_;
  }

  auto run(const U& threadData) const noexcept -> threadResult<T,U>
  {
    threadId_ = std::this_thread::get_id();
    setThreadData(threadData);
    activeClassBody();

    return std::make_tuple(getPrologueResult(),
                           getBodyResult(),
                           getEpilogueResult(),
                           getThreadData());
  }

  auto runThread(const U& threadData) const noexcept
  {
    // the thread gets a lambda; it cannot be a class method because it should be
    // declared static, and this would break the class design, so that's why we
    // use a lambda here capturing by reference the thread data and this
    setThreadFuture(std::async(std::launch::async,
                               [&]() -> threadResult<T,U> {
                                 return run(threadData);
                               } ) );
   return this;
  }

  auto runThreadAndWaitTermination(const U& threadData) const -> threadResult<T,U>
  {
    return runThread(threadData)->waitThreadEndsAndGetResults();
  }

  auto waitThreadEndsAndGetResults() const -> threadResult<T,U>
  {
    try
    {
      auto [prologueResult, bodyResult, epilogueResult, threadData] = getThreadFuture().get();
      return std::make_tuple(prologueResult, bodyResult, epilogueResult, threadData);
    }
    catch (const std::exception& e)
    {
      std::cerr << __func__
                << ": EXCEPTION: "
                << e.what()
                << std::endl;
    }
    return std::make_tuple(bool{}, T{}, bool{}, U{});
  }

  std::future_status waitThreadTerminate(const std::chrono::nanoseconds& delay) const noexcept
  {
    std::future_status futureStatus{std::future_status::timeout};
    // loop here until the end of the thread
    while (futureStatus != std::future_status::ready)
    {
      // wait delay nanosecs for the thread termination
      // NB: if the thread does not end this loop goes on forever
      futureStatus = getThreadFuture().wait_for(delay);
    }
    return futureStatus;
  }

  std::future_status getThreadStatus() const noexcept
  {
    return getThreadFuture().wait_for(std::chrono::nanoseconds{0});
  }

 private:
  mutable fut<T,U> threadFuture_ {};
  prologueFun<U> pfun_ {};
  epilogueFun<U> efun_ {};
  bodyFun<T,U> bodyfun_ {};
  mutable T bodyResult_ {};
  mutable U threadData_ {};

  fut<T,U>& getThreadFuture () const noexcept
  {
    return threadFuture_;
  }

  void setThreadFuture(fut<T,U> threadFuture) const noexcept
  {
    threadFuture_ = std::move(threadFuture);
  }

  void setBodyResult(const T result) const noexcept
  {
    bodyResult_ = result;
  }

  T getBodyResult () const noexcept
  {
    return bodyResult_;
  }

  void setThreadData(const U& threadData) const noexcept
  {
    threadData_ = std::move(threadData);
  }

  void activeClassBody() const noexcept
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
auto createUniquePtr(Args&&... args) -> std::unique_ptr<T>
{
  return std::make_unique<T>(args...);
}

template <typename T, typename U>
activeClassPtr<T,U>
makeActiveClass (const prologueFun<U>& pfun,
                 const bodyFun<T,U>& bodyfun,
                 const epilogueFun<U>& efun) noexcept
{
  return createUniquePtr<activeClass<T,U>>(pfun, bodyfun, efun);
}
}  // namespace activeClass
#endif /* ACTIVECLASS_H */
