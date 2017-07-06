# active-class
A C++11 implementation of an active class: an active class generates a thread and controls it using functional objects passed by the client application

## Requirements

The cmake file compiles with `-std=c++14` (but it would be fine also with `-std=c++11`).

## Install and Run Unit Tests

```bash
$ git clone https://github.com:massimo-marino/active-class.git
$ cd active-class
$ mkdir build
$ cd build
$ cmake ..
$ make
$ cd src/unitTests
$ ./unitTests
```
The unit tests provide examples of usage of active classes.

The unit tests are implemented in googletest: be sure you have installed googletest to compile.

## The Active Class

The active class generates a thread whose function runs a sequence of three client provided functional objects:

- a prologue
- a body
- an epilogue

All these functional objects are unary functions whose unique argument is a data structure provided by the calling application.

The prologue and the epilogue return a `bool`, the body returns a client defined type.

The thread runs as follows:

```C++
bool epilogueResul{};
bool prologueResult = prologue(threadData);
if ( false == prologueResult )
{
  body(threadData);
  epilogueResult = epilogue(threadData);
}
```
See the class [activeClass](https://github.com/massimo-marino/active-class/blob/master/src/activeClass.h#L50) and its method [void activeClassBody()](https://github.com/massimo-marino/active-class/blob/master/src/activeClass.h#L176)  for the details.

The unit tests named `runThreadOK_1` and `runThreadOK_2` provide examples of usage of the active class concept.
