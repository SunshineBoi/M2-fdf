# FdF — Comprehensive Test Plan

---

## Part 1 — Edge-Case Map Files

All files are in `fdf_tests/`. Run each as `./fdf fdf_tests/<file>`.

| File | What it tests | Expected result |
|---|---|---|
| `single_point.fdf` | 1×1 map | Single dot rendered at centre, no lines |
| `single_row.fdf` | 1×N map | One horizontal row of connected points, no vertical lines |
| `single_col.fdf` | N×1 map | One vertical column of connected points, no horizontal lines |
| `two_by_two.fdf` | Smallest grid with all 4 edge directions | 4 points, 4 lines forming a square + 2 diagonals? (check your do_render loop bounds) |
| `flat.fdf` | All z=0 | Flat grid, all points same colour (white/base), no z gradient |
| `flat_nonzero.fdf` | All z=5, z_min==z_max | Flat grid — tests the z_min==z_max guard in `_normalize_z`. No divide-by-zero crash |
| `flat_one.fdf` | All z=1, z_min==z_max | Same guard, different value |
| `all_negative.fdf` | All z < 0 | Negative gradient only, no positive colour |
| `mixed_z.fdf` | Spans negative through positive | Tests colour transition through z=0 (base colour). Gradient should shift across the midpoint |
| `spike.fdf` | One extreme high point, rest zero | Tests colour gradient at one extreme; lines from 0→50 should interpolate full range |
| `pit.fdf` | One extreme low point, rest zero | Same for negative extreme |
| `extreme_z_max.fdf` | z = INT_MAX (2147483647) | No overflow, renders cleanly |
| `extreme_z_min.fdf` | z = INT_MIN (-2147483648) | No overflow, `ft_abs(INT_MIN)` is undefined behaviour — check your abs call |
| `all_colors.fdf` | All cells have explicit colour | No gradient applied; exact colours shown |
| `mixed_colors.fdf` | Some cells explicit colour, some sentinel (-1) | Explicit colour cells use their value; unlabelled cells use gradient |
| `neg_with_color.fdf` | Negative z + explicit colour | Explicit colours should override gradient even for negatives |
| `wide.fdf` | 2 rows × 10 cols | Tests non-square auto-zoom, no distortion |
| `tall.fdf` | 10 rows × 2 cols | Same for portrait aspect |
| `ragged.fdf` | Rows of different lengths | Program should either handle gracefully or exit with a clear error — no crash, no UB |
| `empty.fdf` | Empty file (0 bytes) | Clean error message, no crash |
| `whitespace_only.fdf` | Only newlines | Same — no crash, no infinite loop in GNL |

---

## Part 2 — Error Handling & Launch

Run these before even seeing a render.

```
./fdf                          # no args           → usage message, exit cleanly
./fdf too many args here       # extra args         → error or ignore extras, no crash
./fdf nonexistent.fdf          # file not found     → perror or custom error, exit cleanly
./fdf /etc/passwd              # not a map file     → either parse error or graceful exit
./fdf fdf_tests/empty.fdf      # empty file         → clean error, no crash
./fdf fdf_tests/ragged.fdf     # malformed rows     → defined behaviour, no segfault
```

Also check: does your program leak the fd if `build_map` fails? Open the file → parse error → `close(fd)` must still be called.

---

## Part 3 — Manual Interaction Checklist

Launch with a medium map (`./fdf maps/42.fdf` or similar). Go through each item.

### Rendering

- [ ] Map is centred on first launch (not stuck in a corner)
- [ ] All lines are drawn — no missing edges at map boundary
- [ ] Colour gradient is visible: low z → negative colour, z=0 → base (white), high z → positive colour
- [ ] Lines with mixed z interpolate smoothly — no sudden colour jump mid-line
- [ ] No stray black pixels on non-black background (check zoom-out small, check spike maps)
- [ ] Resize window → image redraws at new size, no artefacts, no crash

### Pan

- [ ] Left-click + drag moves the map
- [ ] Direction matches drag direction (drag right → map moves right)
- [ ] Releasing mouse stops movement (no momentum drift)
- [ ] Pan + zoom together: panning after zooming moves at consistent visual speed

### Rotate

- [ ] Right-click + drag rotates the map
- [ ] Drag right → map spins clockwise (or consistent direction)
- [ ] Drag left → spins opposite direction
- [ ] Drag up/down → pitch changes (map tilts)
- [ ] Pitch is clamped at ±89° — cannot flip map upside-down
- [ ] Releasing stops rotation

### Zoom

- [ ] Scroll up zooms in, scroll down zooms out
- [ ] Zoom is centred on cursor position, not screen centre — place cursor off-centre and verify
- [ ] Zoom in + out returns approximately to original view
- [ ] Zoom very close → no crash, pixels just appear large
- [ ] Zoom very far out → map becomes tiny but no crash, no negative/wrap coordinates

### Exit

