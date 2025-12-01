#ifndef VOXEL_H
#define VOXEL_H

typedef struct {
    int x,y,z;
    int size;
} Voxel;

void CreateVoxel(int x , int y, int z, int size);

#endif
