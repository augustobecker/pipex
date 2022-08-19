#include <unistd.h>
#include "../github/includes/libft.h"
# include <unistd.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <string.h>
# include "types.h"
# include "defines.h"

void	initialize(t_data *data, int argc, char **argv, char **envp);
void	init_data(t_data *data, int argc);
t_bool	is_here_doc_present(t_data *data, int argc, char **argv);
void	check_for_null_argv(t_data *data, int argc, char **argv);
void	init_here_doc(t_data *data, char *limiter, char *outfile, char **envp);
void	init_files(t_data *data, char *infile, char *outfile);
void	find_paths_to_command(t_data *data, char **envp);
void	init_commands(t_data *data, int argc, char **argv);
char	**split_command(char *command);
void	count_not_used_cmds(t_data *data, int argc, char **argv, char **envp);
void	execute_not_used_cmds(t_data *data, int command, char **envp);
void	execute(t_data *data, char **cmd_and_flags, char **envp);
int		fork_and_exec_first_cmd(t_data *data, char **envp);
void	fork_and_exec_cmd(t_data *data, int fd_in, char **envp);
void	exec_last_command(t_data *data, int fd_in, char **envp);
void	command_not_found_message(char *command);
void	error(t_data *data, char *message, int exit_code);
int		change_spaces_to_place_holder(char **comand, int start);
void 	change_ocurrences(char **str, char old_c, char new_c);
void 	treat_spaces_inside_the_command(char **command);
int 	change_ocurrences_until_limiter(char **str, char old_c, char new_c, char limiter);
int 	change_ocurrences_til_limiter(char **str, char old_c, char new_c, char limiter, int start);

char	*ft_strappend(const char *s1, char **s2);
char	*ft_substr_n_free(char **s, unsigned int start, size_t len);
char	*ft_strtrim_n_free(char **s1, char const *set);

// Colocar na Libft
void	free_str_array(char **array);
void	free_commands(t_data *data);

// Checar permissões logo de cara

void free_data_and_exit(t_data *data, int exit_code)
{
	if (data->cmds_alloc == true)
	{
		free_commands(data);
		free_str_array(data->paths);
	}
	free(data);
	exit (exit_code);
}

int main(int argc, char **argv, char **envp)
{
	t_data *data;
	int		fd_pipe_input;

	data = malloc(sizeof(t_data));
	initialize(data, argc, argv, envp);
	fd_pipe_input = fork_and_exec_first_cmd(data, envp);
	fork_and_exec_cmd(data, fd_pipe_input, envp);
	free_data_and_exit(data, 1);
	return (0);
}

void initialize(t_data *data, int argc, char **argv, char **envp)
{
	int i;

	i = 0;
	if (argc < 5)
		error(data, "Some arguments are missing", 1);
	init_data(data, argc);
	data->here_doc.is_present = is_here_doc_present(data, argc, argv);
	check_for_null_argv(data, argc, argv);
	if ((data->here_doc.is_present == true) && \
	(data->here_doc.is_necessary == true))
		init_here_doc(data, argv[2], argv[argc - 1], envp);
	count_not_used_cmds(data, argc, argv, envp);
	init_files(data, argv[1], argv[argc - 1]);
	find_paths_to_command(data, envp);
	init_commands(data, argc, argv);
	data->cmds_alloc = true;
	if (data->not_used_cmds)
	{
		while (data->actual_command < data->not_used_cmds)
			execute_not_used_cmds(data, i, envp);
		unlink(TEMP_FILE);
	}
	if (argv[argc - 2][0] == 0)
		error(data, "pipex: : command not found", 127);
}

void init_data(t_data *data, int argc)
{
	data->actual_command = 0;
	data->nbr_of_commands = argc - 3;
	data->not_used_cmds = 0;
	data->cmds_alloc = false;
	data->null_cmd = false;
	data->here_doc.is_present = false;
	data->here_doc.is_necessary = false;
}

t_bool is_here_doc_present(t_data *data, int argc, char **argv)
{
	if (!ft_strncmp(argv[1], "here_doc", ft_strlen("here_doc")))
	{
		if (argc < 6)
			error(data, "Some arguments are missing", 1);
		data->nbr_of_commands = argc - 3;
		return (true);
	}
	else
		return (false);
}

void	check_for_null_argv(t_data *data, int argc, char **argv)
{	
	int i;

	i = 2;
	if (argv[argc - 1][0] == 0)
		error(data, "pipex: : No such file or directory", 1);
	if (data->here_doc.is_present == true)
		i++;
	while (i < (argc - 1))
	{
		if (argv[i][0] == 0)
		{
			ft_printf(GREY"pipex: : command not found\n"RESET);
			data->nbr_of_commands--;
			data->here_doc.is_necessary = false;
			data->fd_infile = -1;
			data->null_cmd = true;
		}
		i++;
	}
}

