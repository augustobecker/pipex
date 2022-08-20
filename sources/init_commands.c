/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_commands.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/20 02:26:39 by acesar-l          #+#    #+#             */
/*   Updated: 2022/08/20 09:05:45 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

void	find_paths_to_command(t_data *data, char **envp);
void	init_commands(t_data *data, int argc, char **argv);
char	**split_command(char *command);

void	find_paths_to_command(t_data *data, char **envp)
{
	int	i;

	i = 0;
	while (!ft_strnstr(envp[i], "PATH=", ft_strlen("PATH=")))
		i++;
	data->paths = ft_split(envp[i] + ft_strlen("PATH="), ':');
	data->paths[0] = ft_substr_and_free(&data->paths[0], \
	ft_strlen("PATH="), ft_strlen(data->paths[0]));
}

void	init_commands(t_data *data, int argc, char **argv)
{
	int	arg;
	int	i;

	i = 0;
	arg = 2;
	data->command = malloc(sizeof(char ***) * (data->nbr_of_commands + 1));
	if (!data->command)
		error(data, "pipex: Couldn't alloc the commands", 1);
	while (arg < argc - 1)
	{
		if (argv[arg][0] != 0)
		{
			data->command[i] = split_command(argv[arg]);
			if (data->command[i][0][0] != '/')
				data->command[i][0] = ft_strappend("/", &data->command[i][0]);
			i++;
		}
		arg++;
	}
}

char	**split_command(char *command)
{
	char	**array_command;
	char	*full_command;
	int		i;

	i = 0;
	full_command = ft_strdup(command);
	treat_spaces_inside_the_command(&full_command);
	array_command = ft_split(full_command, ' ');
	free(full_command);
	while (array_command[i])
	{
		change_ocurrences(&array_command[i], PLACE_HOLDER, ' ');
		array_command[i] = ft_strtrim_and_free(&array_command[i], "\'");
		array_command[i] = ft_strtrim_and_free(&array_command[i], "\"");
		i++;
	}
	return (array_command);
}
