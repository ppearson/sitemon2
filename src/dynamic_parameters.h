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

#ifndef DYNAMIC_PARAMETERS_H
#define DYNAMIC_PARAMETERS_H

#include <string>

class HTTPEngine;

class DynamicParameter
{
public:
	DynamicParameter() { }
	DynamicParameter(const std::string &name);
	virtual ~DynamicParameter() { }

	virtual DynamicParameter* clone() = 0;
	
	const std::string& getName() const { return m_name; }
	virtual std::string	getValue(HTTPEngine &engine) = 0;
	
protected:
	std::string		m_name;
};

class DynamicDateParameter : public DynamicParameter
{
public:
	DynamicDateParameter(const DynamicDateParameter &rhs);
	DynamicDateParameter(const std::string &name, const std::string &dateFormat, int daysInFuture);
	virtual ~DynamicDateParameter() { }

	virtual DynamicDateParameter* clone();
	
	virtual std::string getValue(HTTPEngine &engine);
	
protected:
	std::string		m_dateFormat;
	int				m_daysInFuture;
};

class DynamicExtractionParameter : public DynamicParameter
{
public:
	DynamicExtractionParameter(const DynamicExtractionParameter &rhs);
	DynamicExtractionParameter(const std::string &name, const std::string &key);
	virtual ~DynamicExtractionParameter() { }
	
	virtual DynamicExtractionParameter* clone();
	
	virtual std::string getValue(HTTPEngine &engine);
	
protected:
	std::string		m_key;
};

#endif
