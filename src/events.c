/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 13:25:41 by kong              #+#    #+#             */
/*   Updated: 2026/05/05 15:19:45 by kong             ###   ########.fr       */
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
	t_win_list			*win_ptr;
	t_xvar				*conn;

	data = (t_data *)param;
	conn = (t_xvar *)data->mlx_obj->conn_ptr;

	// win_ptr is a `t_win_list *` (pointer to MLX wrapper struct)
	// so to access this `Window` type, we have to access the internal struct first.
	win_ptr = (t_win_list *)data->mlx_obj->win_ptr;
	if (!XGetWindowAttributes(conn->display, win_ptr->window, &attr))
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

int	handle_loop_hook(void *param)
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