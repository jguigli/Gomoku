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