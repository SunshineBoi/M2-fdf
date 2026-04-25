/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 12:47:38 by kong              #+#    #+#             */
/*   Updated: 2026/04/23 22:28:25 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

t_map	*init_map(void)
{
	t_map	*map;

	map = malloc(sizeof(t_map));
	if (!map)
		return (perror("FdF"), NULL);
	map->n_col = 0;
	map->n_row = 0;
	map->z_min = INT_MAX;
	map->z_max = INT_MIN;
	map->src_lst = NULL;
	return (map);
}

static void	_update_map_z(t_map *map, t_coord coord)
{
	if (coord.z > map->z_max)
		map->z_max = coord.z;
	else if (coord.z < map->z_min)
		map->z_min = coord.z;
}

/**
 * @brief Parses a coordinate token into dest, extracting an integer height and optional hex color from token, defaults color to 0xFFFFFF if absent, updates the map’s z bounds via _update_map_z, and returns 1 on success or 0 on invalid input.
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
		new_coord.color = 0xFFFFFF;
		if (!is_valid_int(token))
			return (0);
		new_coord.z = ft_atoi(token);
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

	// here already guarding empty "" input
	tokens_lst = ft_split_by_delim(str, ' ');
	if (!tokens_lst)
		return (NULL);
	if (map->n_col != 0 && map->n_col != ft_count_list(tokens_lst))
		return (print_errmsg("FdF: Invalid map!"), freelst(tokens_lst), NULL);
	map->n_col = ft_count_list(tokens_lst);

	coord_arr = malloc((map->n_col) * sizeof(t_coord));
	if (!coord_arr)
		return (perror("FdF"), freelst(tokens_lst), NULL);
	while (tokens_lst[i])
	{
		// each i element is a number string
		// because this is not returning a pointer, it is harder to detect error
			// instead, we pass the coord_arr[i] position for it to fill up,
			// it returns 0/1 to indicate success or failure
		if (!_parse_coord_node(&(coord_arr[i]), tokens_lst[i], map))
			return (print_errmsg("FdF: Invalid map!"), free(coord_arr),
				freelst(tokens_lst), NULL);
		i++;
	}
	freelst(tokens_lst);
	return (coord_arr);
}
