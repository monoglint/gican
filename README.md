# gican
- Game I Can't Actually Name

Terminology Notes:
- Engine
    2. region
        A region is a file that holds a 3d array of chunks. Regions are only used on the disk, and are ignored as an abstraction when chunks are loaded in RAM.
- Shared
    1. voxel
        The standard block unit in gican. Like a minecraft block.
    2. tensor
        A tensor represents a complete world of voxels. Tensors are split into chunks.
- Game
    1. sketch
        A sketch is the template for a voxel, essentially an unplaced block.