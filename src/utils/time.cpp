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

#include <inttypes.h>

#include "time.h"

Time::Time(time_t time) : m_time(time)
{
	
}

Time::Time(unsigned long timestamp) : m_time(timestamp)
{
	
}

const Time& Time::operator=(const Time& rhs)
{
	m_time = rhs.m_time;
	
	return *this;
}

const Time& Time::operator=(time_t time)
{
	m_time = time;
	
	return *this;
}

const Time& Time::operator=(unsigned long timestamp)
{
	m_time = timestamp;

	return *this;
}

void Time::now()
{
	time(&m_time);
}

time_t Time::getNativeTime() const
{
	return m_time;
}

unsigned long Time::get32bitLong() const
{
	uint32_t unixTimestamp;

	// Recent Microsoft compilers have a 64-bit time
	unixTimestamp = static_cast<uint32_t>(m_time);

	return unixTimestamp;
}

std::string Time::formatString(const std::string &format)
{
	struct tm * pTimeinfo;
	pTimeinfo = localtime(&m_time);
	
	char szTime[64];
	strftime(szTime, 64, format.c_str(), pTimeinfo);
	
	std::string finalString(szTime);
	return finalString;
}

void Time::incrementDays(int days)
{
	m_time += (60 * 60 * 24 * days);
}