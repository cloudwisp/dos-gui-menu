#ifndef SpriteUtils_cpp
#define SpriteUtils_cpp

struct SpriteBoundingBox {
    int x1;
    int y1;
    int x2;
    int y2;
};

class SpriteUtils {
    public:
    static SpriteBoundingBox BlitSource(int spriteId, int spriteWidth, int spriteHeight, int tilesWide);
};

SpriteBoundingBox SpriteUtils::BlitSource(int spriteId, int spriteWidth, int spriteHeight, int tilesWide){
    int tileX, tileY, subX, subY;
    tileX = spriteId % tilesWide;
    tileY = (spriteId / tilesWide);
    subX = tileX * spriteWidth;
    subY = tileY * spriteHeight;
    SpriteBoundingBox rslt = {
        subX,
        subY,
        subX+spriteWidth-1,
        subY+spriteHeight-1
    };
    return rslt;
};

#endif