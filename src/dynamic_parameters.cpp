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

#include "dynamic_parameters.h"
#include "utils/time.h"

DynamicParameter::DynamicParameter(const std::string &name) : m_name(name)
{
	
}

DynamicDateParameter::DynamicDateParameter(const std::string &name, const std::string &dateFormat, int daysInFuture) : DynamicParameter(name), m_dateFormat(dateFormat),
											m_daysInFuture(daysInFuture)
{

}

DynamicDateParameter::DynamicDateParameter(const DynamicDateParameter &rhs)
{
	m_name = rhs.m_name;
	m_dateFormat = rhs.m_dateFormat;
	m_daysInFuture = rhs.m_daysInFuture;
}

std::string DynamicDateParameter::getValue()
{
	Time paramTime;
	paramTime.now();
	
	paramTime.incrementDays(m_daysInFuture);
	
	std::string formattedTime = paramTime.formatString(m_dateFormat);
	return formattedTime;
}

DynamicDateParameter* DynamicDateParameter::clone()
{
	DynamicDateParameter *pNew = new DynamicDateParameter(*this);
	
	return pNew;
}