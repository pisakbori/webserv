NAME		=		webserv
COMPILER	=		c++
FLAGS		=		-Wall -Wextra -Werror
STANDARD	=		--std=c++98
HTTP_DIR	=		http_core_module
SOURCES     =       main.cpp \
					$(HTTP_DIR)/Server.cpp \
					$(HTTP_DIR)/ServerSet.cpp \
					$(HTTP_DIR)/ServerGet.cpp \
					$(HTTP_DIR)/Location.cpp \
					$(HTTP_DIR)/LocationSet.cpp \
					$(HTTP_DIR)/LocationGet.cpp
OBJECTS		=		$(SOURCES:.cpp=.o)

all:	$(NAME)

%.o: %.cpp
	$(COMPILER) $(FLAGS) $(STANDARD) -o $@ -c $<

$(NAME): $(OBJECTS)
	$(COMPILER) $(OBJECTS) -o $(NAME)

clean:
	rm -rf $(OBJECTS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
