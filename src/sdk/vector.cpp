#include "vector.h"

#include <cocaine.h>

D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2) {
  D3DMATRIX pOut;
  pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 +
             pM1._14 * pM2._41;
  pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 +
             pM1._14 * pM2._42;
  pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 +
             pM1._14 * pM2._43;
  pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 +
             pM1._14 * pM2._44;
  pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 +
             pM1._24 * pM2._41;
  pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 +
             pM1._24 * pM2._42;
  pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 +
             pM1._24 * pM2._43;
  pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 +
             pM1._24 * pM2._44;
  pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 +
             pM1._34 * pM2._41;
  pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 +
             pM1._34 * pM2._42;
  pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 +
             pM1._34 * pM2._43;
  pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 +
             pM1._34 * pM2._44;
  pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 +
             pM1._44 * pM2._41;
  pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 +
             pM1._44 * pM2._42;
  pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 +
             pM1._44 * pM2._43;
  pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 +
             pM1._44 * pM2._44;

  return pOut;
}

D3DMATRIX toMatrix(Vector3 Rotation, Vector3 origin) {
  float Pitch = (Rotation.x * float(M_PI) / 180.f);
  float Yaw = (Rotation.y * float(M_PI) / 180.f);
  float Roll = (Rotation.z * float(M_PI) / 180.f);

  float SP = sinf(Pitch);
  float CP = cosf(Pitch);
  float SY = sinf(Yaw);
  float CY = cosf(Yaw);
  float SR = sinf(Roll);
  float CR = cosf(Roll);

  D3DMATRIX Matrix;
  Matrix._11 = CP * CY;
  Matrix._12 = CP * SY;
  Matrix._13 = SP;
  Matrix._14 = 0.f;

  Matrix._21 = SR * SP * CY - CR * SY;
  Matrix._22 = SR * SP * SY + CR * CY;
  Matrix._23 = -SR * CP;
  Matrix._24 = 0.f;

  Matrix._31 = -(CR * SP * CY + SR * SY);
  Matrix._32 = CY * SR - CR * SP * SY;
  Matrix._33 = CR * CP;
  Matrix._34 = 0.f;

  Matrix._41 = origin.x;
  Matrix._42 = origin.y;
  Matrix._43 = origin.z;
  Matrix._44 = 1.f;

  return Matrix;
}

Vector2 worldToScreen(Vector3 world_location, Vector3 position,
                      Vector3 rotation, int width, int height, float fov) {
  Vector2 screen_location = Vector2(0, 0);

  D3DMATRIX tempMatrix = toMatrix(rotation);

  Vector3 vAxisX, vAxisY, vAxisZ;

  vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
  vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
  vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

  Vector3 vDelta = world_location - position;
  Vector3 vTransformed =
      Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

  if (vTransformed.z < .1f) vTransformed.z = .1f;

  float FovAngle = fov;
  float ScreenCenterX = width / 2.0f;
  float ScreenCenterY = height / 2.0f;

  screen_location.x =
      ScreenCenterX +
      vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) /
          vTransformed.z;
  screen_location.y =
      ScreenCenterY -
      vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) /
          vTransformed.z;

  return screen_location;
}

float RadianToDegree(float radian) { return radian * (180 / M_PI); }

float DegreeToRadian(float degree) { return degree * (M_PI / 180); }

Vector3 RadianToDegree(Vector3 radians) {
  Vector3 degrees;
  degrees.x = radians.x * (180 / M_PI);
  degrees.y = radians.y * (180 / M_PI);
  degrees.z = radians.z * (180 / M_PI);
  return degrees;
}

Vector3 DegreeToRadian(Vector3 degrees) {
  Vector3 radians;
  radians.x = degrees.x * (M_PI / 180);
  radians.y = degrees.y * (M_PI / 180);
  radians.z = degrees.z * (M_PI / 180);
  return radians;
}

void ClampAngle(Vector3& angle) {
  if (angle.x > 89.0f) angle.x = 89.f;
  if (angle.x < -89.0f) angle.x = -89.f;

  if (angle.y > 180.f) angle.y = 180.f;
  if (angle.y < -180.f) angle.y = -180.f;

  angle.z = 0.0f;
}

void Normalise(Vector3& angle) {
  if (angle.x > 89.0f) angle.x -= 180.0f;
  if (angle.x < -89.0f) angle.x += 180.0f;

  while (angle.y > 180) angle.y -= 360.f;
  while (angle.y < -180) angle.y += 360.f;
}

Vector3 CalcAngle(Vector3 src, Vector3 dst) {
  Vector3 angle;
  angle.x = -atan2f(dst.x - src.x, dst.y - src.y) / M_PI * 180.0f + 180.0f;
  angle.y = asinf((dst.z - src.z) / src.Distance(dst)) * 180.0f / M_PI;
  angle.z = 0.0f;

  return angle;
}
