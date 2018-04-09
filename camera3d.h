#ifndef CAMERA3D_H
#define CAMERA3D_H

#include <QtWidgets>

class Camera3D
{
public:
    Camera3D(QVector3D pos_ = QVector3D(0, 0, 0.5),
             QVector3D up_ = QVector3D(0, 0, 1),
             float pitch_ = -89.0,
             float roll_ = 0.0,
             float fov_ = 45.0,
             float aspectRatio_ = 1.0,
             float nearPlane_ = 0.1,
             float farPlane_ = 100.0,
             float sensitivity_ = 0.1);
    void viewRotate(QVector2D degrees);
    void setFov(float fov);
    void setFovBias(float bias);
    void setAspectRatio(float aspectRatio);
    const QMatrix4x4 &viewMatrix();
    const QMatrix4x4 &perspectMatrix();

private:
    void updateViewMatrix();
    void updateProjectionMatrix();

    QMatrix4x4 view;
    QVector3D pos;
    QVector3D up;
    QVector3D front;
    float yaw, pitch, roll;

    QMatrix4x4 projection;
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    float speed;
    float sensitivity;
};

#endif // CAMERA3D_H
