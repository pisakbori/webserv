/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkijewsk <mkijewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 20:36:50 by mkijewsk          #+#    #+#             */
/*   Updated: 2024/12/17 18:49:01 by mkijewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() :
	allow(),
	redirect(),
	root("html"),
	autoindex(false),
	index()
{
	allow.push_back("GET");
	allow.push_back("HEAD");
	allow.push_back("POST");
	index.push_back("index.html");
}

Location::Location(Location const & src) :
	allow(src.allow),
	redirect(src.redirect),
	root(src.root),
	autoindex(src.autoindex),
	index(src.index)
{
}

Location::~Location()
{
}

Location &	Location::operator=(Location const & rhs)
{
	if (this != &rhs)
	{
		this->allow = rhs.allow;
		this->redirect = rhs.redirect;
		this->root = rhs.root;
		this->autoindex = rhs.autoindex;
		this->index = rhs.index;
	}
	return *this;
}
