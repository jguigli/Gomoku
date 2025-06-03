# # SRC =	$(SRC_DIR)main.cpp

# NAME			=	gomoku
# CC				=	clang++
# RM				=	rm -f

# SRC_DIR			=	src/
# OBJ_DIR			=	build/

# HEADER			=	header/
# SRC     		= 	$(wildcard $(SRC_DIR)*.cpp)
# OBJ         	= 	$(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SRC))
# DEP         	= 	$(OBJ:.o=.d)


# # Compilation flags -------------------------------------------------

# UNAME_S := $(shell uname -s)

# ifeq ($(UNAME_S),Linux)
# 	RAYLIBFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
# 	LDFLAGS = -L/usr/lib
# endif

# ifeq ($(UNAME_S),Darwin)
# 	RAYLIBFLAGS = -lraylib
# 	LDFLAGS = -L/opt/homebrew/lib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
# endif

# CFLAGS			=	-std=c++20 -Wall -Wextra -Werror -g -fsanitize=address
# IFLAGS			=	-I incl/ -I /usr/include -I /opt/homebrew/include


# # Colors ------------------------------------------------------------

# _GREY	=	$'\e[30m
# _RED	=	$'\e[31m
# _GREEN	=	$'\e[32m
# _YELLOW	=	$'\e[33m
# _BLUE	=	$'\e[34m
# _PURPLE	=	$'\e[35m
# _CYAN	=	$'\e[36m
# _WHITE	=	$'\e[37m

# # Compilation part --------------------------------------------------

# $(OBJ_DIR)%.o : $(SRC_DIR)%.cpp $(HEADER)
# 				@echo "Compiling $(_GREEN)$@$(_WHITE) ...\t\t\t\c"
# 				@mkdir -p $(OBJ_DIR)
# 				@$(CC) $(CFLAGS) $(IFLAGS) -MMD -c $< -o $@
# 				@echo "$(_GREEN)DONE$(_WHITE)"

# all:			$(NAME)

# $(NAME): $(OBJ)
# 				@echo "-----\nCreating Binary File $(_GREEN)$@$(_WHITE) ... \c"
# 				@$(CC) $(CFLAGS) $(OBJ) $(IFLAGS) $(RAYLIBFLAGS) $(LDFLAGS) -o $(NAME)
# 				@echo "$(_GREEN)DONE$(_WHITE)\n-----"
# 				@echo "$(_GREEN)FT_CONTAINERS$(_WHITE) ---> execute ./$(NAME)\n"
# -include $(DEP)

# # Install part --------------------------------------------------

# install:
# 				bash ./install_raylib.sh

# # Clean part --------------------------------------------------

# clean:
# 				@echo "$(_WHITE)Deleting Objects Directory $(_YELLOW)$(OBJ_DIR)$(_WHITE) ... \c"
# 				@rm -rf $(OBJ_DIR)
# 				@echo "$(_GREEN)DONE$(_WHITE)\n-----"

# fclean:			clean
# 				@echo "$(_WHITE)Deleting Binary File $(_YELLOW)$(NAME)$(_WHITE) ... \c"
# 				@rm -f $(NAME)
# 				@echo "$(_GREEN)DONE$(_WHITE)\n-----"

# re:				fclean all

# .PHONY:			all clean fclean re%

NAME      := gomoku
CC        := clang++
SRC_DIR   := src
OBJ_DIR   := build
INC_DIR   := includes

# Recherche récursive des .cpp
SRC       := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJ       := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
DEP       := $(OBJ:.o=.d)

UNAME_S   := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    RAYLIBFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    LDFLAGS = -L/usr/lib
	IFLAGS = -I$(INC_DIR) -I/usr/include
endif
ifeq ($(UNAME_S),Darwin)
    RAYLIBFLAGS = -lraylib
    LDFLAGS = -L/opt/homebrew/lib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	IFLAGS = -I/opt/homebrew/include
endif

CFLAGS    := -std=c++20 -Wall -Wextra -Werror -g -fsanitize=address


# Génère les dossiers nécessaires pour les .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $< ..."
	@$(CC) $(CFLAGS) $(IFLAGS) -MMD -c $< -o $@

all: $(NAME)

$(NAME): $(OBJ)
	@echo "Linking $(NAME)..."
	@$(CC) $(CFLAGS) $(OBJ) $(IFLAGS) $(RAYLIBFLAGS) $(LDFLAGS) -o $(NAME)
	@echo "Build complete."

-include $(DEP)

install:
		bash ./install_raylib.sh

clean:
	@echo "Cleaning object files..."
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "Cleaning binary..."
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re install