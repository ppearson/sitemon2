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

#ifndef HTTP_FORM_GENERATOR_H
#define HTTP_FORM_GENERATOR_H

#include <string>
#include <vector>

class HTTPFormItem
{
public:
	HTTPFormItem(const std::string &label, const std::string &name);
	virtual ~HTTPFormItem() { }

	virtual std::string getCode() = 0;

protected:
	std::string		m_label;
	std::string		m_name;
};

class HTTPFormTextItem : public HTTPFormItem
{
public:
	HTTPFormTextItem(const std::string &label, const std::string &name, int size);
	HTTPFormTextItem(const std::string &label, const std::string &name, int size, std::string &value);
	HTTPFormTextItem(const std::string &label, const std::string &name, int size, const std::string &value);
	HTTPFormTextItem(const std::string &label, const std::string &name, int size, int value);

	virtual ~HTTPFormTextItem() { }

	virtual std::string getCode();

protected:
	int			m_size;
	std::string	m_value;
};

class HTTPFormCheckItem : public HTTPFormItem
{
public:
	HTTPFormCheckItem(const std::string &label, const std::string &name, bool checked = false);
	virtual ~HTTPFormCheckItem() { }

	virtual std::string getCode();

protected:
	bool	m_checked;
};

class HTTPFormCheckInlineItem : public HTTPFormItem
{
public:
	HTTPFormCheckInlineItem(const std::string &label, const std::string &name, bool checked = false);
	virtual ~HTTPFormCheckInlineItem() { }
	
	virtual std::string getCode();
	
protected:
	bool	m_checked;
};

class HTTPFormSelectItem : public HTTPFormItem
{
public:
	HTTPFormSelectItem(const std::string &label, const std::string &name, int selected = 0);
	virtual ~HTTPFormSelectItem() { }

	virtual std::string getCode();

	void addOption(const std::string &value) { m_aOptions.push_back(value); }

protected:
	std::vector<std::string> m_aOptions;
	int m_selected;
};

class HTTPFormHiddenItem : public HTTPFormItem
{
public:
	HTTPFormHiddenItem(const std::string &name, std::string &value);
	HTTPFormHiddenItem(const std::string &name, int value);
	virtual ~HTTPFormHiddenItem() { }
	
	virtual std::string getCode();

protected:
	std::string m_value;	
};

class HTTPFormGenerator
{
public:
	HTTPFormGenerator(const std::string &targetURL, const std::string &submitTitle, bool post = true);
	~HTTPFormGenerator() { }

	std::string getGeneratedCode();
	
	void addItem(HTTPFormItem &item) { m_aItems.push_back(item.getCode()); }
	
	void addParameterPair(const std::string &label, const std::string &name1, const std::string &value1, const std::string &name2, const std::string &value2);
	
	void addSeparator();

protected:
	bool			m_post;
	std::string		m_targetURL;
	std::string		m_submitTitle;
	
	// not the most efficient way of doing it, but means we don't have to use Pointers to base class
	std::vector<std::string> m_aItems;
};

#endif

