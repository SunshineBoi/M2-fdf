/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 18:10:19 by kong              #+#    #+#             */
/*   Updated: 2026/04/30 21:24:52 by kong             ###   ########.fr       */
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

static void	_update_map_z(t_map *map, t_coord coord)
{
	if (coord.z > map->z_max)
		map->z_max = coord.z;
	else if (coord.z < map->z_min)
		map->z_min = coord.z;
}

/**
 * @brief Parses a coordinate token into dest, extracting an integer height and optional hex color from token.
 * If no color is provided, and Z = 0, default color to 0xFFFFFF;
 * else if Z > 0, default color to purple.
 * 
 * updates the map’s z bounds via _update_map_z, and returns 1 on success or 0 on invalid input.
 * 
 * @param dest  : the pointer to be filled
 * @param token : each coordinate in string
 * @param map   : t_map object
 * 
 * @returns
 * 1/0 indicates success or failure.
 *
 */
static int	_parse_coord_node(t_coord *dest, char *token, t_map *map)
{
	t_coord	new_coord;
	char	*color_ptr;
	char	*nbr_ptr;
	int		color;

	color_ptr = ft_strchr(token, ',');
	if (!color_ptr)
	{
		if (!is_valid_int(token))
			return (0);
		new_coord.z = ft_atoi(token);
		if (new_coord.z == 0)
			new_coord.color = 0xFFFFFF;
		else
			new_coord.color = -1;  // to be processed later.
	}
	else
	{
		if (!is_valid_color(color_ptr + 1))
			return (0);
		color = ft_color_hexatoi(color_ptr + 1);
		new_coord.color = color;
	
		nbr_ptr = ft_strndup(token, color_ptr - token);
		if (!is_valid_int(nbr_ptr))
			return (free(nbr_ptr), 0);
		new_coord.z = ft_atoi(nbr_ptr);
		free(nbr_ptr);
	}
	*dest = new_coord;
	_update_map_z(map, new_coord);
	return (1);
}

/**
 * @brief Parses a space-delimited line str into a dynamically allocated array of t_coord obj.
 * 
 * Given each word, verify each element is valid int (incl overflow), valid color code (if provided) and equal number of cols for each row.
 * 
 * Validating column count against map->n_col (updating it on first call), and returns the array or NULL on any allocation or parse error while freeing intermediate resources.
 * 
 * @param str : raw string of coordinates in a row.
 * @param map : t_map object
 * 
 * @returns
 * On success, pointer to coordinates array.
 * On failure, prints error and returns NULL.
 *
 */
t_coord	*parse_coord_arr(char *str, t_map* map)
{
	char 	**tokens_lst;
	t_coord	*coord_arr;
	int		i = 0;
	int		list_len;

	// here already guarding empty "" input
	tokens_lst = ft_split_by_delim(str, ' ');
	if (!tokens_lst)
		return (NULL);
	list_len = ft_count_list(tokens_lst);
	if (map->n_col == 0)
		map->n_col = list_len;
	if (map->n_col != 0 && map->n_col != list_len)
		return (print_errmsg("FdF: Invalid map!"), freelst((void **)tokens_lst), NULL);

	coord_arr = malloc((map->n_col) * sizeof(t_coord));
	if (!coord_arr)
		return (perror("FdF"), freelst((void **)tokens_lst), NULL);
	while (tokens_lst[i])
	{
		// each i element is a number string
		// because this is not returning a pointer, it is harder to detect error
			// instead, we pass the coord_arr[i] position for it to fill up,
			// it returns 0/1 to indicate success or failure
		if (!_parse_coord_node(&(coord_arr[i]), tokens_lst[i], map))
			return (print_errmsg("FdF: Invalid map!"), free(coord_arr),
				freelst((void **)tokens_lst), NULL);
		i++;
	}
	freelst((void **)tokens_lst);
	return (coord_arr);
}

/*
1  -2  3  4  5,0xff
6  7  8  9  10,0xFFFFFF
11 12 13 14 15,0x870f0f
*/
t_map	*build_map(int fd)
{
	char 	*new_row;
	t_coord	**src_lst;
	t_coord	**temp;
	t_map	*map;
	int		row_limit;
	char	*has_nextline;

	map = init_map();
	if (!map)
		return (NULL);
	row_limit = 10;  // just a starter number
	src_lst = (t_coord **)ft_calloc_lst(row_limit);
	if (!src_lst)
		return (free(map), NULL);
	while (1)
	{
		// resizing list to allow more rows
		if (map->n_row >= row_limit - 1)
		{
			row_limit *= 2;
			temp = (t_coord **)ft_realloc_lst((void **)src_lst, row_limit);
			if (!temp)
				return (freelst((void **)src_lst), free(map), NULL);
			src_lst = temp;
		}
		new_row = get_next_line(fd);
		if (!new_row)
			break ;
		has_nextline = ft_strchr(new_row, '\n');
		if (has_nextline)
			*has_nextline = '\0';
		src_lst[map->n_row] = parse_coord_arr(new_row, map);
		if (!src_lst[map->n_row])
			return (free(new_row), freelst((void **)src_lst), free(map), NULL);
		map->n_row++;
		free(new_row);
	}
	// validate
	if (map->n_row == 0 || map->n_col == 0)
		return (print_errmsg("FdF: Invalid map!"), freelst((void **)src_lst), free(map), NULL);
	map->src_lst = src_lst;
	return (map);
}
