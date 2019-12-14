#ifndef CAMERA_H
#define CAMERA_H
#include "component.h"
#include "vec3.h"
#include "meshrenderer.h"

class Camera : public Component
{
public:
    Camera(GameObject* attachedTo = nullptr, int height = 800, int width = 1200, float nearPlane = 1.0f, float farPlane = 1000.0f, float fov = 45.0f);
    ~Camera();

    void setAspectRatio(int w, int h, bool ortho = false);
    void setBackgroundColor(float r, float g, float b, float a);
    void setFarPlane(float farPlane);

    float getFarPlane(){return mFarPlane;}
    float getNearPlane(){return mNearPlane;}

    QVector4D mBackgroundColor;

    void initComponent();
    void startComponent(){;}
    void stopComponent(){;}
    void runComponent();

    QMatrix4x4 getPerspectiveMatrix() {return mPerspectiveMatrix;}
    QMatrix4x4 getViewMatrix();
    QVector4D getBackgroundColor();

    void SetFrustrumClipping(bool in){mFrustumClipping = in;}
    bool FrustumClipping(){return mFrustumClipping;}
    void initFrustum();
    void drawFrustum();
    void getFrustumPlaneNormals(Vec3 &left, Vec3 &right, Vec3 &up, Vec3 &down, Vec3 &nearPlane, Vec3 &farPlane);

    void SetLookDirection(float _VerticalAngle = 0, float _HorizontalAngle = 0);
    Vec3 getRight();
    Vec3 getUp();
    Vec3 getForward();

    void HandleCameraInput();

private:

    Vec3 mFrustumNormalRight;
    Vec3 mFrustumNormalLeft;
    Vec3 mFrustumNormalDown;
    Vec3 mFrustumNormalUp;
    Vec3 mFrustumNormalFar;

    QMatrix4x4 mPerspectiveMatrix{};
    QMatrix4x4 mViewMatrix{};

    MeshRenderer* mFrustum{nullptr};
    std::vector<MeshRenderer*> mFrustumNormals;


    float mNearPlane;
    float mFarPlane;
    float mFieldOfView;
    int mHeight;
    int mWidth;
    float mAspectRatio;
    float verticalAngle, horizontalAngle;

    void createFrustum();
    QMatrix4x4 getRotationMatrix();

    bool mFrustumClipping{true};
    float mSpeed{1.0f};

};

#endif // CAMERA_H
