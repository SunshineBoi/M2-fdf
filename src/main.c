/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 14:23:27 by kong              #+#    #+#             */
/*   Updated: 2026/04/25 16:41:13 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

t_data	*init_fdf()
{
	t_data	*fdf;

	fdf = malloc(sizeof(t_data));
	if (!fdf)
		return (perror("FdF"), NULL);
	fdf->map_obj = NULL;
	fdf->mlx_obj = NULL;
	fdf->vp_obj = NULL;
	return (fdf);
}


t_viewpoint	*init_vp(t_map *map)
{
	t_viewpoint *vp;

	vp = malloc(sizeof(t_viewpoint));
	if (!vp)
		return (perror("FdF"), NULL);
	vp->f_zoom = WIN_HEIGHT / (map->n_row + map->n_col);  // check this
	vp->f_zscale = 20;
	vp->spin_deg = 45;
	vp->pitch_deg = 30;
	vp->x_win_anchor = WIN_WIDTH / 2;
	vp->y_win_anchor = WIN_HEIGHT / 2;
	return (vp);
}

t_image	*init_img()
{
	t_image	*img;

	img = malloc(sizeof(t_image));
	if (!img)
		return (perror("FdF"), NULL);
	img->bpp = 0;
	img->spl = ??;
	img->endian = ??;
}

int	main(int ac, char **av)
{
	int		fd;
	t_data	*fdf;

	if (ac != 2)
		errexit("FdF: Error: Wrong argument counts", EXIT_FAILURE);

	fdf = init_fdf();
	if (!fdf)
		return (EXIT_FAILURE);
	
	// todo: setup MLX
	fdf->mlx_obj = setup_mlx();
	if (!fdf->mlx_obj)
		return (freeprog(fdf, EXIT_FAILURE));

	// todo: read map
	fd = open_file_read(av[1]);
	if (fd == -1)
		return (freeprog(fdf, EXIT_FAILURE));
	fdf->map_obj = build_map(fd);
	if (!fdf->map_obj)
		return (freeprog(fdf, EXIT_FAILURE));
	close(fd);

	// todo: setup viewpoint
	fdf->vp_obj = init_vp(fdf->map_obj);
	if (!fdf)
		return (freeprog(fdf, EXIT_FAILURE));

	fdf->img_obj = init_img();
	if (!fdf->img_obj)
		return (freeprog(fdf, EXIT_FAILURE));

	// todo: rendering
	render_img(fdf);

	// start listening
	mlx_loop(fdf->mlx_obj->conn_ptr);

	return (EXIT_SUCCESS);
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