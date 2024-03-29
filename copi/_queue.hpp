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
#if !defined(__QUEUE_HPP)
#define __QUEUE_HPP

#include "copi.hpp"
#include <queue>

namespace COPI {

/** @brief Generic queue (FIFO) type.
 *
 * @tparam T Any C++ type.
 * @note `CQueue` is multi-producer, multi-consumer, and thread-safe.
 *
 */

template <typename T> class CQueue {
public:

    /** @brief `CQueue` constructor.
     *
     * @param spincount
     *      @li If == 0 don't spin.
     @      @li If > 0 Try busy-waiting before acquire lock.
     *
     */
    CQueue(DWORD spincount = 0) : m_lock(spincount), m_cv(spincount) {}

    //~CQueue() {}

    /** @brief Add item to queue.
     *
     * @param data
     *
     */
    void put(const T& data) {
        m_lock.acquire();
        m_queue.push(data);
        m_lock.release();
        m_cv.notify_one();
    }

    /** @brief Get (remove) item from queue, considering a timeout value.
     *
     * @param[out] data Pointer to removed item.
     * @param millis timeout in milliseconds.
     *      @li == 0 Try to get an item (immediately return).
     *      @li == INFINITE wait forever for availability.
     *      @li else Wait at most `millis` milliseconds.
     * @return bool
     *      @li true - success
     *      @li false - timeout.
     *
     */
    bool get(T * data, DWORD millis = INFINITE) {
        bool res;
        m_lock.acquire();

        while (m_queue.empty()) {
            res = m_cv.wait(m_lock, millis);
            if (!res) {
                return false;
            }
        }

        *data = m_queue.front();
        m_queue.pop();
        m_lock.release();
        return true;
    }

    /** @brief Test queue for emptyness.
     *
     * @return bool
     *
     */
    bool empty() {
        bool result;

        m_lock.acquire();
        result = m_queue.empty();
        m_lock.release();
        return result;
    }

private:
    std::queue<T> m_queue;
    win::CriticalSection m_lock;
    CConditionVariable m_cv;
};

}

#endif // __QUEUE_HPP
