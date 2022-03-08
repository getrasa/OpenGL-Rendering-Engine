#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <filesystem>
#include "Shader.h"
#include "stb_image.h"
#include "objectLoader.h"
#include "Camera.h";
#include "Model.h"
#include "Textures.h";

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <openvr/openvr.h>

//#define VR_RUN

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// settings
unsigned int SCR_WIDTH = 1200;
unsigned int SCR_HEIGHT = 800;

// Camera
Camera myCam(glm::vec3(0.0f, 1.6f, 3.0f));
//Camera myCam(glm::vec3(0.0f, 0.0f, 0.0f));
Model ourModel;
Shader shader;
// :/
glm::mat4 modelMatrix;

// Mouse
bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// VR Global
vr::IVRSystem* vr_pointer = NULL;
vr::IVRSystem* m_pHMD = NULL;
vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];

float m_fNearClip;
float m_fFarClip;

glm::mat4 m_mat4HMDPose;
glm::mat4 m_mat4eyePosLeft;
glm::mat4 m_mat4eyePosRight;

glm::mat4 m_mat4ProjectionCenter;
glm::mat4 m_mat4ProjectionLeft;
glm::mat4 m_mat4ProjectionRight;

int m_iValidPoseCount;
int m_iValidPoseCount_Last;
std::string m_strPoseClasses;
glm::mat4 m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];
char m_rDevClassChar[vr::k_unMaxTrackedDeviceCount];   // for each device, a character representing its class

struct FramebufferDesc
{
    GLuint m_nDepthBufferId;
    GLuint m_nRenderTextureId;
    GLuint m_nRenderFramebufferId;
    GLuint m_nResolveTextureId;
    GLuint m_nResolveFramebufferId;
};
FramebufferDesc leftEyeDesc;
FramebufferDesc rightEyeDesc;

uint32_t m_nRenderWidth;
uint32_t m_nRenderHeight;

void initializeVR();
bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc& framebufferDesc);
bool SetupStereoRenderTargets();

void SetupCameras();
glm::mat4 GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye);
glm::mat4 GetHMDMatrixPoseEye(vr::Hmd_Eye nEye);
glm::mat4 GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye);
glm::mat4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t& matPose);
void UpdateHMDMatrixPose();

void RenderScene(vr::Hmd_Eye nEye);
void RenderStereoTargets();


