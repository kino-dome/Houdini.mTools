# mPointCloudSkinner
**Meshes(skins) a pointcloud but at the same time keeps the original points. Can produce normals for pointclouds if needed (using the excellent method mentioned on [this odforce thread](http://forums.odforce.net/topic/13655-point-cloud-normals/))**

The process uses Houdini's own surfacing methods to produce a skinned model and then it transfers, triangulates and cleans the initial points on the surfaced model. You can output all the different phases of the process, Points, Iso(Houdini Skinning) and Skinned (final result). There's a field for keeping selected attributes from the initial pointclooud for further use.  The sample "sample_headWithDensity" uses this feature and a technique learned from Ben Watt's great guest tutorial for Entagma (found [here](https://vimeo.com/197596529)) to grow a tree-like pattern on a head's surface, with the option of having different density of points on the surface. You can Also unique points and have access to Iso's basic parameters.

Can produce wireframes easily by appending a "convertline" SOP.
