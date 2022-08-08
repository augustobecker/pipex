/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/08 19:03:53 by acesar-l          #+#    #+#             */
/*   Updated: 2022/08/08 19:05:41 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_H
# define TYPES_H

typedef enum e_bool
{
	false,
	true
}	t_bool;

typedef struct s_here_doc
{
	char	*limiter;
	char	*file;
	t_bool	is_present;
	t_bool	is_necessary;
}	t_here_doc;

typedef struct s_data
{
	int     	fd_infile;
	int     	fd_outfile;
	int    		fd_temp_file;
	int     	actual_command;
	int    		nbr_of_commands;
	int     	not_used_cmds;
	t_bool		null_cmd;
	char		***command;
	char		**paths;
	t_here_doc	here_doc;
}	t_data;

#endif
