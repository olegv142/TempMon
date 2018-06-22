#pragma once

//
// Collect the minimum / maximum values and activity count for the last 7 days
//
class WeekStat {
public:
	WeekStat() : m_day(-1) {}

	// Update min / max values and activity count
	void update(int t, int active);

	// Returns minimum value
	int minimum();

	// Returns maximum value
	int maximum();

	// Returns activity count 
	int active_count();

private:
	int m_min[7];
	int m_max[7];
	int m_active[7];
	int m_day;
};
