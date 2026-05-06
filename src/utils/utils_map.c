/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 22:37:00 by kong              #+#    #+#             */
/*   Updated: 2026/05/05 17:01:18 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	j;
	size_t	i;

	if (*little == '\0')
		return ((char *)big);
	i = 0;
	while (i < len && big[i])
	{
		j = 0;
		while (i + j < len && big[i + j] && big[i + j] == little[j])
			j++;
		if (little[j] == '\0')
			return ((char *)big + i);
		i++;
	}
	return (NULL);
}

int ft_count_list(char **str)
{
	int	counts = 0;

	while (str[counts])
		counts++;
	return (counts);
}

int	count_word_by_delim(char *str, char ch)
{
	int	count;

	count = 0;
	while (*str)
	{
		while (*str && *str == ch)
			str++;
		if (*str)
		{
			while (*str && *str != ch)
				str++;
			count++;
		}
	}
	return (count);
}

static int	_split_and_insert_str(char **lst, char *str, char delim)
{
	int	word_len;
	int	delim_count;

	delim_count = 0;
	while (*str && *str == delim)
	{
		str++;
		delim_count++;
	}
	word_len = 0;
	if (*str)
	{
		while (str[word_len] && str[word_len] != delim)
			word_len++;
		*lst = ft_strndup(str, word_len);
		if (!*lst)
			return (-1);
	}
	return (delim_count + word_len);
}

char	**ft_split_by_delim(char *str, char delim)
{
	int		word_count;
	int		lst_i;
	int		size;
	char	**lst;

	word_count = count_word_by_delim(str, delim);
	if (word_count == 0)
		return (perrmsg("FdF: Invalid map!"), NULL);
	lst = (char **)ft_calloc_lst(word_count);
	if (!lst)
		return (NULL);
	lst_i = 0;
	while (*str)
	{
		size = _split_and_insert_str(&(lst[lst_i]), str, delim);
		if (size == -1)
		{
			freelst((void **)lst);
			return (NULL);
		}
		if (lst[lst_i])
			lst_i++;
		str += size;
	}
	return (lst);
}
