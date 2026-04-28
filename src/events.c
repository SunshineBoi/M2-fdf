/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 13:34:19 by kong              #+#    #+#             */
/*   Updated: 2026/04/27 15:37:31 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

// int	handle_mouse(int button, int x, int y, void *param)
// {
// 	if (button == 1)
// 	{
// 		...
// 	}
// 	...
// }

int	handle_close(void *param)
{
	t_data *data;

	data = (t_data *)param;
	freeprog(data, EXIT_SUCCESS);
	exit(0);
}

int	handle_keyboard(int keycode, void *param)
{
	t_data	*data;

	data = (t_data *)param;
	// esc key
	if (keycode == 0xff1b)
	{
		freeprog(data, EXIT_SUCCESS);
		exit(0);
	}
	return (0);
}

void	setup_events(t_data *data)
{
	void *win_ptr = data->mlx_obj->win_ptr;

	// DestroyNotify - mouse click on window close button
	mlx_hook(win_ptr, 17, 0, handle_close, data);

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
*/