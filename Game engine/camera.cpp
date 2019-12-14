#include "camera.h"
#include "frustum.h"
#include "transform.h"
#include "input.h"

Camera::Camera(GameObject *attachedTo, int height, int width, float nearPlane, float farPlane, float fov)
    : mNearPlane(nearPlane), mFarPlane(farPlane), mFieldOfView(fov), mHeight(height), mWidth(width)
{

    type = Orf::CAMERA;
    mAspectRatio = static_cast<float>(mWidth)/static_cast<float>(mHeight ? mHeight : 1);
    mBackgroundColor = QVector4D(0.3f, 0.3f, 0.3f, 1.0f);
    mPerspectiveMatrix.perspective(mFieldOfView, mAspectRatio, mNearPlane, mFarPlane);

    if(attachedTo == nullptr){
        attachedGameobject = new GameObject();
        attachedGameobject->addComponent(this);
    } else {
        attachedGameobject = attachedTo;
    }
}

Camera::~Camera()
{

}

void Camera::createFrustum()
{

    Mesh *mesh = new Frustum(this);

    // Get the frustum vertices positions
    Vec3 nearBotRight = mesh->getVertices()[0].get_xyz();
    Vec3 nearBotLeft = mesh->getVertices()[1].get_xyz();
    Vec3 farBotLeft = mesh->getVertices()[2].get_xyz();
    Vec3 nearTopRight = mesh->getVertices()[4].get_xyz();
    Vec3 farTopLeft = mesh->getVertices()[6].get_xyz();
    Vec3 farTopRight = mesh->getVertices()[7].get_xyz();

    // Set the frustum plane normals for frustum clipping
    mFrustumNormalLeft = ((nearTopRight-nearBotRight) ^ (nearTopRight - farTopRight)).normalized();
    mFrustumNormalDown = ((nearTopRight-farTopRight) ^ (farTopRight - farTopLeft)).normalized();
    mFrustumNormalRight = ((farTopLeft-farBotLeft) ^ (farBotLeft - nearBotLeft)).normalized();
    mFrustumNormalUp = ((nearBotRight - nearBotLeft) ^ (nearBotLeft - farBotLeft)).normalized();
    mFrustumNormalFar = Vec3(0,0,1);

    mFrustum = new MeshRenderer(mesh);
    mFrustum->setMaterial(MaterialTypes::GetMaterial(0));
    mFrustum->attachComponent(attachedGameobject);
}

QMatrix4x4 Camera::getViewMatrix()
{
    Vec3 mLocalScale = attachedGameobject->mTransform->getScale();
    Vec3 mLocalRotation = attachedGameobject->mTransform->getRotation();
    Vec3 mLocalPosition = attachedGameobject->mTransform->getPosition();

    QMatrix4x4 mModelMatrix;
    mModelMatrix.scale(mLocalScale.x, mLocalScale.y, mLocalScale.z);
    mModelMatrix.rotate(mLocalRotation.x, 1.0, 0.0, 0.0);
    mModelMatrix.rotate(mLocalRotation.z, 0.0, 0.0, 1.0);
    mModelMatrix.rotate(mLocalRotation.y, 0.0, 1.0, 0.0);
    mModelMatrix.translate(mLocalPosition.x, mLocalPosition.y, mLocalPosition.z );

    return mModelMatrix;
}

void Camera::setAspectRatio(int w, int h, bool ortho)
{
    mAspectRatio = static_cast<float>(w)/static_cast<float>(h ? h : 1);
    mPerspectiveMatrix.setToIdentity();

    if (ortho)
    {
        float scale{0.01f};
        mPerspectiveMatrix.ortho(-scale*w, scale*w, -scale*h , scale*h, mNearPlane, mFarPlane);
    }
    else
        mPerspectiveMatrix.perspective(mFieldOfView, mAspectRatio, mNearPlane, mFarPlane);

    createFrustum();
}

