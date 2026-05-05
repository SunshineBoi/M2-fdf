/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_draw.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 18:38:21 by kong              #+#    #+#             */
/*   Updated: 2026/05/05 19:09:23 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

// for gradient color, split into individual RGB, interpolate, then combine back
// in 1-byte: r = 0xFF (hex) / 255 (dec)
// int has 4-bytes, doing bitshift to form the full integer.
int	rgb_to_int(int r, int g, int b)
{
	return (r << 16 | g << 8 | b);
}

int	interpolate_color(int color_start, int color_end, float f)
{
	int	r_start;
	int	g_start;
	int	b_start;
	int	r_end;
	int	g_end;

	b_start = (color_start & 0x0000FF);
	g_start = (color_start & 0x00FF00) >> 8;
	r_start = (color_start & 0xFF0000) >> 16;

	g_end = (color_end & 0x00FF00) >> 8;
	r_end = (color_end & 0xFF0000) >> 16;

	return (rgb_to_int(
		r_start + ((r_end - r_start) * f),
		g_start + ((g_end - g_start) * f),
		b_start + (((color_end & 0x0000FF) - b_start) * f)));
}

int	interpolate_color_from_base(float z_curr, t_coord start, t_coord end)
{
	float	f;
	float	z_start;
	float	z_end;

	z_start = (float)start.z;
	z_end = (float)end.z;
	if ((z_start > 0 && z_end > 0) || (z_start < 0 && z_end < 0))
	{
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

void	color_pixel(t_data *data, int x, int y, int color)
{
	int	offset;
	int win_width;
	int win_height;

	win_width = data->vp_obj->win_width;
	win_height = data->vp_obj->win_height;

	if (x < 0 || x >= win_width || y < 0 || y >= win_height)
		return ;
	offset = (y * (data->img_obj->spl)) + (x * (data->img_obj->bpp/8));
	*(unsigned int *)(data->img_obj->pixel_ptr + offset) = color;
}
