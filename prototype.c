#include <unistd.h>
#include "../../prototype/libraries/Libft/libft.h"
# include <unistd.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <string.h>

# define GREY 				"\033[0;90m"
# define RED				"\033[0;31m"
# define RESET 				"\033[0m"

# define PLACE_HOLDER 		16
# define FILE_HEREDOC		"this-file-name-should-simply-not-exist"
# define FILE_HEREDOC_EXTRA	"ok-lets-try-another-name"	

# define IN 				0
# define OUT	 			1

typedef enum e_bool
{
	false,
	true
}	t_bool;

typedef struct s_data
{
	int		fd_infile;
	int		fd_outfile;
	int 	atual_command;
	int		nbr_of_commands;
	t_bool	here_doc;
	char	*doc_file;
	char 	***command;
	char	**paths;
	char	*limiter;
}	t_data;

void	initialize(t_data *data, int argc, char **argv, char **envp);
void	init_files(t_data *data, char *infile, char *outfile);
void	find_paths_to_command(t_data *data, char **envp);
void	init_commands(t_data *data, char **argv);
char	**split_command(char *command);
void	execute_command(t_data *data, char **cmd_and_flags, char **envp);
int		fork_and_exec_first_cmd(t_data *data, char **envp);
void	fork_and_exec_middle_cmd(t_data *data, int fd_in, char **envp);
void	exec_last_command(t_data *data, int fd_in, char **envp);
void	command_not_found_message(char *command);
void	error(char *message);

void		read_here_doc(t_data *data, char *limiter, char **envp);

int change_spaces_to_place_holder(char **comand, int start);
void change_ocurrences(char **str, char old_c, char new_c);

void treat_spaces_inside_the_command(char **command);
int change_ocurrences_until_limiter(char **str, char old_c, char new_c, char limiter);

void init_here_doc_infile(t_data *data, char *outfile);

int main(int argc, char **argv, char **envp)
{
	t_data *data;
	int		fd_pipe_input;

	data = malloc(sizeof(t_data));
	initialize(data, argc, argv, envp);
	fd_pipe_input = fork_and_exec_first_cmd(data, envp);
	if (data->nbr_of_commands == 2)
			exec_last_command(data, fd_pipe_input, envp);
	fork_and_exec_middle_cmd(data, fd_pipe_input, envp);
	return (0);
}

void initialize(t_data *data, int argc, char **argv, char **envp)
{
	if (argc < 5)
		error("Some arguments are missing");
	if (!ft_strncmp(argv[1], "here_doc", ft_strlen("here_doc")))
	{
		data->here_doc = true;
		init_here_doc_infile(data, argv[argc - 1]);
		read_here_doc(data, argv[2], envp);
		data->nbr_of_commands = argc - 4;
	}
	else
		data->nbr_of_commands = argc - 3;
	init_files(data, argv[1], argv[argc - 1]);
	find_paths_to_command(data, envp);
	init_commands(data, argv);
}

void	error(char *message)
{
	ft_printf(RED"Error\n%s\n"RESET, message);
	exit (EXIT_FAILURE);
}

void init_files(t_data *data, char *infile, char *outfile)
{
	if (data->here_doc == true)
	{
		data->fd_infile = open(data->doc_file, O_RDONLY);
		if (data->fd_outfile == -1)
			error("infile: something unexpected happened");
		data->fd_outfile = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0777);
		if (data->fd_outfile == -1)
			error("outfile: something unexpected happened");
	}
	else
	{
		data->fd_infile = open(infile, O_RDONLY);
		if (data->fd_infile == -1)
			ft_printf(GREY"pipex: %s: No such file or directory", infile);
		data->fd_outfile = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if (data->fd_outfile == -1)
			error("outfile: something unexpected happened");
	}
}

void find_paths_to_command(t_data *data, char **envp)
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
	if (data->here_doc == true)
		arg = 3;
	else
		arg = 2;
	data->command = malloc(sizeof(char ***) * (data->nbr_of_commands + 1));
	while (i < data->nbr_of_commands)
	{
		data->command[i] = split_command(argv[arg]);
		if(data->command[i][0][0] != '/')
			data->command[i][0] = ft_strjoin("/", data->command[i][0]);
		i++;
		arg++;
	}
}

void	execute_command(t_data *data, char **cmd_and_flags, char **envp)
{
	char	*command;
	int 	i;

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
			exec_last_command(data, fd_new_pipe[0], envp);
		fork_and_exec_middle_cmd(data, fd_new_pipe[0], envp);
	}
}

void	exec_last_command(t_data *data, int fd_in, char **envp)
{
	dup2(fd_in, STDIN_FILENO);
	dup2(data->fd_outfile, STDOUT_FILENO);
	execute_command(data, data->command[data->nbr_of_commands - 1], envp);
	exit (127);
}

void command_not_found_message(char *command)
{
	dup2(STDERR_FILENO, STDOUT_FILENO);
	ft_printf(GREY"pipex: %s : command not found\n"RESET, command);
	exit (0);
}

char **split_command(char *command)
{
	char 	**array_command;
	char 	*full_command;
	int		i;

	i = 0;
	if (ft_strnstr(command, "awk", ft_strlen("awk")) 
	|| ft_strnstr(command, "sed", ft_strlen("sed")))
	{
		full_command = ft_strdup(command);
		treat_spaces_inside_the_command(&full_command);
	}
	else
		return (ft_split(command, ' '));
	array_command = ft_split(full_command, ' ');
	while (array_command[i])
	{
		change_ocurrences(&array_command[i], PLACE_HOLDER, ' ');
		array_command[i] = ft_strtrim(array_command[i], "\'");
		i++;
	}
	return (array_command);
}

void treat_spaces_inside_the_command(char **command)
{
	int i;

	i = 0;
	while (command[0][i])
	{
		if (command[0][i] == '\'')
		{
			i++;
			while (command[0][i] != '\'')
			{
				if (command[0][i] == ' ')
					command[0][i] = PLACE_HOLDER;
				i++;
			}
		}
		if (command[0][i] == '{')
		{
			while (command[0][i] != '}')
			{
				if (command[0][i] == ' ')
					command[0][i] = PLACE_HOLDER;
				i++;
			}
		}
		i++;
	}

}

void change_ocurrences(char **str, char old_c, char new_c)
{
	int i;

	i = 0;
	while (str[0][i])
	{
		if (str[0][i] == old_c)
			str[0][i] = new_c;
		i++;
	}
}

void	read_here_doc(t_data *data, char *limiter, char **envp)
{
	char	*buf;
	int		rd;

	buf = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	dup2(data->fd_infile, STDOUT_FILENO);
	rd = 1;
	while (rd > 0)
	{
		rd = read(STDIN_FILENO, buf, BUFFER_SIZE);
		buf[rd] = '\0';
		if (ft_strnstr(buf, limiter, ft_strlen(limiter)))
			break;
		ft_printf("%s",buf);
	}
	free(buf);
	close(data->fd_infile);
}

void init_here_doc_infile(t_data *data, char *outfile)
{
	if (ft_strncmp(outfile, FILE_HEREDOC, ft_strlen(outfile)))
	{
		data->fd_infile = \
		open(FILE_HEREDOC, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		data->doc_file = ft_strdup(FILE_HEREDOC);
	}
	else
	{
		data->fd_infile = \
		open(FILE_HEREDOC_EXTRA, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		data->doc_file = ft_strdup(FILE_HEREDOC_EXTRA);
	}
}