void init_here_doc(t_data *data, char *limiter, char *outfile, char **envp)
{
	char	*buf;
	int		rd;

	data->fd_infile = open(HEREDOC_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (data->fd_infile == -1)
		error(data, "pipex : couldn't read here_doc", 1);
	buf = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buf)
		error(data, "pipex : couldn't alloc here_doc", 1);
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

void count_not_used_cmds(t_data *data, int argc, char **argv, char **envp)
{
	int i;
	int argv_first_cmd;

	i = argc - 2;
	argv_first_cmd = 2;
	if (data->here_doc.is_present == true)
		argv_first_cmd = 3;
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

void init_files(t_data *data, char *infile, char *outfile)
{
	if (data->here_doc.is_present == true)
	{
		if (data->here_doc.is_necessary == true)
		{
			close(data->fd_infile);
			data->fd_infile = open(HEREDOC_FILE, O_RDONLY);
			if (data->fd_infile == -1)
				error(data, "infile: something unexpected happened", 1);
		}
		data->fd_outfile = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0777);
		if (data->fd_outfile == -1)
			error(data, "outfile: something unexpected happened", 1);
	}
	else
	{
		data->fd_outfile = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (data->fd_outfile == -1)
			error(data, "No such file or directory", 1);
		data->fd_infile = open(infile, O_RDONLY);
		if (data->fd_infile == -1)
			ft_printf(GREY"pipex: %s: \
No such file or directory\n"RESET, infile);
		if (data->not_used_cmds)
		{
			data->fd_temp_file = open(TEMP_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			data->fd_infile = open(TEMP_FILE, O_RDONLY | O_CREAT | O_TRUNC, 0644);
		}
	}
}

void find_paths_to_command(t_data *data, char **envp)
{
	int		i;

	i = 0;
	while (!ft_strnstr(envp[i], "PATH=", ft_strlen("PATH=")))
		i++;
	data->paths = ft_split(envp[i] + ft_strlen("PATH="), ':');
	data->paths[0] = ft_substr_n_free(&data->paths[0], ft_strlen("PATH="), ft_strlen(data->paths[0]));
}

void	init_commands(t_data *data, int argc, char **argv)
{
	int arg;
	int i;

	i = 0;
	arg = 2;
	if (data->here_doc.is_present == true)
		arg++;
	data->command = malloc(sizeof(char ***) * (data->nbr_of_commands + 1));
	if (!data->command)
		error(data, "pipex: Couldn't alloc the commands", 1);
	while (arg < argc - 1)
	{
		if (argv[arg][0] != 0)
		{
			data->command[i] = split_command(argv[arg]);
			if(data->command[i][0][0] != '/')
				data->command[i][0] = ft_strappend("/", &data->command[i][0]);
			i++;
		}
		arg++;
	}
}

char **split_command(char *command)
{
	char 	**array_command;
	char 	*full_command;
	int		i;

	i = 0;
	full_command = ft_strdup(command);
	treat_spaces_inside_the_command(&full_command);
	array_command = ft_split(full_command, ' ');
	free(full_command);
	while (array_command[i])
	{
		change_ocurrences(&array_command[i], PLACE_HOLDER, ' ');               
		array_command[i] = ft_strtrim_n_free(&array_command[i], "\'");
		array_command[i] = ft_strtrim_n_free(&array_command[i], "\"");
		i++;
	}
	return (array_command);
}

void execute_not_used_cmds(t_data *data, int command, char **envp)
{
	int	pid;

	pid = fork();
	if (pid == 0)
	{
		dup2(data->fd_infile, STDIN_FILENO);
		dup2(data->fd_temp_file, STDOUT_FILENO);
		execute(data, data->command[data->actual_command], envp);
	}
	else
	{
		waitpid(pid, NULL, 0);
		data->actual_command++;
	}
}

void	error(t_data *data, char *message, int exit_code)
{
	ft_printf(GREY"%s\n"RESET, message);
	free_data_and_exit(data, exit_code);
}

void	execute(t_data *data, char **cmd_and_flags, char **envp)
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
		free(command);
	}
	command_not_found_message(&cmd_and_flags[0][1]);
	if (data->actual_command != data->nbr_of_commands - 1)
		free_data_and_exit(data, 0);
}

