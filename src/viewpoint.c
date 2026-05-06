/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   viewpoint.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 18:12:05 by kong              #+#    #+#             */
/*   Updated: 2026/05/06 19:35:25 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

t_viewpoint	*init_vp(t_map *map)
{
	t_viewpoint *new_vp;

	new_vp = malloc(sizeof(t_viewpoint));
	if (!new_vp)
		return (perror("FdF"), NULL);
	new_vp->win_width = WIN_WIDTH;
	new_vp->win_height = WIN_HEIGHT;
	iso_vp(new_vp, map->n_row, map->n_col);
	return (new_vp);
}

void	iso_vp(t_viewpoint *vp, int rows, int cols)
{
	// total map size relative to window size
	vp->f_zoom = ft_min(
		(float)vp->win_width / (float)(rows + cols),
		(float)vp->win_height / (float)(rows + cols));
	// is just a constant ratio ~10% relative to X-Y plane.
	// controlling the height
	vp->f_zscale = (float)ft_max(rows, cols) / 10.0f;
	vp->spin_deg = 45;
	vp->pitch_deg = 30;
	vp->x_win_anchor = vp->win_width / 2;
	vp->y_win_anchor = vp->win_height / 2;
}

void	set_vp(t_data *data, double spin_deg, double pitch_deg)
{
	int	rows;
	int	cols;
	t_viewpoint	*vp;

	vp = data->vp_obj;
	rows = data->map_obj->n_row;
	cols = data->map_obj->n_col;
	vp->f_zoom = ft_min(
		(float)vp->win_width / (float)(rows + cols),
		(float)vp->win_height / (float)(rows + cols));
	vp->spin_deg = spin_deg;
	vp->pitch_deg = pitch_deg;
	vp->x_win_anchor = vp->win_width / 2;
	vp->y_win_anchor = vp->win_height / 2;
}
