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

typedef struct s_pipex
{
	int		fd_pipe[2];
	int		fd_infile;
	int		fd_outfile;
	char 	**first_command;
	char 	**secnd_command;
	char	**paths;
}	t_pipex;

void	ft_error_msg(char *message);
void	ft_check_command_line_arguments(int argc);
void	ft_init_files(t_pipex *pipex, char *infile, char *outfile);
void	ft_find_paths(t_pipex *pipex, char **envp);
void	ft_init_commands(t_pipex *pipex, char *first_cmd, char *secnd_cmd);
void	execute_command(t_pipex *pipex, char **cmd_and_flags, char **envp);
void	child_process(t_pipex *pipex, char **envp);

int main(int argc, char **argv, char **envp)
{
	t_pipex	*pipex;

	pipex = malloc(sizeof(t_pipex));
	ft_check_command_line_arguments(argc);
	ft_init_files(pipex, argv[1], argv[4]);
	ft_find_paths(pipex, envp);
	ft_init_commands(pipex, argv[2], argv[3]);
	pipe(pipex->fd_pipe);
	child_process(pipex, envp);
	return (0);
}

void	ft_error_msg(char *message)
{
	ft_printf(RED"Error\n%s\n"RESET, message);
	exit (EXIT_FAILURE);
}

void ft_check_command_line_arguments(int argc)
{
	if (argc < 5)
		ft_error_msg("Some arguments are missing");
}

void ft_init_files(t_pipex *pipex, char *infile, char *outfile)
{
	pipex->fd_infile = open(infile, O_RDONLY);
	if (pipex->fd_infile == -1)
		ft_printf(GREY"pipex: %s: No such file or directory", infile);
	pipex->fd_outfile = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (pipex->fd_outfile == -1)
		ft_error_msg("outfile: something unexpected happened");
}

void ft_find_paths(t_pipex *pipex, char **envp)
{
	int		i;

	i = 0;
	while (!ft_strnstr(envp[i], "PATH=", ft_strlen("PATH=")))
		i++;
	pipex->paths = ft_split(envp[i] + ft_strlen("PATH="), ':');
	pipex->paths[0] = ft_substr(pipex->paths[0], ft_strlen("PATH="), ft_strlen(pipex->paths[0]));
}

void	ft_init_commands(t_pipex *pipex, char *first_cmd, char *secnd_cmd)
{
	first_cmd = ft_strjoin("/", first_cmd);
	secnd_cmd = ft_strjoin("/", secnd_cmd);
	pipex->first_command = ft_split(first_cmd, ' '); // tratamento para espaços
	pipex->secnd_command = ft_split(secnd_cmd, ' '); // tratamento para espaços
}

void execute_command(t_pipex *pipex, char **cmd_and_flags, char **envp)
{
	char	*command;
	int 	i;

	i = 0;
	while (pipex->paths[i])
	{
		command = ft_strjoin(pipex->paths[i++], cmd_and_flags[0]);
		execve(command, &cmd_and_flags[0], envp);
	}
}

void child_process(t_pipex *pipex, char **envp)
{
	int 	pid;

	pid = fork();
	if (pid == 0)
	{
		close(pipex->fd_pipe[IN]);
		dup2(pipex->fd_infile, 0);
		dup2(pipex->fd_pipe[1], 1);
		execute_command(pipex, pipex->first_command, envp);
	}
	else
	{
		waitpid(pid, NULL, 0);
		close(pipex->fd_pipe[1]);
		dup2(pipex->fd_pipe[0], 0);
		dup2(pipex->fd_outfile, 1);
		execute_command(pipex, pipex->secnd_command, envp);
		exit (127);
	}
}