//int WinMain()
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    //glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
    glfwWindowHint(GLFW_DECORATED, NULL);
    GLFWmonitor* MyMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(MyMonitor);
    SCR_WIDTH = mode->width;
    SCR_HEIGHT = mode->height;

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Engine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    glEnable(GL_SAMPLE_ALPHA_TO_ONE);

    stbi_set_flip_vertically_on_load(true);

    // build and compile our shader program
        // ------------------------------------
    shader = Shader("../shaders/pbrT.vert", "../shaders/pbrT.frag");

    ourModel = Model("../models/vampire2.fbx");
    printf("Model Loaded");


    // lighting
    glm::vec3 lightPositions[] = {
        glm::vec3(4.0f,  2.0f, 4.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(100.0f, 100.0f, 100.0f),
    };

    shader.use();

    modelMatrix = glm::rotate(glm::mat4(1.0f), 3.14f * 1.5f, glm::vec3(1, 0, 0));
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(myCam.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    glm::mat4 m0 = projection;

#ifdef VR_RUN
    // INITIALIZE VR
    initializeVR();
    SetupCameras();
    SetupStereoRenderTargets();
    UpdateHMDMatrixPose();
    glm::mat4 inversePose = glm::inverse(m_mat4HMDPose);
    myCam.InitialPosVR = glm::vec3(inversePose[3][0], inversePose[3][1], inversePose[3][2]);
#endif // VR_RUN

    // render loop
    // -----------
    float timeStart = glfwGetTime();
    int frames = 0;
    while (!glfwWindowShouldClose(window))
    {
        
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // -----
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        #ifdef VR_RUN
        shader.use();
        glm::mat4 inversePose = glm::inverse(m_mat4HMDPose);
        glm::vec3 position = glm::vec3(inversePose[3][0], inversePose[3][1], inversePose[3][2]);

        glm::mat4 viewMatrix = myCam.GetViewMatrix();
        glm::mat4 translatePos = glm::inverse(glm::translate(glm::mat4(1.0f), myCam.InitialPosVR));
        glm::mat4 finalMatrix = m_mat4HMDPose * glm::translate(glm::mat4(1.0f), myCam.InitialPosVR) * viewMatrix * translatePos;
        shader.setMat4("view", finalMatrix);
        shader.setMat4("projection", projection);
        shader.setMat4("model", glm::mat4(1.0f));
        shader.setVec3("camPos", position + myCam.Position);
        shader.setFloat("metallic", 0.0f);
        shader.setFloat("roughness", 0.65f);
        shader.setFloat("ao", 1.0f);
        float currentTime = glfwGetTime();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        ourModel.Draw(shader, currentTime);
#endif // VR_RUN
        shader.use();
        viewMatrix = myCam.GetViewMatrix();
        shader.setMat4("view", viewMatrix);
        shader.setMat4("projection", projection);
        shader.setMat4("model", glm::mat4(1.0));
        shader.setVec3("camPos", myCam.Position);
        shader.setFloat("metallic", 0.0f);
        shader.setFloat("roughness", 0.65f);
        shader.setFloat("ao", 1.0f);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        ourModel.updatePose(currentFrame);
        ourModel.Draw(shader, currentFrame);

#ifdef VR_RUN

        RenderStereoTargets();
        vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)leftEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
        vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
        vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)rightEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
        vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

        //vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
        UpdateHMDMatrixPose();
#endif


        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            shader.setVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
            shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        frames++;
        if (glfwGetTime() - timeStart >= 1)
        {
            printf("FPS: %d\n", frames);
            timeStart = glfwGetTime();
            frames = 0;
        }
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        //vr::VR_Shutdown();
        glfwSetWindowShouldClose(window, true);
    }
        

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        myCam.ProcessKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        myCam.ProcessKeyboard(BACKWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        myCam.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        myCam.ProcessKeyboard(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        //modelMatrix = glm::rotate(glm::mat4(1.0f), 3.14f * 1.5f, glm::vec3(1, 0, 0));
        ourModel.setAnimation("Armature|ArmatureAction");
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;
    myCam.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    myCam.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void initializeVR()
{
    m_pHMD = NULL;
    vr::EVRInitError eError = vr::VRInitError_None;
    m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);
    if (eError != vr::VRInitError_None)
    {
        m_pHMD = NULL;
        printf("Unable to init VR runtime: %s \n",
            vr::VR_GetVRInitErrorAsEnglishDescription(eError));
        exit(EXIT_FAILURE);
    }

    m_fNearClip = 0.1f;
    m_fFarClip = 30.0f;
}

bool SetupStereoRenderTargets()
{
    if (!m_pHMD)
        return false;

    m_pHMD->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);

    CreateFrameBuffer(m_nRenderWidth, m_nRenderHeight, leftEyeDesc);
    CreateFrameBuffer(m_nRenderWidth, m_nRenderHeight, rightEyeDesc);

    return true;
}

bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc& framebufferDesc)
{
    glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

    glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);

    glGenTextures(1, &framebufferDesc.m_nRenderTextureId);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

    glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);

    glGenTextures(1, &framebufferDesc.m_nResolveTextureId);
    glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);

    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void SetupCameras()
{
    m_mat4ProjectionLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);
    m_mat4ProjectionRight = GetHMDMatrixProjectionEye(vr::Eye_Right);
    m_mat4eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
    m_mat4eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);
    glm::mat4 m0 = m_mat4ProjectionLeft;
    printf("\nm_mat4ProjectionLeft:\n");
    printf("%f %f %f %f\n", m0[0][0], m0[1][0], m0[2][0], m0[3][0]);
    printf("%f %f %f %f\n", m0[0][1], m0[1][1], m0[2][1], m0[3][1]);
    printf("%f %f %f %f\n", m0[0][2], m0[1][2], m0[2][2], m0[3][2]);
    printf("%f %f %f %f\n", m0[0][3], m0[1][3], m0[2][3], m0[3][3]);
}

glm::mat4 GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye)
{
    if (!m_pHMD)
        return glm::mat4(1.0f);

    vr::HmdMatrix44_t mat = m_pHMD->GetProjectionMatrix(nEye, m_fNearClip, m_fFarClip);

    return glm::mat4(
        mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
        mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
        mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
        mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
    );
}

