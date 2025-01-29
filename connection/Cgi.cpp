#include "Cgi.hpp"

// Constructor
Cgi::Cgi()
{
	_cgi2parent[1] = -1;
	_cgi2parent[0] = -1;
	_parent2cgi[0] = -1;
	_parent2cgi[1] = -1;
	_cgiPid = 0;
	// std::cout << "\e[2mDefault constructor Cgi called\e[0m" << std::endl;
}

// Parameterized constructor
// Cgi::Cgi()
// {
// 	std::cout << "\e[2mParameterized constructor Cgi called\e[0m" << std::endl;
// }

// Copy constructor
Cgi::Cgi(const Cgi &other)
{
	// std::cout << "\e[2mCopy constructor Cgi called\e[0m" << std::endl;
	*this = other;
}

// Destructor
Cgi::~Cgi()
{
	std::cout << "\e[2mDestructor Cgi called\e[0m" << std::endl;
}

// Overloads
Cgi &Cgi::operator=(const Cgi &other)
{
	if (this != &other)
	{
		(void)other;
	}
	// std::cout << "\e[2mAssignation operator Cgi called\e[0m" << std::endl;
	return (*this);
}

// Member functions
void Cgi::setCgiEnv(const Request *req, const Server &server)
{
	_cgiEnv = std::vector<std::string>{};
	for (auto it = req->getHeader().begin(); it != req->getHeader().end(); ++it)
	{
		std::string key = it->first;
		std::replace(key.begin(), key.end(), '-', '_');
		if (key != "CONTENT_LENGTH" && key != "CONTENT_TYPE")
			key = "HTTP_" + key;
		_cgiEnv.push_back(key + "=" + it->second);
	}
	Location location = server.get_location(req->getUri());
	_cgiEnv.push_back("REQUEST_METHOD=" + req->getMethod());
	_cgiEnv.push_back("SERVER_PROTOCOL=HTTP/1.1");
	_cgiEnv.push_back("QUERY_STRING=" + req->getQuery());
	_cgiEnv.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_cgiEnv.push_back("SERVER_NAME=" + server.get_listen().get_host());
	_cgiEnv.push_back("SERVER_PORT=" + std::to_string(server.get_listen().get_port()));
	_cgiEnv.push_back("SERVER_SOFTWARE=Webserv");
	_cgiEnv.push_back("REDIRECT_STATUS=200");
	_cgiEnv.push_back("REQUEST_URI=" + req->getUri());
	_cgiEnv.push_back("PATH_INFO=" + req->getUri());
	auto uri = location.get_uri();
	auto str = req->getUri();
	auto route = std::filesystem::path(str.substr(uri.length()));
	_cgiEnv.push_back("SCRIPT_NAME=" + route.string()); // The path of the CGI script relative to the server's root
};

void Cgi::startCGIprocess(const Request *req, std::filesystem::path path, const Server &server)
{
	std::string cgiPath = server.get_location(req->getUri()).get_cgi_path();
	// pass the body to the CGI script via stdin
	setCgiEnv(req, server);

	_cgiPid = fork();

	if (_cgiPid == 0)
	{
		close(_parent2cgi[1]);
		close(_cgi2parent[0]);
		std::vector<char *> args;
		args.push_back(const_cast<char *>(cgiPath.c_str()));
		args.push_back(const_cast<char *>(path.c_str()));
		args.push_back(nullptr);
		dup2(_parent2cgi[0], STDIN_FILENO);
		dup2(_cgi2parent[1], STDOUT_FILENO);
		std::vector<char *> env;
		for (auto it = _cgiEnv.begin(); it != _cgiEnv.end(); ++it)
			env.push_back(const_cast<char *>(it->c_str()));
		env.push_back(nullptr);
		if (execve(cgiPath.c_str(), args.data(), env.data()) == -1)
			exit(EXIT_FAILURE);
	}
	else if (_cgiPid < 0)
	{
		throw HttpError("CGI failed to fork", 500);
	}
}

void Cgi::init()
{
	_cgiPid = 0;
	_cgiResult = "";
	_cgiEnv.clear();
	if (pipe(_cgi2parent) == -1)
		throw std::runtime_error("Pipe failed");
	if (pipe(_parent2cgi) == -1)
		throw std::runtime_error("Pipe failed");
}

void Cgi::killCgi()
{
	if (!_cgiPid)
		return;
	std::cout << "ouch I'm dead " << _cgiPid << std::endl;
	kill(_cgiPid, SIGKILL);
	waitpid(_cgiPid, NULL, 0);
	_cgiPid = 0;
}
// Getters

// Setters
