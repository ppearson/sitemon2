#ifndef SITEMON_H
#define SITEMON_H

#include <openssl/ssl.h>
#include <openssl/crypto.h>

#include "http_request.h"
#include "http_response.h"
#include "script.h"

#ifdef _MSC_VER

static HANDLE *lock_cs;

void win32_locking_callback(int mode, int type, char *file, int line);
void thread_setup();
void thread_cleanup();
static unsigned long id_function(void);

#endif

bool performSingleRequest(HTTPRequest &request, bool outputHeader);
bool performScriptRequest(Script &script);
bool performConcurrentScriptRequest(Script &script, int threads, const std::string &outputPath);

void outputResponse(HTTPRequest &request, const HTTPResponse &response);

#endif