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

#include "http_server_request_thread.h"

#include "http_server_request.h"
#include "http_server_responses.h"

HTTPServerRequestThread::HTTPServerRequestThread(Socket *socket, HTTPServerRequestDespatcher &despatcher) : m_pSocket(socket),
							m_despatcher(despatcher)
{

}

HTTPServerRequestThread::~HTTPServerRequestThread()
{
}

void HTTPServerRequestThread::run()
{
	if (!m_pSocket)
		return;
	
	std::string clientAddress = m_pSocket->getClientAddress();
	
	std::string recvData;
	m_pSocket->recv(recvData);
	
	HTTPServerRequest request(recvData);
	std::string response;
	
	if (request.parse())
	{
		m_despatcher.handleRequest(request, response);
	}
	else
	{
		HTTPServerResponse resp(500, "<html>\n<body>\nCouldn't understand request\n</body>\n</html>");
		response = resp.responseString();
	}
	
	m_pSocket->send(response);
	
	m_pSocket->close();
	
	delete m_pSocket;
}
