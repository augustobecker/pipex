/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_memory.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/20 08:55:24 by acesar-l          #+#    #+#             */
/*   Updated: 2022/08/20 08:56:24 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

void	free_commands(t_data *data);
void	free_data_and_exit(t_data *data, int exit_code);

void	free_commands(t_data *data)
{
	int	cmds;

	cmds = 0;
	while (cmds < data->nbr_of_commands)
	{
		free_str_array(data->command[cmds]);
		cmds++;
	}
	free(data->command);
}

void	free_data_and_exit(t_data *data, int exit_code)
{
	if (data->cmds_alloc == true)
	{
		free_commands(data);
		free_str_array(data->paths);
	}
	free(data);
	exit (exit_code);
}
