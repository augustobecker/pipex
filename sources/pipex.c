/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/01 16:14:01 by acesar-l          #+#    #+#             */
/*   Updated: 2022/08/01 19:06:27 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

void	execute(t_data *data, char **cmd_and_flags, char **envp);
int		fork_and_exec_first_cmd(t_data *data, char **envp);
void	fork_and_exec_middle_cmd(t_data *data, int fd_in, char **envp);
void	exec_last_command(t_data *data, int fd_in, char **envp);

int	main(int argc, char **argv, char **envp)
{
	t_data	*data;
	int		fd_pipe_input;

	data = malloc(sizeof(t_data));
	initialize(data, argc, argv, envp);
	fd_pipe_input = fork_and_exec_first_cmd(data, envp);
	if (data->nbr_of_commands == 2)
		exec_last_command(data, fd_pipe_input, envp);
	fork_and_exec_middle_cmd(data, fd_pipe_input, envp);
	return (0);
}

void	execute(t_data *data, char **cmd_and_flags, char **envp)
{
	char	*command;
	int		i;

	i = 0;
	if (ft_count_occurrences(cmd_and_flags[0], '/') > 1)
	{
		execve(cmd_and_flags[0], &cmd_and_flags[0], envp);
		command_not_found_message(cmd_and_flags[0]);
	}
	while (data->paths[i])
	{
		command = ft_strjoin(data->paths[i++], cmd_and_flags[0]);
		execve(command, &cmd_and_flags[0], envp);
	}
	command_not_found_message(&cmd_and_flags[0][1]);
}

int	fork_and_exec_first_cmd(t_data *data, char **envp)
{
	int	fd_pipe[2];
	int	pid;

	pipe(fd_pipe);
	pid = fork();
	if (pid == 0)
	{
		data->atual_command = 0;
		close(fd_pipe[IN]);
		dup2(data->fd_infile, STDIN_FILENO);
		dup2(fd_pipe[OUT], STDOUT_FILENO);
		execute(data, data->command[0], envp);
		return (0);
	}
	else
	{
		waitpid(pid, NULL, 0);
		close(fd_pipe[1]);
		close(data->fd_infile);
		if (data->here_doc == true)
			unlink(data->doc_file);
		return (fd_pipe[IN]);
	}
}

void	fork_and_exec_middle_cmd(t_data *data, int fd_in, char **envp)
{
	int	fd_new_pipe[2];
	int	pid;

	data->atual_command++;
	pipe(fd_new_pipe);
	pid = fork();
	if (pid == 0)
	{
		close(fd_new_pipe[IN]);
		dup2(fd_in, STDIN_FILENO);
		dup2(fd_new_pipe[OUT], STDOUT_FILENO);
		execute(data, data->command[data->atual_command], envp);
	}
	else
	{
		waitpid(pid, NULL, 0);
		close(fd_in);
		close(fd_new_pipe[OUT]);
		if (data->atual_command == data->nbr_of_commands - 2)
			exec_last_command(data, fd_new_pipe[0], envp);
		fork_and_exec_middle_cmd(data, fd_new_pipe[0], envp);
	}
}

void	exec_last_command(t_data *data, int fd_in, char **envp)
{
	dup2(fd_in, STDIN_FILENO);
	dup2(data->fd_outfile, STDOUT_FILENO);
	execute(data, data->command[data->nbr_of_commands - 1], envp);
	exit (127);
}
