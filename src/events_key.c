/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events_key.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 13:34:19 by kong              #+#    #+#             */
/*   Updated: 2026/05/05 17:01:17 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

int	handle_keyboard(int keycode, void *param)
{
	t_data	*data;

	data = (t_data *)param;
	// esc key
	if (keycode == XK_Escape)
		return (freeprogexit(data, EXIT_SUCCESS));
	// 'q' key
	else if (keycode == XK_q)
		iso_vp(data->vp_obj, data->map_obj->n_row, data->map_obj->n_col);
	// 'w' key - top view
	else if (keycode == XK_w)
		set_vp(data, 0, 0);
	// 's' key - front view
	else if (keycode == XK_s)
		set_vp(data, 0, 90);
	// 'd' key - right view
	else if (keycode == XK_d)
		set_vp(data, 90, 90);
	// 'a' key - left view
	else if (keycode == XK_a)
		set_vp(data, -90, 90);
	data->img_obj->is_dirty = 1;
	return (0);
}

int	handle_mouse_click(int button, int x, int y, void *param)
{
	t_data	*data;

	data = (t_data *)param;
	// panning with left button
	if (button == 1)
		set_pan(data, x, y);
	else if (button == 3)
		set_rotate(data, x, y);
	// scroll up
	else if (button == 4)
		mouse_zoom(data, x, y, 1.1f);
	// scroll down
	else if (button == 5)
		mouse_zoom(data, x, y, 0.9f);
	return (0);
}

int	handle_mouse_release(int button, int x, int y, void *param)
{
	t_data	*data;

	(void)x;
	(void)y;
	data = (t_data *)param;
	if (button == 1 || button == 3)
		data->mlx_obj->is_clickhold = 0;
	return (0);
}
/*
Before fix — mouse_x never updated in the move handler, only at button press. So every event computed dx from the original click position:
Click at x=400
Event at x=410 → dx = 410-400 = 10 → spin += 0.1°
Event at x=420 → dx = 420-400 = 20 → spin += 0.2°  ← cumulative distance!
Event at x=430 → dx = 430-400 = 30 → spin += 0.3°
Event at x=450 → dx = 450-400 = 50 → spin += 0.5°
Total spin added across 5 events: 1.1°  (10+20+30+40+50 × 0.01)

After fix — mouse_x updates after each event. Each event only gets the incremental delta:
Click at x=400
Event at x=410 → dx = 10 → spin += 0.1°
Event at x=420 → dx = 10 → spin += 0.1°  ← just the step
Event at x=430 → dx = 10 → spin += 0.1°
Event at x=450 → dx = 10 → spin += 0.1°
Total spin added across 5 events: 0.5°  (10+10+10+10+10 × 0.01)

Same mouse distance (400→450 = 50px), but the old approach summed 10+20+30+40+50 = 150px worth of rotation while the new correct approach sums 10×5 = 50px. That's a 3× difference in speed for this example — more events = bigger gap.
*/
int	handle_mouse_move(int x, int y, void *param)
{
	t_data	*data;

	data = (t_data *)param;
	if (data->mlx_obj->is_clickhold == 0)
		return (0);
	if (data->mlx_obj->is_clickhold == 1)
		mouse_pan(data, x, y);
	else if (data->mlx_obj->is_clickhold == 3)
		mouse_rotate(data, x, y);
	// else it will accumulates the difference as we move further from first click
	data->mlx_obj->mouse_x = x;
	data->mlx_obj->mouse_y = y;
	data->img_obj->is_dirty = 1;
	return (0);
}
