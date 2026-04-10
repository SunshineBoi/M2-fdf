/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 14:23:27 by kong              #+#    #+#             */
/*   Updated: 2026/04/10 23:01:21 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

t_coord	*parse_coord_arr(char *str, int *col_ptr)
{
	// ft_split strictly on space
	// given words list, verify each element is valid int (incl overflow),
		// and valid color code.
		// and along the way validate cols
	// create t_coord for each element and store the values.
	char **tokens_lst;
	t_coord	*coord_arr;
	int	i = 0;

	tokens_lst = ft_split_by_delim(str, ' ');
	if (!tokens_lst) // ! make sure no empty string as well
		return (NULL);
	if (*col_ptr != 0 && *col_ptr != ft_count_list(str))
		// ! todo: throw error invalid map, see if things need free
	*col_ptr = ft_count_list(str);

	coord_arr = malloc((*col_ptr) * sizeof(t_coord));
	while (tokens_lst[i])
	{
		// todo: validation

	}
}

/*
1  -2  3  4  5,0xff
6  7  8  9  10,0xFFFFFF
11 12 13 14 15,0x870f0f
*/
t_map	*build_map(int fd)
{
	char *new_row;
	t_coord	**coords_lst;
	int	row;
	int	row_limit;
	int	col;

	t_map	*map;
	map = malloc(sizeof(t_map));
	if (!map)
		perror_exit("FdF", EXIT_FAILURE);

	row = 0;
	row_limit = 10;
	coords_lst = ft_calloc_lst(row_limit);
	while (row++)
	{
		// resizing list to allow more rows
		if (row >= row_limit - 1)
		{
			row_limit *= 2;
			ft_realloc_lst(coords_lst, row_limit);
		}
		new_row = get_next_line(fd);
		if (!new_row)
			break ;
		coords_lst[row] = parse_coord_arr(new_row, &map->col);
	}
	col = get_validate_coords_col(coords_lst);
	if (col == -1)
		errexit("FdF: Invalid map!", EXIT_FAILURE);
	map->row = row;
	map->coords_lst = coords_lst;

	return (map);
}

int	open_file_as_read(char *file_path)
{
	int	fd;

	fd = open(file_path, O_RDONLY);
	if (fd == -1)
		perror_exit("FdF", EXIT_FAILURE);
	return (fd);
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

	// todo:
	prog->mlx_obj = setup_mlx();

	// read map
	fd = open_file_as_read(av[1]);
	prog->map_obj = build_map(fd);

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