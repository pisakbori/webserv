NAME		=		webserv
COMPILER	=		c++
FLAGS		=		-Wall -Wextra -Werror
STANDARD	=		--std=c++17
HTTP_DIR	=		http_core_module
REQ_RES_DIR	=		response_request
CONNECTION	=		connection
TEST_DIR	=		tests
INCLUDE		=		-I./http_core_module -I./connection -I./response_request
SOURCES     =		main.cpp \
					$(HTTP_DIR)/Server.cpp \
					$(HTTP_DIR)/ServerSet.cpp \
					$(HTTP_DIR)/ServerGet.cpp \
					$(HTTP_DIR)/Listen.cpp \
					$(HTTP_DIR)/Location.cpp \
					$(HTTP_DIR)/LocationSet.cpp \
					$(HTTP_DIR)/LocationGet.cpp \
					$(HTTP_DIR)/Parsing.cpp \
					$(REQ_RES_DIR)/Validate.cpp \
					$(REQ_RES_DIR)/HttpError.cpp \
					$(REQ_RES_DIR)/Request.cpp \
					$(REQ_RES_DIR)/Response.cpp \
					$(REQ_RES_DIR)/HttpMessage.cpp \
					$(CONNECTION)/Connection.cpp \
					$(CONNECTION)/Webserv.cpp \
					$(CONNECTION)/Cgi.cpp 
OBJECTS		=		$(SOURCES:.cpp=.o)

all:	$(NAME)

%.o: %.cpp
	$(COMPILER) $(FLAGS) $(STANDARD) $(INCLUDE) -o $@ -c $<

$(NAME): $(OBJECTS)
	$(COMPILER) $(OBJECTS) -o $(NAME)

test: $(OBJECTS) test.o
	$(COMPILER) $(OBJECTS) test.o -o test

clean:
	rm -rf $(OBJECTS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
