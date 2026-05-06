# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/16 12:40:32 by kong              #+#    #+#              #
#    Updated: 2026/03/27 20:39:37 by kong             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Compiler and Flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -g

# Archive and Flags
AR = ar
ARFLAGS = rcs

SRC_DIR = src
BUILD_DIR = build
FDF_HEADER_DIR = include
MLX_DIR = minilibx-linux

# Source Files
# SRCS = ops_all.c ops.c parse_data.c push_swap.c sort_small.c \
# 		sort_turk.c utils_free.c utils_mergesort.c utils_ops.c \
# 		utils_stack.c utils_turk_source.c utils_turk_target.c \
# 		utils_turk.c utils_validate.c utils1.c utils2.c utils3.c
SRC_FILES := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Executable
NAME = fdf

# Header Files
MLXFLAGS = -L${MLX_DIR} -lmlx -L/usr/lib/X11 -lXext -lX11 -lm
INCLUDES = -I${FDF_HEADER_DIR} -I${MLX_DIR}

# OBJS = $(SRCS:.c=.o)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Default Target
all: $(NAME)

$(MLX_DIR)/libmlx.a:
	$(MAKE) -C $(MLX_DIR)

# Build Executable
$(NAME): $(MLX_DIR)/libmlx.a $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) $(MLXFLAGS) -o $(NAME)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re