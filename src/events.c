/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 13:34:19 by kong              #+#    #+#             */
/*   Updated: 2026/04/30 21:14:14 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

int	handle_close(void *param)
{
	t_data	*data;

	data = (t_data *)param;
	return (freeprogexit(data, EXIT_SUCCESS));
}

int	handle_resize(void *param)
{
	t_data				*data;
	XWindowAttributes	attr;
	t_win_list			*win;
	t_xvar				*conn;

	data = (t_data *)param;
	conn = (t_xvar *)data->mlx_obj->conn_ptr;

	// win_ptr is a `t_win_list *` (pointer to MLX wrapper struct)
	// so to access this `Window` type, we have to access the internal struct first.
	win = (t_win_list *)data->mlx_obj->win_ptr;
	if (!XGetWindowAttributes(conn->display, win->window, &attr))
		return (freeprogexit(data, EXIT_FAILURE));

    // 1. Update stored width/height
	data->vp_obj->win_width = attr.width;
	data->vp_obj->win_height = attr.height;
	// 2. Recompute all required viewpoint params
	set_vp(data, data->vp_obj->spin_deg, data->vp_obj->pitch_deg);

	// 3. destroy old image and create new image because of buffer size.
	if (rerender_img(data) == -1)
		return (freeprogexit(data, EXIT_FAILURE));
	return (0);
}

int	handle_keyboard(int keycode, void *param)
{
	t_data	*data;

	data = (t_data *)param;
	// esc key
	if (keycode == XK_Escape)
		return (freeprogexit(data, EXIT_SUCCESS));
	// 'r' key
	if (keycode == XK_r)
	{
		iso_vp(data->vp_obj, data->map_obj->n_row, data->map_obj->n_col);
		data->img_obj->is_dirty = 1;
	}
	return (0);
}

int	handle_mouse_click(int button, int x, int y, void *param)
{
	t_data	*data;
	float	f_zoom;

	data = (t_data *)param;
	// panning with left button
	if (button == 1)
	{
		data->mlx_obj->is_clickhold = 1;
		data->mlx_obj->mouse_x = x;
		data->mlx_obj->mouse_y = y;
	}
	if (button == 3)
	{
		data->mlx_obj->is_clickhold = 3;
		data->mlx_obj->mouse_x = x;
		data->mlx_obj->mouse_y = y;
	}
	// scroll up
	if (button == 4)
	{
		f_zoom = 1.1f;
		data->vp_obj->x_win_anchor = x - ((x - data->vp_obj->x_win_anchor) * f_zoom);
		data->vp_obj->y_win_anchor = y - ((y - data->vp_obj->y_win_anchor) * f_zoom);
		data->vp_obj->f_zoom *= f_zoom;
		data->img_obj->is_dirty = 1;
	}
	// scroll down
	if (button == 5)
	{
		f_zoom = 1.0f / 1.1f;
		data->vp_obj->x_win_anchor = x - ((x - data->vp_obj->x_win_anchor) * f_zoom);
		data->vp_obj->y_win_anchor = y - ((y - data->vp_obj->y_win_anchor) * f_zoom);
		data->vp_obj->f_zoom *= f_zoom;
		data->img_obj->is_dirty = 1;
	}
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

int	handle_mouse_move(int x, int y, void *param)
{
	t_data	*data;
	int		d_mouse_x;
	int		d_mouse_y;

	data = (t_data *)param;
	if (data->mlx_obj->is_clickhold == 0)
		return (0);
	d_mouse_x = x - data->mlx_obj->mouse_x;
	d_mouse_y = y - data->mlx_obj->mouse_y;
	if (data->mlx_obj->is_clickhold == 1)
	{
		// anchored new point - distance from last mouse pos to new pos
		data->vp_obj->x_win_anchor += d_mouse_x;
		data->vp_obj->y_win_anchor += d_mouse_y;
		// update mouse pos
		data->mlx_obj->mouse_x = x;
		data->mlx_obj->mouse_y = y;
	}
	else if (data->mlx_obj->is_clickhold == 3)
	{
		data->vp_obj->spin_deg -= d_mouse_x * 0.01f;
		data->vp_obj->pitch_deg -= d_mouse_y * 0.01f;
		if (data->vp_obj->pitch_deg > 89.0)
			data->vp_obj->pitch_deg = 89.0;
		if (data->vp_obj->pitch_deg < -89.0)
			data->vp_obj->pitch_deg = -89.0;
	}
	data->img_obj->is_dirty = 1;
	return (0);
}

int	loop_hook(void *param)
{
	t_data	*data;

	data = (t_data *)param;
	if (!data->img_obj->is_dirty)
		return (0);
	if (render_img(data) == -1)
		return (freeprogexit(data, EXIT_FAILURE));
	data->img_obj->is_dirty = 0;
	return (0);
}

void	setup_events(t_data *data)
{
	void *win_ptr = data->mlx_obj->win_ptr;

	// DestroyNotify - mouse click on window close button
	mlx_hook(win_ptr, DestroyNotify, NoEventMask, handle_close, data);

	// ConfigureNotify - resizing window size
	mlx_hook(win_ptr, ConfigureNotify, StructureNotifyMask, handle_resize, data);

	// pan + zoom
	mlx_hook(win_ptr, ButtonPress, ButtonPressMask, handle_mouse_click, data);
	mlx_hook(win_ptr, ButtonRelease, ButtonReleaseMask, handle_mouse_release, data);
	mlx_hook(win_ptr, MotionNotify, PointerMotionMask, handle_mouse_move, data);

	// shorthand for event 2 - KeyPress
	mlx_key_hook(win_ptr, handle_keyboard, data);

	// shorthand for event 4 - ButtonPress
	// mlx_mouse_hook(data->mlx_obj->win_ptr, handle_mouse, data);
}

/*
* How:
Whenever an event has occured, eg. a button is pressed. The `mlx_hook` gets fired up --> it calls our handler, and our handler is responsible to do certain action on specific button pressed.

* Definition for each event handler:
1. Key press / release - mlx_key_hook()
? > int  fn(int keycode, void *param)
2. Mouse button press / release - mlx_mouse_hook()
? > int  fn(int button, int x, int y, void *param)
3. Mouse movement
? > int  fn(int x, int y, void *param)
4. Window close (red X) - no extra args
? > int  fn(void *param)
All hooks must return int (return 0 conventionally)

* KEY CODE:
PATH: 
	\\wsl.localhost\Ubuntu\usr\include\X11\keysymdef.h
- keycode tells you which physical key is pressed.

* EVENT CODE:
PATH: 
	\\wsl.localhost\Ubuntu\usr\include\X11\X.h
- example, key is pressed, released; mouse is clicked, moved.


* Idea:
- press 'v' to go back default view
- press 'esc' to quit
- mouse button to move view
- mouse button to zoom in out
- loop hook to allows slow rotation


* mlx_loop iteration with Flag:
  → process queued events (handlers run instantly, just set flag=1)
  → call loop_hook → needs_render=1 → render_img (4s, blocks everything)
  → during those 4s: 50 mouse events queue up, each sets flag=1 (already 1, no effect)
  → render_img finishes → flag=0 → loop_hook returns
  → mlx_loop processes 50 queued events instantly → flag=1 again
  → call loop_hook → renders ONCE for all 50 events combined

*/