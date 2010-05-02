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

#include "http_form_generator.h"

HTTPFormItem::HTTPFormItem(const std::string &label, const std::string &name) : m_label(label), m_name(name)
{

}

HTTPFormTextItem::HTTPFormTextItem(const std::string &label, const std::string &name, int size) :
						HTTPFormItem(label, name), m_size(size)
{
	
}

HTTPFormTextItem::HTTPFormTextItem(const std::string &label, const std::string &name, int size, std::string &value) :
						HTTPFormItem(label, name), m_size(size), m_value(value)
{

}

HTTPFormTextItem::HTTPFormTextItem(const std::string &label, const std::string &name, int size, const std::string &value) :
						HTTPFormItem(label, name), m_size(size), m_value(value)
{
	
}

HTTPFormTextItem::HTTPFormTextItem(const std::string &label, const std::string &name, int size, int value) :
						HTTPFormItem(label, name), m_size(size)
{
	char szTemp[12];
	memset(szTemp, 0, 12);
	sprintf(szTemp, "%i", value);
	m_value.assign(szTemp);
}

std::string HTTPFormTextItem::getCode()
{
	std::string content;
	
	content += "\t<dt>";
	content += m_label;
	content += "</dt>\n";
		
	content += "\t<dd><input type=\"text\" name=\"";
	content += m_name + "\" value=\"";
	content += m_value;
	content += "\" size=\"";
	
	char szSize[8];
	memset(szSize, 0, 8);
	sprintf(szSize, "%i", m_size);
	content.append(szSize);
	content += "\"/></dd>\n";
	
	return content;
}

HTTPFormCheckItem::HTTPFormCheckItem(const std::string &label, const std::string &name, bool checked) : HTTPFormItem(label, name),
						m_checked(checked)
{

}

std::string HTTPFormCheckItem::getCode()
{
	std::string content;
	
	content += "\t<dt>";
	content += m_label;
	content += "</dt>\n";
	
	content += "\t<dd><input type=\"checkbox\" name=\"";
	content += m_name;
	content += "\"";
	
	if (m_checked)
	{
		content += " checked";
	}
	
	content += "/></dd>\n";	
	
	return content;
}

HTTPFormCheckInlineItem::HTTPFormCheckInlineItem(const std::string &label, const std::string &name, bool checked) : HTTPFormItem(label, name),
					m_checked(checked)
{
	
}

std::string HTTPFormCheckInlineItem::getCode()
{
	std::string content;
	
	content += "\t<dt>&nbsp;</dt>\n";
	
	content += "\t<dd><input type=\"checkbox\" name=\"";
	content += m_name;
	content += "\"";
	
	if (m_checked)
	{
		content += " checked";
	}
	
	content += "/>  ";
	content += m_label + "</dd>\n";	
	
	return content;
}

HTTPFormSelectItem::HTTPFormSelectItem(const std::string &label, const std::string &name, int selected) : HTTPFormItem(label, name), 
						m_selected(selected)
{

}

std::string HTTPFormSelectItem::getCode()
{
	std::string content;
	
	content += "\t<dt>";
	content += m_label;
	content += "</dt>\n";
	
	content += "\t<dd><select name=\"";
	content += m_name;
	content += "\">\n";
	
	std::vector<std::string>::iterator it = m_aOptions.begin();
	for (int count = 0; it != m_aOptions.end(); ++it, count++)
	{
		if (count != m_selected)
		{
			content += "\t<option>";
		}
		else
		{
			content += "\t<option selected=\"yes\">";
		}
		
		content += *it;
		content += "</option>\n";		
	}	
	
	content += "\t</select></dd>\n";	
	
	return content;
}

HTTPFormHiddenItem::HTTPFormHiddenItem(const std::string &name, std::string &value) : HTTPFormItem("", name), m_value(value)
{

}

HTTPFormHiddenItem::HTTPFormHiddenItem(const std::string &name, int value) : HTTPFormItem("", name)
{
	char szTemp[12];
	memset(szTemp, 0, 12);
	sprintf(szTemp, "%i", value);
	m_value.assign(szTemp);
}

std::string HTTPFormHiddenItem::getCode()
{
	std::string content;
	
		
	content += "\t<input type=\"hidden\" name=\"";
	content += m_name;
	content += "\" value=\"";
	
	content += m_value;
	content += "\">\n";
	
	return content;	
}

HTTPFormGenerator::HTTPFormGenerator(const std::string &targetURL, const std::string &submitTitle, bool post) : m_targetURL(targetURL),
							m_submitTitle(submitTitle), m_post(post)
{

}

std::string HTTPFormGenerator::getGeneratedCode()
{
	std::string content;
	
	content += "<form action=\"";
	content += m_targetURL;
	content += "\" method=\"";
	
	if (m_post)
		content += "post";
	else
		content += "get";
	
	content += "\">\n";
	content += "<div class=\"def_wide\">\n<dl>\n";
	
	std::vector<std::string>::const_iterator it = m_aItems.begin();
	for (; it != m_aItems.end(); ++it)
	{
		content += *it;		
	}	
	
	content += "</dl>\n</div>\n";
	
	content += "<input type='submit' value='";
	content += m_submitTitle;
	content += "'/>\n</form>\n";	
	
	return content;
}

void HTTPFormGenerator::addSeparator()
{
	m_aItems.push_back("<div class=\"sep\"></div>\n");
}