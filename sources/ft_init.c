/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_init.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 00:58:05 by acesar-l          #+#    #+#             */
/*   Updated: 2022/07/29 01:08:38 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libraries/pipex.h"

void	ft_check_command_line_arguments(int argc);
void	ft_init_files(t_pipex *pipex, char *infile, char *outfile);
void	ft_find_paths(t_pipex *pipex, char **envp);
void	ft_init_commands(t_pipex *pipex, char *first_cmd, char *secnd_cmd);

void ft_check_command_line_arguments(int argc)
{
	if (argc < 5)
		ft_error_msg("some arguments are missing");
	else if (argc > 5)
		ft_error_msg("Too many arguments \n       It should be only five");
}

void ft_init_files(t_pipex *pipex, char *infile, char *outfile)
{
	pipex->fd_infile = open(infile, O_RDONLY);
	if (pipex->fd_infile == -1)
		ft_error_msg("infile: No such file or directory");
	pipex->fd_outfile = open(outfile, O_WRONLY | O_CREAT, 0777);
	if (pipex->fd_outfile == -1)
		ft_error_msg("outfile: something unexpected happened");
}

void ft_find_paths(t_pipex *pipex, char **envp)
{
	char	*path;
	int	i;

	i = 0;
	while (!ft_strnstr(envp[i], "PATH=", sizeof(envp[i])))
		i++;
	path = ft_strdup(envp[i]);
	pipex->paths = ft_split(path, ':');
	pipex->paths[0] = ft_substr(pipex->paths[0], ft_strlen("PATH="),\
	 ft_strlen(pipex->paths[0]) - ft_strlen("PATH="));
	i = 0;
	while (pipex->paths[i])
	{
		pipex->paths[i++] = ft_strjoin(pipex->paths[i], "/")
	}
}

void	ft_init_commands(t_pipex *pipex, char *first_cmd, char *secnd_cmd)
{
	pipex->first_command = ft_split(first_cmd, ' ');
	pipex->secnd_command = ft_split(secnd_cmd, ' ');
}
