/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 11:10:00 by kong              #+#    #+#             */
/*   Updated: 2026/04/27 23:48:38 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

/*
https://gontjarow.github.io/MiniLibX/mlx-tutorial-create-image.html
The “proper” way to draw the image according to the manual is to:

1. Check how many bits there are per pixel --> how?
2. Convert your color with mlx_get_color_value if necessary.
3. Check whether the environment is little/big endian. --> how?
4. Write your color value byte-by-byte into the pixel array, according to endianness.
	- this pixel array is returned by `mlx_get_data_addr`
*/


t_image	*init_img(t_mlx *mlx_obj)
{
	t_image	*new_img;

	new_img = malloc(sizeof(t_image));
	if (!new_img)
		return (perror("FdF"), NULL);

	new_img->img_ptr = mlx_new_image(mlx_obj->conn_ptr, WIN_WIDTH, WIN_HEIGHT);
	if (!new_img->img_ptr)
		return (free(new_img), print_errmsg("FdF: MLX image init failure."), NULL);

	new_img->pixel_ptr = mlx_get_data_addr(new_img->img_ptr, &new_img->bpp, &new_img->spl, &new_img->endian);
	if (!new_img->pixel_ptr)
		return (mlx_destroy_image(mlx_obj->conn_ptr, new_img->img_ptr), free(new_img), print_errmsg("FdF: MLX image init failure."), NULL);
	return (new_img);
}


static void	_color_pixel(t_data *data, int x, int y, int color)
{
	int	offset;

	if (x < 0 || x >= WIN_WIDTH || y < 0 || y >= WIN_HEIGHT)
		return ;
	// ! pixel_ptr is char *, so every pointer moves by 1 byte. so 'offset' value should be pass as byte. we cast to int * because each pixel is 4 bytes.
	// target pixel to color
	// why do we cast to int *? - because originally is char * (1 byte)
	// we need it to be int - 4 bytes to store RGB.
	// it tells compiler to read/write 4 bytes instead of 1.
	// why we cast later? because spl / bpp works with 1 byte, if we define int * initially, pointer increment would have shift 4bytes for every 1 step increment.
	offset = (y * (data->img_obj->spl)) + (x * (data->img_obj->bpp/8));
	*(unsigned int *)(data->img_obj->pixel_ptr + offset) = color;
}

void	draw_line(t_data *data, t_coord start, t_coord end)
{
	int	major;
	int	minor;
	int	major_dir = 1;
	int	minor_dir = 1;
	int	*screen_x;
	int	*screen_y;
	
	int	step;
	int	boundary;
	int	reset;

	int dx = ft_abs(end.x - start.x);
	int dy = ft_abs(end.y - start.y);
	int	err = 0;
	if (dx >= dy)
	{
		major = start.x;
		if (end.x < start.x)
			// drawing left
			major_dir = -1;
		minor = start.y;
		if (end.y < start.y)
			// drawing up
			minor_dir = -1;
		screen_x = &major;
		screen_y = &minor;
		step = 2 * dy;
		boundary = dx;
		reset = 2 * dx;
	}
	else
	{
		major = start.y;
		if (end.y < start.y)
			// drawing up
			major_dir = -1;
		minor = start.x;
		if (end.x < start.x)
			// drawing left
			minor_dir = -1;
		screen_x = &minor;
		screen_y = &major;
		step = 2 * dx;
		boundary = dy;
		reset = 2 * dy;
	}
	int i = ft_max(dx, dy) + 1;
	while (i--)
	{
		_color_pixel(data, *screen_x, *screen_y, start.color);
		if (err >= boundary)
		{
			minor += minor_dir;
			err -= reset;
		}
		err += step;
		major += major_dir;
		// ! update gradient color
	}
}

void	do_render(t_data *data, t_coord *screen_arr)
{
	// [T][R][G][B] - 1 byte each, total 4 bytes repr 1 pixel
	int	i = 0;
	int	n_col = data->map_obj->n_col;
	int	n_row = data->map_obj->n_row;
	while (i < data->map_obj->n_row * data->map_obj->n_col)
	{
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
	int		row;
	int		row_size = data->map_obj->n_row;
	int		col;
	int		col_size = data->map_obj->n_col;

	float	f_zscale = data->vp_obj->f_zscale;
	float	z_world;
	float	x_world;
	float	y_world;
	float	x_screen;
	float	y_screen;
	int		x_pixel;
	int		y_pixel;
	t_coord	**src_lst = data->map_obj->src_lst;
	t_coord	*point_arr;
	t_coord	point;

	point_arr = malloc((row_size * col_size) * sizeof(t_coord));
	if (!point_arr)
		return (perror("FdF"), -1);
	row = 0;
	while (row < row_size)
	{
		col = 0;
		while (col < col_size)
		{
			// 1. applies z-scale factor
			if (data->map_obj->z_max != data->map_obj->z_min)
				z_world = ((float)src_lst[row][col].z - data->map_obj->z_min) / (data->map_obj->z_max - data->map_obj->z_min);
			z_world = z_world * f_zscale;
			// 2. centering map coordinates. Need to force float
			x_world = col - ((data->map_obj->n_col - 1) / 2.0);
			y_world = row - ((data->map_obj->n_row - 1) / 2.0);
			// 3. spin rotation
			x_screen = (x_world * cos(data->vp_obj->spin_deg * DEG_TO_RAD)) -
				(y_world * sin(data->vp_obj->spin_deg * DEG_TO_RAD));
			y_screen = (x_world * sin(data->vp_obj->spin_deg * DEG_TO_RAD)) +
				(y_world * cos(data->vp_obj->spin_deg * DEG_TO_RAD));
			// 4. pitch rotation
			y_screen = (y_screen * cos(data->vp_obj->pitch_deg * DEG_TO_RAD)) -
				(z_world * sin(data->vp_obj->pitch_deg * DEG_TO_RAD));
			// 5. zoom factor
			x_screen *= data->vp_obj->f_zoom;
			y_screen *= data->vp_obj->f_zoom;
			// 6. convert to pixel coordinate
			x_pixel = x_screen + data->vp_obj->x_win_anchor;
			y_pixel = y_screen + data->vp_obj->y_win_anchor;
			// create a new point
			point.x = x_pixel;
			point.y = y_pixel;
			point.color = src_lst[row][col].color;
			// store into a new arr
			point_arr[(row * col_size) + col] = point;

			col++;
		}
		row++;
	}
	ft_memset(data->img_obj->pixel_ptr, 0, WIN_HEIGHT * data->img_obj->spl);
	do_render(data, point_arr);
	free(point_arr);
	return (1);
}
