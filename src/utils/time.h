/*
 Sitemon
 Copyright 2010 Peter Pearson.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 You may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 */

#ifndef TIME_H
#define TIME_H

#include <ctime>

class Time
{
public:
	Time() { }
	Time(time_t time);
	Time(unsigned long timestamp);
	
	const Time& operator=(const Time& rhs);
	const Time& operator=(time_t time);
	const Time& operator=(unsigned long timestamp);
	
	~Time() { }
	
	bool operator==(Time time) const { return m_time == time.m_time; }
	bool operator!=(Time time) const { return m_time != time.m_time; }
	bool operator>(Time time) const { return m_time > time.m_time; }
	bool operator<(Time time) const { return m_time < time.m_time; }
	bool operator>=(Time time) const { return m_time >= time.m_time; }
	bool operator<=(Time time) const { return m_time <= time.m_time; }
	
	void now();

	void incrementMinutes(int minutes) { m_time += (60 * minutes); }
	
	time_t getNativeTime() const;
	unsigned long get32bitLong() const;
	
protected:
	time_t m_time;	
};

#endif
