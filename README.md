*This project has been created as part of the 42 curriculum by kong.*

## Description
### The Goal
FdF (French for *fil de fer* — "wire frame") is a graphical project from the 42 curriculum. The goal is to read a map file of elevation values and render it as an interactive isometric wireframe in a window, using the MiniLibX graphics library.

Each point in the map represents a vertex on a 3D grid. Vertices are projected onto a 2D screen using isometric projection and connected by lines. The result is a wire-frame terrain that can be explored interactively.

The project covers: parsing structured text input; 3D-to-2D coordinate transformation; line drawing with Bresenham's algorithm; colour interpolation; and event-driven programming with MiniLibX on X11.

## Instructions
### Compilation
This project includes a `Makefile` to automate the compilation process. To compile the executable, simply run:

```bash
make
```

### Execution
This will generate the executable:
```bash
./fdf path/to/map.fdf
```

In addition, use `make clean` to remove object files, `make fclean` to remove all build artefacts, and `make re` to recompile from scratch.

### Map Format

Maps are plain text files where each row is a space-separated list of integer elevation values:

```
0 0 0 1 2 3
0 0 1 2 3 4
0 1 2 3 4 5
```

A colour can optionally be attached to any value using `z,0xRRGGBB` format:

```
1,0x810202 1,0x810202 5,0xFF4400
```

### Controls

| Input | Action |
|---|---|
| Left click + drag | Pan |
| Right click + drag | Rotate (spin + pitch) |
| Scroll up | Zoom in towards cursor |
| Scroll down | Zoom out from cursor |
| ESC | Exit |
| Window × button | Exit |

## Feature List
- **Isometric projection** with adjustable spin and pitch angles
- **Per-pixel colour gradient** — base colour at z=0, interpolated towards user-defined colours at positive and negative extremes; per-pixel z interpolation along each line for smooth transitions
- **Zoom to cursor** — scroll wheel zooms towards the point under the cursor
- **Auto-zoom** — initial zoom and centering scale automatically to fit map size in the window
- **Window resize** — image buffer recreates at the new dimensions and the view is preserved proportionally
- **Dirty-flag render loop** — the scene only redraws when state has changed, keeping interaction responsive
- **Clean exit** — all memory is freed on ESC, window close

## Technical Choices

The renderer works as a multi-stage pipeline. Each stage transforms the data into a form the next stage can consume, from raw text to pixels on screen.

### 1. Map Parsing

The input file is read line by line using `get_next_line`. Each row is split on spaces into tokens. Each token is parsed as a decimal integer `z`; if a comma is present the hex suffix is parsed as an explicit RGB colour. The result is a 2D array of `t_coord` structs holding `(col, row, z, color)`. Row and column indices become the logical x and y grid coordinates before any projection is applied.

### 2. Elevation Range Scan (z_min / z_max)

Before any rendering can begin, a full pass over every coordinate establishes the global minimum and maximum z values. These bounds are the denominator for every subsequent normalisation calculation — without them the gradient has no reference frame.

### 3. Z Normalisation and Scaling

Raw z integers can span arbitrary ranges. They are normalised to a `[0, 1]` float using:

```
f = (z_raw - z_min) / (z_max - z_min)
```

A configurable scale factor then stretches this into world-space height. This decouples the visual height of terrain from whatever integers happened to be in the file, and ensures maps with small elevation deltas still produce visible relief.

### 4. Vertex Colour Assignment

Each point's display colour is computed once, before drawing, and stored on the point itself. The logic is:

- If the point carries an explicit `0xRRGGBB` colour code, that value is used as-is.
- If `z > 0`, the colour is interpolated between `COLOR_BASE` (white, at z=0) and `COLOR_POSITIVE` using the normalised z fraction.
- If `z < 0`, the colour is interpolated between `COLOR_BASE` and `COLOR_NEGATIVE` using the normalised z fraction.
- If `z == 0`, the point is `COLOR_BASE`.

### 5. Rotation — Spin and Pitch

To give the flat grid a three-dimensional appearance, two successive rotations are applied in world space before projection.

**Spin** rotates around the vertical (z) axis by angle θ:

```
x' =  x·cos θ − y·sin θ
y' =  x·sin θ + y·cos θ
```

