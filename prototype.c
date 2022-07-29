#include <unistd.h>
#include "../../prototype/libraries/Libft/libft.h"
# include <unistd.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <string.h>

# define CYAN				"\033[0;96m"
# define GREY 				"\033[0;90m"
# define GREEN				"\033[0;32m"
# define RED				"\033[0;31m"
# define RESET 				"\033[0m"

# define IN 				0
# define OUT	 			1


typedef struct s_data
{
	int		fd_pipe[2];
	int		fd_next_pipe[2];
	int		fd_infile;
	int		fd_outfile;
	int 		atual_command;
	int		nbr_of_commands;
	char 		***command;
	char		**paths;
}	t_data;

void	error_msg(char *message);
void	check_command_line_arguments(int argc);
void	init_files(t_data *data, char *infile, char *outfile);
void	find_paths(t_data *data, char **envp);
void	init_commands(t_data *data, char **argv);
void	execute_command(t_data *data, char **cmd_and_flags, char **envp);

void	run_first_process(t_data *data, char **envp);
void	execute_last_command(t_data *data, int fd_in, char **envp);
void	run_pipes(t_data *data, int fd_in, char **envp);

int main(int argc, char **argv, char **envp)
{
	t_data *data;

	data = malloc(sizeof(t_data));
	data->nbr_of_commands = argc - 3;
	check_command_line_arguments(argc);
	init_files(data, argv[1], argv[argc - 1]);
	find_paths(data, envp);
	init_commands(data, argv);
	run_first_process(data, envp);
	return (0);
}

void	error_msg(char *message)
{
	ft_printf(RED"Error\n%s\n"RESET, message);
	exit (EXIT_FAILURE);
}

void check_command_line_arguments(int argc)
{
	if (argc < 5)
		error_msg("Some arguments are missing");
}

void init_files(t_data *data, char *infile, char *outfile)
{
	data->fd_infile = open(infile, O_RDONLY);
	if (data->fd_infile == -1)
		ft_printf(GREY"pipex: %s: No such file or directory", infile);
	data->fd_outfile = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (data->fd_outfile == -1)
		error_msg("outfile: something unexpected happened");
}

void find_paths(t_data *data, char **envp)
{
	int		i;

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
		data->command[i] = ft_split(argv[arg], ' '); // falta o tratamento para espaços
		data->command[i][0] = ft_strjoin("/", data->command[i][0]);
		i++;
		arg++;
	}
}

void execute_command(t_data *data, char **cmd_and_flags, char **envp)
{
	char	*command;
	int 	i;

	i = 0;
	while (data->paths[i])
	{
		command = ft_strjoin(data->paths[i++], cmd_and_flags[0]);
		execve(command, &cmd_and_flags[0], envp);
	}
}

void	run_first_process(t_data *data, char **envp)
{
	int		fd_pipe[2];
	int 	pid;

	pipe(fd_pipe);
	pid = fork();
	if (pid == 0)
	{
		data->atual_command = 0;
		close(fd_pipe[IN]);
		dup2(data->fd_infile, STDIN_FILENO);
		dup2(fd_pipe[OUT], STDOUT_FILENO);
		execute_command(data, data->command[0], envp);
	}
	else
	{
		waitpid(pid, NULL, 0);
		close(fd_pipe[1]);
		if (data->nbr_of_commands == 2)
			execute_last_command(data, fd_pipe[0], envp);
		run_pipes(data, fd_pipe[0], envp);
	}
}

void	run_pipes(t_data *data, int fd_in, char **envp)
{
	int fd_new_pipe[2];
	int pid;

	data->atual_command++;
	pipe(fd_new_pipe);
	pid = fork();
	if (pid == 0)
	{
		close(fd_new_pipe[IN]);
		dup2(fd_in, STDIN_FILENO);
		dup2(fd_new_pipe[OUT], STDOUT_FILENO);
		execute_command(data, data->command[data->atual_command], envp);
	}
	else
	{
		waitpid(pid, NULL, 0);
		close(fd_in);
		close(fd_new_pipe[OUT]);
		if (data->atual_command == data->nbr_of_commands - 2)
			execute_last_command(data, fd_new_pipe[0], envp);
		run_pipes(data, fd_new_pipe[0], envp);
	}
}

void	execute_last_command(t_data *data, int fd_in, char **envp)
{
	dup2(fd_in, STDIN_FILENO);
	dup2(data->fd_outfile, STDOUT_FILENO);
	execute_command(data, data->command[data->nbr_of_commands - 1], envp);
	exit (127);
}
