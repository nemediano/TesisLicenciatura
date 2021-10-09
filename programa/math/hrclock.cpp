#include "hrclock.h"

namespace math {

HRClock::HRClock()
    : m_DeltaTime(0)
{
	m_Start = std::chrono::high_resolution_clock::now();
	m_DeltaTime = std::chrono::high_resolution_clock::duration();
}

void HRClock::tick()
{
	m_DeltaTime = std::chrono::high_resolution_clock::duration();
	m_T0 = std::chrono::high_resolution_clock::now();
}

void HRClock::tock()
{
    auto t1 = std::chrono::high_resolution_clock::now();
    m_DeltaTime = t1 - m_T0;
}

void HRClock::reset()
{
	m_Start = std::chrono::high_resolution_clock::now();
    m_DeltaTime = std::chrono::high_resolution_clock::duration();
}

double HRClock::getDeltaNs() const
{
    return m_DeltaTime.count() * 1.0;
}
double HRClock::getDeltaMicroS() const
{
    return m_DeltaTime.count() * 1e-3;
}

double HRClock::getDeltaMiliS() const
{
    return m_DeltaTime.count() * 1e-6;
}

double HRClock::getDeltaS() const
{
    return m_DeltaTime.count() * 1e-9;
}

double HRClock::getTotalNs() const
{
	auto t1 = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::duration total_time = t1 - m_Start;
	return total_time.count() * 1.0;
}

double HRClock::getTotalMicroS() const
{
	auto t1 = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::duration total_time = t1 - m_Start;
	return total_time.count() * 1e-3;
}

double HRClock::getTotalMiliS() const
{
	auto t1 = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::duration total_time = t1 - m_Start;
	return total_time.count() * 1e-6;
}

double HRClock::getTotalS() const
{
	auto t1 = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::duration total_time = t1 - m_Start;
	return total_time.count() * 1e-9;
}

} // namespace math
