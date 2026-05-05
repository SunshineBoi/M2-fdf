/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_mem.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 13:03:45 by kong              #+#    #+#             */
/*   Updated: 2026/05/05 14:54:24 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

void *ft_memset(void *s, int value, size_t nbyte)
{
	unsigned char	*s_ptr;

	s_ptr = (unsigned char *)s;
	while (nbyte--)
		*s_ptr++ = (unsigned char)value;
	return (s);
}

/**
 * @brief Allocates an array of void * pointers with size + 1 elements, initializes every element to NULL.
 * 
 * @param size: size of list, excluding extra space for null.
 * 
 * @returns
 * On success, The pointer to the new list;
 * On allocation failure it prints FdF via perror and returns NULL.
 *
 */
void	**ft_calloc_lst(size_t size)
{
	void	**new;
	size_t	i;

	new = malloc((size + 1) * sizeof(void *));
	if (!new)
		return (perror("FdF"), NULL);
	i = 0;
	while (i <= (size))
	{
		new[i] = NULL;
		i++;
	}
	return (new);
}

/**
 * @brief Allocates a new array of void * pointers with new_size + 1 elements initialized to NULL, copies up to new_size existing entries from lst until a NULL terminator, frees the old list, and returns the new array, or returns NULL on error. If lst is NULL, it simply allocates and returns a fresh initialized list.
 * 
 * @param lst: old list
 * @param new_size: new size of list
 * 
 * @returns
 * If size <= 0, returns NULL;
 * If lst is empty, returns newly initialized list;
 * On success, the pointer to the new list;
 * On allocation failure it prints FdF via perror and returns NULL.
 *
 */
void	**ft_realloc_lst(void **lst, size_t new_size)
{
	void	**new;
	size_t	i;

	if (new_size <= 0)
		return (perrmsg("FdF: realloc list fails!"), NULL);
	if (!lst)
		return (ft_calloc_lst(new_size));
	new = ft_calloc_lst(new_size);
	if (!new)
		return (NULL);
	i = 0;
	while (i < new_size && lst[i])
	{
		new[i] = lst[i];
		i++;
	}
	free(lst);
	return (new);
}
