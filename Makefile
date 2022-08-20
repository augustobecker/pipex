# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/09/07 17:02:42 by acesar-l          #+#    #+#              #
#    Updated: 2022/08/20 09:20:04 by acesar-l         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= pipex

GREEN		= \033[0;32m
GREY		= \033[0;90m
RESET		= \033[0m

LIBFT_PATH	= ./libraries/Libft
LIBFT		= ./libraries/Libft/libft.a

SRCS_PATH	= ./sources

CC 		= clang

FLAGS 		= -Wall -Werror -Wextra

REMOVE 		= rm -f

SRCS 		= $(SRCS_PATH)/pipex.c 			\
		$(SRCS_PATH)/initialize.c		\
		$(SRCS_PATH)/init_commands.c		\
		$(SRCS_PATH)/messages.c			\
		$(SRCS_PATH)/free_memory.c		\
		$(SRCS_PATH)/utils.c			\
		$(SRCS_PATH)/utils2.c

all:		$(NAME)

$(NAME): 	$(LIBFT)
		@$(CC) -o $(NAME) $(SRCS) $(FLAGS) $(LIBFT)
		@echo "$(GREY)$(NAME): $(GREEN)$(NAME) was created$(RESET)"

${LIBFT}:
		@make bonus -C $(LIBFT_PATH)

clean:
		@${REMOVE} ${NAME}
		@echo "$(GREY)$(NAME): $(NAME) was deleted$(RESET)"

fclean:		
		@make fclean -C $(LIBFT_PATH)
		@$(REMOVE) $(NAME)
		@echo "$(GREY)$(NAME): $(NAME) was deleted$(RESET)"

re:		clean $(NAME)

.PHONY:		all clean fclean re
