#ifndef HR_CLOCK_H_
#define HR_CLOCK_H_

/*
 *  Copyright(c) 2018 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

/**
 *  @file HighResolutionClock.h
 *  @date October 24, 2018
 *  @author Jeremiah van Oosten
 *
 *  @brief High resolution clock used for performing timings.
 */


#include <chrono>

namespace math {

class HRClock
{
public:
	HRClock();

    // Tick the high resolution clock.
    // Tick the clock before reading the delta time for the first time.
    // Only tick the clock once per frame.
    // Use the Get* functions to return the elapsed time between ticks.
    void tick();
    void tock();

    // Reset the clock.
    void reset();

    double getDeltaNs() const;
    double getDeltaMicroS() const;
    double getDeltaMiliS() const;
    double getDeltaS() const;

    double getTotalNs() const;
    double getTotalMicroS() const;
    double getTotalMiliS() const;
    double getTotalS() const;

private:
    // Begining of all time.
    std::chrono::high_resolution_clock::time_point m_Start;
    // Initial time point.
    std::chrono::high_resolution_clock::time_point m_T0;
    // Time between tick - tock.
    std::chrono::high_resolution_clock::duration m_DeltaTime;
};

} // namespace math
#endif
