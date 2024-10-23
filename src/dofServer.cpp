#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include <cargs.h>

#include "DOF/DOF.h"
#include "Logger.h"
#include "ini.h"
#include "sockpp/tcp_acceptor.h"

using namespace std;

DOF::DOF* pDof;
bool opt_verbose = false;

static struct cag_option options[] = {
    {.identifier = 'c',
     .access_letters = "c",
     .access_name = "config",
     .value_name = "VALUE",
     .description = "Config file (optional, default is no config file)"},
    {.identifier = 'a',
     .access_letters = "a",
     .access_name = "addr",
     .value_name = "VALUE",
     .description = "IP address or host name (optional, default is 'localhost')"},
    {.identifier = 'p',
     .access_letters = "p",
     .access_name = "port",
     .value_name = "VALUE",
     .description = "Port (optional, default is '6789')"},
    {.identifier = 'l',
     .access_letters = "l",
     .access_name = "logging",
     .value_name = NULL,
     .description = "Enable logging to stderr (optional, default is no logging)"},
    {.identifier = 'v',
     .access_letters = "v",
     .access_name = "verbose-logging",
     .value_name = NULL,
     .description = "Enables verbose logging, includes normal logging (optional, default is no logging)"},
    {.identifier = 'h', .access_letters = "h", .access_name = "help", .description = "Show help"}};

void LIBDOFCALLBACK LogCallback(DOF_LogLevel logLevel, const char* format, va_list args)
{
  char buffer[1024];
  vsnprintf(buffer, sizeof(buffer), format, args);
  fprintf(stderr, "%s\n", buffer);
}

void run(sockpp::tcp_socket sock, uint32_t threadId) {}

int main(int argc, char* argv[])
{
  DOF::Config* pConfig = DOF::Config::GetInstance();

  cag_option_context cag_context;
  bool opt_wait = false;

  cag_option_prepare(&cag_context, options, CAG_ARRAY_SIZE(options), argc, argv);
  while (cag_option_fetch(&cag_context))
  {
    char identifier = cag_option_get(&cag_context);
    if (identifier == 'c')
    {
      inih::INIReader r{cag_option_get_value(&cag_context)};
      pConfig->SetDOFServerAddr(r.Get<string>("DOFServer", "Addr", "localhost").c_str());
      pConfig->SetDOFServerPort(r.Get<int>("DOFServer", "Port", 6789));
      if (opt_verbose) DOF::Log(DOF_LogLevel_INFO, "Loaded config file");
    }
    else if (identifier == 'a')
    {
      pConfig->SetDOFServerAddr(cag_option_get_value(&cag_context));
    }
    else if (identifier == 'p')
    {
      std::stringstream ssPort(cag_option_get_value(&cag_context));
      in_port_t port;
      ssPort >> port;
      pConfig->SetDOFServerPort(port);
    }
    else if (identifier == 'v')
    {
      opt_verbose = true;
      pConfig->SetLogCallback(LogCallback);
    }
    else if (identifier == 'l')
    {
      pConfig->SetLogCallback(LogCallback);
    }
    else if (identifier == 'h')
    {
      cout << "Usage: dofserver [OPTION]..." << endl;
      cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
      return 0;
    }
  }

  sockpp::initialize();
  if (opt_verbose)
    DOF::Log(DOF_LogLevel_INFO, "Opening DOFServer, listining for TCP connections on %s:%d",
             pConfig->GetDOFServerAddr(), pConfig->GetDOFServerPort());
  sockpp::tcp_acceptor acc({pConfig->GetDOFServerAddr(), (in_port_t)pConfig->GetDOFServerPort()});
  if (!acc)
  {
    DOF::Log(DOF_LogLevel_ERROR, "Error creating the DOFServer acceptor: %s", acc.last_error_str().c_str());
    return 1;
  }

  pDof = new DOF::DOF();
}
