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

#ifndef SCRIPT_DEBUGGER_H
#define SCRIPT_DEBUGGER_H

#include <string>

#include "script.h"
#include "debug_settings.h"

class ScriptDebugger
{
public:
	ScriptDebugger(Script &script);
	
	void setDebugSettings(DebugSettings &settings);
	DebugSettings &getDebugSettings() { return m_debugSettings; }
	
	void run();
	
protected:
	DebugSettings	m_debugSettings;
	Script &		m_script;	
};

#endif
