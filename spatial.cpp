#ifndef spatial_cpp
#define spatial_cpp


struct coord {
    int x = 0;
    int y = 0;
};

struct dimension {
    int width = 0;
    int height = 0;
};

struct bounds {
    coord topleft;
    coord bottomright;
    struct dimension dimension;
};

int coord_intersects_bounds(struct coord pt, struct bounds bds){
    if (pt.x >= bds.topleft.x && pt.x <= bds.bottomright.x && pt.y >= bds.topleft.y && pt.y <= bds.bottomright.y){
        return 1;
    } else {
        return 0;
    }
}

struct coord center_of_bounds(struct bounds bds){
    struct coord cent;
    cent.x = bds.topleft.x + (bds.dimension.width / 2);
    cent.y = bds.topleft.y + (bds.dimension.height / 2);
    return cent;
};

int bounds_intersect(struct bounds bds1, struct bounds bds2){
    struct coord topright;
    topright.x = bds1.bottomright.x;
    topright.y = bds1.topleft.y;
    struct coord bottomleft;
    bottomleft.x = bds1.topleft.x;
    bottomleft.y = bds1.bottomright.y;
    if (coord_intersects_bounds(bds1.topleft,bds2)
            || coord_intersects_bounds(bds1.bottomright,bds2)
            || coord_intersects_bounds(topright,bds2)
            || coord_intersects_bounds(bottomleft,bds2)) {
                return 1;
            }
            //this might not cover bds2 fully inside of bds1
    return 0;
};


struct bounds bounds_offset(struct bounds bds, int offsetX, int offsetY){
    bds.topleft.x += offsetX;
    bds.topleft.y += offsetY;
    bds.bottomright.x += offsetX;
    bds.bottomright.y += offsetY;
    return bds;
}


#endif // spatial_cpp
