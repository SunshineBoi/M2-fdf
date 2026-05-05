/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 11:10:00 by kong              #+#    #+#             */
/*   Updated: 2026/05/05 17:08:23 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

t_line init_line(t_coord start, t_coord end)
{
	t_line	new_line;

	new_line.start = start;
	new_line.end = end;
	new_line.major_axis = 0;
	new_line.minor_axis = 0;
	new_line.major_step = 1; // drawing right
	new_line.minor_step = 1; // drawing right
	new_line.gradient = 0;
	new_line.dx = 0;
	new_line.dy = 0;
	new_line.boundary = 0;
	new_line.reset = 0;
	return (new_line);
}

static void	_horizontal_line(t_line *line)
{
	line->major_axis = line->start.x;
	if (line->end.x < line->start.x)
		// drawing left
		line->major_step = -1;
	line->minor_axis = line->start.y;
	if (line->end.y < line->start.y)
		// drawing up
		line->minor_step = -1;
	line->gradient = 2 * line->dy;
	line->boundary = line->dx;
	line->reset = 2 * line->dx;
}

static void	_vertical_line(t_line *line)
{
	line->major_axis = line->start.y;
	if (line->end.y < line->start.y)
		// drawing up
		line->major_step = -1;
	line->minor_axis = line->start.x;
	if (line->end.x < line->start.x)
		// drawing left
		line->minor_step = -1;
	line->gradient = 2 * line->dx;
	line->boundary = line->dy;
	line->reset = 2 * line->dy;
}
// !
static void	_draw(t_data *data, t_line *line, int *screen_x, int *screen_y)
{
	int		err;
	int		step;
	int 	total_steps;
	float	z_curr;
	float	ratio;

	total_steps = ft_max(line->dx, line->dy) + 1;
	err = 0;
	step = 0;
	while (step < total_steps)
	{
		// {f (progression) = pixel step / total pixel steps}
		// then, we use the % progression translate to Z-distance
		// {Z-step = f * total Z}
		// {z target = z initial + Z-step}
		// defines progression bar (0 - 100%)
		ratio = (float)step / ft_max(line->dx, line->dy);
		// defines the actual z-position we are at given current pixel step (it handles negative coordinate!)
		z_curr = (float)line->start.z + (((float)line->end.z - (float)line->start.z) * ratio);
		if (line->start.z == line->end.z)
			color_pixel(data, *screen_x, *screen_y,
				interpolate_color(line->start.color, line->end.color, ratio));
		else
			color_pixel(data, *screen_x, *screen_y,
				interpolate_color_from_base(z_curr, line->start, line->end));
		if (err >= line->boundary)
		{
			line->minor_axis += line->minor_step;
			err -= line->reset;
		}
		err += line->gradient;
		line->major_axis += line->major_step;
		step++;
	}
}

void	draw_line(t_data *data, t_coord start, t_coord end)
{
	int		*screen_x;
	int		*screen_y;
	t_line	line;

	line = init_line(start, end);
	line.dx = ft_abs(end.x - start.x);
	line.dy = ft_abs(end.y - start.y);
	if (line.dx == 0 && line.dy == 0)
	{
		color_pixel(data, start.x, start.y, start.color);
		return ;
	}
	if (line.dx >= line.dy)
	{
		_horizontal_line(&line);
		screen_x = &line.major_axis;
		screen_y = &line.minor_axis;
	}
	else
	{
		_vertical_line(&line);
		screen_x = &line.minor_axis;
		screen_y = &line.major_axis;
	}
	_draw(data, &line, screen_x, screen_y);
}
