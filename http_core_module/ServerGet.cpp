#include "Server.hpp"

// Getters
Listen &					Server::get_listen(void)
{
	return listen;
}

std::vector<std::string>	Server::get_server_name(void) const
{
	return server_name;
}

err_page_t 					Server::get_error_page(void) const
{
	return error_page;
}

long long					Server::get_client_max_body_size(void) const
{
	return client_max_body_size;
}

const Location &			Server::get_location(std::string requested_uri) const
{
	unsigned int	longest_match;
	std::string 	location_uri;
	int				position_match;

	longest_match = 0;
	position_match = -1;
	for (size_t i = 0; i < location.size(); ++i)
	{
		location_uri = location.at(i).get_uri();
		if (requested_uri.compare(0, location_uri.length(), location_uri) == 0 &&
			longest_match < location_uri.length())
			{
				position_match = i;
				longest_match = location_uri.length();
			}
	}
	if (position_match == -1)
		throw HttpError("Oh no! " + requested_uri + " not found.", 404);
	return location.at(position_match);
}

int Server::getListenFd() const
{
	return listen.getFd();
}
