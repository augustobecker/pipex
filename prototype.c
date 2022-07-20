#include <stdio.h>
#include <unistd.h>
#include "../prototype/libraries/Libft/libft.h"
#include <sys/wait.h>
#include <fcntl.h>

# define CYAN				"\033[0;96m"
# define GREY 				"\033[0;90m"
# define GREEN				"\033[0;32m"
# define RED				"\033[0;31m"
# define RESET 				"\033[0m"

char *ft_read_pipe(int fd);
int	ft_error_msg(char *message);
char *ft_read_infile(char *filename);

int main(int argc, char **argv, char **envp)
{
	char 	**possible_path;
	char 	*paths;
	char 	*cmd;
	char	*cmd2;
	int 	i;
	int 	exec;
	int		pid;
	int		pid_2;
	int		fd_out;
	int		fd_out_dup;
	char	*outfile;
	int 	pipe_fd[2];
	char 	*flags;

	char **first_command;
	char **secnd_command;
	char *infile;
	int fd;
	char *file;

	if (argc < 5)
		ft_error_msg("some arguments are missing");
	else if (argc > 5)
		ft_error_msg("Too many arguments \n       It should be only five");
	file = ft_read_infile(argv[1]);
	first_command = ft_split(argv[2], ' ');
	secnd_command = ft_split(argv[3], ' ');
	i = 0;
	while (!ft_strnstr(envp[i], "PATH=", sizeof(envp[i])))
		i++;
	paths = ft_strdup(envp[i]);
	possible_path = ft_split(paths, ':');
	i = 0;
	possible_path[0] = ft_substr(possible_path[0], ft_strlen("PATH="), ft_strlen(possible_path[0]));
	while (possible_path[i])
	{
		possible_path[i] = ft_strjoin(possible_path[i], "/");
		i++;
	}
	pipe(pipe_fd);
	pid = fork();
	if (pid == 0)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], 1);
		dup2(fd, 0);
		i = 0;
		while (possible_path[i])
		{
			cmd = ft_strjoin(possible_path[i++], first_command[0]);
			execve(cmd, &first_command[0], envp);
		}
	}
	else
	{
		wait(NULL);
		ft_printf("DONE\n");
		close(pipe_fd[1]);
		fd_out = open(argv[4 ], O_WRONLY | O_CREAT, 0777);
		dup2(fd_out, 1);
		dup2(pipe_fd[0], 0);
		i = 0;
		while (possible_path[i])
		{
			cmd2 = ft_strjoin(possible_path[i++], secnd_command[0]);
			execve(cmd2, &secnd_command[0] , envp);
		}
		close(fd_out);
	}
}

char *ft_read_infile(char *filename)
{
	char	*file_temp;
	char	*line_temp;
	int		fd;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
		ft_error_msg("infile: No such file or directory");
	file_temp = ft_strdup("");
	while (1)
	{
		line_temp = get_next_line(fd);
		if (line_temp == NULL)
			break;
		file_temp = ft_strjoin(file_temp, line_temp);
		free(line_temp);
	}
	return (file_temp);
}


char *ft_read_pipe(int fd)
{
	char	*file_temp;
	char	*line_temp;

	file_temp = ft_strdup("");
	while (1)
	{
		line_temp = get_next_line(fd);
		if (line_temp == NULL)
			break;
		file_temp = ft_strjoin(file_temp, line_temp);
		free(line_temp);
	}
	return (file_temp);
}

int	ft_error_msg(char *message)
{
	ft_printf(GREY"pipex: %s\n"RESET, message);
	exit (EXIT_FAILURE);
}
