/*
 *  http_response.cpp
 *  sitemon
 *
 *  Created by Peter Pearson on 27/02/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "http_response.h"

HTTPComponentResponse::HTTPComponentResponse() : contentSize(0), downloadSize(0), errorCode(HTTP_OK), responseCode(0),
									lookupTime(0.0), connectTime(0.0), dataStartTime(0.0), totalTime(0.0)
{
	
}

HTTPResponse::HTTPResponse(bool storeHeader, bool storeBody) : contentSize(0), downloadSize(0), totalContentSize(0), totalDownloadSize(0), m_storeHeader(storeHeader), m_storeBody(storeBody),
															errorCode(HTTP_OK), responseCode(0), redirectCount(0), redirectTime(0.0), componentProblem(false),
															lookupTime(0.0), connectTime(0.0), dataStartTime(0.0), totalTime(0.0), m_thread(0)
{

}

void HTTPResponse::addComponent(HTTPComponentResponse &component)
{
	if (component.errorCode == HTTP_OK)
	{
		if (component.responseCode == 200)
		{
			totalContentSize += component.contentSize;
			totalDownloadSize += component.downloadSize;
		}
		else
		{
			componentProblem = true;
		}
	}
	else
	{
		componentProblem = true;
	}
	
	m_aComponents.push_back(component);
}