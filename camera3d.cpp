#include "camera3d.h"

Camera3D::Camera3D(QVector3D pos_, QVector3D up_, float pitch_,
                   float roll_, float fov_, float aspectRatio_,
                   float nearPlane_, float farPlane_, float sensitivity_) :
    pos(pos_),
    up(up_),
    front(QVector3D(0, 0.0174524, -0.999848)),
    pitch(pitch_),
    roll(roll_),
    fov(fov_),
    aspectRatio(aspectRatio_),
    nearPlane(nearPlane_),
    farPlane(farPlane_),
    sensitivity(sensitivity_)
{
    updateViewMatrix();
    updateProjectionMatrix();
}

void Camera3D::viewRotate(QVector2D degrees)
{
    degrees *= sensitivity;
    roll -= degrees.x();
    pitch += degrees.y();
    if(pitch > 89.0)
        pitch = 89.0;
    else if(pitch < -89.0)
        pitch = -89.0;

    float rpitch = qDegreesToRadians(pitch);
    float rroll = qDegreesToRadians(roll);
    QVector3D front(qCos(rpitch)*qSin(rroll), qCos(rpitch)*qCos(rroll), qSin(rpitch));
    this->front = front.normalized();

    updateViewMatrix();
}

void Camera3D::setFov(float fov)
{
    this->fov = fov;
    updateProjectionMatrix();
}

void Camera3D::setFovBias(float bias)
{
    float fov = this->fov + bias * sensitivity;
    if(fov < 45.0)
        fov = 45.0;
    else if(fov > 150.0)
        fov = 150.0;
    this->fov = fov;
    updateProjectionMatrix();
}

void Camera3D::setAspectRatio(float aspectRatio)
{
    this->aspectRatio = aspectRatio;
    updateProjectionMatrix();
}

const QMatrix4x4 &Camera3D::viewMatrix()
{
    return view;
}

const QMatrix4x4 &Camera3D::perspectMatrix()
{
    return projection;
}

void Camera3D::updateViewMatrix()
{
    view.setToIdentity();
    view.lookAt(pos, pos + front, up);
}

void Camera3D::updateProjectionMatrix()
{
    projection.setToIdentity();
    projection.perspective(fov, aspectRatio, nearPlane, farPlane);
}


