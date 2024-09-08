#pragma once

#include "../utils/fmath.h"

class Camera {
public:
  Camera();
  ~Camera();
  void SetPerpective(float aspectRatio, float yfov, float znear, float zfar);

  void SetPosition(const glm::vec3 &pos);
  const glm::vec3 &GetPosition() const { return m_pos; }

  void SetRotation(const glm::vec3 &rotation);
  const glm::vec3 &GetRotation() const { return m_rotation; }

  void Tick(float delta_time);

  const glm::mat4 &GetViewMatrix() const { return m_view; }
  const glm::mat4 &GetProjectionMatrix() const { return m_projection; }
  const glm::mat4 &GetViewProjectionMatrix() const { return m_viewProjection; }

  glm::vec3 GetLeft() const {
    return glm::vec3(-m_world[0].x, -m_world[0].y, -m_world[0].z);
  }
  glm::vec3 GetRight() const {
    return glm::vec3(m_world[0].x, m_world[0].y, m_world[0].z);
  }
  glm::vec3 GetForward() const {
    return glm::vec3(m_world[2].x, m_world[2].y, m_world[2].z);
  }
  glm::vec3 GetBack() const {
    return glm::vec3(-m_world[2].x, -m_world[2].y, -m_world[2].z);
  }
  glm::vec3 GetUp() const {
    return glm::vec3(m_world[1].x, m_world[1].y, m_world[1].z);
  }
  glm::vec3 GetDown() const {
    return glm::vec3(-1.0) *
           glm::vec3(m_world[1].x, m_world[1].y, m_world[1].z);
  }
  void OnWindowResize(uint32_t width, uint32_t height);

private:
  void UpdateMatrix();

private:
  glm::vec3 m_pos;
  glm::vec3 m_rotation;

  glm::mat4 m_world;
  glm::mat4 m_view;
  glm::mat4 m_projection;
  glm::mat4 m_viewProjection;
  float m_fov;
  float m_znear;
  float m_zfar;
};