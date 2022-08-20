/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialize.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/20 02:21:24 by acesar-l          #+#    #+#             */
/*   Updated: 2022/08/20 08:52:05 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

void	initialize(t_data *data, int argc, char **argv, char **envp);
void	init_data(t_data *data, int argc);
void	check_for_null_argv(t_data *data, int argc, char **argv);
void	count_not_used_cmds(t_data *data, int argc, char **argv);
void	init_files(t_data *data, char *infile, char *outfile);

void	initialize(t_data *data, int argc, char **argv, char **envp)
{
	init_data(data, argc);
	if (argc < 5)
		error(data, "Missing arguments", 1);
	check_for_null_argv(data, argc, argv);
	count_not_used_cmds(data, argc, argv);
	init_files(data, argv[1], argv[argc - 1]);
	find_paths_to_command(data, envp);
	init_commands(data, argc, argv);
	data->cmds_alloc = true;
	if (data->not_used_cmds)
	{
		while (data->actual_command < data->not_used_cmds)
			execute_not_used_cmds(data, envp);
	}
	if (argv[argc - 2][0] == 0)
		error(data, "pipex: : command not found", 127);
}

void	init_data(t_data *data, int argc)
{
	data->actual_command = 0;
	data->nbr_of_commands = argc - 3;
	data->not_used_cmds = 0;
	data->cmds_alloc = false;
	data->null_cmd = false;
	data->invalid_infile = false;
}

void	check_for_null_argv(t_data *data, int argc, char **argv)
{	
	int	i;

	i = 2;
	if (argv[argc - 1][0] == 0)
		error(data, "pipex: : No such file or directory", 1);
	while (i < (argc - 1))
	{
		if (argv[i][0] == 0)
		{
			ft_printf(GREY"pipex: : command not found\n"RESET);
			data->nbr_of_commands--;
			data->null_cmd = true;
		}
		i++;
	}
}

void	count_not_used_cmds(t_data *data, int argc, char **argv)
{
	int	i;
	int	argv_first_cmd;

	i = argc - 2;
	argv_first_cmd = 2;
	while (i >= argv_first_cmd)
	{
		if (argv[i][0] == 0)
			break ;
		i--;
	}
	while (i >= argv_first_cmd)
	{
		if (argv[i][0] != 0)
			data->not_used_cmds++;
		i--;
	}
}

void	init_files(t_data *data, char *infile, char *outfile)
{
	data->fd_outfile = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (data->fd_outfile == -1)
		error(data, "No such file or directory", 1);
	data->fd_infile = open(infile, O_RDONLY);
	if (data->fd_infile == -1)
	{
		ft_printf(GREY"pipex: %s: No such file or directory\n"RESET, \
		infile);
		data->invalid_infile = true;
	}
	if (data->not_used_cmds || data->invalid_infile == true)
	{
		data->fd_temp_file = \
		open(TEMP_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		data->fd_infile = open(TEMP_INFILE, O_RDONLY | O_CREAT | O_TRUNC, 0644);
	}
}
