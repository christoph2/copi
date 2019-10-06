/*
BSD License

Copyright (c) 2019, Christoph Schueler
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from this
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __SHARED_PTR_HPP
#define __SHARED_PTR_HPP

#include <iostream>

#include "copi.hpp"

namespace COPI {


using namespace std;

/** @brief Reference counter implementation for `CSharedPointer`.
 *
 *
 */

class CRefCounter {
public:

    /** @brief Construct with initial counter value.
     *
     * @param counter Initial counter value.
     *
     */
    CRefCounter(LONG counter = 0) : m_counter(counter) {
        cout << "\tInitialized RefCounter with: " << counter << endl;
    }

    /** @brief Copy constructor.
     *
     * @param other
     *
     */
    CRefCounter(const CRefCounter & other) : m_counter(other.getCounter()) {
        cout << "\tCRefCounter -- copy cons: " << m_counter << endl;
    }

    /** @brief Construct from a pointer to another instance.
     *
     * @param other
     *
     */
    CRefCounter(CRefCounter * other) : m_counter(other->getCounter()) {
        cout << "\tCRefCounter -- copy cons#2: " << m_counter << endl;
    }

    /** @brief Assignment operator.
     *
     * @param other
     *
     */
    CRefCounter & operator = (const CRefCounter & other)
    {
        cout << "\tassignment" << endl;
        m_counter = other.getCounter();
    }

    /** @brief Increment counter by one.
     *
     *  @note `addRef()` is thread-safe.
     *
     */
    void addRef() {
        InterlockedIncrement(&m_counter);
        cout << "\taddRef() - Counter is now: " << m_counter << endl;
    }

    /** @brief Decrement counter by one.
     *
     * @return Counter value after decrementation.
     * @note `release()` is thread-safe.
     *
     */
    LONG release() {
        if (m_counter > 0) {
            InterlockedDecrement(&m_counter);
        }
        cout << "\trelease() - Counter is now: " << m_counter << endl;
        return m_counter;
    }

    /** @brief Get current counter value.
     *
     * @return LONG
     *
     */
    LONG getCounter() const {
        return m_counter;
    }

private:
    volatile LONG m_counter;
};

/** Generic shared pointer class.
 *
 *  Implements a generic reference counted smart pointer.
 *
 *  @tparam T Any C++ type.
 *
 */

template <typename T> class CSharedPointer {
public:

    /** @brief Default constructor.
     *
     *
     */
    CSharedPointer() : m_pointer(0), m_counter(0) {
        cout << "CSharedPointer() -- default" << endl;
        m_counter = new CRefCounter();
        m_counter->addRef();
    }

    /** @brief Construct  `CSharedPointer` from a pointer variable.
     *
     * @param pointer
     *
     */
    CSharedPointer(T * pointer) : m_pointer(pointer), m_counter(0) {
        cout << "CSharedPointer() -- from pointer: " << m_pointer << endl;
        m_counter = new CRefCounter();
        m_counter->addRef();
    }

    /** @brief Copy constructor
     *
     *  @param other
     */
    CSharedPointer(CSharedPointer<T> const & other) : m_pointer(other.m_pointer), m_counter(other.m_counter) {
        cout << "CSharedPointer() -- copy construction" << endl;
        m_counter->addRef();
    }

    /** @brief Assignment operator.
     *
     * @param other
     *
     */
    CSharedPointer<T>& operator = (const CSharedPointer<T>& other) {
        cout << "CSharedPointer() -- assignment" << endl;

        if (this != &other) // No self assignment please.
        {
            if(m_counter->release() == 0)
            {
                delete m_pointer;
                delete m_counter;
            }

            m_pointer = other.m_pointer;
            m_counter = other.m_counter;
            m_counter->addRef();
        }
        return *this;
    }

    /** @brief Destructor.
     *
     *  Decrement reference counter and free resources if zero.
     */
    ~CSharedPointer() {
        LONG nc;
        nc = m_counter->release();
        cout << "Releasing pointer " << m_pointer << " [" << m_counter->getCounter() << "]" << endl;
        if (nc == 0) {
            cout << "\tOK, freeing resources" << endl;
            //cout << "\t\tafter delete pointer" << endl;
            delete m_counter;
            delete m_pointer;
        }
    }

    /** @brief Dereference operator
     *
     * @return Reference to T.
     *
     */
    T& operator* ()
    {
        return *m_pointer;
    }

    /** @brief Indirection operator.
     *
     * @return Raw pointer value.
     *
     */
    T* operator-> ()
    {
        return m_pointer;
    }


private:
    T * m_pointer;
    CRefCounter * m_counter;
};


}

#endif // __SHARED_PTR_HPP

