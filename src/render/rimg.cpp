#define STB_IMAGE_IMPLEMENTATION
#include "rimg.h"

#include <cocaine.h>
#include <gl.h>
#include <stb_image.h>

#include "generated/img_ares.h"
#include "generated/img_bucky.h"
#include "generated/img_bulldog.h"
#include "generated/img_classic.h"
#include "generated/img_frenzy.h"
#include "generated/img_ghost.h"
#include "generated/img_guardian.h"
#include "generated/img_judge.h"
#include "generated/img_marshal.h"
#include "generated/img_melee.h"
#include "generated/img_odin.h"
#include "generated/img_operator.h"
#include "generated/img_phantom.h"
#include "generated/img_sheriff.h"
#include "generated/img_shorty.h"
#include "generated/img_spectre.h"
#include "generated/img_stinger.h"
#include "generated/img_vandal.h"

namespace render {
  namespace tex {
    OurTex ares;
    OurTex bucky;
    OurTex bulldog;
    OurTex classic;
    OurTex frenzy;
    OurTex ghost;
    OurTex guardian;
    OurTex judge;
    OurTex marshal;
    OurTex melee;
    OurTex odin;
    OurTex oper8or;
    OurTex phantom;
    OurTex sheriff;
    OurTex shorty;
    OurTex spectre;
    OurTex stinger;
    OurTex vandal;
  }  // namespace tex
  bool loadTexture(const uint8_t* data, int len, _Out_ OurTex* outTex) {
    int iWidth = 0;
    int iHeight = 0;
    unsigned char* image_data =
        stbi_load_from_memory(data, len, &iWidth, &iHeight, nullptr, 4);
    if (image_data == nullptr) {
      DebugBreak();
      throw std::runtime_error("Image is corrupted 3:");
    }

    GLuint imageTex;
    glGenTextures(1, &imageTex);
    glBindTexture(GL_TEXTURE_2D, imageTex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);  // This is required on WebGL for non
                                        // power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    GL_CLAMP_TO_EDGE);  // Same

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image_data);

    stbi_image_free(image_data);
    outTex->tex = imageTex;
    outTex->width = iWidth;
    outTex->height = iHeight;

    return false;
  }

  void loadTextures() {
    loadTexture(IMG_ARES, IMG_ARES_size, &tex::ares);
    loadTexture(IMG_BUCKY, IMG_BUCKY_size, &tex::bucky);
    loadTexture(IMG_BULLDOG, IMG_BULLDOG_size, &tex::bulldog);
    loadTexture(IMG_CLASSIC, IMG_CLASSIC_size, &tex::classic);
    loadTexture(IMG_FRENZY, IMG_FRENZY_size, &tex::frenzy);
    loadTexture(IMG_GHOST, IMG_GHOST_size, &tex::ghost);
    loadTexture(IMG_GUARDIAN, IMG_GUARDIAN_size, &tex::guardian);
    loadTexture(IMG_JUDGE, IMG_JUDGE_size, &tex::judge);
    loadTexture(IMG_MARSHAL, IMG_MARSHAL_size, &tex::marshal);
    loadTexture(IMG_MELEE, IMG_MELEE_size, &tex::melee);
    loadTexture(IMG_ODIN, IMG_ODIN_size, &tex::odin);
    loadTexture(IMG_OPERATOR, IMG_OPERATOR_size, &tex::oper8or);
    loadTexture(IMG_PHANTOM, IMG_PHANTOM_size, &tex::phantom);
    loadTexture(IMG_SHERIFF, IMG_SHERIFF_size, &tex::sheriff);
    loadTexture(IMG_SHORTY, IMG_SHORTY_size, &tex::shorty);
    loadTexture(IMG_SPECTRE, IMG_SPECTRE_size, &tex::spectre);
    loadTexture(IMG_STINGER, IMG_STINGER_size, &tex::stinger);
    loadTexture(IMG_VANDAL, IMG_VANDAL_size, &tex::vandal);
  }
}  // namespace render