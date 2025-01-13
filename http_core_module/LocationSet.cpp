#include "Location.hpp"

// Member functions
void	Location::populate_location(std::ifstream & infile, std::string line)
{
	this->set_uri(line);
	while (std::getline(infile, line))
	{
		if (line.find("	}") != std::string::npos)
			return ;
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

	i = 0;
	while (i < N && directive.find(directives[i]) == std::string::npos)
		i++;
	std::string arg = extract_parameters(directives[i], directive);
	if (i != N)
		(this->*fnptr[i])(arg);
}

void	Location::set_uri(std::string uri)
{
	this->uri = extract_parameters("location", uri);
}

void	Location::set_allow(std::string arg)
{
	std::istringstream	iss(arg);
	allow.clear();
	for (std::string token; std::getline(iss, token, ' ');)
	{
		if (token == "HEAD" || token == "GET" || token == "POST")
			allow.push_back(token);
	}
}

void	Location::set_redirect(std::string arg)
{
	std::istringstream	iss(arg);
	std::string	token;
	if (std::getline(iss, token, ' '))
	{
		try
		{
			redirect.first = std::stoi(token);
		}
		catch(const std::invalid_argument&)
		{
			redirect.first = 0;
			redirect.second = token;
			return;
		}
		if (std::getline(iss, token))
			redirect.second = token;
	}
}

void	Location::set_root(std::string arg)
{
	root = arg;
}

void	Location::set_autoindex(std::string arg)
{
	if (arg == "on")
		autoindex = true;
	else if (arg == "off")
		autoindex = false;
}

void	Location::set_index(std::string arg)
{
	index.pop_back();
	std::istringstream	iss(arg);
	for (std::string token; std::getline(iss, token, ' ');)
        index.push_back(std::move(token));
}
