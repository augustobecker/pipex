/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messages.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/20 08:58:54 by acesar-l          #+#    #+#             */
/*   Updated: 2022/08/20 08:59:51 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

void	command_not_found_message(char *command);
void	error(t_data *data, char *message, int exit_code);

void	command_not_found_message(char *command)
{
	dup2(STDERR_FILENO, STDOUT_FILENO);
	ft_printf(GREY"pipex: %s : command not found\n"RESET, command);
}

void	error(t_data *data, char *message, int exit_code)
{
	ft_printf(GREY"%s\n"RESET, message);
	free_data_and_exit(data, exit_code);
}
