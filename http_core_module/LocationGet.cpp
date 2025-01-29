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

std::string Location::get_cgi_extension(void) const
{
	return cgi_extension;
}

std::string Location::get_cgi_path(void) const
{
	return cgi_path;
}

std::filesystem::path Location::get_route(const std::string &str)
{
	std::filesystem::path route;
	if (str == uri)
		route = std::filesystem::path(root);
	else if (str.find(uri) == 0)
		route = std::filesystem::path(root) / std::filesystem::path(str.substr(uri.length())).relative_path();
	else
		throw HttpError("Oh no! " + str + " not found.", 404);
	return route;
}