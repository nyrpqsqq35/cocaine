#ifndef CONFIG_COLOR_H
#define CONFIG_COLOR_H

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

class Color {
 public:
  Color();
  Color(int _r, int _g, int _b);
  Color(int _r, int _g, int _b, int _a);
  Color(float _r, float _g, float _b) : Color(_r, _g, _b, 1.0f) {}
  Color(float _r, float _g, float _b, float _a)
      : Color(static_cast<int>(_r * 255.0f), static_cast<int>(_g * 255.0f),
              static_cast<int>(_b * 255.0f), static_cast<int>(_a * 255.0f)) {}
  Color(std::string hex);
  explicit Color(float* rgb) : Color(rgb[0], rgb[1], rgb[2], 1.0f) {}
  explicit Color(unsigned long argb) {
    _u32color = argb;
    _u8color[2] = static_cast<uint8_t>((argb & 0x000000FF) >> (0 * 8));
    _u8color[1] = static_cast<uint8_t>((argb & 0x0000FF00) >> (1 * 8));
    _u8color[0] = static_cast<uint8_t>((argb & 0x00FF0000) >> (2 * 8));
    _u8color[3] = static_cast<uint8_t>((argb & 0xFF000000) >> (3 * 8));
  }

  void setRawColor(uint32_t color32);
  uint32_t getRawColor() const;
  void setColor(int _r, int _g, int _b, int _a = 0);
  void setColor(float _r, float _g, float _b, float _a = 0);
  void setAlpha(int a);
  void setAlpha(float a);
  void getColor(int& _r, int& _g, int& _b, int& _a) const;
  void getHexColor(std::string& str, bool alpha = true) const;
  std::string getHexColor(bool alpha = true) const;

  int r() const { return _u8color[0]; }
  int g() const { return _u8color[1]; }
  int b() const { return _u8color[2]; }
  int a() const { return _u8color[3]; }

  uint8_t& operator[](int index) { return _u8color[index]; }
  const uint8_t& operator[](int index) const { return _u8color[index]; }

  bool operator==(const Color& rhs) const;
  bool operator!=(const Color& rhs) const;
  Color& operator=(const Color& rhs);

  static Color fromHSB(float hue, float saturation, float brightness) {
    float h = hue == 1.0f ? 0 : hue * 6.0f;
    float f = h - (int)h;
    float p = brightness * (1.0f - saturation);
    float q = brightness * (1.0f - saturation * f);
    float t = brightness * (1.0f - (saturation * (1.0f - f)));

    if (h < 1) {
      return Color((unsigned char)(brightness * 255), (unsigned char)(t * 255),
                   (unsigned char)(p * 255));
    } else if (h < 2) {
      return Color((unsigned char)(q * 255), (unsigned char)(brightness * 255),
                   (unsigned char)(p * 255));
    } else if (h < 3) {
      return Color((unsigned char)(p * 255), (unsigned char)(brightness * 255),
                   (unsigned char)(t * 255));
    } else if (h < 4) {
      return Color((unsigned char)(p * 255), (unsigned char)(q * 255),
                   (unsigned char)(brightness * 255));
    } else if (h < 5) {
      return Color((unsigned char)(t * 255), (unsigned char)(p * 255),
                   (unsigned char)(brightness * 255));
    } else {
      return Color((unsigned char)(brightness * 255), (unsigned char)(p * 255),
                   (unsigned char)(q * 255));
    }
  }

  static Color Black;
  static Color White;
  static Color Red;
  static Color Green;
  static Color Blue;

  friend void to_json(nlohmann::json& j, const Color& c) {
    j = nlohmann::json::array({c.r(), c.g(), c.b(), c.a()});
  }
  friend void from_json(const nlohmann::json& j, Color& c) {
    j.at(0).get_to(c._u8color[0]);  // r
    j.at(1).get_to(c._u8color[1]);  // g
    j.at(2).get_to(c._u8color[2]);  // b
    j.at(3).get_to(c._u8color[3]);  // a
  }

 private:
  union {
    uint32_t _u32color;
    uint8_t _u8color[4];
  };
};

#endif /* CONFIG_COLOR_H */
