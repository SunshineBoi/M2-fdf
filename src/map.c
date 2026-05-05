/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 18:10:19 by kong              #+#    #+#             */
/*   Updated: 2026/05/05 18:50:46 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

t_map	*init_map(void)
{
	t_map	*new_map;

	new_map = malloc(sizeof(t_map));
	if (!new_map)
		return (perror("FdF"), NULL);
	new_map->n_col = 0;
	new_map->n_row = 0;
	new_map->z_min = INT_MAX;
	new_map->z_max = INT_MIN;
	new_map->src_lst = NULL;
	return (new_map);
}

static int	_add_coord_arr(t_map *map, t_coord **src_lst, int fd)
{
	char	*new_row;
	char	*has_nextline;

	new_row = get_next_line(fd);
	if (!new_row)
		return (free(new_row), 0);
	has_nextline = ft_strchr(new_row, '\n');
	if (has_nextline)
		*has_nextline = '\0';
	src_lst[map->n_row] = parse_coord_arr(new_row, map);
	if (!src_lst[map->n_row])
		return (get_next_line(-1), free(new_row), -1);
	map->n_row++;
	free(new_row);
	return (1);
}

static int	_build_map_helper(t_map *map, int fd, int row_limit)
{
	t_coord	**src_lst;
	t_coord	**temp;
	int		finished;

	src_lst = (t_coord **)ft_calloc_lst(row_limit);
	if (!src_lst)
		return (-1);
	while (1)
	{
		// resizing list to allow more rows
		if (map->n_row >= row_limit - 1)
		{
			row_limit *= 2;
			temp = (t_coord **)ft_realloc_lst((void **)src_lst, row_limit);
			if (!temp)
				return (freelst((void **)src_lst), -1);
			src_lst = temp;
		}
		finished = _add_coord_arr(map, src_lst, fd);
		if (finished == 0)
			break ;
		else if (finished == -1)
			return (freelst((void **)src_lst), -1);
	}
	map->src_lst = src_lst;
	return (1);
}

t_map	*build_map(int fd)
{
	t_map	*map;

	map = init_map();
	if (!map)
		return (NULL);
	if (_build_map_helper(map, fd, 10) == -1)
		return (free(map), NULL);
	// validate
	if (map->n_row == 0 || map->n_col == 0)
		return (perrmsg("FdF: Invalid map"), 
			freelst((void **)map->src_lst), free(map), NULL);
	return (map);
}

t_map	*read_map_file(char *file_path)
{
	int		fd;
	t_map	*new_map;

	fd = open(file_path, O_RDONLY);
	if (fd == -1)
		return (perror("FdF"), NULL);
	new_map = build_map(fd);
	close(fd);
	if (!new_map)
		return (NULL);
	return (new_map);
}
