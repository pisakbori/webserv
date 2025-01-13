#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <HttpError.hpp>

/*
 * https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms
 * #TODO
 * - [ ] Implement all CGI stuff...
 */

class Location
{
	private:
		std::string					uri;
		std::vector<std::string>	allow;
		std::pair<int, std::string>	redirect;
		std::string					root;
		bool						autoindex;
		std::vector<std::string>	index;

	// Setters
		void						set_location(std::string directive);
		void						set_uri(std::string uri);
		void						set_allow(std::string arg);
		void						set_redirect(std::string arg);
		void						set_root(std::string arg);
		void						set_autoindex(std::string arg);
		void						set_index(std::string arg);

	public:
	// Constructor
		Location();

	// Copy constructor
		Location(Location const & src);

	// Destructor
		~Location();

	// Overloads
		Location &	operator=(Location const & rhs);
	
	// Member functions
		void						populate_location(std::ifstream & infile, std::string line);

	// Getters
		std::string					get_uri(void) const;
		std::vector<std::string>	get_allow(void) const;
		std::pair<int, std::string>	get_redirect(void) const;
		std::string					get_root(void) const;
		bool						get_autoindex(void) const;
		std::vector<std::string>	get_index(void) const;
		std::string					get_route(const std::string &uri);

};
std::ostream &	operator<<(std::ostream & os, const Location & location);
std::string		extract_parameters(
	const std::string & name,
	const std::string & directive
	);

#endif
