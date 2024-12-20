NAME		=		webserv
COMPILER	=		c++
FLAGS		=		-Wall -Wextra -Werror
STANDARD	=		--std=c++17
HTTP_DIR	=		http_core_module
REQ_RES_DIR	=		response_request
TEST_DIR	=		tests
INCLUDE		=		-I./http_core_module -I./connection -I./response_request
SOURCES     =       main.cpp \
					$(HTTP_DIR)/Server.cpp \
					$(HTTP_DIR)/ServerSet.cpp \
					$(HTTP_DIR)/ServerGet.cpp \
					$(HTTP_DIR)/Location.cpp \
					$(HTTP_DIR)/LocationSet.cpp \
					$(HTTP_DIR)/LocationGet.cpp \
					$(REQ_RES_DIR)/Validate.cpp \
					$(REQ_RES_DIR)/HttpError.cpp \
					$(REQ_RES_DIR)/Request.cpp \
					$(REQ_RES_DIR)/Response.cpp \
					connection/Connection.cpp \
					connection/RunningServer.cpp \
					$(TEST_DIR)/server.cpp \
					$(TEST_DIR)/location.cpp \
					$(TEST_DIR)/parse_request_head.cpp \
					$(TEST_DIR)/one_connection.cpp \
					$(TEST_DIR)/client.cpp
OBJECTS		=		$(SOURCES:.cpp=.o)

all:	$(NAME)

%.o: %.cpp
	$(COMPILER) $(FLAGS) $(STANDARD) $(INCLUDE) -o $@ -c $<

$(NAME): $(OBJECTS)
	$(COMPILER) $(OBJECTS) -o $(NAME)

clean:
	rm -rf $(OBJECTS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
