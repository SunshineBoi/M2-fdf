/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 14:23:27 by kong              #+#    #+#             */
/*   Updated: 2026/04/13 16:46:43 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

void	_update_map_z(t_map *map, t_coord coord)
{
	if (coord->z_height > map->z_max)
		map->z_max = coord->z_height;
	else if (coord->z_height < map->z_min)
		map->z_min = coord->z_height;
}

int	_parse_coord_node(t_coord *dest, char *token, t_map *map)
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
		new_coord.z_height = ft_atoi(token);
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
		new_coord.z_height = ft_atoi(nbr_ptr);
		free(nbr_ptr);
	}
	*dest = new_coord;
	_update_map_z(map, new_coord);
	return (1);
}
/*
ft_split strictly on space
given words list, verify each element is valid int (incl overflow),
	and valid color code.
	and along the way validate cols
create t_coord for each element and store the values.
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
		return (print_error_msg("FdF: Invalid map!"), free_lst(tokens_lst), NULL);
	map->n_col = ft_count_list(tokens_lst);

	coord_arr = malloc((map->n_col) * sizeof(t_coord));
	if (!coord_arr)
		return (perror("FdF"), free_lst(tokens_lst), NULL);
	while (tokens_lst[i])
	{
		// each i element is a number string
		// because this is not returning a pointer, it is harder to detect error
			// instead, we pass the coord_arr[i] position for it to fill up,
			// it returns 0/1 to indicate success or failure
		if (!_parse_coord_node(&(coord_arr[i]), tokens_lst[i], map))
			return (print_error_msg("FdF: Invalid map!"), free(coord_arr),
				free_lst(tokens_lst), NULL);
		i++;
	}
	free_lst(tokens_lst);
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
	t_coord	**coords_lst;
	t_coord	**temp;
	t_map	*map;
	int		row_limit;
	// ! todo: add in zmin and zmax

	map = init_map();
	if (!map)
		return (perror("FdF"), NULL);
	row_limit = 10;
	coords_lst = ft_calloc_lst(row_limit);
	if (!coords_lst)
		return (free(map), NULL);
	while (1)
	{
		// resizing list to allow more rows
		if (map->n_row >= row_limit - 1)
		{
			row_limit *= 2;
			temp = ft_realloc_lst(coords_lst, row_limit);
			if (!temp)
				return (free_lst(coords_lst), free(map), NULL);
			coords_lst = temp;
		}
		new_row = get_next_line(fd);
		if (!new_row)
			break ;
		coords_lst[map->n_row] = parse_coord_arr(new_row, map);
		if (!coords_lst[map->n_row])
			return (free(new_row), free_lst(coords_lst), free(map), NULL);
		map->n_row++;
		free(new_row);
	}
	if (map->n_row == 0 || map->n_col == 0)
		return (print_error_msg("FdF: Invalid map!"), free_lst(coords_lst), free(map), NULL);
	map->coords_lst = coords_lst;
	return (map);
}

int	main(int ac, char **av)
{
	int	fd;
	t_prog	*prog;

	if (ac != 2)
		errexit("fdf: Error: Wrong argument counts", 1);

	prog = malloc(sizeof(t_prog));
	if (!prog)
		// ! error exit

	// todo: setup MLX
	prog->mlx_obj = setup_mlx();

	// read map
	fd = open_file_as_read(av[1]);
	if (fd == -1)
		return (perror("FdF"), free_prog(prog), EXIT_FAILURE);
	prog->map_obj = build_map(fd);
	if (!prog->map_obj)
		return (free_prog(prog), EXIT_FAILURE);

	// todo: setup viewpoint

	close(fd); // when should this be closed?
	return (0);
}



int main_test(void)
{
	void *mlx;
	void *mlx_winm;

	mlx = mlx_init();
	if (!mlx)
		return (EXIT_FAILURE);
	mlx_winm = mlx_new_window(mlx, 500, 500, "hello window!");
	mlx_pixel_put(mlx, mlx_winm, 250, 250, rand() % 0x1000000);
	mlx_loop(mlx);

	mlx_destroy_display(mlx); // because mlx_init malloc a display
	free(mlx);
	return (0);
}

/*
compile: cc src/main.c -Iinclude -Iminilibx-linux minilibx-linux/libmlx.a -lXext -lX11 -lm

testing:
- in so_long, there is possibilities where permission denied to images cause mem leaks
	could there be similar failure path.

*/