/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messages.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/30 17:05:06 by acesar-l          #+#    #+#             */
/*   Updated: 2022/07/30 17:07:21 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libraries/pipex.h"

void	error_message(char *message);
void command_not_found_message(char *command);

void	error_message(char *message)
{
	ft_printf(RED"Error\n%s\n"RESET, message);
	exit (EXIT_FAILURE);
}

void command_not_found_message(char *command)
{
	dup2(STDERR_FILENO, STDOUT_FILENO);
	ft_printf(GREY"pipex: %s : command not found\n"RESET, command);
}
