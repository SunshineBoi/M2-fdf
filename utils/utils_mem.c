/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_mem.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 13:03:45 by kong              #+#    #+#             */
/*   Updated: 2026/04/11 19:11:52 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t	i;
	unsigned char	*dest_p;
	const unsigned char	*src_p;

	dest_p = (unsigned char *)dest;
	src_p = (const unsigned char *)src;
	while (n--)
	{
		*dest_p++ = *src_p++;
	}
	return (dest);
}

void	**ft_calloc_lst(size_t size)
{
	void	**new;
	size_t	i;

	new = malloc((size + 1) * sizeof(void *));
	if (!new)
		return (NULL);
	i = 0;
	if (i <= (size))
	{
		new[i] = NULL;
		i++;
	}
	return (new);
}

/*
- working with List (pointer to pointer of struct):
	- because it only stores pointer, we could generalize it with (void *), 
		with size = 8-bytes.
	- and use NULL as stopper. Because it is only used to store mem address.
- working with Array (pointer to struct):
	- first know that we cannot use NULL as stopper because it can be
		valid data to represent part of the struct, hence,
		we have to treat struct as one block.
	- i.e. if we intend to copy structs array, we have to know beforehand
		the entire size of struct array.
	- (TYPELESS / GENERIC APPROACH)
		- Total bytes = size of struct * no. of struct obj in the array
		- then, we could define an `unsigned char *new_arr` and while loop
		through the total byte sizes to do a byte-by-byte copy,
		and we also making sure that we are copying the entire block of struct.
	- (STRONGLY-TYPE APPROACH)
		- on the other hand, we define the exact struct type that the function is 
		going to work with.
		- In this case, when we increment the pointer, compiler knows exactly
		how much bytes to skip for this struct type.
		- so, we only need to while loop through the number of elements 
		in this array only.
	
- the key distinction is: are we allocating memory to 
	- deal with unknown size of struct? just like a `char`, `int`
	- or fix size of `void *` type to only stores -- pointer address?
- side note, we need to do a strndup from a reference string, if it is meant
	to have multiple copies of it with each individual changes.
	We need individual copies for different sets of operation.
*/
void	**ft_realloc_lst(void **lst, size_t new_size)
{
	void	**new;
	int		i;

	if (new_size <= 0)
		return (NULL);
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
