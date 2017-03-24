/*
 * Copyright (c) 2016 Romeo Calota
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * Author: Romeo Calota
 */

#include "libgearbox_return_type.h"

/*!
    \class gearbox::ReturnType
    \brief Agregator for a data type and an error state

    Objects of this type are returned by method calls that make HTTP requests
    to a server. It contains a data type, which represents the resulting data
    set from the call and a gearbox::Error that representes any possible errors
    that resulted by making the call.

    When gearbox::Error::Code is not gearbox::Error::Code::Ok the resulting data set
    should be considered invalid or, at the very least, incomplete.
*/

/*!
    \fn gearbox::ReturnType::ReturnType(gearbox::Error &&, T &&)

    Constructs a gearbox::ReturnType from a pair of gearbox::Error and T
*/

/*!
    \fn gearbox::ReturnType::ReturnType(gearbox::ReturnType &&)

    Move constructor
*/

/*!
    \fn gearbox::ReturnType &ReturnType::operator =(gearbox::ReturnType &&) noexcept(true)

    Move asignment operator
*/

/*!
    \fn gearbox::ReturnType::operator T()

    Implicit conversion operator to T
*/

/*!
    \var gearbox::ReturnType::error

    The error state returned by a method call
*/

/*!
    \var gearbox::ReturnType::value

    The data set returned by a method call
*/
