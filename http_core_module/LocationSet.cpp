#include "Location.hpp"

// Member functions
void	Location::populate_location(std::istringstream & infile, std::string line)
{
	this->set_uri(line);
	while (std::getline(infile, line))
	{
		trim(line);
		if (line.empty())
			continue ;
		if (line.find("}") != std::string::npos)
			break ;
		this->set_location(line);
	}
}

// Setters
void	Location::set_location(std::string directive)
{
	std::string		directives[] =
	{
		"allow",
		"redirect",
		"root",
		"autoindex",
		"index"
	};
	void	(Location::*fnptr[])( std::string ) =
	{
		&Location::set_allow,
		&Location::set_redirect,
		&Location::set_root,
		&Location::set_autoindex,
		&Location::set_index
	};
	int			i;
	const int	N = sizeof(directives) / sizeof(directives[0]);
	std::string	directive_name = directive.substr(0, directive.find_first_of(" ;"));

	i = 0;
	while (i < N && directive_name.find(directives[i]) == std::string::npos)
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

void Location::set_uri(std::string uri)
{
	std::string	result;

	result = uri.substr(uri.find(' ') + 1);
	result = result.substr(0, result.find_last_not_of("{ ") + 1);
	this->uri = result;
}

void Location::set_allow(std::string arg)
{
	std::istringstream iss(arg);
	allow.clear();
	for (std::string token; std::getline(iss, token, ' ');)
	{
		if (token == "HEAD" || token == "GET" || token == "POST" || token == "DELETE")
			allow.push_back(token);
	}
}

void Location::set_redirect(std::string arg)
{
	std::istringstream iss(arg);
	std::string token;
	if (std::getline(iss, token, ' '))
	{
		try
		{
			redirect.first = std::stoi(token);
		}
		catch (const std::invalid_argument &)
		{
			redirect.first = 0;
			redirect.second = token;
			return;
		}
		if (std::getline(iss, token))
			redirect.second = token;
	}
}

void Location::set_root(std::string arg)
{
	root = arg;
}

void Location::set_autoindex(std::string arg)
{
	if (arg == "on")
		autoindex = true;
	else if (arg == "off")
		autoindex = false;
}

void Location::set_index(std::string arg)
{
	index = {};
	std::istringstream iss(arg);
	for (std::string token; std::getline(iss, token, ' ');)
		index.push_back(std::move(token));
}
