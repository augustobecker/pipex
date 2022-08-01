#ifndef PIPEX_H
# define PIPEX_H

# include <unistd.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <errno.h>
# include <stdio.h>
# include "libft.h"

# define GREY 				      "\033[0;90m"
# define RED				        "\033[0;31m"
# define RESET 				      "\033[0m"

# define FILE_HEREDOC		    "/tmp/in"
# define FILE_HEREDOC_EXTRA "/temp/temp_here_doc"

# define IN 				        0
# define OUT	 			        1

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
void	treat_spaces_inside_the_command(char **command, char place_holder);
void	change_ocurrences(char **str, char old_c, char new_c);
void	execute(t_data *data, char **cmd_and_flags, char **envp);
void	command_not_found_message(char *command);
int		fork_and_exec_first_cmd(t_data *data, char **envp);
void	fork_and_exec_middle_cmd(t_data *data, int fd_in, char **envp);
void	exec_last_command(t_data *data, int fd_in, char **envp);
void	error(char *message);
char	define_a_not_used_place_holder(char *str);
void	init_heredoc_file(t_data *data, char *outfile);
void	read_heredoc(t_data *data, char *limiter, char **envp);

#endif
