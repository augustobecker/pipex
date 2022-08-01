/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialize.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/01 16:30:56 by acesar-l          #+#    #+#             */
/*   Updated: 2022/08/01 23:40:25 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

void	initialize(t_data *data, int argc, char **argv, char **envp);
void	init_files(t_data *data, char *infile, char *outfile);
void	find_paths_to_command(t_data *data, char **envp);
void	init_commands(t_data *data, char **argv);
char	**split_command(char *command);

void	initialize(t_data *data, int argc, char **argv, char **envp)
{
	if (argc < 5)
		error(GREY"pipex: Error : Bad argument\n\
Ex: ./pipex <infile> <cmd1> <cmd2> <...> <outfile>\n\
  : ./pipex \"here_doc\" <LIMITER> <cmd> <cmd1> <...> <file>\n"RESET);
	if (!ft_strncmp(argv[1], "here_doc", ft_strlen("here_doc")))
	{
		data->here_doc = true;
		init_heredoc_file(data, argv[argc - 1]);
		read_heredoc(data, argv[2]);
		data->nbr_of_commands = argc - 4;
	}
	else
		data->nbr_of_commands = argc - 3;
	init_files(data, argv[1], argv[argc - 1]);
	find_paths_to_command(data, envp);
	init_commands(data, argv);
}

void	init_files(t_data *data, char *infile, char *outfile)
{
	if (data->here_doc == true)
	{
		data->fd_infile = open(data->doc_file, O_RDONLY);
		if (data->fd_outfile == -1)
			error(GREY"infile: something unexpected happened\n"RESET);
		data->fd_outfile = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0777);
		if (data->fd_outfile == -1)
			error(GREY"outfile: something unexpected happened\n"RESET);
	}
	else
	{
		data->fd_infile = open(infile, O_RDONLY);
		if (data->fd_infile == -1)
			ft_printf(GREY"pipex: %s: No such file or directory\n"RESET, infile);
		data->fd_outfile = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if (data->fd_outfile == -1)
			error(GREY"outfile: something unexpected happened\n"RESET);
	}
}

void	find_paths_to_command(t_data *data, char **envp)
{
	int		i;

	i = 0;
	while (!ft_strnstr(envp[i], "PATH=", ft_strlen("PATH=")))
		i++;
	data->paths = ft_split(envp[i] + ft_strlen("PATH="), ':');
	data->paths[0] = ft_substr(data->paths[0], ft_strlen("PATH="), \
	ft_strlen(data->paths[0]));
}

void	init_commands(t_data *data, char **argv)
{
	int	arg;
	int	i;

	i = 0;
	if (data->here_doc == true)
		arg = 3;
	else
		arg = 2;
	data->command = malloc(sizeof(char ***) * (data->nbr_of_commands + 1));
	while (i < data->nbr_of_commands)
	{
		data->command[i] = split_command(argv[arg]);
		if (data->command[i][0][0] != '/')
			data->command[i][0] = ft_strjoin("/", data->command[i][0]);
		i++;
		arg++;
	}
}

char	**split_command(char *command)
{
	char	**array_cmd;
	char	*full_command;
	char	place_holder;
	int		i;

	i = 0;
	if (ft_strnstr(command, "awk", ft_strlen("awk"))
		|| ft_strnstr(command, "sed", ft_strlen("sed")))
	{
		full_command = ft_strdup(command);
		place_holder = define_a_not_used_place_holder(full_command);
		treat_spaces_in_command(&full_command, place_holder, 0);
	}
	else
		return (ft_split(command, ' '));
	array_cmd = ft_split(full_command, ' ');
	while (array_cmd[i])
	{
		change_ocurrences(&array_cmd[i], place_holder, ' ');
		array_cmd[i] = ft_strtrim(array_cmd[i], "\'");
		i++;
	}
	return (array_cmd);
}
