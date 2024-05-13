#pragma once

#include "glm/gtc/quaternion.hpp"

namespace Vitrae
{
    struct SimpleTransformation
    {
        glm::vec3 pos;
        glm::quat rot;
        glm::vec3 scale;

        inline float getPitch() const
        {
            return glm::pitch(rot);
        }

        inline float getYaw() const
        {
            return glm::yaw(rot);
        }

        inline float getRoll() const
        {
            return glm::roll(rot);
        }

        inline void setPitch(float pitch)
        {
            rot = glm::quat(glm::vec3(pitch, getYaw(), getRoll()));
        }

        inline void setYaw(float yaw)
        {
            rot = glm::quat(glm::vec3(getPitch(), yaw, getRoll()));
        }

        inline void setRoll(float roll)
        {
            rot = glm::quat(glm::vec3(getPitch(), getYaw(), roll));
        }

        glm::mat4 getModelMatrix() const
        {
            return glm::translate(glm::mat4_cast(rot) * glm::scale(glm::mat4(1), scale), pos);
        }

        glm::mat4 getViewMatrix() const
        {
            return glm::inverse(getModelMatrix());
        }

        void move(glm::vec3 pos)
        {
            this->pos += pos;
        }

        void rotate(glm::vec3 rotPiYaRo)
        {
            this->rot = glm::quat(glm::eulerAngles(this->rot) + rotPiYaRo);
        }

        void scale(glm::vec3 scaling)
        {
            this->scale = glm::vec3(this->scale.x * scaling.x, this->scale.y * scaling.y,
                                    this->scale.z * scaling.z);
        }
    };
}