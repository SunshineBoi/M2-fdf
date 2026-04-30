/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 14:23:27 by kong              #+#    #+#             */
/*   Updated: 2026/04/30 18:59:17 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

t_data	*init_data()
{
	t_data	*new_data;

	new_data = malloc(sizeof(t_data));
	if (!new_data)
		return (perror("FdF"), NULL);
	new_data->map_obj = NULL;
	new_data->mlx_obj = NULL;
	new_data->vp_obj = NULL;
	new_data->img_obj = NULL;
	return (new_data);
}

t_mlx	*setup_mlx()
{
	t_mlx	*mlx_obj;

	mlx_obj = malloc(sizeof(t_mlx));
	if (!mlx_obj)
		return (perror("FdF"), NULL);
	mlx_obj->conn_ptr = mlx_init();
	if (!mlx_obj->conn_ptr)
		return (print_errmsg("FdF: MLX init failure."), NULL);
	mlx_obj->win_ptr = mlx_new_window(mlx_obj->conn_ptr, WIN_WIDTH, WIN_HEIGHT, "FdF");
	if (!mlx_obj->win_ptr)
		return (freemlx(mlx_obj), NULL);
	mlx_obj->is_clickhold = 0;
	mlx_obj->mouse_x = 0;
	mlx_obj->mouse_y = 0;
	return (mlx_obj);
}


int	fdf(char *file)
{
	int		fd;
	t_data	*data;

	data = init_data();
	if (!data)
		return (EXIT_FAILURE);
	
	// setup MLX
	data->mlx_obj = setup_mlx();
	if (!data->mlx_obj)
		return (freeprogexit(data, EXIT_FAILURE));

	// read map
	fd = open_file_read(file);
	if (fd == -1)
		return (freeprogexit(data, EXIT_FAILURE));
	data->map_obj = build_map(fd);
	if (!data->map_obj)
		return (close(fd), freeprogexit(data, EXIT_FAILURE));
	close(fd);

	// setup viewpoint
	data->vp_obj = init_vp(data->map_obj);
	if (!data->vp_obj)
		return (freeprogexit(data, EXIT_FAILURE));

	data->img_obj = init_img(data->mlx_obj, data->vp_obj);
	if (!data->img_obj)
		return (freeprogexit(data, EXIT_FAILURE));

	// rendering
	if (render_img(data) == -1)
		return (freeprogexit(data, EXIT_FAILURE));

	// hook
	setup_events(data);
	mlx_loop_hook(data->mlx_obj->conn_ptr, loop_hook, data);

	// start listening
	mlx_loop(data->mlx_obj->conn_ptr);
	return (EXIT_SUCCESS);
}

int	main(int ac, char **av)
{
	if (ac != 2)
		errexit("FdF: Error: Wrong argument counts", EXIT_FAILURE);
	return (fdf(av[1]));
}

int main_test(void)
{
	void *mlx;
	void *mlx_winm;

	mlx = mlx_init();
	if (!mlx)
		return (EXIT_FAILURE);
	mlx_winm = mlx_new_window(mlx, 500, 500, "hello window!");
	mlx_pixel_put(mlx, mlx_winm, 250, 250, rand() % 0x1000000);
	mlx_loop(mlx);

	mlx_destroy_display(mlx); // because mlx_init malloc a display
	free(mlx);
	return (0);
}

/*
compile: cc src/main.c -Iinclude -Iminilibx-linux minilibx-linux/libmlx.a -lXext -lX11 -lm

testing:
- in so_long, there is possibilities where permission denied to images cause mem leaks
	could there be similar failure path.

	
https://gontjarow.github.io/MiniLibX/mlx-tutorial-create-image.html
The “proper” way to draw the image according to the manual is to:

1. Check how many bits there are per pixel --> how?
2. Convert your color with mlx_get_color_value if necessary.
3. Check whether the environment is little/big endian. --> how?
4. Write your color value byte-by-byte into the pixel array, according to endianness.
	- this pixel array is returned by `mlx_get_data_addr`

### MLX Error ###
Reading the valgrind error — line by line
|  Syscall param writev(vector[0]) points to uninitialised byte(s)  |
The OS syscall writev (like write but takes multiple buffers) received a buffer where some bytes were never assigned a value. Valgrind tracks every byte's initialisation state and flags this.
Now read the call stack bottom-up to trace who caused it:
by 0x10C11C: main          (main.c:95)        ← your code
by 0x10BF79: fdf           (main.c:56)        ← your code
by 0x10BF12: setup_mlx     (main.c:39)        ← your code calls mlx_new_window
by 0x10D8D5: mlx_new_window                   ← MLX: creates window, then waits
by 0x10DC65: mlx_int_wait_first_expose        ← MLX: waits for first paint event
by 0x48BC52B: XWindowEvent                    ← X11: blocks waiting for event
by 0x48BC148: _XReadEvents                    ← X11: reads events from server
by 0x48B70B8: _XSend                          ← X11: flushes pending data to server
by 0x4CBDD7E: xcb_writev                      ← XCB: sends data as iovec buffers
at 0x4BC5894: writev                          ← kernel syscall: HERE is the fault
What MLX is doing wrong: mlx_new_window calls mlx_int_wait_first_expose, which blocks until the X server sends back a paint event. While waiting, X11 needs to flush its outgoing buffer to the server via _XSend → xcb_writev → writev. That outgoing buffer lives inside the Display* struct allocated by XOpenDisplay during mlx_init:
|   Address 0x4d1a5ac is 28 bytes inside a block of size 16,384 alloc'd  |
|   by 0x48A642D: XOpenDisplay    ← X11 allocated this 16KB block        |
X11 allocated 16KB for the display connection buffer but left 28 bytes within it uninitialised. When _XSend packs X protocol data into that buffer to send to the server, those uninitialised bytes get included. Valgrind catches it at writev.
Why both maps show identical errors: The callstack and address are exactly the same in both runs. This happens once per mlx_new_window call regardless of which map you load. It's purely an MLX/X11 initialisation issue, triggered before your map data is ever touched.

It lives 28 bytes inside the Display * struct that XOpenDisplay allocates internally. Display is an opaque type — X11 intentionally hides its internals. There is no field you can reach from user code. Even MLX can't touch it because it comes from libX11 itself.
The suppression file is genuinely the only workaround short of patching libX11 source. There's no pointer you can dereference to zero-initialise it.
*/


