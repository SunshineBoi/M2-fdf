/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_parse.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 22:37:00 by kong              #+#    #+#             */
/*   Updated: 2026/04/20 19:23:07 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

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
	// ! to check what is the minimum number of input
	if (word_count == 0)
		return (print_errmsg("FdF: Invalid map!"), NULL);
	lst = ft_calloc_lst(word_count);
	if (!lst)
		return (NULL);
	lst_i = 0;
	while (*str)
	{
		size = _split_and_insert_str(&(lst[lst_i]), str, delim);
		if (size == -1)
		{
			freelst(lst);
			return (NULL);
		}
		if (lst[lst_i])
			lst_i++;
		str += size;
	}
	return (lst);
}
