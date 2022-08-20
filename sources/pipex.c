/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/20 02:31:14 by acesar-l          #+#    #+#             */
/*   Updated: 2022/08/20 08:52:02 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

int	fork_and_exec_first_cmd(t_data *data, char **envp);
void	fork_and_exec_cmd(t_data *data, int fd_in, char **envp);
void	conditions_to_redirect_io(t_data *data, int fd_in, int fd_out);
void	execute(t_data *data, char **cmd_and_flags, char **envp);

int	main(int argc, char **argv, char **envp)
{
	t_data	*data;
	int	fd_pipe_input;

	data = malloc(sizeof(t_data));
	initialize(data, argc, argv, envp);
	fd_pipe_input = fork_and_exec_first_cmd(data, envp);
	fork_and_exec_cmd(data, fd_pipe_input, envp);
	close(data->fd_infile);
	close(data->fd_outfile);
	if (data->not_used_cmds == true || data->invalid_infile == true)
		unlink(TEMP_FILE);
	free_data_and_exit(data, 0);
	return (0);
}

int	fork_and_exec_first_cmd(t_data *data, char **envp)
{
	int	fd_pipe[2];
	int	pid;

	pipe(fd_pipe);
	pid = fork();
	if (pid == 0)
	{
		close(fd_pipe[IN]);
		if (data->null_cmd == true)
			free_data_and_exit(data, 1);
		if (data->invalid_infile == true)
			redirect_input_and_output(data->fd_infile, data->fd_temp_file);
		else
			redirect_input_and_output(data->fd_infile, fd_pipe[OUT]);
		execute(data, data->command[data->actual_command], envp);
		return (0);
	}
	else
	{
		waitpid(pid, NULL, 0);
		close(fd_pipe[1]);
		if (data->null_cmd == false)
			data->actual_command++;
		return (fd_pipe[IN]);
	}
}

void	fork_and_exec_cmd(t_data *data, int fd_in, char **envp)
{
	int	fd_new_pipe[2];
	int	pid;

	pipe(fd_new_pipe);
	pid = fork();
	if (pid == 0)
	{
		close(fd_new_pipe[IN]);
		conditions_to_redirect_io(data, fd_in, fd_new_pipe[OUT]);
		execute(data, data->command[data->actual_command], envp);
		error(data, "", 127);
	}
	else
	{
		waitpid(pid, NULL, 0);
		close(fd_in);
		close(fd_new_pipe[OUT]);
		data->actual_command++;
		if (data->actual_command == data->nbr_of_commands)
			return ;
		fork_and_exec_cmd(data, fd_new_pipe[IN], envp);
	}
}

void	conditions_to_redirect_io(t_data *data, int fd_in, int fd_out)
{
	if (data->invalid_infile == true && data->actual_command == 0)
	{
		if (data->actual_command == data->nbr_of_commands - 1)
			redirect_input_and_output(data->fd_infile, data->fd_outfile);
		else
			redirect_input_and_output(data->fd_infile, fd_out);
	}
	else
	{
		if (data->actual_command == data->nbr_of_commands - 1)
			redirect_input_and_output(fd_in, data->fd_outfile);
		else
			redirect_input_and_output(fd_in, fd_out);
	}
}

void	execute(t_data *data, char **cmd_and_flags, char **envp)
{
	char	*command;
	int	i;

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
		free(command);
	}
	command_not_found_message(&cmd_and_flags[0][1]);
	if (data->actual_command != data->nbr_of_commands - 1)
		free_data_and_exit(data, 0);
}