glm::mat4 GetHMDMatrixPoseEye(vr::Hmd_Eye nEye)
{
    if (!m_pHMD)
        return glm::mat4();

    vr::HmdMatrix34_t matEyeRight = m_pHMD->GetEyeToHeadTransform(nEye);
    glm::mat4 matrixObj(
        matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0,
        matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
        matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
        matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f
    );

    return glm::inverse(matrixObj);
}

void RenderScene(vr::Hmd_Eye nEye)
{
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    shader.use();
    glm::mat4 inversePose = glm::inverse(m_mat4HMDPose);
    glm::vec3 position = glm::vec3(inversePose[3][0], inversePose[3][1], inversePose[3][2]);

    glm::mat4 viewMatrix = myCam.GetViewMatrix();
    glm::mat4 translatePos = glm::inverse(glm::translate(glm::mat4(1.0f), myCam.InitialPosVR));
    glm::mat4 finalMatrix = m_mat4HMDPose * glm::translate(glm::mat4(1.0f), myCam.InitialPosVR) * viewMatrix * translatePos;
    shader.setMat4("view", finalMatrix);
    shader.setMat4("projection", GetCurrentViewProjectionMatrix(nEye));
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setVec3("camPos", position + myCam.Position);
    shader.setFloat("metallic", 0.0f);
    shader.setFloat("roughness", 0.65f);
    shader.setFloat("ao", 1.0f);
    float currentTime = glfwGetTime();
    ourModel.Draw(shader, currentTime);
}

glm::mat4 GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye)
{
    glm::mat4 matMVP;
    if (nEye == vr::Eye_Left)
    {
        matMVP = m_mat4ProjectionLeft * m_mat4eyePosLeft;
    }
    else if (nEye == vr::Eye_Right)
    {
        matMVP = m_mat4ProjectionRight * m_mat4eyePosRight;
    }

    return matMVP;
}

void RenderStereoTargets()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_MULTISAMPLE);

    // Left Eye
    glBindFramebuffer(GL_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
    glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
    RenderScene(vr::Eye_Left);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_MULTISAMPLE);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, leftEyeDesc.m_nResolveFramebufferId);

    glBlitFramebuffer(0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight,
        GL_COLOR_BUFFER_BIT,
        GL_LINEAR);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glEnable(GL_MULTISAMPLE);

    // Right Eye
    glBindFramebuffer(GL_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId);
    glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
    RenderScene(vr::Eye_Right);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_MULTISAMPLE);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rightEyeDesc.m_nResolveFramebufferId);

    glBlitFramebuffer(0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight,
        GL_COLOR_BUFFER_BIT,
        GL_LINEAR);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void UpdateHMDMatrixPose()
{
    if (!m_pHMD)
        return;

    vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

    m_iValidPoseCount = 0;
    m_strPoseClasses = "";
    for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
    {
        if (m_rTrackedDevicePose[nDevice].bPoseIsValid)
        {
            m_iValidPoseCount++;
            m_rmat4DevicePose[nDevice] = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
            if (m_rDevClassChar[nDevice] == 0)
            {
                switch (m_pHMD->GetTrackedDeviceClass(nDevice))
                {
                case vr::TrackedDeviceClass_Controller:        m_rDevClassChar[nDevice] = 'C'; break;
                case vr::TrackedDeviceClass_HMD:               m_rDevClassChar[nDevice] = 'H'; break;
                case vr::TrackedDeviceClass_Invalid:           m_rDevClassChar[nDevice] = 'I'; break;
                case vr::TrackedDeviceClass_GenericTracker:    m_rDevClassChar[nDevice] = 'G'; break;
                case vr::TrackedDeviceClass_TrackingReference: m_rDevClassChar[nDevice] = 'T'; break;
                default:                                       m_rDevClassChar[nDevice] = '?'; break;
                }
            }
            m_strPoseClasses += m_rDevClassChar[nDevice];
        }
    }

    if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
    {
        m_mat4HMDPose = m_rmat4DevicePose[vr::k_unTrackedDeviceIndex_Hmd];
        m_mat4HMDPose = glm::inverse(m_mat4HMDPose);
    }
}

glm::mat4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t& matPose)
{
    glm::mat4 matrixObj(
        matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
        matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
        matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
        matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
    );
    return matrixObj;
}