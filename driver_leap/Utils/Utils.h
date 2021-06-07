#pragma once

void ConvertMatrix(const vr::HmdMatrix34_t &f_matVR, glm::mat4 &f_mat);
template<class T, class U> void ConvertQuaternion(const T &f_quatA, U &f_quatB)
{
    f_quatB.x = f_quatA.x;
    f_quatB.y = f_quatA.y;
    f_quatB.z = f_quatA.z;
    f_quatB.w = f_quatA.w;
}
void ConvertVector3(const vr::HmdVector4_t &f_vrVec, glm::vec3 &f_glmVec);
void ConvertVector3(const glm::vec3 &f_glmVec, vr::HmdVector4_t &f_vrVec);

size_t ReadEnumVector(const std::string &f_val, const std::vector<std::string> &f_vec);
size_t ReadEnumVector(const char *f_val, const std::vector<std::string> &f_vec);
