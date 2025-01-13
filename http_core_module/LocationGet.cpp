#include "Location.hpp"

// Getters
std::string					Location::get_uri(void) const
{
	return uri;
}

std::vector<std::string> Location::get_allow(void) const
{
	return allow;
}

std::pair<int, std::string> Location::get_redirect(void) const
{
	return redirect;
}

std::string Location::get_root(void) const
{
	return root;
}

bool Location::get_autoindex(void) const
{
	return autoindex;
}

std::vector<std::string> Location::get_index(void) const
{
	return index;
}

std::string Location::get_route(const std::string &str)
{
	std::string route;
	if (str == uri)
		route = root;
	else if (str.find(uri) == 0)
		route = root + "/" + str.substr(uri.length());
	else
		throw HttpError("Oh no! " + str + " not found.", 404);
	return route;
}