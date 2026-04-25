/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 11:10:00 by kong              #+#    #+#             */
/*   Updated: 2026/04/25 23:25:52 by kong             ###   ########.fr       */
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

void	render_img(t_data *fdf)
{
	int		row;
	int		row_size = fdf->map_obj->n_row;
	int		col;
	int		col_size = fdf->map_obj->n_col;

	int		f_zscale = fdf->vp_obj->f_zscale;
	float	z_world;
	float	x_world;
	float	y_world;
	float	x_screen;
	float	y_screen;
	int		x_pixel;
	int		y_pixel;
	t_coord	**src_lst = fdf->map_obj->src_lst;
	t_coord	*point_arr;
	t_coord	point;

	point_arr = malloc((row_size * col_size) * sizeof(t_coord));
	if (!point_arr)
		return (NULL);
	row = 0;
	while (row < row_size)
	{
		col = 0;
		while (col < col_size)
		{
			// 1. applies z-scale factor
			z_world = src_lst[row][col].z * f_zscale;
			// 2. centering map coordinates
			x_world = col - ((fdf->map_obj->n_col - 1) / 2);
			y_world = row - ((fdf->map_obj->n_row - 1) / 2);
			// 3. spin rotation
			x_screen = (x_world * cos(fdf->vp_obj->spin_deg)) -
				(y_world * sin(fdf->vp_obj->spin_deg * DEG_TO_RAD));
			y_screen = (x_world * sin(fdf->vp_obj->spin_deg)) +
				(y_world * cos(fdf->vp_obj->spin_deg * DEG_TO_RAD));
			// 4. pitch rotation
			y_screen = (y_screen * cos(fdf->vp_obj->pitch_deg * DEG_TO_RAD)) -
				(z_world * sin(fdf->vp_obj->pitch_deg * DEG_TO_RAD));
			// 5. zoom factor
			x_screen *= fdf->vp_obj->f_zoom;
			y_screen *= fdf->vp_obj->f_zoom;
			// 6. convert to pixel coordinate
			x_pixel = x_screen + fdf->vp_obj->x_win_anchor;
			y_pixel = y_screen + fdf->vp_obj->y_win_anchor;
			
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
	do_render(fdf, point_arr);
}

void	do_render(t_data *fdf, t_coord *screen_arr)
{
	void	*new_img_ptr;
	char	*pixel_ptr;
	int		bpp;
	int		spl; // no. of bytes per row in the image
	int		edn;
	t_image	*img_obj;  // ! this should gets created outside

	new_img_ptr = mlx_new_image(fdf->mlx_obj->conn_ptr, WIN_WIDTH, WIN_HEIGHT);
	
	// why do we cast to int *? - because originally is char * (1 byte)
	// we need it to be int - 4 bytes to store RGB.
	// it tells compiler to read/write 4 bytes instead of 1.
	pixel_ptr = mlx_get_data_addr(new_img_ptr, &bpp, &spl, &edn);
	fdf->img_obj->bpp = bpp;
	fdf->img_obj->spl = spl;
	fdf->img_obj->endian = edn;
	fdf->img_obj->pixel_ptr = pixel_ptr;

	// [T][R][G][B] - 1 byte each, total 4 bytes repr 1 pixel
	int	i = 0;
	int	n_col = fdf->map_obj->n_col;
	int	n_row = fdf->map_obj->n_row;
	while (i < fdf->map_obj->n_row * fdf->map_obj->n_col)
	{
		// ! how do i map buffer space to pixel space?
		// why do we cast to int *? - because originally is char * (1 byte)
		// we need it to be int - 4 bytes to store RGB.
		// it tells compiler to read/write 4 bytes instead of 1.
		// why we cast later? because spl / bpp works with 1 byte, if we define int * initially, pointer increment would have shift 4bytes for every 1 step increment.

		// draw line to right node
		if (i % n_col < n_col - 1)
			draw_line(fdf, screen_arr[i], screen_arr[i + 1]);
		// draw line to bottom node
		if (i < (n_row - 1) * n_col)
			draw_line(fdf, screen_arr[i], screen_arr[i + n_col]);
		i++;
	}
	mlx_put_image_to_window(fdf->mlx_obj->conn_ptr, fdf->mlx_obj->win_ptr, new_img_ptr, 0, 0);
}

void	draw_line(t_data *fdf, t_coord start, t_coord end)
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
		_color_pixel(fdf, *screen_x, *screen_y, start.color);
		if (err > boundary)
		{
			minor += minor_dir;
			err -= reset;
		}
		err += step;
		major += major_dir;
		// ! update gradient color
	}
}

void	_color_pixel(t_data *fdf, int x, int y, int color)
{
	int	offset;

	// target pixel to color
	offset = (y * (fdf->img_obj->spl)) + (x * (fdf->img_obj->bpp/8));
	*(unsigned int *)(fdf->img_obj->pixel_ptr + offset) = color;
}
