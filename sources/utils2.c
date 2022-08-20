/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesar-l <acesar-l@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/20 09:04:00 by acesar-l          #+#    #+#             */
/*   Updated: 2022/08/20 09:06:11 by acesar-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

char	*ft_strappend(const char *s1, char **s2);
char	*ft_substr_and_free(char **s, unsigned int start, size_t len);
char	*ft_strtrim_and_free(char **s1, char const *set);

char	*ft_strappend(const char *s1, char **s2)
{
	char	*str;

	if (!s1 || !s2[0])
		return (NULL);
	str = \
	(char *)ft_calloc((ft_strlen(s1) + ft_strlen(s2[0])) + 1, sizeof(char));
	if (!str)
		return (NULL);
	ft_strlcpy(str, s1, ft_strlen(s1) + 1);
	ft_strlcat(str, s2[0], ft_strlen(s1) + ft_strlen(s2[0]) + 1);
	free(s2[0]);
	return (str);
}

char	*ft_substr_and_free(char **s, unsigned int start, size_t len)
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

char	*ft_strtrim_and_free(char **s1, char const *set)
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
	str = ft_substr_and_free(s1, start, final - start + 1);
	if (!str)
		return (NULL);
	return (str);
}
