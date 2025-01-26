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
		if (line.find('}') != std::string::npos)
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
	static bool already_set = false;
	if (!already_set)
		allow.clear();
	already_set = true;
	std::istringstream iss(arg);
	for (std::string token; std::getline(iss, token, ' ');)
	{
		if (token == "HEAD" || token == "GET" || token == "POST" || token == "DELETE")
			allow.push_back(token);
		else
			throw std::runtime_error("invalid method \"" + token + "\"");
	}
}

void Location::set_redirect(std::string arg)
{
	if (redirect_set)
		throw std::runtime_error(
			"\"redirect\" directive is duplicate");
	redirect_set = true;
	std::istringstream iss(arg);
	std::string token;
	if (std::getline(iss, token, ' '))
	{
		try
		{
			redirect.first = std::stoi(token);
			if (redirect.first < 100 || redirect.first > 599)
				throw std::runtime_error(
					"value \"" + token + "\" must be between 100 and 599");
		}
		catch (const std::invalid_argument &)
		{
			redirect.first = 0;
			redirect.second = token;
			return;
		}
		if (std::getline(iss, token, ' '))
			redirect.second = token;
		if (!iss.eof())
			throw std::runtime_error(
				"invalid number of arguments in \"error_page\" directive");
	}
}

void Location::set_root(std::string arg)
{
	if (root_set)
		throw std::runtime_error(
			"\"root\" directive is duplicate");
	root_set = true;
	root = arg;
}

void Location::set_autoindex(std::string arg)
{
	if (autoindex_set)
		throw std::runtime_error(
			"\"autoindex\" directive is duplicate");
	autoindex_set = true;
	if (arg == "on")
		autoindex = true;
	else if (arg == "off")
		autoindex = false;
	else
		throw std::runtime_error(
			"invalid value \"" + \
			arg + "\" in \"autoindex\" directive," + \
			"it must be \"on\" or \"off\"");
}

void Location::set_index(std::string arg)
{
	std::istringstream iss(arg);
	for (std::string token; std::getline(iss, token, ' ');)
		index.push_back(std::move(token));
}
