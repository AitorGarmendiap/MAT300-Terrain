#include "camera_controller.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp> // rotate
#include "imgui.h"

namespace mat300_terrain {

    static bool pressed = false;
    float moveSpeed = 25.0f;
    float lookSpeed = 5.0f;
    const float scrollMultiplier = 5.0f;

    // Move speed
    void ScrollCallback(GLFWwindow * window, double xOffset, double yOffset)
    {
        // Modify moveSpeed based on the vertical scroll
        moveSpeed += scrollMultiplier * static_cast<float>(yOffset);  // Adjust the rate as needed
        moveSpeed = std::max(0.1f, moveSpeed);     // Prevent negative or zero move speed
    }

    bool CameraControler::HandleInput(float dt, GLFWwindow* window, Camera& cam)
    {
        bool movedCamera = false;
        glfwSetScrollCallback(window, ScrollCallback);

        // Axial movement respect to the camera and world up
        float speed = glm::max(moveSpeed, 0.0f);

        cam.mRight = glm::normalize(glm::cross(cam.mForward, cam.mUp));

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                cam.mTransform.translation += speed * dt * glm::normalize(cam.mForward);
            };
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                cam.mTransform.translation -= speed * dt * glm::normalize(cam.mForward);
            };
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                cam.mTransform.translation += speed * dt * glm::normalize(cam.mRight);
            };
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                cam.mTransform.translation -= speed * dt * glm::normalize(cam.mRight);
            };
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            {
                cam.mTransform.translation.y += speed * dt;
            };
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            {
                cam.mTransform.translation.y -= speed * dt;
            };
        }
        static glm::dvec2 prevPos{};
        static glm::dvec2 cursorPos{};

        // Get current cursor position
        glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2((float)cursorPos.x, (float)cursorPos.y);

        // Cameras rotational movement
        io.MouseDown[0] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        io.MouseDown[1] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

        if (io.MouseDown[1])
        {
            movedCamera = true;
            glm::dvec3 rightVec = glm::normalize(glm::cross(cam.mForward, { 0, 1, 0 }));

            glm::dvec2 cursorDelta = cursorPos - prevPos;
            cam.mForward = glm::vec3(glm::vec4(cam.mForward, 0) * glm::rotate(glm::radians(15.0f) * 0.01f * cursorDelta.y, rightVec));
            cam.mForward = glm::vec3(glm::vec4(cam.mForward, 0) * glm::rotate(glm::radians(15.0f) * 0.01f * cursorDelta.x, glm::dvec3(0, 1, 0)));
        }
        if (!io.MouseDown[0] && selected) selected = false;
        // Select Patch or Control Point
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS || (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && (io.MouseDown[0] && !selected)))
        {
            selected = true;
            SelectPatch(static_cast<float>(cursorPos.x), static_cast<float>(cursorPos.y));
        }
        // Create river or select Control Point
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && (io.MouseDown[0] && !selected))
        {
            selected = true;
            SelectRiver(static_cast<float>(cursorPos.x), static_cast<float>(cursorPos.y));
        }
        
        prevPos = cursorPos;
        return movedCamera;
    }

}