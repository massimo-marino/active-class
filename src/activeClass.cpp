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
////////////////////////////////////////////////////////////////////////////////
namespace activeClass
{
std::string baseActiveClass::version_ {"1.0.0"};
////////////////////////////////////////////////////////////////////////////////
//
// activeClassVersion
//
// Purpose: Return the library version
//
std::string baseActiveClass::activeClassVersion () noexcept
{
  return version_;

}  // activeClassVersion
}  // namespace activeClass
