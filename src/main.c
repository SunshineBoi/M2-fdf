/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 14:23:27 by kong              #+#    #+#             */
/*   Updated: 2026/04/27 18:52:44 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

t_data	*init_data()
{
	t_data	*new_data;

	new_data = malloc(sizeof(t_data));
	if (!new_data)
		return (perror("FdF"), NULL);
	new_data->map_obj = NULL;
	new_data->mlx_obj = NULL;
	new_data->vp_obj = NULL;
	new_data->img_obj = NULL;
	return (new_data);
}

t_mlx	*setup_mlx()
{
	t_mlx	*mlx_obj;

	mlx_obj = malloc(sizeof(t_mlx));
	if (!mlx_obj)
		return (perror("FdF"), NULL);
	mlx_obj->conn_ptr = mlx_init();
	if (!mlx_obj->conn_ptr)
		return (print_errmsg("FdF: MLX init failure."), NULL);
	mlx_obj->win_ptr = mlx_new_window(mlx_obj->conn_ptr, WIN_WIDTH, WIN_HEIGHT, "FdF");
	if (!mlx_obj->win_ptr)
		return (freemlx(mlx_obj), NULL);
	return (mlx_obj);
}


int	fdf(char *file)
{
	int		fd;
	t_data	*data;

	data = init_data();
	if (!data)
		return (EXIT_FAILURE);
	
	// setup MLX
	data->mlx_obj = setup_mlx();
	if (!data->mlx_obj)
		return (freeprog(data, EXIT_FAILURE));

	// read map
	fd = open_file_read(file);
	if (fd == -1)
		return (freeprog(data, EXIT_FAILURE));
	data->map_obj = build_map(fd);
	if (!data->map_obj)
		return (close(fd), freeprog(data, EXIT_FAILURE));
	close(fd);

	// setup viewpoint
	data->vp_obj = init_vp(data->map_obj);
	if (!data->vp_obj)
		return (freeprog(data, EXIT_FAILURE));

	data->img_obj = init_img(data->mlx_obj);
	if (!data->img_obj)
		return (freeprog(data, EXIT_FAILURE));

	// rendering
	if (render_img(data) == -1)
		return (freeprog(data, EXIT_FAILURE));

	// hook
	setup_events(data);

	// start listening
	mlx_loop(data->mlx_obj->conn_ptr);

	return (EXIT_SUCCESS);
}

int	main(int ac, char **av)
{
	if (ac != 2)
		errexit("FdF: Error: Wrong argument counts", EXIT_FAILURE);
	return (fdf(av[1]));
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

	
https://gontjarow.github.io/MiniLibX/mlx-tutorial-create-image.html
The “proper” way to draw the image according to the manual is to:

1. Check how many bits there are per pixel --> how?
2. Convert your color with mlx_get_color_value if necessary.
3. Check whether the environment is little/big endian. --> how?
4. Write your color value byte-by-byte into the pixel array, according to endianness.
	- this pixel array is returned by `mlx_get_data_addr`
*/


