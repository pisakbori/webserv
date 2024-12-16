/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 20:31:20 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/16 22:51:19 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "Server.hpp"

/*
 * https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms
 * #TODO
 * - [ ] Implement all CGI stuff...
*/

class Server::Location
{

	private:
		
		std::vector<std::string>	allow;
		std::pair<int, std::string>	redirect;
		std::string					root;
		bool						autoindex;
		std::vector<std::string>	index;

	public:

		Location();
		Location(Location const & src);
		~Location();
		Location &	operator=(Location const & rhs);
		
		void						set_allow(std::string directive);
		void						set_redirect(std::string directive);
		void						set_root(std::string directive);
		void						set_autoindex(std::string directive);
		void						set_index(std::string directive);
		std::vector<std::string>	get_allow(void) const;
		std::pair<int, std::string>	get_redirect(void) const;
		std::string					get_root(void) const;
		bool						get_autoindex(void) const;
		std::vector<std::string>	get_index(void) const;


};

#endif
