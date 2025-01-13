#include "Location.hpp"

// Constructor
Location::Location() :
	uri(),
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

// Copy constructor
Location::Location(Location const & src) :
	uri(src.uri),
	allow(src.allow),
	redirect(src.redirect),
	root(src.root),
	autoindex(src.autoindex),
	index(src.index)
{
}

// Destructor
Location::~Location()
{
}

// Overloads
Location &	Location::operator=(Location const & rhs)
{
	if (this != &rhs)
	{
		this->uri = rhs.uri;
		this->allow = rhs.allow;
		this->redirect = rhs.redirect;
		this->root = rhs.root;
		this->autoindex = rhs.autoindex;
		this->index = rhs.index;
	}
	return *this;
}

std::ostream &	operator<<(std::ostream & os, const Location & location)
{
	os << "location uri: " << location.get_uri() << std::endl;
	os << "  allow: ";
	std::vector<std::string> v(location.get_allow());
	for (std::vector<std::string>::const_iterator i = v.begin(); i != v.end(); ++i)
		os << *i << ' ';
	os << std::endl;
	os << "  redirect: ";
	os << location.get_redirect().first << ' ';
	os << location.get_redirect().second << std::endl;
	os << "  root: " << location.get_root() << std::endl;
	os << "  autoindex: " << std::boolalpha << location.get_autoindex() << std::endl;
	os << "  index: ";
	v = location.get_index();
	for (std::vector<std::string>::const_iterator i = v.begin(); i != v.end(); ++i)
		os << *i << ' ';
	return os;
}