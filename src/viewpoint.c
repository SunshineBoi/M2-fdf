/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   viewpoint.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 18:12:05 by kong              #+#    #+#             */
/*   Updated: 2026/04/27 23:48:06 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

t_viewpoint	*init_vp(t_map *map)
{
	t_viewpoint *new_vp;

	new_vp = malloc(sizeof(t_viewpoint));
	if (!new_vp)
		return (perror("FdF"), NULL);
	default_vp(new_vp, map->n_row, map->n_col);
	return (new_vp);
}

void	default_vp(t_viewpoint *vp, int rows, int cols)
{
	vp->f_zoom = WIN_HEIGHT / (rows + cols);  // check this
	vp->f_zscale = ft_max(cols, rows) / 8;
	vp->spin_deg = 45;
	vp->pitch_deg = 30;
	vp->x_win_anchor = WIN_WIDTH / 2;
	vp->y_win_anchor = WIN_HEIGHT / 2;
}
