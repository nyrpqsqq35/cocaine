#include "Color.h"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string_view>

Color Color::Black(0, 0, 0, 255);
Color Color::White(255, 255, 255, 255);
Color Color::Red(255, 0, 0, 255);
Color Color::Green(0, 128, 0, 255);
Color Color::Blue(0, 0, 255, 255);

Color::Color() { *((int*)this) = 0; }
Color::Color(int _r, int _g, int _b) { setColor(_r, _g, _b, 255); }
Color::Color(int _r, int _g, int _b, int _a) { setColor(_r, _g, _b, _a); }
Color::Color(std::string hex) {
  if (!hex._Starts_with("#")) throw std::runtime_error("Invalid color");
  auto ours = hex;
  if (hex.length() != 9) ours += "ff";  // default alpha
  uint32_t rgba = std::stoul(ours.substr(1), nullptr, 16);
  _u8color[0] = static_cast<uint8_t>((rgba & 0xFF000000) >> (3 * 8));
  _u8color[1] = static_cast<uint8_t>((rgba & 0x00FF0000) >> (2 * 8));
  _u8color[2] = static_cast<uint8_t>((rgba & 0x0000FF00) >> (1 * 8));
  _u8color[3] = static_cast<uint8_t>((rgba & 0x000000FF) >> (0 * 8));
}

void Color::getHexColor(std::string& str, bool alpha) const {
  std::stringstream ss;
  ss << "#" << std::uppercase << std::setw(2) << std::setfill('0') << std::hex
     << r() << g() << b();
  if (alpha) ss << a();
  str = ss.str();
}

std::string Color::getHexColor(bool alpha) const {
  std::string hex;
  getHexColor(hex, alpha);
  return hex;
}

void Color::setRawColor(uint32_t color32) { *((uint32_t*)this) = color32; }
uint32_t Color::getRawColor() const { return *((uint32_t*)this); }
void Color::setColor(int _r, int _g, int _b, int _a) {
  _u8color[0] = static_cast<uint8_t>(std::min(_r, 0xff));
  _u8color[1] = static_cast<uint8_t>(std::min(_g, 0xff));
  _u8color[2] = static_cast<uint8_t>(std::min(_b, 0xff));
  _u8color[3] = static_cast<uint8_t>(std::min(_a, 0xff));
}
void Color::setColor(float _r, float _g, float _b, float _a) {
  _u8color[0] = static_cast<uint8_t>(std::min(_r * 255.f, 255.f));
  _u8color[1] = static_cast<uint8_t>(std::min(_g * 255.f, 255.f));
  _u8color[2] = static_cast<uint8_t>(std::min(_b * 255.f, 255.f));
  _u8color[3] = static_cast<uint8_t>(std::min(_a * 255.f, 255.f));
}
void Color::getColor(int& _r, int& _g, int& _b, int& _a) const {
  _r = _u8color[0];
  _g = _u8color[1];
  _b = _u8color[2];
  _a = _u8color[3];
}

void Color::setAlpha(int a) { _u8color[3] = a; }
void Color::setAlpha(float a) { _u8color[3] = static_cast<uint8_t>(a * 255.f); }

bool Color::operator==(const Color& rhs) const {
  return (*((int*)this) == *((int*)&rhs));
}
bool Color::operator!=(const Color& rhs) const { return !(operator==(rhs)); }
Color& Color::operator=(const Color& rhs) {
  setRawColor(rhs.getRawColor());
  return *this;
}