# Normal mapping

[Normal mapping](https://en.wikipedia.org/wiki/Normal_mapping) is a technique aims to fake a high-accuracy surface on a low-accuracy mesh.
It uses a 2D texture (a lookup table) to record surface normals for a mesh.
Each point on the surface of the mesh has a corresponding normal in the table.
When computing the color for a fragment,
we use the normal from the table instead of the interpolated vertex normal.
As a result, we get a fake smooth surface with relatively low cost.

# Note

When computing specular, using `dot(H, N)` instead of `dot(R, V)` can obtain better result.
`H` is called halfway vector and `H = normalize(L, V)`.
Here, `N, R, V, L` denotes normal, reflect, view and light vector, respectively.

# Result
![normalMapping](./res/result.png)
