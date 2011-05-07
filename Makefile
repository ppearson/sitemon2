CC = g++
CXX_LDFLAGS = -lpthread -lcurl
INCLDFLAGS = -I./http_server/ -I./load_testing/ -I./scheduler/ -I./utils/ 
OBS = component_downloader.o config.o debug_settings.o dynamic_parameters.o html_parser.o http_engine.o http_request.o http_response.o load_test_settings.o main.o script.o script_debugger.o script_result.o sitemon.o uri_helper.o http_form_generator.o http_server.o http_server_db_helpers.o http_server_html_formatters.o http_server_load_testing_helpers.o http_server_request.o http_server_request_despatcher.o http_server_request_thread.o http_server_responses.o hit_load_request_thread.o hit_test_engine.o load_test_db_helpers.o load_test_results_saver.o profile_load_request_thread.o profile_test_engine.o scheduled_results_saver.o scheduler.o scheduler_db_helpers.o scheduler_test_thread.o event.o misc.o mutex.o socket.o sqlite3.o sqlite_db.o sqlite_query.o string_helper.o thread.o thread_pool.o time.o tinyxml.o tinyxmlerror.o tinyxmlparser.o 

#MAIN_CC=g++ -c -g -Wall $(INCLDFLAGS)
#MAIN_C=g++ -x c -c -g -Wall $(INCLDFLAGS)
MAIN_CC=g++ -c -O3 -Wall $(INCLDFLAGS)
MAIN_C=g++ -x c -c -O3 -Wall $(INCLDFLAGS)

all: sitemon

sitemon: $(OBS)
	$(LINK.cc) $(CXX_LDFLAGS) -o sitemon $(OBS)

clean:
	rm -f $(OBS)

component_downloader.o: src/component_downloader.cpp
	$(MAIN_CC) src/component_downloader.cpp
config.o: src/config.cpp
	$(MAIN_CC) src/config.cpp
debug_settings.o: src/debug_settings.cpp
	$(MAIN_CC) src/debug_settings.cpp
dynamic_parameters.o: src/dynamic_parameters.cpp
	$(MAIN_CC) src/dynamic_parameters.cpp
html_parser.o: src/html_parser.cpp
	$(MAIN_CC) src/html_parser.cpp
http_engine.o: src/http_engine.cpp
	$(MAIN_CC) src/http_engine.cpp
http_request.o: src/http_request.cpp
	$(MAIN_CC) src/http_request.cpp
http_response.o: src/http_response.cpp
	$(MAIN_CC) src/http_response.cpp
load_test_settings.o: src/load_test_settings.cpp
	$(MAIN_CC) src/load_test_settings.cpp
main.o: src/main.cpp
	$(MAIN_CC) src/main.cpp
script.o: src/script.cpp
	$(MAIN_CC) src/script.cpp
script_debugger.o: src/script_debugger.cpp
	$(MAIN_CC) src/script_debugger.cpp
script_result.o: src/script_result.cpp
	$(MAIN_CC) src/script_result.cpp
sitemon.o: src/sitemon.cpp
	$(MAIN_CC) src/sitemon.cpp
uri_helper.o: src/uri_helper.cpp
	$(MAIN_CC) src/uri_helper.cpp
http_form_generator.o: src/http_server/http_form_generator.cpp
	$(MAIN_CC) src/http_server/http_form_generator.cpp
http_server.o: src/http_server/http_server.cpp
	$(MAIN_CC) src/http_server/http_server.cpp
http_server_db_helpers.o: src/http_server/http_server_db_helpers.cpp
	$(MAIN_CC) src/http_server/http_server_db_helpers.cpp
http_server_html_formatters.o: src/http_server/http_server_html_formatters.cpp
	$(MAIN_CC) src/http_server/http_server_html_formatters.cpp
http_server_load_testing_helpers.o: src/http_server/http_server_load_testing_helpers.cpp
	$(MAIN_CC) src/http_server/http_server_load_testing_helpers.cpp
http_server_request.o: src/http_server/http_server_request.cpp
	$(MAIN_CC) src/http_server/http_server_request.cpp
http_server_request_despatcher.o: src/http_server/http_server_request_despatcher.cpp
	$(MAIN_CC) src/http_server/http_server_request_despatcher.cpp
http_server_request_thread.o: src/http_server/http_server_request_thread.cpp
	$(MAIN_CC) src/http_server/http_server_request_thread.cpp
http_server_responses.o: src/http_server/http_server_responses.cpp
	$(MAIN_CC) src/http_server/http_server_responses.cpp
hit_load_request_thread.o: src/load_testing/hit_load_request_thread.cpp
	$(MAIN_CC) src/load_testing/hit_load_request_thread.cpp
hit_test_engine.o: src/load_testing/hit_test_engine.cpp
	$(MAIN_CC) src/load_testing/hit_test_engine.cpp
load_test_db_helpers.o: src/load_testing/load_test_db_helpers.cpp
	$(MAIN_CC) src/load_testing/load_test_db_helpers.cpp
load_test_results_saver.o: src/load_testing/load_test_results_saver.cpp
	$(MAIN_CC) src/load_testing/load_test_results_saver.cpp
profile_load_request_thread.o: src/load_testing/profile_load_request_thread.cpp
	$(MAIN_CC) src/load_testing/profile_load_request_thread.cpp
profile_test_engine.o: src/load_testing/profile_test_engine.cpp
	$(MAIN_CC) src/load_testing/profile_test_engine.cpp
scheduled_results_saver.o: src/scheduler/scheduled_results_saver.cpp
	$(MAIN_CC) src/scheduler/scheduled_results_saver.cpp
scheduler.o: src/scheduler/scheduler.cpp
	$(MAIN_CC) src/scheduler/scheduler.cpp
scheduler_db_helpers.o: src/scheduler/scheduler_db_helpers.cpp
	$(MAIN_CC) src/scheduler/scheduler_db_helpers.cpp
scheduler_test_thread.o: src/scheduler/scheduler_test_thread.cpp
	$(MAIN_CC) src/scheduler/scheduler_test_thread.cpp
event.o: src/utils/event.cpp
	$(MAIN_CC) src/utils/event.cpp
misc.o: src/utils/misc.cpp
	$(MAIN_CC) src/utils/misc.cpp
mutex.o: src/utils/mutex.cpp
	$(MAIN_CC) src/utils/mutex.cpp
socket.o: src/utils/socket.cpp
	$(MAIN_CC) src/utils/socket.cpp
sqlite3.o: src/utils/sqlite3.c
	$(MAIN_C) src/utils/sqlite3.c
sqlite_db.o: src/utils/sqlite_db.cpp
	$(MAIN_CC) src/utils/sqlite_db.cpp
sqlite_query.o: src/utils/sqlite_query.cpp
	$(MAIN_CC) src/utils/sqlite_query.cpp
string_helper.o: src/utils/string_helper.cpp
	$(MAIN_CC) src/utils/string_helper.cpp
thread.o: src/utils/thread.cpp
	$(MAIN_CC) src/utils/thread.cpp
thread_pool.o: src/utils/thread_pool.cpp
	$(MAIN_CC) src/utils/thread_pool.cpp
time.o: src/utils/time.cpp
	$(MAIN_CC) src/utils/time.cpp
tinyxml.o: src/utils/tinyxml.cpp
	$(MAIN_CC) src/utils/tinyxml.cpp
tinyxmlerror.o: src/utils/tinyxmlerror.cpp
	$(MAIN_CC) src/utils/tinyxmlerror.cpp
tinyxmlparser.o: src/utils/tinyxmlparser.cpp
	$(MAIN_CC) src/utils/tinyxmlparser.cpp