- [ ] ESC key closes and exits cleanly
- [ ] Clicking window × button closes cleanly
- [ ] Ctrl+C (SIGINT) closes cleanly — no "definitely lost" in valgrind
- [ ] None of the above leave zombie processes (`ps aux | grep fdf`)

---

## Part 4 — Memory & Crash Checks

### Valgrind run

```bash
valgrind --leak-check=full --track-origins=yes ./fdf maps/42.fdf
```

Expected clean output (exit via ESC):
```
definitely lost: 0 bytes in 0 blocks
indirectly lost: 0 bytes in 0 blocks
```

"Still reachable" from X11/MLX internals is acceptable (not your code).
The `writev` warning about XSizeHints is an MLX internal bug — suppress with a `.supp` file.

### Specific leak scenarios

- [ ] Launch → immediately ESC → no leaks
- [ ] Launch → resize window several times → ESC → no leaks (image buffer recreated each resize)
- [ ] Launch → zoom/pan/rotate → ESC → no leaks
- [ ] Launch with invalid file → exits → no leaks (fd closed, all allocs freed before exit)

### Crash scenarios

```bash
./fdf fdf_tests/extreme_z_min.fdf    # INT_MIN through ft_abs — UB risk
./fdf fdf_tests/single_point.fdf     # 1-point map — loop bounds check
./fdf fdf_tests/empty.fdf            # empty file
```

None of these should segfault. Run under valgrind for each.

---

## Part 5 — Unit-Level Logic Tests

These are not compiled test cases (42 doesn't expect a test harness), but you should be able to trace through these by reading the code or adding temporary `printf` debug output.

### `interpolate_color(c1, c2, f)`

| f | Expected |
|---|---|
| 0.0 | Returns c1 exactly |
| 1.0 | Returns c2 exactly |
| 0.5 | Each channel is the midpoint of c1 and c2 channels |
| -0.1 | Behaviour defined? Check for clamping or wrap |
| 1.1 | Same — does it overflow a channel byte? |

### `interpolate_color_from_base(z_curr, start, end)`

| z_start | z_end | z_curr | Expected |
|---|---|---|---|
| 5 | 5 | 5 | Returns `start.color` (z_start==z_end guard) |
| 0 | 0 | 0 | Returns `COLOR_BASE` |
| 10 | 10 | 10 | Returns `start.color` — not NaN black |
| 5 | 10 | 5 | Returns `start.color` (f=0) |
| 5 | 10 | 10 | Returns `end.color` (f=1) |
| 5 | 10 | 7.5 | Returns midpoint colour |
| -5 | 5 | 0 | Returns `COLOR_BASE` (crosses zero) |
| -5 | 5 | -2.5 | Interpolates between `COLOR_BASE` and `start.color` |
| -5 | 5 | 2.5 | Interpolates between `COLOR_BASE` and `end.color` |

### Bresenham `_draw`

| dx | dy | Expected |
|---|---|---|
| 0 | 0 | `color_pixel` called once for single point (handled before `_draw`) |
| 5 | 0 | 6 pixels drawn in a horizontal line |
| 0 | 5 | 6 pixels drawn in a vertical line |
| 3 | 3 | 45° diagonal, 4 pixels |
| 10 | 1 | Mostly horizontal, minor axis steps once |
| 1 | 10 | Mostly vertical, minor axis steps once |

Check: does `ratio` reach exactly 1.0 at the last step? (`step / ft_max(dx,dy)` → at `step == max_delta`, ratio = 1.0 ✓)

### `_normalize_z` / z scaling

| z_raw | z_min | z_max | f_zscale | Expected z_world |
|---|---|---|---|---|
| any | 5 | 5 | 2 | 0 (guard: no div by 0) |
| 5 | 0 | 10 | 1 | 0.5 |
| 0 | 0 | 10 | 2 | 0.0 |
| 10 | 0 | 10 | 2 | 2.0 |
| -5 | -10 | 0 | 1 | 0.5 |

### Zoom-to-cursor formula

Verify mentally: if cursor is at `(x, y)` and you zoom by factor `k`, the new anchor satisfies:

```
new_anchor = cursor - (cursor - old_anchor) * k
```

If cursor == old_anchor → new_anchor == old_anchor (zooming on anchor: no move). ✓
If cursor is right of anchor → anchor moves right (towards cursor). ✓

---

## Part 6 — Stress / Performance

```bash
./fdf maps/julia.fdf          # large map — does it eventually render?
./fdf maps/julia.fdf          # then drag mouse rapidly — does it coalesce correctly?
```

Expected: first render may be slow but the program doesn't hang. Interaction between renders remains responsive.

---

## Known Open Issue

**Black dots at low zoom** — small numbers of pixels may appear black under certain zoom/map combinations. Suspected cause: projected coordinates underflow or colour interpolation produces a 0x000000 value from a near-zero z. Check `color_pixel` bounds guard and whether `interpolate_color` can produce black for valid mid-range z values.
