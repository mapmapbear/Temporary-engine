#include "camera.h"
#include "engine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"


Camera::Camera()
// m_resizeConnection({})
{
  m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
  m_rotation = glm::vec3(0.0, 0.0, 0.0f);
  // m_resizeConnection =
  // Engine::GetInstance()->WindowResizeSignal.connect(this,
  // &Camera::OnWindowResize);
}

Camera::~Camera() {
  // Engine::GetInstance()->WindowResizeSignal.disconnect(m_resizeConnection);
}

void Camera::SetPerpective(float aspectRatio, float yfov, float znear,
                           float zfar) {
  m_fov = yfov;
  m_znear = znear;
  m_zfar = zfar;
  m_projection =
      glm::perspectiveLH_ZO(glm::radians(yfov), aspectRatio, znear, zfar);
  UpdateMatrix();
}

void Camera::SetPosition(const glm::vec3 &pos) {
  if (m_pos != pos) {
    m_pos = pos;
    UpdateMatrix();
  }
}

void Camera::SetRotation(const glm::vec3 &rotation) {
  if (m_rotation != rotation) {
    m_rotation = rotation;
    UpdateMatrix();
  }
}

void Camera::Tick(float delta_time) {
  // TODO
  const float move_speed = 10.0f;

  if (ImGui::IsKeyDown(ImGuiKey_A)) {
    m_pos += GetLeft() * move_speed * delta_time;
  } else if (ImGui::IsKeyDown(ImGuiKey_S)) {
    m_pos += GetBack() * move_speed * delta_time;
  } else if (ImGui::IsKeyDown(ImGuiKey_D)) {
    m_pos += GetRight() * move_speed * delta_time;
  } else if (ImGui::IsKeyDown(ImGuiKey_W)) {
    m_pos += GetForward() * move_speed * delta_time;
  }

  m_pos += GetForward() * ImGui::GetIO().MouseWheel * move_speed * delta_time;

  /*
  if (ImGui::IsMouseClicked(1))
  {
          m_mousePos = float2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
  }

  if (ImGui::IsMouseDragging(1))
  {
          float2 mouse_delta = float2(ImGui::GetMousePos().x,
  ImGui::GetMousePos().y) - m_mousePos;

          float3 rotation = m_camera.GetRotation();
          rotation.x += mouse_delta.y * m_cameraRotateSpeed;
          rotation.y += mouse_delta.x * m_cameraRotateSpeed;
          m_camera.SetRotation(rotation);

          m_mousePos += mouse_delta;
  }
  */

  UpdateMatrix();
}

void Camera::UpdateMatrix() {
  glm::mat4 trMat = glm::mat4(1.0);
  glm::mat4 roMat = glm::mat4(1.0);
  trMat = glm::translate(trMat, m_pos);

  roMat = glm::eulerAngleYXZ(m_rotation.y, m_rotation.x, m_rotation.z);;
  m_world = trMat * roMat;
  m_view = glm::inverse(m_world);
  m_viewProjection = m_projection * m_view;
}

void Camera::OnWindowResize(uint32_t width, uint32_t height) {
  SetPerpective((float)width / height, m_fov, m_znear, m_zfar);
}
