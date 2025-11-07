NAME = campus_system
SRC = main.cpp Resource.cpp ResourceManager.cpp Event.cpp EventManager.cpp CampusSystem.cpp outils.cpp
INC = Resource.hpp ResourceManager.hpp Event.hpp EventManager.hpp CampusSystem.hpp outils.hpp
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
M := $(shell tput -Txterm setaf 5)
C := $(shell tput -Txterm setaf 6)
W := $(shell tput -Txterm setaf 7)
END := $(shell tput -Txterm sgr0)

all: check-structure $(NAME)
	@echo "$(G)Campus Management System compiled successfully!$(END)"

$(NAME): $(OBJS)
	@echo "$(B)Linking executable: $@$(END)"
	@c++ $(CFLAGS) $(OBJS) -o $@
	@echo "$(G)Build complete: $(NAME)$(END)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(addprefix $(INCDIR)/, $(INC))
	@echo "$(Y)Compiling: $< ... Done!$(END)"
	@c++ $(CFLAGS) -c $< -o $@

check-structure: check-dirs check-headers check-sources

check-dirs:
	@echo "$(C)Checking project structure...$(END)"
	@if [ ! -d "$(SRCDIR)" ]; then \
		echo "$(R)ERROR: Missing directory: $(SRCDIR)/$(END)"; \
		echo "$(R)Please create the $(SRCDIR)/ directory for source files$(END)"; \
		exit 1; \
	fi
	@if [ ! -d "$(INCDIR)" ]; then \
		echo "$(R)ERROR: Missing directory: $(INCDIR)/$(END)"; \
		echo "$(R)Please create the $(INCDIR)/ directory for header files$(END)"; \
		exit 1; \
	fi
	@if [ ! -d "$(OBJDIR)" ]; then \
		echo "$(R)ERROR: Missing directory: $(OBJDIR)/$(END)"; \
		echo "$(R)Please create the $(OBJDIR)/ directory for object files$(END)"; \
		exit 1; \
	fi
	@if [ ! -d "$(DATADIR)" ]; then \
		echo "$(R)ERROR: Missing directory: $(DATADIR)/$(END)"; \
		echo "$(R)Please create the $(DATADIR)/ directory for database files$(END)"; \
		exit 1; \
	fi
	@echo "$(G)✓ All directories found$(END)"

check-headers:
	@echo "$(C)Checking header files...$(END)"
	@missing_headers=""; \
	for header in $(INC); do \
		if [ ! -f "$(INCDIR)/$$header" ]; then \
			missing_headers="$$missing_headers $(INCDIR)/$$header"; \
		fi \
	done; \
	if [ ! -z "$$missing_headers" ]; then \
		echo "$(R)ERROR: Missing header files:$(END)"; \
		for file in $$missing_headers; do \
			echo "$(R)  - $$file$(END)"; \
		done; \
		echo "$(R)Please create all required header files in $(INCDIR)/$(END)"; \
		exit 1; \
	fi
	@echo "$(G)✓ All header files found$(END)"

check-sources:
	@echo "$(C)Checking source files...$(END)"
	@missing_sources=""; \
	for source in $(SRC); do \
		if [ ! -f "$(SRCDIR)/$$source" ]; then \
			missing_sources="$$missing_sources $(SRCDIR)/$$source"; \
		fi \
	done; \
	if [ ! -z "$$missing_sources" ]; then \
		echo "$(R)ERROR: Missing source files:$(END)"; \
		for file in $$missing_sources; do \
			echo "$(R)  - $$file$(END)"; \
		done; \
		echo "$(R)Please create all required source files in $(SRCDIR)/$(END)"; \
		exit 1; \
	fi
	@echo "$(G)✓ All source files found$(END)"

run: $(NAME)
	@echo "$(M)Running Campus Management System...$(END)"
	@./$(NAME)

clean:
	@echo "$(R)Cleaning object files...$(END)"
	@rm -f $(OBJS)
	@echo "$(R)Cleaned$(END)"

fclean: clean
	@echo "$(R)Cleaning executable...$(END)"
	@rm -f $(NAME)
	@echo "$(R)Cleaning database files...$(END)"
	@rm -f $(DATADIR)/*.txt
	@echo "$(R)Fully cleaned$(END)"

re: fclean all

.PHONY: all clean fclean re run check-structure check-dirs check-headers check-sources