**Pitch** tilts the result around the horizontal (x') axis by angle φ:

```
y'' =  y'·cos φ − z·sin φ
```

Together these two angles determine the viewing direction.

### 6. Isometric Projection (3D → 2D)

This is the projection step — it collapses a 3D point into a 2D plane. The default spin and pitch are chosen to produce a classic isometric view (approximately 30° elevation, 45° lateral), but the user can adjust both at runtime.

### 7. Zoom Factor

The projected coordinates are in a normalised world space. Multiplying both x and y by a zoom scalar converts them to pixel distances. A larger zoom factor spreads points further apart; a smaller one compresses the map. The initial zoom is chosen automatically so the full map fits within the window with a small margin.

### 8. Screen Anchor — Converting to Pixel Coordinates

The anchor is a 2D point in pixel space that defines where the origin of the world coordinate system sits on screen. The final pixel position of any projected, zoomed point is:

```
pixel_x = anchor_x + world_x · zoom
pixel_y = anchor_y + world_y · zoom
```

At startup the anchor is set to the window centre so the map is centred. During pan, the anchor is translated directly. During zoom-to-cursor, the anchor is updated so that the point under the cursor remains stationary — achieved by scaling the vector from anchor to cursor by the zoom ratio and recomputing the anchor position.

### 9. Frame Buffer and Dirty-Flag Loop

MiniLibX exposes a raw pixel buffer (an `mlx_image`). Rather than drawing directly to the window, all pixels are written into this buffer and the buffer is pushed to the display in a single `mlx_put_image_to_window` call. A dirty flag tracks whether any interactive state (position, rotation, zoom) has changed since the last frame. The render loop only clears and redraws the buffer when the flag is set. Before each redraw the entire buffer is filled with the background colour to erase the previous frame.

### 10. Edge Iteration

Each point in the grid is the potential start of two line segments: one to its right neighbour (same row, next column) and one to its bottom neighbour (next row, same column). Iterating over every point and drawing these two segments covers every edge in the wireframe exactly once, without duplication or omission.

### 11. Bresenham's Line Algorithm

Each line segment is rasterised using Bresenham's algorithm. The key insight is that for any line, only the major axis (the longer dimension) advances by exactly one pixel per step. The minor axis advances conditionally, controlled by an accumulating error term that avoids any floating-point arithmetic. The implementation is generalised across all eight octants by normalising the direction of travel at the start of each line.

### 12. Per-Pixel Colour Interpolation

As Bresenham steps along a line, the current z value is interpolated between the start and end z values using the step ratio:

```
z_curr = z_start + (z_end − z_start) × (step / total_steps)
```

If both endpoints share the same z, the colour is interpolated linearly between the two endpoint colours using the same ratio. If they differ, `z_curr` is passed back through the elevation gradient logic (`COLOR_NEGATIVE → COLOR_BASE → COLOR_POSITIVE`) to determine the pixel colour. This produces smooth colour transitions along every edge.

### 13. Pixel Write

Each computed colour is written directly into the raw image buffer at the address corresponding to the pixel's (x, y) position:

```
buffer[y · line_length + x · bytes_per_pixel] = color_as_int
```

A bounds check guards every write against coordinates outside the window — projected points near the edge or extreme zoom levels can produce off-screen coordinates that would otherwise corrupt memory.

### 14. Zoom — Interaction Model

When the scroll wheel moves, the zoom factor is multiplied by a fixed ratio (zoom in) or divided (zoom out). Simultaneously the anchor is adjusted so that the screen position under the cursor does not move:

```
new_anchor = cursor − (cursor − old_anchor) × new_zoom / old_zoom
```

This makes zooming feel natural — the terrain expands or contracts around the pointer, not around an arbitrary screen centre.

### 15. Pan — Interaction Model

On left-button press, the cursor position is recorded. On each subsequent mouse-move event while the button is held, the delta between the current and previous cursor positions is added directly to the anchor. On button release, the delta tracking stops. Because the anchor is in pixel space, pan speed is constant regardless of zoom level.

### 16. Rotation — Interaction Model

Right-button drag works the same way as pan but instead of translating the anchor, the horizontal delta is added to the spin angle and the vertical delta is added to the pitch angle. Each drag event increments the angles by a small fraction of the pixel delta, converted to radians. The dirty flag is set and the scene is redrawn from the rotated world coordinates on the next loop iteration.


## Resources
### Documentation & References

- [MiniLibX documentation (42 unofficial)](https://harm-smits.github.io/42docs/libs/minilibx) — MiniLibX API reference and usage guide
- [Bresenham's Line Algorithm — Wikipedia](https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm) — Integer line drawing, generalised to all octants
- [Isometric projection — Wikipedia](https://en.wikipedia.org/wiki/Isometric_projection) — Mathematical basis for the 3D-to-2D transform used
- [Minilibx Article](https://medium.com/@jalal92/understanding-the-minilibx-a-practical-dive-into-x-window-programming-api-in-c-cb8a6f72bec3)
- [Minilibx Tutorial](https://gontjarow.github.io/MiniLibX/?source=post_page-----7184cca317fc---------------------------------------)
- [Server X](https://medium.com/mindorks/x-server-client-what-the-hell-305bd0dc857f)

### AI Usage
AI tools were used as a learning assistant and code reviewer, specifically for:
- **Concept explanations** — Understanding topics that were new before starting: the X11 client-server model, how MiniLibX wraps Xlib, the inversion-of-control pattern in event loops, the mathematical derivation of Bresenham's algorithm across all octants, and the geometry of isometric projection.
- **Code review and debugging** — AI identified logic errors, naming issues, and edge cases (e.g. integer division truncation, unguarded divide-by-zero in colour interpolation, incorrect pointer casts into MiniLibX internals). All code was written by the student; AI reviewed and explained the problems rather than rewriting.
- **Design decisions** — Discussing architectural choices such as the dirty-flag render loop, the anchor-based pan/zoom/rotate system, and the zoom-to-cursor coordinate formula. AI helped reason through trade-offs but the decisions and implementations were the student's own.
- All core logic, implementation, and final decisions were written and verified manually.
