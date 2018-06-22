#include "WeekStat.h"
#include <Arduino.h>

// Update min / max values and activity count
void WeekStat::update(int t, int active)
{
	int d = millis() / (24UL*3600UL*1000UL);
	d %= 7; // day 0..6
	if (m_day < 0) {
		// First time initialization
		for (int i = 0; i < 7; ++i) {
			m_min[i] = t;
			m_max[i] = t;
			m_active[i] = 0;
		}
	} else if (m_day != d) {
		// Day changed
		m_min[d] = t;
		m_max[d] = t;    
		m_active[d] = active;
	} else {
		// Update current day
		if (m_min[d] > t) m_min[d] = t;
		if (m_max[d] < t) m_max[d] = t;
		m_active[d] += active;
	}
	m_day = d;
}

// Returns minimum value
int WeekStat::minimum()
{
	int t = m_min[0];
	for (int i = 1; i < 7; ++i) {
		if (m_min[i] < t) t = m_min[i];
	}
	return t;
}

// Returns maximum value
int WeekStat::maximum()
{
	int t = m_max[0];
	for (int i = 1; i < 7; ++i) {
		if (m_max[i] > t) t = m_max[i];
	}
	return t;
}

// Returns activity count 
int WeekStat::active_count()
{
	int active = 0;
	for (int i = 0; i < 7; ++i) {
		active += m_active[i];
	}
	return active;
}
