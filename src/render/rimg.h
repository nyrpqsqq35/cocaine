#ifndef MENU_RIMG_H
#define MENU_RIMG_H

#include <gl.h>

#include <cstdint>

typedef struct OurTex {
  GLuint tex;
  int width;
  int height;
} OurTex;

namespace render {
  namespace tex {
    extern OurTex ares;
    extern OurTex bucky;
    extern OurTex bulldog;
    extern OurTex classic;
    extern OurTex frenzy;
    extern OurTex ghost;
    extern OurTex guardian;
    extern OurTex judge;
    extern OurTex marshal;
    extern OurTex melee;
    extern OurTex odin;
    extern OurTex oper8or;
    extern OurTex phantom;
    extern OurTex sheriff;
    extern OurTex shorty;
    extern OurTex spectre;
    extern OurTex stinger;
    extern OurTex vandal;
  }  // namespace tex
  bool loadTexture(const uint8_t* data, int len, _Out_ OurTex* outTex);
  void loadTextures();
}  // namespace render

#endif /* MENU_RIMG_H */
