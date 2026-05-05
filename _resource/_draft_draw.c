/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw copy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 11:10:00 by kong              #+#    #+#             */
/*   Updated: 2026/05/04 23:31:05 by kong             ###   ########.fr       */
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

int	rerender_img(t_data *data)
{
	mlx_destroy_image(data->mlx_obj->conn_ptr, data->img_obj->img_ptr);
	if (init_mlx_img(data->mlx_obj, data->img_obj, data->vp_obj) == -1)
		return (-1);
	if (render_img(data) == -1)
		return (-1);
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

int	interpolate_color_from_base(float z_curr, t_coord start, t_coord end)
{
	float f;
	int	z_start;
	int	z_end;

	z_start = start.z;
	z_end = end.z;
	if ((z_start > 0 && z_end > 0) || (z_start < 0 && z_end < 0))
	{
		if (z_start == z_end)
			return (start.color);
		f = fabsf((z_curr - z_start) / (z_start - z_end));
		return (interpolate_color(start.color, end.color, f));
	}
	else if ((z_curr < 0 && z_start < 0) || (z_curr > 0 && z_start > 0))
	{
		f = fabsf(z_curr / z_start);
		return (interpolate_color(COLOR_BASE, start.color, f));
	}
	else if ((z_curr < 0 && z_end < 0) || (z_curr > 0 && z_end > 0))
	{
		f = fabsf(z_curr / z_end);
		return (interpolate_color(COLOR_BASE, end.color, f));
	}
	return (COLOR_BASE);
}

int	interpolate_color(int color_start, int color_end, float f)
{
	int	color;
	int	r_start;
	int	g_start;
	int	b_start;
	int	r_end;
	int	g_end;
	int	b_end;

	b_start = (color_start & 0x0000FF);
	g_start = (color_start & 0x00FF00) >> 8;
	r_start = (color_start & 0xFF0000) >> 16;

	b_end = (color_end & 0x0000FF);
	g_end = (color_end & 0x00FF00) >> 8;
	r_end = (color_end & 0xFF0000) >> 16;

	int	b_x = b_start + ((b_end - b_start) * f);
	int	g_x = g_start + ((g_end - g_start) * f);
	int	r_x = r_start + ((r_end - r_start) * f);
	color = rgb_to_int(r_x, g_x, b_x);

	return (color);
}

static void	color_pixel(t_data *data, int x, int y, int color)
{
	int	offset;
	int win_width;
	int win_height;

	win_width = data->vp_obj->win_width;
	win_height = data->vp_obj->win_height;

	if (x < 0 || x >= win_width || y < 0 || y >= win_height)
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
	
	int	gradient;
	int	boundary;
	int	reset;

	int dx = ft_abs(end.x - start.x);
	int dy = ft_abs(end.y - start.y);
	if (dx == 0 && dy == 0)
	{
		color_pixel(data, start.x, start.y, start.color);
		return ;
	}
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
		gradient = 2 * dy;
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
		gradient = 2 * dx;
		boundary = dy;
		reset = 2 * dy;
	}
	int total_steps = ft_max(dx, dy) + 1;
	int	step = 0;
	int	z_start = start.z;
	int	z_end = end.z;
	while (step < total_steps)
	{
		// {f (progression) = pixel step / total pixel steps}
		// then, we use the % progression translate to Z-distance
		// {Z-step = f * total Z}
		// {z target = z initial + Z-step}
		// defines progression bar (0 - 100%)
		float f = (float)step / ft_max(dx, dy);
		// defines the actual z-position we are at given current pixel step (it handles negative coordinate!)
		float z_curr = (float)z_start + ((z_end - z_start) * f);
		int color = interpolate_color_from_base(z_curr, start, end);
		color_pixel(data, *screen_x, *screen_y, color);
		if (err >= boundary)
		{
			minor += minor_dir;
			err -= reset;
		}
		err += gradient;
		major += major_dir;
		step++;
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
	int	z_min = data->map_obj->z_min;
	int	z_max = data->map_obj->z_max;
	float f;

	float	f_zscale = data->vp_obj->f_zscale;
	float	z_world;
	float	x_world;
	float	y_world;
	float	x_trf;
	float	y_trf;
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
			// ! normalize z value to between 0 - 1
			z_world = 0;
			if (data->map_obj->z_max != data->map_obj->z_min)
				// calc relative % to max z
				z_world = ((float)src_lst[row][col].z - z_min) / (z_max - z_min);
			z_world = z_world * f_zscale;
			// 2. centering map coordinates. Need to force float
			x_world = col - ((data->map_obj->n_col - 1) / 2.0);
			y_world = row - ((data->map_obj->n_row - 1) / 2.0);
			// 3. spin rotation
			x_trf = (x_world * cos(data->vp_obj->spin_deg * DEG_TO_RAD)) -
				(y_world * sin(data->vp_obj->spin_deg * DEG_TO_RAD));
			y_trf = (x_world * sin(data->vp_obj->spin_deg * DEG_TO_RAD)) +
				(y_world * cos(data->vp_obj->spin_deg * DEG_TO_RAD));
			// 4. pitch rotation
			y_trf = (y_trf * cos(data->vp_obj->pitch_deg * DEG_TO_RAD)) -
				(z_world * sin(data->vp_obj->pitch_deg * DEG_TO_RAD));
			// 5. zoom factor - it determines the final spacing between points.
			x_trf *= data->vp_obj->f_zoom;
			y_trf *= data->vp_obj->f_zoom;
			// 6. convert to pixel coordinate
			x_pixel = x_trf + data->vp_obj->x_win_anchor;
			y_pixel = y_trf + data->vp_obj->y_win_anchor;
			// create a new point
			point.x = x_pixel;
			point.y = y_pixel;
			point.z = src_lst[row][col].z;

			// 7. fill up default point color.
			f = 0.0;
			if (src_lst[row][col].color == -1 && src_lst[row][col].z >= 0)
			{
				if (z_max != 0)
					f = (float)src_lst[row][col].z / z_max;
				point.color = interpolate_color(COLOR_BASE, COLOR_POSITIVE, f);
			}
			else if (src_lst[row][col].color == -1 && src_lst[row][col].z < 0)
			{
				if (z_min != 0)
					f = (float)ft_abs(src_lst[row][col].z) / ft_abs(z_min);
				point.color = interpolate_color(COLOR_BASE, COLOR_NEGATIVE, f);
			}
			else
				point.color = src_lst[row][col].color;

			// store into a new arr
			point_arr[(row * col_size) + col] = point;
			col++;
		}
		row++;
	}
	ft_memset(data->img_obj->pixel_ptr, 0, data->vp_obj->win_height * data->img_obj->spl);
	do_render(data, point_arr);
	free(point_arr);
	return (1);
}
