/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 14:23:27 by kong              #+#    #+#             */
/*   Updated: 2026/04/11 23:57:00 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

int	_is_in_range(char *str, long long limit)
{
	long long	total;
	int			digit;

	total = 0;
	while (*str)
	{
		if (!(*str >= '0' && *str <= '9'))
			return (0);
		digit = *str - '0';
		if (total > (limit / 10) || (total == (limit / 10) && digit > (limit % 10)))
			return (0);	
		total = (total * 10) + digit;
		str++;
	}
	return (1);
}

int	is_valid_int(char *str)
{
	long long	limit;

	if (!str || !*str)
		return (0);
	limit = INT_MAX;
	// check sign
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			limit = -INT_MIN;
		str++;
	}
	// check if sign only
	if (!*str || *str == ',')
		return (0);
	return (_is_in_range(str, limit));
}

t_coord	_parse_coord_node(char *str)
{
	t_coord	new_coord;
	char	*color_ptr;
	char	*nbr_ptr;
	int		color;

	color_ptr = ft_strchr(str, ',');
	if (!color_ptr)
	{
		new_coord.color = 0xFFFFFF;  // ! is this the correct way?
		if (!is_valid_int(str))
			return (...); // ! what is the null value of this?
		new_coord.z_height = ft_atoi(str);
	}
	else
	{
		color = hexatoi(color_ptr + 1);
		if (...) // ! how can i validate int for color?
			return (...);
		new_coord.color = color;
	
		nbr_ptr = ft_strndup(str, str - color_ptr);
		if (!is_valid_int(nbr_ptr))
			return (...); // ! what is the null value of this?
		new_coord.z_height = ft_atoi(nbr_ptr);
	}
	return (new_coord);
}

t_coord	*parse_coord_arr(char *str, int *col_ptr)
{
	// ft_split strictly on space
	// given words list, verify each element is valid int (incl overflow),
		// and valid color code.
		// and along the way validate cols
	// create t_coord for each element and store the values.
	char 	**tokens_lst;
	t_coord	*coord_arr;
	t_coord new_node;
	int		i = 0;

	tokens_lst = ft_split_by_delim(str, ' ');
	if (!tokens_lst)
		return (NULL);
	if (*col_ptr != 0 && *col_ptr != ft_count_list(str))
		return (print_error_msg("FdF: Invalid map!"), freelst(tokens_lst), NULL);
	*col_ptr = ft_count_list(str);

	coord_arr = malloc((*col_ptr) * sizeof(t_coord));
	while (tokens_lst[i])
	{
		if (!tokens_lst[i][0])
			return (print_error_msg("FdF: Invalid map!"), free(coord_arr),
				freelst(tokens_lst), NULL);
		// each i element is a number string
		// ! todo: because this is not returning a pointer, it is harder to detect error
		// 		instead, we pass the coord_arr[i] position for it to fill up,
		//		it returns 0/1 to indicate success or failure
		new_node = _parse_coord_node(tokens_lst[0]);
		if (!new_node)
			return (print_error_msg("FdF: Invalid map!"), free(coord_arr),
				freelst(tokens_lst), NULL);
		coord_arr[i] = new_node;
		
	}
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
	int		row;
	int		row_limit;
	int		col;

	t_map	*map;
	
	map = malloc(sizeof(t_map));
	if (!map)
		return (perror("FdF"), NULL);
	row = 0;
	row_limit = 10;
	coords_lst = ft_calloc_lst(row_limit);
	if (!coords_lst)
		return (perror("FdF"), free(map), NULL);
	while (row)
	{
		// resizing list to allow more rows
		if (row >= row_limit - 1)
		{
			row_limit *= 2;
			temp = ft_realloc_lst(coords_lst, row_limit);
			if (!temp)
				return (perror("FdF"), free(map), NULL);
			coords_lst = temp;
		}
		new_row = get_next_line(fd);
		if (!new_row)
			break ;
		coords_lst[row] = parse_coord_arr(new_row, &map->col);
		if (!coords_lst[row])
			return (free(map), NULL);
		row++;
	}
	map->row = row;
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
		return (perror("FdF"), free_prog(prog, EXIT_FAILURE));
	prog->map_obj = build_map(fd);
	if (!prog->map_obj)
		free_prog(prog, EXIT_FAILURE);

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