void Camera::setBackgroundColor(float r, float g, float b, float a)
{
    mBackgroundColor = QVector4D(r, g, b, a);
}

void Camera::setFarPlane(float farPlane)
{
    mFarPlane = farPlane;
    mPerspectiveMatrix.perspective(mFieldOfView, mAspectRatio, mNearPlane, mFarPlane);
}

void Camera::initComponent()
{
    qDebug() << "init camera components";
    createFrustum();
    qDebug() << "init frustum";
    initFrustum();
    qDebug() << "init camera components done";
}

void Camera::runComponent()
{
    //drawFrustum();
}

void Camera::initFrustum()
{
    mFrustum->initComponent();
}

void Camera::drawFrustum()
{
    mFrustum->runComponent();
}

void Camera::getFrustumPlaneNormals(Vec3 &left, Vec3 &right, Vec3 &up, Vec3 &down, Vec3 &nearPlane, Vec3 &farPlane)
{
    // Camera gameobjects modelmatrix
    QMatrix4x4 rotationMatrix = attachedGameobject->mTransform->getRotationMatrix().inverted();

    // Assign the in-variables - multiply localFrustumNormals with modelmatrix
    left = mFrustumNormalRight * rotationMatrix;
    right = mFrustumNormalLeft * rotationMatrix;
    up = mFrustumNormalDown * rotationMatrix;
    down = mFrustumNormalUp * rotationMatrix;
    nearPlane = (mFrustumNormalFar * -1) * rotationMatrix;
    farPlane = mFrustumNormalFar * rotationMatrix;
}

Vec3 Camera::getUp()
{
    Vec3 up (0,1,0);
    return up * getRotationMatrix();
}

Vec3 Camera::getRight()
{
    Vec3 right (1,0,0);
    return right * getRotationMatrix();
}

Vec3 Camera::getForward()
{
    Vec3 forward (0,0,1);
    return forward * getRotationMatrix();
}

QMatrix4x4 Camera::getRotationMatrix()
{
    QMatrix4x4 rotationMatrix;

    Vec3 mLocalRotation = attachedGameobject->mTransform->getRotation();
    rotationMatrix.rotate(mLocalRotation.x, 1.0, 0.0, 0.0);
    rotationMatrix.rotate(mLocalRotation.z, 0.0, 0.0, 1.0);
    rotationMatrix.rotate(mLocalRotation.y, 0.0, 1.0, 0.0);

    return rotationMatrix;
}

void Camera::HandleCameraInput()
{
    //Transform *tra = attachedGameobject->mTransform;
    //qDebug() << tra->getPosition().x << " " << tra->getPosition().y << " " << tra->getPosition().z;
    //qDebug() << tra->getRotation().x << " " << tra->getRotation().y << " " << tra->getRotation().z;
    //qDebug() << tra->getScale().x << " " << tra->getScale().y << " " << tra->getScale().z;

    //qDebug() << "move camera function";
    //move camera
    if(Input::KeyPressed(Qt::Key_A))
    {
        attachedGameobject->mTransform->translate(getRight() * mSpeed);
    }
    if(Input::KeyPressed(Qt::Key_D))
    {
        attachedGameobject->mTransform->translate(getRight() * mSpeed * -1);
    }
    if(Input::KeyPressed(Qt::Key_W))
    {
        attachedGameobject->mTransform->translate(getForward() * mSpeed);
    }
    if(Input::KeyPressed(Qt::Key_S))
    {
        attachedGameobject->mTransform->translate(getForward() * mSpeed * -1);
    }

    if(Input::KeyPressed(Qt::Key_Q))
    {
        attachedGameobject->mTransform->translate(getUp() * mSpeed * -1);
    }
    if(Input::KeyPressed(Qt::Key_E))
    {
        attachedGameobject->mTransform->translate(getUp() * mSpeed );
    }
    drawFrustum();
}














