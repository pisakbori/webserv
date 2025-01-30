#include "Server.hpp"

// Member functions
void	Server::populate_server(std::istringstream & infile)
{
	std::string		line;
	bool			bracket_closed;

	bracket_closed = false;
	while (std::getline(infile, line))
	{
		trim(line);
		if (line.empty())
			continue ;
		if (line.find('}') != std::string::npos)
		{
			bracket_closed = true;
			break ;
		}
		if (line.find("location ") == 0)
		{
			Location	location;
			locationBlockValid(line);
			location.populate_location(infile, line);
			this->location.push_back(location);
		}
		else
			this->set_server(line);
	}
	if (!bracket_closed)
		throw std::runtime_error("unexpected end of file, expecting \"}\"");
}

// Setters
void	Server::set_server(std::string directive)
{
	std::string		directives[] =
	{
		"listen",
		"server_name",
		"error_page",
		"client_max_body_size"
	};
	void	(Server::*fnptr[])( std::string ) =
	{
		&Server::parse_listen,
		&Server::set_server_name,
		&Server::set_error_page,
		&Server::set_client_max_body_size
	};
	int			i;
	const int	N = sizeof(directives) / sizeof(directives[0]);
	std::string	directive_name = directive.substr(0, directive.find_first_of(" ;"));

	i = 0;
	while (i < N && directive_name.compare(directives[i]) != 0)
		i++;
	if (i != N)
	{
		std::string arg = extract_parameters(directives[i], directive);
		if (arg.empty())
			throw std::runtime_error(
				"invalid number of arguments for directive \"" + directive_name + "\"");
		(this->*fnptr[i])(arg);
	}
	else
		throw std::runtime_error("unknown directive \"" + directive_name + "\"");
}

void	Server::parse_listen(std::string arg)
{
	if (listen_set)
		throw std::runtime_error(
			"\"listen\" directive is duplicate");
	listen_set = true;
	listen.parse_listen(arg);
}

void	Server::set_server_name(std::string arg)
{
	std::istringstream	iss(arg);
	for (std::string token; std::getline(iss, token, ' ');)
		server_name.push_back(std::move(token));
}

static void	set_code(int *code, const std::string &value)
{
	if (!value.empty() && std::all_of(value.begin(), value.end(), ::isdigit))
		*code = stoi(value);
	else
		throw std::runtime_error("invalid value \"" + value + "\"");
}

void	Server::set_error_page(std::string arg)
{
	std::istringstream			iss(arg);
	std::vector<std::string>	tokens;
	err_page_t					error;

	for (std::string token; std::getline(iss, token, ' ');)
		tokens.push_back(token);
	if (tokens.size() < 2)
		throw std::runtime_error(
			"invalid number of arguments in \"error_page\" directive");
	error.uri = tokens.back();
	tokens.pop_back();
	std::string	overwrite;
	overwrite = tokens.back();
	error.overwrite = 0;
	if (overwrite.front() == '=')
	{
		std::string num = overwrite.substr(1);
		set_code(&error.overwrite, num);
		tokens.pop_back();
	}
	for (const std::string &token : tokens) 
	{
		int	code;
		set_code(&code, token);
		if (code < 300 || code > 599)
			throw std::runtime_error("value \"" + token + "\" must be between 300 and 599");
		error.code.push_back(code);
	}
	this->error_page.push_back(error);
}

void	Server::set_client_max_body_size(std::string arg)
{
	if (client_max_body_size_set)
		throw std::runtime_error(
			"\"client_max_body_size\" directive is duplicate");
	client_max_body_size_set = true;
	if (arg.substr(0, arg.size() - 2)
		.find_first_not_of("0123456789")
		== std::string::npos)
	{
		if (std::tolower(arg.back()) == 'k')
		{
			client_max_body_size = (std::stoi(arg) << 10);
			return ;
		}
		if (std::tolower(arg.back()) == 'm')
		{
			client_max_body_size = (std::stoi(arg) << 20);
			return ;
		}
		if (std::isdigit(arg.back()))
		{
			client_max_body_size = std::stoi(arg);
			return ;
		}
	}
	throw std::runtime_error(
		"\"client_max_body_size\" directive invalid value");
}
