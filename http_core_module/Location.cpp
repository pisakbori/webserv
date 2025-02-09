#include "Location.hpp"

// Constructor
Location::Location() :
	uri(),
	allow(),
	redirect(),
	root("html"),
	autoindex(false),
	index(),
	cgi_extension(),
	cgi_path(),
	redirect_set(false),
	root_set(false),
	autoindex_set(false),
	cgi_extension_set(false),
	cgi_path_set(false)
{
	allow.push_back("GET");
	allow.push_back("HEAD");
	allow.push_back("POST");
	allow.push_back("DELETE");
}

// Copy constructor
Location::Location(Location const & src) :
	uri(src.uri),
	allow(src.allow),
	redirect(src.redirect),
	root(src.root),
	autoindex(src.autoindex),
	index(src.index),
	cgi_extension(src.cgi_extension),
	cgi_path(src.cgi_path),
	redirect_set(src.redirect_set),
	root_set(src.root_set),
	autoindex_set(src.autoindex_set),
	cgi_extension_set(src.cgi_extension_set),
	cgi_path_set(src.cgi_path_set)
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
		this->cgi_extension = rhs.cgi_extension;
		this->cgi_path = rhs.cgi_path;
		this->redirect_set = rhs.redirect_set;
		this->root_set = rhs.root_set;
		this->autoindex_set = rhs.autoindex_set;
		this->cgi_extension_set = rhs.cgi_extension_set;
		this->cgi_path_set = rhs.cgi_path_set;
	}
	return *this;
}

std::ostream &operator<<(std::ostream &os, const Location &location)
{
	os << "location uri: " << location.get_uri() << std::endl;
	os << "  allow: ";
	std::vector<std::string> v(location.get_allow());
	for (std::vector<std::string>::const_iterator i = v.begin(); i != v.end(); ++i)
		os << *i << ' ';
	os << std::endl;
	os << "  redirect: ";
	os << "code: " << location.get_redirect().first << ' ';
	os << "url: " <<location.get_redirect().second << std::endl;
	os << "  root: " << location.get_root() << std::endl;
	os << "  autoindex: " << std::boolalpha << location.get_autoindex() << std::endl;
	os << "  index: ";
	v = location.get_index();
	for (std::vector<std::string>::const_iterator i = v.begin(); i != v.end(); ++i)
		os << *i << ' ';
	os << std::endl;
	os << "  cgi_extension: " << location.get_cgi_extension() << std::endl;
	os << "  cgi_path: " << location.get_cgi_path();
	return os;
}

// Member functions
// TODO:move to utils
std::string joinStrings(const std::vector<std::string> &vec, const std::string &delimiter)
{
	std::ostringstream oss;
	for (size_t i = 0; i < vec.size(); ++i)
	{
		oss << vec[i];
		if (i != vec.size() - 1)
			oss << delimiter;
	}
	return oss.str();
}

void	Location::validate_allowed(const std::string &method) const
{
	if (std::find(allow.begin(), allow.end(), method) == allow.end())
	{
		auto err = HttpError(method + " method not allowed for " + uri, 405);
		err.setField("Allow", joinStrings(allow, ", "));
		throw err;
	}
}