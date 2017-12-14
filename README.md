# active-class
A C++17 implementation of an active class: an active class generates a thread and controls it using functional objects passed by the client application

## Requirements

`cmake` is used to compile the sources.

The default compiler used is `clang++-5.0`.

The cmake files compile with `-std=c++17`.

The unit tests are implemented in `googletest`: be sure you have installed `googletest` to compile.


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


## The Active Class

The active class generates a thread whose function runs a sequence of three client provided functional objects:

- a prologue
- a body
- an epilogue

All the above functional objects are unary functions whose unique argument is a data structure provided by the calling application.

The prologue and the epilogue return a `bool`, the body returns a client defined type.

The thread runs as follows:

```C++
bool epilogueResult{};
bool prologueResult = prologue(threadData);
if ( false == prologueResult )
{
  body(threadData);
  epilogueResult = epilogue(threadData);
}
```
See the class [activeClass](https://github.com/massimo-marino/active-class/blob/master/src/activeClass.h#L60) and its method [void activeClassBody()](https://github.com/massimo-marino/active-class/blob/master/src/activeClass.h#L182)  for the details.

The unit tests named `runThreadOK_1`, `runThreadOK_2`, and `runThreadOK_3` provide examples of usage of the active class concept.
