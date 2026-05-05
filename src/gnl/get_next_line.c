/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 16:37:22 by kong              #+#    #+#             */
/*   Updated: 2026/05/05 18:53:38 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*ft_join_buffer(char *stash, char *buffer)
{
	char	*new;

	new = ft_strjoin(stash, buffer);
	if (!new)
	{
		free(buffer);
		return (NULL);
	}
	free(stash);
	return (new);
}

static char	*ft_write_stash(char *stash, int fd)
{
	ssize_t	bytes_read;
	char	*buffer;

	bytes_read = 1;
	buffer = malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (!buffer)
		return (NULL);
	while (!ft_hasnewline(stash) && bytes_read > 0)
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read < 0)
		{
			free(buffer);
			return (NULL);
		}
		if (bytes_read == 0)
			break ;
		buffer[bytes_read] = '\0';
		stash = ft_join_buffer(stash, buffer);
		if (!stash)
			return (NULL);
	}
	free(buffer);
	return (stash);
}

static char	*ft_get_one_line(char *stash)
{
	size_t	line_len;
	char	*next_line;
	char	*curr_line;

	if (!stash || !stash[0])
		return (NULL);
	next_line = ft_strchr(stash, '\n');
	if (next_line)
		line_len = next_line - stash + 1;
	else
		line_len = ft_strlen(stash);
	if (line_len == 0)
		return (NULL);
	curr_line = ft_strndup(stash, line_len);
	if (!curr_line)
		return (NULL);
	return (curr_line);
}

static char	*ft_update_stash(char *stash)
{
	char	*new_stash;
	char	*next_line_ptr;
	size_t	next_line_len;

	if (!stash)
		return (NULL);
	next_line_ptr = ft_strchr(stash, '\n');
	if (!next_line_ptr)
	{
		free(stash);
		return (NULL);
	}
	next_line_len = ft_strlen(next_line_ptr + 1);
	new_stash = ft_strndup(next_line_ptr + 1, next_line_len);
	free(stash);
	if (!new_stash || !new_stash[0])
	{
		free(new_stash);
		return (NULL);
	}
	return (new_stash);
}

char	*get_next_line(int fd)
{
	char		*line;
	static char	*stash;
	char		*new_stash;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (free(stash), NULL);
	new_stash = ft_write_stash(stash, fd);
	if (!new_stash)
	{
		free(stash);
		stash = NULL;
		return (NULL);
	}
	else
		stash = new_stash;
	line = ft_get_one_line(stash);
	if (!line && stash)
	{
		free(stash);
		stash = NULL;
		return (NULL);
	}
	stash = ft_update_stash(stash);
	return (line);
}

/**
Philosophy: If there is error, try to cascade to caller to free it.
1. stash started as NULL and it gets combined with buffer.
	Take note of helper functions, would it get error if input is NULL?
	How to handle initial status of stash. For example `strjoin` fn.

2. Create stash:
	If stash has leftover data from previous read, we returned it;
		If not, we start a new read;
	Loop until there is \n found, or no more byte is read:
	Case: If bytes read < 0:
		return Error
	Case: If bytes read == 0:
		break loop, return existing stash.
	Case: If bytes read > 0:
		Merge old stash with new buffer (strjoin).
			If not success, free stash is handled by main function,
			return NULL;
			Else, free old stash and return this new stash.

3. Get one line:
	By workflow, we shouldnt receive NULL as it will be handled when writing
	stash and gets free and returned already.
	Locate \n (strchr);
	Duplicate first section by getting mem diff between position of (\n + 1) and
	start of stash (strndup)
		No empty line is possible as it was handled in both write and update
		stash. No harm adding this line.
		If duplication fails, return NULL;
		Else, return duplicated line.

4. Update existing stash.
	If there isnt any data inside ald, we could free stash;
	Else, we locate the next line (strchr),
		If not found, free stash, return NULL,
		Else, create new mem for it (strdup), free old stash;
	Check new stash, 
		If is empty, i.e. no new lines after \n, free it;
		Else, return new stash.
**/
