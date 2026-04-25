/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 18:10:19 by kong              #+#    #+#             */
/*   Updated: 2026/04/22 20:06:35 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

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
	// ! todo: add in zmin and zmax

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
			temp = ft_realloc_lst(src_lst, row_limit);
			if (!temp)
				return (freelst(src_lst), free(map), NULL);
			src_lst = temp;
		}
		new_row = get_next_line(fd);
		if (!new_row)
			break ;
		src_lst[map->n_row] = parse_coord_arr(new_row, map);
		if (!src_lst[map->n_row])
			return (free(new_row), freelst(src_lst), free(map), NULL);
		map->n_row++;
		free(new_row);
	}
	// validate
	if (map->n_row == 0 || map->n_col == 0)
		return (print_errmsg("FdF: Invalid map!"), freelst(src_lst), free(map), NULL);
	map->src_lst = src_lst;
	return (map);
}
