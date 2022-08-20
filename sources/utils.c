/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/20 02:29:53 by acesar-l          #+#    #+#             */
/*   Updated: 2022/08/20 08:51:48 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

void	redirect_input_and_output(int fd_in, int fd_out);
void	execute_not_used_cmds(t_data *data, char **envp);
void	treat_spaces_inside_the_command(char **command);
void	change_ocurrences(char **str, char old_c, char new_c);
int	change_ocurrences_til_limiter(char **str, char old_c, char new_c, \
char limiter);

void	execute_not_used_cmds(t_data *data, char **envp)
{
	int	pid;

	pid = fork();
	if (pid == 0)
	{
		redirect_input_and_output(data->fd_infile, data->fd_temp_file);
		execute(data, data->command[data->actual_command], envp);
	}
	else
	{
		waitpid(pid, NULL, 0);
		data->actual_command++;
	}
}

void	redirect_input_and_output(int fd_in, int fd_out)
{
	dup2(fd_in, STDIN_FILENO);
	dup2(fd_out, STDOUT_FILENO);
}

void	treat_spaces_inside_the_command(char **command)
{
	int	i;
	char	*ptr;

	i = 0;
	while (command[0][i])
	{
		if (command[0][i] == '\'')
		{
			ptr = &(command[0][i + 1]);
			i += change_ocurrences_til_limiter(&ptr, ' ', \
			PLACE_HOLDER, '\'');
		}
		if (command[0][i] == '\"')
		{
			ptr = &(command[0][i + 1]);
			i += change_ocurrences_til_limiter(&ptr, ' ', \
			PLACE_HOLDER, '\"');
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

int	change_ocurrences_til_limiter(char **str, char old_c, char new_c, \
char limiter)
{
	int	i;

	i = 0;
	while (str[0][i] && str[0][i] != limiter)
	{
		if (str[0][i] == old_c)
			str[0][i] = new_c;
		i++;
	}
	return (i + 1);
}
