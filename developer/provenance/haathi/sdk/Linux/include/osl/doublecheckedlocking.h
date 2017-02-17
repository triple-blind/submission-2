/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#if !defined INCLUDED_OSL_DOUBLECHECKEDLOCKING_H
#define INCLUDED_OSL_DOUBLECHECKEDLOCKING_H

#if defined __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A platform specific macro needed to make double-checked locking work.

    See
    <http://www.cs.umd.edu/~pugh/java/memoryModel/DoubleCheckedLocking.html>
    for a description of double-checked locking, why it is broken, and how it
    can be fixed.  On platforms where it is necessary, this macro will expand
    to some memory barrier instruction.  On many platforms, double-checked
    locking works as it is, though, so on those platforms this macro will be
    empty.  This is a macro instead of a (C++ inline) function to allow for
    maximum performance in both C and C++.

    If possible, use the rtl_Instance template instead of explicitly spelling
    out the double-checked locking pattern.  There are few cases where you
    will have to spell it out explicitly (e.g., the logic of a certain
    instance of the pattern is too complex to be mapped to the template, or
    some compiler refuses to compile a template instantiation due to internal
    compiler errors), though, and you should always call this macro at the
    right places then:

      static T * pInstance = 0;

      T * p = pInstance;
      if (!p)
      {
          Guard aGuard(aMutex);
          p = pInstance;
          if (!p)
          {
              p = ...;
              OSL_DOUBLE_CHECKED_LOCKING_MEMORY_BARRIER();
              pInstance = p;
          }
      }
      else
          OSL_DOUBLE_CHECKED_LOCKING_MEMORY_BARRIER();
      return p;

    One extra advantage of this macro is that it makes it easier to find all
    places where double-checked locking is used.
 */
#define OSL_DOUBLE_CHECKED_LOCKING_MEMORY_BARRIER() /* empty */

#if defined __cplusplus
}
#endif /* __cplusplus */

#endif /* INCLUDED_OSL_DOUBLECHECKEDLOCKING_H */
