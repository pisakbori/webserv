#ifndef LISTEN_HPP
#define LISTEN_HPP

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <tuple>
#define MAX_QUEUED 1000

class Listen
{
	private:
		std::string		host;
		unsigned short	port;
		int				_fd;

	public:
	// Constructor
		Listen();

	// Parameterized constructor
		
	// Copy constructor
		Listen(const Listen &);
		
	// Destructor
		~Listen();
		
	// Overloads
		Listen	&operator=(const Listen &);
		bool operator<(const Listen& other) const
		{
			return std::tie(host, port) < std::tie(other.host, other.port);
		}
		
	// Member functions
		void			parse_listen(std::string arg);
		void			startListening(void);
		void			stopListening(void);

	// Setters
		void			set_host(std::string arg);
		void			set_port(std::string arg);
		void			setFd(int fd);

	// Getters
		std::string		get_host(void) const;
		unsigned short	get_port(void) const;
		int				getFd() const;
		
};
std::ostream &	operator<<(std::ostream & os, const Listen & listen);

#endif
