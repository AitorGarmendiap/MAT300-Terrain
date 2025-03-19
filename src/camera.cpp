#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace mat300_terrain {

    void Camera::SetPrespectiveProjection(float vertFov, float aspect, float near, float far)
    {
        mProjectionMatrix = glm::perspective(glm::radians(vertFov), aspect, near, far);

        mFovy = vertFov;
        mAspect = aspect;
        mNear = near;
        mFar = far;
    }

    void Camera::SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
    {
        mViewMatrix = glm::lookAt(position, position + direction, up);
    }

    void Camera::SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up)
    {
        SetViewDirection(position, target - position, up);
    }


} // namespace mat300_terrain

