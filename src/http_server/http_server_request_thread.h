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

#ifndef HTTP_SERVER_REQUEST_THREAD_H
#define HTTP_SERVER_REQUEST_THREAD_H

#include "../utils/Socket.h"
#include "../utils/thread.h"

#include "http_server_request_despatcher.h"

class HTTPServerRequestThread: public Thread
{
public:
	HTTPServerRequestThread(Socket *socket, HTTPServerRequestDespatcher &despatcher);
	virtual ~HTTPServerRequestThread();
	
	virtual void run();
	
protected:
	Socket *	m_pSocket;
	HTTPServerRequestDespatcher &	m_despatcher;
};
	
#endif
