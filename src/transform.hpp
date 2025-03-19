#pragma once

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

namespace mat300_terrain {

    struct Transform
    {
        glm::vec3 translation{ 0.0f, 0.0f, 0.0f };
        glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
        glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };

        // Get m2w matrix
        glm::mat4 mat4() const
        {
            glm::mat4 transform( 1.0f);
            transform = glm::translate(transform, translation);

            transform = glm::rotate(transform, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            transform = glm::rotate(transform, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            transform = glm::rotate(transform, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

            transform = glm::scale(transform, scale);

            return transform;

        }
    };

} // namespace mat300_terrain
