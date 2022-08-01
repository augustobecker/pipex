/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/01 16:20:09 by acesar-l          #+#    #+#             */
/*   Updated: 2022/08/01 16:28:37 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libraries/pipex.h"

void	error(char *message);
void	command_not_found_message(char *command);
void	treat_spaces_in_command(char **command, char place_holder, int i);
void	change_ocurrences(char **str, char old_c, char new_c);
char	define_a_not_used_place_holder(char *str);

void	error(char *message)
{
	perror(message);
	exit (EXIT_FAILURE);
}

void	command_not_found_message(char *command)
{
	dup2(STDERR_FILENO, STDOUT_FILENO);
	ft_printf(GREY"pipex: %s : command not found\n"RESET, command);
	exit (0);
}

void	treat_spaces_in_command(char **command, char place_holder, int i)
{
	while (command[0][i])
	{
		if (command[0][i] == '\'')
		{
			i++;
			while (command[0][i] != '\'')
			{
				if (command[0][i] == ' ')
					command[0][i] = place_holder;
				i++;
			}
		}
		if (command[0][i] == '{')
		{
			while (command[0][i] != '}')
			{
				if (command[0][i] == ' ')
					command[0][i] = place_holder;
				i++;
			}
		}
		i++;
	}
}

void	change_ocurrences(char **str, char old_c, char new_c)
{
	int	i;

	i = 0;
	while (str[0][i])
	{
		if (str[0][i] == old_c)
			str[0][i] = new_c;
		i++;
	}
}

char	define_a_not_used_place_holder(char *str)
{
	char	place_holder;

	place_holder = 16;
	while (ft_count_occurrences(str, place_holder) != 0)
		place_holder++;
	return (place_holder);
}
