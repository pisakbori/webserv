#include "Parsing.hpp"

void trim(std::string& str, const std::string& whitespace)
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
	{
		str = "";
		return;
	}

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;
	str = str.substr(strBegin, strRange);
}

std::string		extract_parameters(
	const std::string & name,
	std::string & directive
	)
{
	std::string	arg;
	size_t		pos;

	if (directive.back() != ';')
		throw std::runtime_error("directive " + name + " is not terminated by \";\"");
	if (directive.find(name) == 0 &&
		directive.find(' ') != std::string::npos)
	{
		pos = name.size();
		arg = directive.substr(directive.find_first_not_of(' ', pos));
		arg = arg.substr(0, arg.find_last_not_of("; ") + 1);
		return arg;
	}
	return "";
}

void serverBlockValid(std::string & line)
{
	const std::string	directive("server");
	auto iter_after_directive = line.begin() + directive.size();
	if (iter_after_directive < line.end())
		line.erase(
			remove_if(iter_after_directive, line.end(), isspace),
			line.end());
	if (!line.compare("server{"))
		return;
	if (line.find("server") == 0 && line.back() != '{')
		throw std::runtime_error("directive \"server\" has no opening \"{\"");
	else
		throw std::runtime_error("expected server block");
}

std::string normalize_config(const std::string & file_name)
{
	std::string		line;
	std::ifstream	infile(file_name);
	std::string		config;

	if (!infile)
	{
		std::cerr << "Cannot open file " + file_name << std::endl;
		exit(EXIT_FAILURE);
	}
	while (std::getline(infile, line))
	{
		std::string result;
		trim(line);
		char end = line.back();
		if (end != ';' && end != '{' && end != '}')
			line += '\n';
		bool lastWasSpace = false;
		for (char ch : line)
		{
			if (ch == '\t')
				result += ' ';
			if (ch == ' ')
			{
				if (!lastWasSpace)
					result += ch;
				lastWasSpace = true;
			}
			else
			{
				result += ch;
				lastWasSpace = false;
			}
			if (ch == ';' || ch == '{' || ch == '}')
				result += '\n';
		}
		config.append(result);
	}
	return config;
}

void parse_config(std::string file_name, std::vector<Server> &servers)
{
	std::istringstream	stream(normalize_config(file_name));
	std::string			line;
	while (std::getline(stream, line))
	{
		Server serv;
		trim(line);
		if (line.empty())
			continue ;
		serverBlockValid(line);
		serv.populate_server(stream);
		servers.push_back(serv);
	}
}

void locationBlockValid(std::string & line)
{
	const std::string directive("location");
	if (line.back() != '{')
		throw std::runtime_error("directive \"location\" has no opening \"{\"");
	std::string after_directive = line.substr(directive.size());
	std::string uri = after_directive.substr(0, line.find('{'));
	uri.erase(std::remove_if(uri.begin(), uri.end(), isspace), uri.end());
	if (uri.empty() || uri == "{")
		throw std::runtime_error("directive \"location\" is missing a URI");
}
