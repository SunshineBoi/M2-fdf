/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_mouse.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 13:47:44 by kong              #+#    #+#             */
/*   Updated: 2026/05/05 15:31:57 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

void	mouse_zoom(t_data *data, int x, int y, float f_zoom_ratio)
{
	data->vp_obj->x_win_anchor =
		x - ((x - data->vp_obj->x_win_anchor) * f_zoom_ratio);
	data->vp_obj->y_win_anchor =
		y - ((y - data->vp_obj->y_win_anchor) * f_zoom_ratio);
	data->vp_obj->f_zoom *= f_zoom_ratio;
	data->img_obj->is_dirty = 1;
}

void	set_pan(t_data *data, int x, int y)
{
	data->mlx_obj->is_clickhold = 1;
	data->mlx_obj->mouse_x = x;
	data->mlx_obj->mouse_y = y;
}

void	set_rotate(t_data *data, int x, int y)
{
	data->mlx_obj->is_clickhold = 3;
	data->mlx_obj->mouse_x = x;
	data->mlx_obj->mouse_y = y;
}

void	mouse_pan(t_data *data, int x, int y)
{
	int	d_mouse_x;
	int	d_mouse_y;

	d_mouse_x = x - data->mlx_obj->mouse_x;
	d_mouse_y = y - data->mlx_obj->mouse_y;
	// anchored new point - distance from last mouse pos to new pos
	data->vp_obj->x_win_anchor += d_mouse_x;
	data->vp_obj->y_win_anchor += d_mouse_y;
	// update mouse pos
	data->mlx_obj->mouse_x = x;
	data->mlx_obj->mouse_y = y;
}

void	mouse_rotate(t_data *data, int x, int y)
{
	int	d_mouse_x;
	int	d_mouse_y;

	d_mouse_x = x - data->mlx_obj->mouse_x;
	d_mouse_y = y - data->mlx_obj->mouse_y;
	data->vp_obj->spin_deg -= d_mouse_x * 0.15f;
	data->vp_obj->pitch_deg -= d_mouse_y * 0.15f;
}
