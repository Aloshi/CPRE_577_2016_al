The `GL_TRIANGLES` model uses the same vertices as shown in the `GL_TRIANGLE_STRIP` sketch. The only difference is that I manually expanded the triangle strips into a series of triangles. This is trivial for the 4-point faces:

```
  3-----4
  | \   |
  |   \ |
  1-----2
```

Becomes a two triangles, using vertices `1, 2, 3` and `3, 2, 4`.

The slightly more interesting surface:

```
  3---5
  |      \
  |          4
  |          |
  1----------2
```

Becomes three triangles, `1, 2, 3`, `3, 2, 4`, and `3, 4, 5`.