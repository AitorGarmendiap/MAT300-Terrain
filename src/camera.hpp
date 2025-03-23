#pragma once

#include "transform.hpp"

#include <glm/glm.hpp>
#include <array>

namespace mat300_terrain{

    class Camera
    {
    public:
        void SetPrespectiveProjection(float vertFov, float aspect, float nearPlane, float farPlane);

        void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.0f, 1.0f, 0.0 });
        void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.0f, 1.0f, 0.0 });

        void Update();

        const glm::mat4& GetProjection() const { return mProjectionMatrix; }
        const glm::mat4& GetView() const { return mViewMatrix; }
        const glm::vec3 GetPosition() const { return mTransform.translation; }

        Transform mTransform;

        glm::vec3 mForward = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 mUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 mRight = glm::normalize(glm::cross(mForward, mUp));

        float mFovy = 60.0f;
        float mAspect = 16.0f / 9.0f;
        float mNear = 0.1f;
        float mFar = 500.0f;

    private:
        glm::mat4 mProjectionMatrix{ 1.0f };
        glm::mat4 mViewMatrix{ 1.0f };
    };


} // namespace AFL