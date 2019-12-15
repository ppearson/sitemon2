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

#include <stdio.h>
#include <string.h>

#include <fstream>

#include "utils/string_helper.h"

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

HTTPServerAuthenticationResponse::HTTPServerAuthenticationResponse()
{

}

std::string HTTPServerAuthenticationResponse::responseString()
{
	std::string response;

	char szTemp[128];
	memset(szTemp, 0, 128);

	sprintf(szTemp, "HTTP/1.1 %i Access Denied\n", 401);
	response += szTemp;
	
	//
	std::string authName = StringHelpers::generateRandomASCIIString(8) + "_";
	memset(szTemp, 0, 128);
	sprintf(szTemp, "WWW-Authenticate: Basic realm=\"%s\"\n", authName.c_str());
	response += szTemp;

	response += "Content-Length: 0\n\n";

	return response;
}

HTTPServerFileResponse::HTTPServerFileResponse(const std::string &path, bool binary) : m_path(path), m_binary(binary)
{

}

std::string HTTPServerFileResponse::responseString()
{
	std::string response;

	bool image = false;
	FileContentType contentType = eContentTextHTML;
	// work out if it's an image
	int extensionPos = m_path.rfind(".");
	if (extensionPos != -1)
	{
		std::string extension = m_path.substr(extensionPos + 1);
		
		if (extension == "png")
		{
			image = true;
			contentType = eContentImagePNG;
		}
		else if (extension == "css")
		{
			contentType = eContentTextCSS;
		}
		else if (extension == "js")
		{
			contentType = eContentTextJS;
		}
	}

	std::ios::openmode mode = std::ios::in;

	if (image)
	{
		mode = std::ios::in | std::ios::binary;
	}

	std::fstream fileStream(m_path.c_str(), mode);

	std::string content;
	int returnCode = 200;

	if (fileStream.fail())
	{
		content = "File not found: " + m_path + "\n";
		returnCode = 404;
	}
	else
	{
		if (!image)
		{
			std::string line;
			char buf[1024];

			while (fileStream.getline(buf, 1024))
			{
				line.assign(buf);
				content += line + "\n";
			}
		}
		else
		{
			std::stringstream ssOut;
			ssOut << fileStream.rdbuf();

			content = ssOut.str();
		}
	}
	fileStream.close();

	char szTemp[64];
	memset(szTemp, 0, 64);

	sprintf(szTemp, "HTTP/1.1 %i \n", returnCode);
	response += szTemp;
	
	std::string contentTypeString;
	
	switch (contentType)
	{
		case eContentImagePNG:
			contentTypeString = "image/png";
			break;
		case eContentTextCSS:
			contentTypeString = "text/css; charset=UTF-8";
			break;
		case eContentTextJS:
			contentTypeString = "application/javascript";
			break;
		case eContentTextHTML:
			contentTypeString = "text/html; charset=UTF-8";
			break;
		default:
			break;
	}

	if (!contentTypeString.empty())
	{
		response += "Content-Type: " + contentTypeString + "\n";
	}

	memset(szTemp, 0, 64);
	sprintf(szTemp, "Content-Length: %ld\n\n", content.size());
	response += szTemp;

	response += content + "\n";

	return response;
}

HTTPServerTemplateFileResponse::HTTPServerTemplateFileResponse(const std::string &path, std::string &content) : m_path(path)
{
	m_templateArgs = 1;

	m_aContent.push_back(&content);
}

HTTPServerTemplateFileResponse::HTTPServerTemplateFileResponse(const std::string &path, std::string &content1, std::string &content2) : m_path(path)
{
	m_templateArgs = 2;

	m_aContent.push_back(&content1);
	m_aContent.push_back(&content2);
}

HTTPServerTemplateFileResponse::HTTPServerTemplateFileResponse(const std::string &path, std::string &content1, std::string &content2, std::string &content3) : m_path(path)
{
	m_templateArgs = 3;

	m_aContent.push_back(&content1);
	m_aContent.push_back(&content2);
	m_aContent.push_back(&content3);
}

std::string templatePlaceholders[] = {"<%1%>", "<%2%>", "<%3%>"};

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
		if (m_templateArgs == 1)
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
						line.replace(nPlacement, 4, *m_aContent[0]);
						doneReplace = true;
					}
				}

				content += line + "\n";
			}
		}
		else
		{
			std::string line;
			char buf[1024];

			int thisArg = 0;

			while (fileStream.getline(buf, 1024))
			{
				line.assign(buf);

				// replace template with content if found

				if (thisArg < m_templateArgs)
				{
					int nPlacement = line.find(templatePlaceholders[thisArg]);
					if (nPlacement != -1)
					{
						line.replace(nPlacement, 5, *m_aContent[thisArg++]);
					}
				}

				content += line + "\n";
			}
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

