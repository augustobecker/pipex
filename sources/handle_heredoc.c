/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/01 16:37:25 by acesar-l          #+#    #+#             */
/*   Updated: 2022/08/01 16:38:58 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libraries/pipex.h"

void	init_heredoc_file(t_data *data, char *outfile);
void	read_heredoc(t_data *data, char *limiter, char **envp);

void	init_heredoc_file(t_data *data, char *outfile)
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

void	read_heredoc(t_data *data, char *limiter, char **envp)
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
			break ;
		ft_printf("%s", buf);
	}
	free(buf);
	close(data->fd_infile);
}
