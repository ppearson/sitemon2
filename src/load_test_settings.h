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

#ifndef LOAD_TEST_SETTINGS_H
#define LOAD_TEST_SETTINGS_H

#include <vector>

#include "utils/tinyxml.h"

enum LoadTestType
{
	LOAD_HIT_TEST,
	LOAD_PROFILE_TEST
};

struct LoadTestProfileSeg
{
	LoadTestProfileSeg(int threads, int duration) : m_threads(threads), m_duration(duration) { }
	
	int	m_threads;
	int	m_duration;
};

class LoadTestSettings
{
public:
	LoadTestSettings();
	
	bool loadLoadTestElement(TiXmlElement *pElement);
	void loadSegmentsElement(TiXmlElement *pElement);
	
	inline std::vector<LoadTestProfileSeg>::iterator begin() { return m_aProfileSegments.begin(); }
	inline std::vector<LoadTestProfileSeg>::iterator end() { return m_aProfileSegments.end(); }
	
	int getHitThreads() const { return m_threads; }
	int getHitRepeats() const { return m_repeats; }
	
	bool			m_set;
	
	LoadTestType	m_type;
	
protected:
	
	void addProfile(LoadTestProfileSeg &seg);
	
	// hit settings
	int		m_threads;
	int		m_repeats;
	
	// profile settings
	std::vector<LoadTestProfileSeg>	m_aProfileSegments;
};


#endif
