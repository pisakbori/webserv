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
	std::filesystem::path abs_root = std::filesystem::canonical(root);
	std::filesystem::path requested;
	if (str == uri)
		requested = abs_root;
	else if (str.find(uri) == 0)
	{
		auto requestedRelative = std::filesystem::path(str.substr(uri.length())).relative_path();
		std::filesystem::path abs_root = std::filesystem::canonical(root);
		requested = std::filesystem::weakly_canonical(root / requestedRelative);

		std::cout << abs_root << std::endl;
		std::cout << requested << std::endl;
		if (requested.string().compare(0, abs_root.string().size(), abs_root.string()) != 0)
			throw HttpError("Not allowed to go beyond root", 403);
	}
	else
		throw HttpError("Oh no! " + str + " not found.", 404);
	return requested;
}