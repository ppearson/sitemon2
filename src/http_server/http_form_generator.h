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

protected:
	bool			m_post;
	std::string		m_targetURL;
	std::string		m_submitTitle;
	
	// not the most efficient way of doing it, but means we don't have to use Pointers to base class
	std::vector<std::string> m_aItems;
};

#endif