int	fork_and_exec_first_cmd(t_data *data, char **envp)
{
	int		fd_pipe[2];
	int 	pid;

	pipe(fd_pipe);
	pid = fork();
	if (pid == 0)
	{
		close(fd_pipe[IN]);
		if (data->null_cmd == true)
			free_data_and_exit(data, 1);
		dup2(data->fd_infile, STDIN_FILENO);
		dup2(fd_pipe[OUT], STDOUT_FILENO);
		execute(data, data->command[data->actual_command], envp);
		return (0);
	}
	else
	{
		waitpid(pid, NULL, 0);
		close(fd_pipe[1]);
		if (data->fd_infile != -1)
			close(data->fd_infile);
		if (data->here_doc.is_present == true)
			unlink(HEREDOC_FILE);
		return (fd_pipe[IN]);
	}
}

void	fork_and_exec_cmd(t_data *data, int fd_in, char **envp)
{
	int fd_new_pipe[2];
	int pid;

	pipe(fd_new_pipe);
	pid = fork();
	if (pid == 0)
	{
		close(fd_new_pipe[IN]);
		dup2(fd_in, STDIN_FILENO);
		if (data->actual_command == data->nbr_of_commands - 1)
			dup2(data->fd_outfile, STDOUT_FILENO);
		else
			dup2(fd_new_pipe[OUT], STDOUT_FILENO);
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
		fork_and_exec_cmd(data, fd_new_pipe[0], envp);
	}
}

void command_not_found_message(char *command)
{
	dup2(STDERR_FILENO, STDOUT_FILENO);
	ft_printf(GREY"pipex: %s : command not found\n"RESET, command);
}

void treat_spaces_inside_the_command(char **command)
{
	int i;

	i = 0;
	while (command[0][i])
	{
		if (command[0][i] == '\'')
			i += change_ocurrences_til_limiter(command, ' ', PLACE_HOLDER, '\'', i + 1);
		if (command[0][i] == '\"')
			i += change_ocurrences_til_limiter(command, ' ', PLACE_HOLDER, '\"', i + 1);
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

int change_ocurrences_til_limiter(char **str, char old_c, char new_c, char limiter, int start)
{
	int i;

	i = start;
	while (str[0][i] && str[0][i] != limiter)
	{
		if (str[0][i] == old_c)
			str[0][i] = new_c;
		i++;
	}
	return (i - start);
}

void	free_commands(t_data *data)
{
	int	cmds;

	cmds = 0;
	while (cmds < data->nbr_of_commands)
	{
		free_str_array(data->command[cmds]);
		cmds++;
	}
	free(data->command);
}

void	free_str_array(char **array)
{
	int	string;

	string = 0;
	while (array[string])
		free(array[string++]);
	free(array);
}

char	*ft_strappend(const char *s1, char **s2)
{
	char	*str;

	if (!s1 || !s2[0])
		return (NULL);
	str = (char *)ft_calloc((ft_strlen(s1) + ft_strlen(s2[0])) + 1, sizeof(char));
	if (!str)
		return (NULL);
	ft_strlcpy(str, s1, ft_strlen(s1) + 1);
	ft_strlcat(str, s2[0], ft_strlen(s1) + ft_strlen(s2[0]) + 1);
	free(s2[0]);
	return (str);
}

char	*ft_substr_n_free(char **s, unsigned int start, size_t len)
{
	size_t	substr_length;
	char	*substr;

	if (!*s)
		return (NULL);
	substr_length = ft_strlen(*s) - start;
	if (ft_strlen(*s) > start)
	{
		if (substr_length > len)
			substr = (char *) ft_calloc(len + 1, sizeof(char));
		else
			substr = (char *) ft_calloc(substr_length + 1, sizeof(char));
		if (!substr)
			return (NULL);
		ft_strlcpy(substr, &s[0][start], len + 1);
	}
	else
		substr = (char *) ft_calloc(1, sizeof(char));
	free(*s);
	return (substr);
}

static int	ft_isunwanted(char c, char const *unwanted)
{
	int	i;

	i = 0;
	while (unwanted[i])
	{
		if (unwanted[i] == c)
			return (1);
		i++;
	}
	return (0);
}

char	*ft_strtrim_n_free(char **s1, char const *set)
{
	size_t	start;
	size_t	final;
	char	*str;

	if (!*s1 || !set)
		return (NULL);
	start = 0;
	final = ft_strlen(*s1) - 1;
	while (ft_isunwanted(s1[0][start], set) && s1[0][start])
		start++;
	while (ft_isunwanted(s1[0][final], set) && final > 0)
		final--;
	if (final < start)
	{
		str = ft_calloc(1, 1);
		return (str);
	}
	str = ft_substr_n_free(s1, start, final - start + 1);
	if (!str)
		return (NULL);
	return (str);
}
