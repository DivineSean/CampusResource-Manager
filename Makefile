NAME = campus_system
SRC = main.cpp Value.cpp Document.cpp Json.cpp Collection.cpp Database.cpp CampusApp.cpp outils.cpp
INC = Value.hpp Document.hpp Json.hpp Collection.hpp Database.hpp CampusApp.hpp outils.hpp
SRCDIR = src
INCDIR = include
OBJDIR = obj
DATADIR = data

OBJS = $(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))
CFLAGS = -Wall -Wextra -Werror -std=c++17 -I$(INCDIR)

R := $(shell tput -Txterm setaf 1)
G := $(shell tput -Txterm setaf 2)
Y := $(shell tput -Txterm setaf 3)
B := $(shell tput -Txterm setaf 4)
C := $(shell tput -Txterm setaf 6)
END := $(shell tput -Txterm sgr0)

all: check-structure $(NAME)
	@echo "$(G)Campus Document Database compiled successfully!$(END)"

$(NAME): $(OBJS)
	@echo "$(B)Linking executable: $@$(END)"
	@c++ $(CFLAGS) $(OBJS) -o $@
	@echo "$(G)Build complete: $(NAME)$(END)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(addprefix $(INCDIR)/, $(INC))
	@echo "$(Y)Compiling: $< ... Done!$(END)"
	@c++ $(CFLAGS) -c $< -o $@

check-structure:
	@if [ ! -d "$(OBJDIR)" ]; then mkdir -p $(OBJDIR); fi
	@if [ ! -d "$(DATADIR)" ]; then mkdir -p $(DATADIR); fi

run: $(NAME)
	@./$(NAME)

clean:
	@echo "$(R)Cleaning object files...$(END)"
	@rm -f $(OBJS)
	@echo "$(R)Cleaned$(END)"

fclean: clean
	@echo "$(R)Cleaning executable...$(END)"
	@rm -f $(NAME)
	@echo "$(R)Cleaning database files...$(END)"
	@rm -f $(DATADIR)/*.jsonl
	@echo "$(R)Fully cleaned$(END)"

re: fclean all

.PHONY: all clean fclean re run check-structure
