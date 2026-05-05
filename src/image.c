/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 17:55:01 by kong              #+#    #+#             */
/*   Updated: 2026/05/05 17:41:23 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

int	init_mlx_img(t_mlx *mlx_obj, t_image *img_obj, t_viewpoint *vp_obj)
{
	img_obj->img_ptr = mlx_new_image(
			mlx_obj->conn_ptr, vp_obj->win_width, vp_obj->win_height);
	if (!img_obj->img_ptr)
		return (perrmsg("FdF: MLX image failure"), -1);
	img_obj->pixel_ptr = mlx_get_data_addr(
			img_obj->img_ptr, &img_obj->bpp, &img_obj->spl, &img_obj->endian);
	if (!img_obj->pixel_ptr)
		return (mlx_destroy_image(mlx_obj->conn_ptr, img_obj->img_ptr),
			perrmsg("FdF: MLX image failure"), -1);
	img_obj->is_dirty = 0;
	return (0);
}

t_image	*init_img(t_mlx *mlx_obj, t_viewpoint *vp_obj)
{
	t_image	*new_img;

	new_img = malloc(sizeof(t_image));
	if (!new_img)
		return (perror("FdF"), NULL);
	if (init_mlx_img(mlx_obj, new_img, vp_obj) == -1)
		return (free(new_img), NULL);
	return (new_img);
}

int	rerender_img(t_data *data)
{
	mlx_destroy_image(data->mlx_obj->conn_ptr, data->img_obj->img_ptr);
	if (init_mlx_img(data->mlx_obj, data->img_obj, data->vp_obj) == -1)
		return (-1);
	if (render_img(data) == -1)
		return (-1);
	return (0);
}

static void	_do_render(t_data *data, t_coord *screen_arr)
{
	// [T][R][G][B] - 1 byte each, total 4 bytes repr 1 pixel
	int	i = 0;
	int	n_col = data->map_obj->n_col;
	int	n_row = data->map_obj->n_row;
	while (i < data->map_obj->n_row * data->map_obj->n_col)
	{
		// 1 pixel case
		if (i == 0)
			color_pixel(
				data, screen_arr[0].x, screen_arr[0].y, screen_arr[0].color);
		// draw line to right node
		if (i % n_col < n_col - 1)
			draw_line(data, screen_arr[i], screen_arr[i + 1]);
		// draw line to bottom node
		if (i < (n_row - 1) * n_col)
			draw_line(data, screen_arr[i], screen_arr[i + n_col]);
		i++;
	}
	mlx_put_image_to_window(data->mlx_obj->conn_ptr, data->mlx_obj->win_ptr, data->img_obj->img_ptr, 0, 0);
}

int	render_img(t_data *data)
{
	int		row_size;
	int		col_size;
	t_coord	*point_arr;

	row_size = data->map_obj->n_row;
	col_size = data->map_obj->n_col;
	point_arr = malloc((data->map_obj->n_row * col_size) * sizeof(t_coord));
	if (!point_arr)
		return (perror("FdF"), -1);
	build_points(data, point_arr, row_size, col_size);
	ft_memset(data->img_obj->pixel_ptr, 0, 
		data->vp_obj->win_height * data->img_obj->spl);
	_do_render(data, point_arr);
	free(point_arr);
	return (1);
}
