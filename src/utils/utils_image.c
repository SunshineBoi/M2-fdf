/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_image.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 18:03:02 by kong              #+#    #+#             */
/*   Updated: 2026/05/06 15:46:52 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

static void	_normalize_z(t_data *data, float *z_world, int z_raw)
{
	float	z_min;
	float	z_max;
	float	f_zscale;

	*z_world = 0;
	z_min = (float)data->map_obj->z_min;
	z_max = (float)data->map_obj->z_max;
	f_zscale = data->vp_obj->f_zscale;
	if (data->map_obj->z_max != data->map_obj->z_min)
		// calc relative % to max z
		*z_world = ((float)z_raw - z_min) / (z_max - z_min);
	*z_world = *z_world * f_zscale;
}

static void	_transform_map(t_data *data, t_coord *point, int row, int col)
{
	float	z_world;
	float	x_world;
	float	y_world;
	float	x_trf;
	float	y_trf;

	// 1. applies z-scale factor - normalize z value to between 0 - 1
	_normalize_z(data, &z_world, data->map_obj->src_lst[row][col].z);
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
	point->x = x_trf + data->vp_obj->x_win_anchor;
	point->y = y_trf + data->vp_obj->y_win_anchor;
	point->z = data->map_obj->src_lst[row][col].z;  // store raw z val
}

static void	_fill_point_color(t_data *data, t_coord *point, int row, int col)
{
	float	f;
	float	z_max;
	float	z_min;
	t_coord	**src_lst;

	src_lst = data->map_obj->src_lst;
	z_max = (float)data->map_obj->z_max;
	z_min = (float)data->map_obj->z_min;
	// 7. fill up default point color.
	f = 0.0;
	if (src_lst[row][col].color == -1 && src_lst[row][col].z >= 0)
	{
		if (z_max != 0)
			f = (float)src_lst[row][col].z / z_max;
		point->color = interpolate_color(COLOR_BASE, COLOR_POSITIVE, f);
	}
	else if (src_lst[row][col].color == -1 && src_lst[row][col].z < 0)
	{
		if (z_min != 0)
			f = fabsf((float)src_lst[row][col].z) / fabsf(z_min);
		point->color = interpolate_color(COLOR_BASE, COLOR_NEGATIVE, f);
	}
	else
		point->color = src_lst[row][col].color;
}

void	build_img_points(t_data *data, t_coord *points, int rows, int cols)
{
	int		row;
	int		col;
	t_coord	point;

	row = 0;
	while (row < rows)
	{
		col = 0;
		while (col < cols)
		{
			_transform_map(data, &point, row, col);
			_fill_point_color(data, &point, row, col);
			// store into a new arr
			points[(row * cols) + col] = point;
			col++;
		}
		row++;
	}
}
