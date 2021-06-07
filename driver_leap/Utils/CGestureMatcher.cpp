#include "stdafx.h"

#include "Utils/CGestureMatcher.h"

const float g_pi = glm::pi<float>();
const float g_piHalf = g_pi * 0.5f;
const float g_piQuarter = g_pi * 0.25f;
extern const glm::mat4 g_identityMatrix;

void CGestureMatcher::GetGestures(const LEAP_HAND *f_hand, std::vector<float> &f_result, const LEAP_HAND *f_oppHand)
{
    f_result.resize(HG_Count, 0.f);

    // Finger bends
    float l_fingerBend[5U] = { 0.f };
    for(size_t i = 0U; i < 5U; i++)
    {
        const LEAP_DIGIT &l_finger = f_hand->digits[i];
        glm::vec3 l_prevDirection;
        for(size_t j = 0U; j < 4U; j++)
        {
            const LEAP_BONE &l_bone = l_finger.bones[j];
            glm::vec3 l_direction(l_bone.next_joint.x - l_bone.prev_joint.x, l_bone.next_joint.y - l_bone.prev_joint.y, l_bone.next_joint.z - l_bone.prev_joint.z);
            l_direction = glm::normalize(l_direction);
            if(j > 0) l_fingerBend[i] += glm::acos(glm::dot(l_direction, l_prevDirection));
            l_prevDirection = l_direction;
        }
    }

    for(size_t i = 0U; i <= HG_PinkyBend; i++) f_result[i] = NormalizeRange(l_fingerBend[i], g_piHalf, g_pi);

    // Simple gestures
    f_result[HG_Trigger] = f_result[HG_IndexBend];
    f_result[HG_Grab] = NormalizeRange((l_fingerBend[2U] + l_fingerBend[3U] + l_fingerBend[4U]) / 3.f, g_piHalf, g_pi);

    // Little complex gestures
    glm::vec3 l_start(f_hand->thumb.distal.next_joint.x, f_hand->thumb.distal.next_joint.y, f_hand->thumb.distal.next_joint.z);
    glm::vec3 l_end(f_hand->index.intermediate.prev_joint.x, f_hand->index.intermediate.prev_joint.y, f_hand->index.intermediate.prev_joint.z);
    f_result[HG_ThumbPress] = NormalizeRange(glm::distance(l_start, l_end), 35.f, 20.f);

    // Two-handed gestures
    if(f_oppHand)
    {
        l_start = glm::vec3(f_oppHand->index.distal.next_joint.x, f_oppHand->index.distal.next_joint.y, f_oppHand->index.distal.next_joint.z);
        l_end = glm::vec3(f_hand->thumb.distal.next_joint.x, f_hand->thumb.distal.next_joint.y, f_hand->thumb.distal.next_joint.z);
        f_result[HG_ThumbCrossTouch] = NormalizeRange(glm::distance(l_start, l_end), 35.f, 20.f);

        l_end = glm::vec3(f_hand->middle.distal.next_joint.x, f_hand->middle.distal.next_joint.y, f_hand->middle.distal.next_joint.z);
        f_result[HG_MiddleCrossTouch] = NormalizeRange(glm::distance(l_start, l_end), 35.f, 20.f);

        const glm::vec3 l_handNormal(f_hand->palm.normal.x, f_hand->palm.normal.y, f_hand->palm.normal.z);

        l_start = glm::vec3(f_hand->palm.position.x, f_hand->palm.position.y, f_hand->palm.position.z);
        l_end = glm::vec3(f_oppHand->index.distal.next_joint.x, f_oppHand->index.distal.next_joint.y, f_oppHand->index.distal.next_joint.z);

        if(glm::acos(glm::dot(glm::normalize(l_end - l_start), -l_handNormal)) <= g_piQuarter)
        {
            f_result[HG_OpisthenarTouch] = NormalizeRange(glm::distance(l_start, l_end), 50.f, 30.f);
        }

        if(glm::acos(glm::dot(glm::normalize(l_end - l_start), l_handNormal)) <= g_piQuarter)
        {
            f_result[HG_PalmTouch] = NormalizeRange(glm::distance(l_start, l_end), 50.f, 30.f);
        }

        const glm::vec3 l_handPos(f_hand->palm.position.x, f_hand->palm.position.y, f_hand->palm.position.z);
        const glm::quat l_handRot(f_hand->palm.orientation.w, f_hand->palm.orientation.x, f_hand->palm.orientation.y, f_hand->palm.orientation.z);
        const glm::mat4 l_handTransform = glm::translate(g_identityMatrix, l_handPos)*glm::mat4_cast(l_handRot);

        l_start = glm::vec3(f_oppHand->index.distal.next_joint.x, f_oppHand->index.distal.next_joint.y, f_oppHand->index.distal.next_joint.z);
        glm::vec3 l_planePoint = glm::inverse(l_handTransform)*glm::vec4(l_start, 1.f);
        if((l_planePoint.y < 0.f) && (l_planePoint.y >= -150.f))
        {
            glm::vec2 l_uv(-l_planePoint.x, -l_planePoint.z);
            if(glm::length(l_uv) <= 125.f)
            {
                l_uv /= (f_hand->palm.width*0.5f);
                if(glm::length(l_uv) > 1.f) l_uv = glm::normalize(l_uv);

                f_result[HG_PalmPointX] = l_uv.x;
                f_result[HG_PalmPointY] = l_uv.y;
            }
        }
    }
}

float CGestureMatcher::NormalizeRange(float f_val, float f_min, float f_max)
{
    const float l_mapped = (f_val - f_min) / (f_max - f_min);
    return glm::clamp(l_mapped, 0.f, 1.f);
}
