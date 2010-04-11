/*
 *  http_server_responses.cpp
 *  sitemon
 *
 *  Created by Peter Pearson on 02/04/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <fstream>

#include "http_server_responses.h"

HTTPServerResponse::HTTPServerResponse(int returnCode, const std::string &text) : m_returnCode(returnCode), m_text(text)
{
	
}

std::string HTTPServerResponse::responseString()
{
	std::string response;
	
	char szTemp[64];
	memset(szTemp, 0, 64);
	
	sprintf(szTemp, "HTTP/1.1 %i \n", m_returnCode);
	response += szTemp;
	
	response += "Content-Type: text/html; charset=UTF-8\n";
	
	memset(szTemp, 0, 64);
	sprintf(szTemp, "Content-Length: %ld\n\n", m_text.size());
	response += szTemp;
	
	response += m_text + "\n";
	
	return response;
}

HTTPServerRedirectResponse::HTTPServerRedirectResponse(const std::string &url) : m_url(url)
{

}

std::string HTTPServerRedirectResponse::responseString()
{
	std::string response;
	
	char szTemp[128];
	memset(szTemp, 0, 128);
	
	sprintf(szTemp, "HTTP/1.1 %i \n", 302);
	response += szTemp;

	memset(szTemp, 0, 128);
	sprintf(szTemp, "Location: %s\n\n", m_url.c_str());
	response += szTemp;
	
	return response;
}
	
HTTPServerFileResponse::HTTPServerFileResponse(const std::string &path, bool binary) : m_path(path), m_binary(binary)
{
	
}

std::string HTTPServerFileResponse::responseString()
{
	std::string response;

	std::fstream fileStream(m_path.c_str(), std::ios::in);
	
	std::string content;
	int returnCode = 200;
	
	if (fileStream.fail())
	{
		content = "File not found.\n";
		returnCode = 404;
	}
	else
	{
		std::string line;
		char buf[1024];
		
		while (fileStream.getline(buf, 1024))
		{
			line.assign(buf);
			content += line + "\n";		
		}
	}
	fileStream.close();
	
	char szTemp[64];
	memset(szTemp, 0, 64);
	
	sprintf(szTemp, "HTTP/1.1 %i \n", returnCode);
	response += szTemp;
	
	response += "Content-Type: text/html; charset=UTF-8\n";
	
	memset(szTemp, 0, 64);
	sprintf(szTemp, "Content-Length: %ld\n\n", content.size());
	response += szTemp;
	
	response += content + "\n";

	return response;
}

HTTPServerTemplateFileResponse::HTTPServerTemplateFileResponse(const std::string &path, std::string &content) : m_path(path), m_content(content)
{
	
}

std::string HTTPServerTemplateFileResponse::responseString()
{
	std::string response;
	
	std::fstream fileStream(m_path.c_str(), std::ios::in);
	
	std::string content;
	int returnCode = 200;
	
	if (fileStream.fail())
	{
		content = "Template file not found.\n";
		returnCode = 404;
	}
	else
	{
		bool doneReplace = false;
		std::string line;
		char buf[1024];
		
		while (fileStream.getline(buf, 1024))
		{
			line.assign(buf);
			
			// replace template with content if found
			
			if (!doneReplace) // only once per file - should make things slightly faster...
			{
				int nPlacement = line.find("<%%>");
				
				if (nPlacement != -1)
				{
					line.replace(nPlacement, 4, m_content);
					doneReplace = true;
				}
			}
			
			content += line + "\n";		
		}
	}
	fileStream.close();
	
	char szTemp[64];
	memset(szTemp, 0, 64);
	
	sprintf(szTemp, "HTTP/1.1 %i \n", returnCode);
	response += szTemp;
	
	response += "Content-Type: text/html; charset=UTF-8\n";
	
	memset(szTemp, 0, 64);
	sprintf(szTemp, "Content-Length: %ld\n\n", content.size());
	response += szTemp;
	
	response += content + "\n";
	
	return response;
}

HTTPServerTemplateFileResponse2::HTTPServerTemplateFileResponse2(const std::string &path, std::string &content1, std::string &content2) : m_path(path), m_content1(content1), m_content2(content2)
{
	
}

std::string HTTPServerTemplateFileResponse2::responseString()
{
	std::string response;
	
	std::fstream fileStream(m_path.c_str(), std::ios::in);
	
	std::string content;
	int returnCode = 200;
	
	if (fileStream.fail())
	{
		content = "Template file not found.\n";
		returnCode = 404;
	}
	else
	{
		std::string line;
		char buf[1024];
		
		while (fileStream.getline(buf, 1024))
		{
			line.assign(buf);
			
			// replace template with content if found
			
			int nPlacement1 = line.find("<%1%>");
			if (nPlacement1 != -1)
			{
				line.replace(nPlacement1, 5, m_content1);
			}
			
			int nPlacement2 = line.find("<%2%>");
			if (nPlacement2 != -1)
			{
				line.replace(nPlacement2, 5, m_content2);
			}
			
			content += line + "\n";		
		}
	}
	fileStream.close();
	
	char szTemp[64];
	memset(szTemp, 0, 64);
	
	sprintf(szTemp, "HTTP/1.1 %i \n", returnCode);
	response += szTemp;
	
	response += "Content-Type: text/html; charset=UTF-8\n";
	
	memset(szTemp, 0, 64);
	sprintf(szTemp, "Content-Length: %ld\n\n", content.size());
	response += szTemp;
	
	response += content + "\n";
	
	return response;
}
