/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 14:23:27 by kong              #+#    #+#             */
/*   Updated: 2026/05/05 16:57:15 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

t_data	*init_data(void)
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

t_mlx	*setup_mlx(void)
{
	t_mlx	*mlx_obj;

	mlx_obj = malloc(sizeof(t_mlx));
	if (!mlx_obj)
		return (perror("FdF"), NULL);
	mlx_obj->conn_ptr = mlx_init();
	if (!mlx_obj->conn_ptr)
		return (perrmsg("FdF: mlx_init error"), NULL);
	mlx_obj->win_ptr = mlx_new_window(mlx_obj->conn_ptr,
		WIN_WIDTH, WIN_HEIGHT, "FdF");
	if (!mlx_obj->win_ptr)
		return (freemlx(mlx_obj), NULL);
	mlx_obj->is_clickhold = 0;
	mlx_obj->mouse_x = 0;
	mlx_obj->mouse_y = 0;
	return (mlx_obj);
}

void	setup_events(t_data *data)
{
	void	*win_ptr;

	win_ptr = (void *)data->mlx_obj->win_ptr;
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
	// detect dirty image and perform rerender
	mlx_loop_hook(data->mlx_obj->conn_ptr, handle_loop_hook, data);
}

int	fdf(char *file)
{
	t_data	*data;

	data = init_data();
	if (!data)
		return (EXIT_FAILURE);
	data->mlx_obj = setup_mlx();
	if (!data->mlx_obj)
		return (freeprogexit(data, EXIT_FAILURE));
	// read map
	data->map_obj = read_map_file(file);
	if (!data->map_obj)
		return (freeprogexit(data, EXIT_FAILURE));
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
*/

/* ### WRITE COLOR INTO MEMORY ###
pixel_ptr is char *, so every pointer moves by 1 byte. 
	so 'offset' value should be pass as byte.
	we cast to int * because each pixel is 4 bytes (RGB).
	it tells compiler to read/write 4 bytes instead of 1.
why we cast later? because `pixel_ptr` works with 1 byte,
	- so using 1-byte, we shift the pointer to the correct position
	- cast to 4-bytes pointer, and insert with dereference (auto treat as 4 bytes write)
	
Endianness — same machine, same byte order
The endian field from mlx_get_data_addr tells you what byte order the X display expects. When you write:
*(unsigned int *)(pixel_ptr + offset) = color;
You're writing a 32-bit integer in the CPU's native byte order. The X server runs on the same machine with the same native byte order. Both your program and X read those 4 bytes identically — no conversion needed.
Endianness would matter if you wrote byte by byte manually (then you'd need to decide which byte goes first). Writing as a full unsigned int lets the CPU handle it, and since display and program share the same architecture, they agree automatically.
*/

/* ### POINTER CONCEPT ###
- working with List (pointer to pointer of struct):
	- because it only stores pointer, we could generalize it with (void *), 
		with size = 8-bytes.
	- and use NULL as stopper. Because it is only used to store mem address.
- working with Array (pointer to struct):
	- first know that we cannot use NULL as stopper because it can be
		valid data to represent part of the struct, hence,
		we have to treat struct as one block.
	- i.e. if we intend to copy structs array, we have to know beforehand
		the entire size of struct array.
	- (TYPELESS / GENERIC APPROACH)
		- Total bytes = size of struct * no. of struct obj in the array
		- then, we could define an `unsigned char *new_arr` and while loop
		through the total byte sizes to do a byte-by-byte copy,
		and we also making sure that we are copying the entire block of struct.
	- (STRONGLY-TYPE APPROACH)
		- on the other hand, we define the exact struct type that the function is 
		going to work with.
		- In this case, when we increment the pointer, compiler knows exactly
		how much bytes to skip for this struct type.
		- so, we only need to while loop through the number of elements 
		in this array only.
	
- the key distinction is: are we allocating memory to 
	- deal with unknown size of struct? just like a `char`, `int`
	- or fix size of `void *` type to only stores -- pointer address?
- side note, we need to do a strndup from a reference string, if it is meant
	to have multiple copies of it with each individual changes.
	We need individual copies for different sets of operation.
*/

/* ### ZOOM CONCEPT ###
The goal is to compute new anchor position as we apply new zoom while keeping
the W(x, y) under cursor fix.

The way to derive each screen position from given W(x,y) is:
(1)>> screen = (W(x,y) * f_zoom) + anchor
So, to get the point at cursor, we rearrange the formula:
(2)>> W(x,y) = (screen|cursor - anchor) / f_zoom_old

In order to keep this point fix, we need a correspond new anchor position.
So, from (1), we rearrange:
(3)>> new_anchor = screen|cursor - (W(x,y) * f_zoom_new)
substituting (2) into (3):
(4)>> new_anchor = screen|cursor - {(2) * f_zoom_new}
		= cursor - {(cursor - old_anchor) * (fzoomnew / fzoomold)}
*/

/*
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
