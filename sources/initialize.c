/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialize.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 00:58:05 by acesar-l          #+#    #+#             */
/*   Updated: 2022/07/30 12:28:38 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libraries/pipex.h"

void	initialize(t_data *data, int argc, char **argv, char **envp);
void	init_files(t_pipex *pipex, char *infile, char *outfile);
void	find_paths(t_pipex *pipex, char **envp);
void	init_commands(t_pipex *pipex, char *first_cmd, char *secnd_cmd);

void	initialize(t_data *data, int argc, char **argv, char **envp)
{
	if (argc < 5)
		error_msg("Some arguments are missing");
	if (!ft_strncmp(argv[1], "here_doc", ft_strlen("here_doc")))
		read_here_doc(data, argv[1], argv[2], envp);
	data->nbr_of_commands = argc - 3;
	init_files(data, argv[1], argv[argc - 1]);
	find_paths(data, envp);
	init_commands(data, argv);
}

void	init_files(t_data *data, char *infile, char *outfile)
{
	if (data->here_doc == true)
	{
		data->fd_outfile = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0777);
		if (data->fd_outfile == -1)
			error_msg("outfile: something unexpected happened");
	}
	else
	{
		data->fd_infile = open(infile, O_RDONLY);
		if (data->fd_infile == -1)
			ft_printf(GREY"pipex: %s: No such file or directory", infile);
		data->fd_outfile = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if (data->fd_outfile == -1)
			error_msg("outfile: something unexpected happened");
	}
}

void	find_paths(t_data *data, char **envp)
{
	int	i;

	i = 0;
	while (!ft_strnstr(envp[i], "PATH=", ft_strlen("PATH=")))
		i++;
	data->paths = ft_split(envp[i] + ft_strlen("PATH="), ':');
	data->paths[0] = ft_substr(data->paths[0], ft_strlen("PATH="), ft_strlen(data->paths[0]));
}

void	init_commands(t_data *data, char **argv)
{
	int arg;
	int i;

	i = 0;
	arg = 2;
	data->command = malloc(sizeof(char ***) * (data->nbr_of_commands + 1));
	while (i < data->nbr_of_commands)
	{
		data->command[i] = split_command(argv[arg]);
		data->command[i][0] = ft_strjoin("/", data->command[i][0]);
		i++;
		arg++;
	}
}
