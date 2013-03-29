#include "Animation.h"
//#include "GL/freeglut.h"
#include <stack>
#include <GLUtils/GLUtils.h>
#include <ctime>
using namespace std;

const float Animation::kScale = 10.0f;
const float Animation::kScaleX = 1.0f;
const float Animation::kTranslateX = -5.0f;
const float Animation::kTranslateZ = 1.0f;

/*
 * Default constructor
 */
Animation::Animation(FootDrawable *left, FootDrawable *right)
{
	left_ = left;
	right_ = right;
    InitStates();
    Reset();
	RunTest();
	
	//added by david
	character_orientation = 0;
}

/*
 * Destructor
 */
Animation::~Animation()
{
	character_orientation = 0; //added
}

void Animation::ConvertAbsoluteToRelative(vector<vector<Quaternion2>> *jointQuaternion)
{
    for (int frameIdx = jointQuaternion->size() - 1; frameIdx > 0; --frameIdx) {
        (*jointQuaternion)[frameIdx][0].x -= (*jointQuaternion)[frameIdx - 1][0].x;
        (*jointQuaternion)[frameIdx][0].z -= (*jointQuaternion)[frameIdx - 1][0].z;
    }

    int lastFrameIdx = jointQuaternion->size() - 1;

    /*if (jointQuaternion->size() > 0) {
        (*jointQuaternion)[0][0].x -= (*jointQuaternion)[lastFrameIdx][0].x;
        (*jointQuaternion)[0][0].z -= (*jointQuaternion)[lastFrameIdx][0].z;
    }*/
}

void Animation::ConvertRelativetoAbsolute(vector<vector<Quaternion2>> *jointQuaternion)
{
    for (int frameIdx = 1; frameIdx < jointQuaternion->size(); ++frameIdx) {
        (*jointQuaternion)[frameIdx][0].x += (*jointQuaternion)[frameIdx - 1][0].x;
        (*jointQuaternion)[frameIdx][0].z += (*jointQuaternion)[frameIdx - 1][0].z;
    }
}

void Animation::GetRootPosition(double *x, double *y, double *z)
{
    VectorR3 pos = node_[0].GetGlobalPosition();
    *x = pos.x;
    *y = pos.y;
    *z = pos.z;
}

// return the transition from source to target
// frameCount is the number of frame generated
vector<vector<Quaternion2>> Animation::GetTransition(int firstBegin, int firstEnd,
                                         vector<vector<Quaternion2>> &firstMotion, int secondBegin, int secondEnd,
                                         vector<vector<Quaternion2>> &secondMotion)
{
    vector<vector<Quaternion2>> retVal = secondMotion;

    // the number of frames is the minimum of the two
    int firstLength = firstEnd - firstBegin + 1;
    int secondLength = secondEnd - secondBegin + 1;
    int frameCount = firstLength;
    if (frameCount > secondLength)
        frameCount = secondLength;

    //retVal.resize(secondMotion.size());

    double t = 0.0;
    double step = 1.0 / frameCount;

    // gradual blending
    for (int k = 0; k < frameCount; ++k) {
        int i = k + firstBegin;
        int j = k + secondBegin;

        //retVal[k].resize(firstMotion[i].size());
        // root pos
        retVal[j][0] = firstMotion[i][0] * (1.0 - t) + secondMotion[j][0] * t;
        // rest of joints
        for (int jointIdx = 1; jointIdx < firstMotion[i].size(); ++jointIdx) {
            retVal[j][jointIdx] = firstMotion[i][jointIdx].SlerpWith(secondMotion[j][jointIdx], t);
        }

        t += step;
    }

    return retVal;
}

vector<vector<Quaternion2>> Animation::SpeedUp(double ratio, vector<vector<Quaternion2>> &source)
{
    // create and resize the result
    vector<vector<Quaternion2>> retVal;
    retVal.resize((int) source.size() * ratio);
    for (int frameIdx = 0; frameIdx < retVal.size(); ++frameIdx) {
        retVal[frameIdx].resize(source[0].size());
    }

    // convert source to absolute
    ConvertRelativetoAbsolute(&source);

    for (int i = 0; i < retVal.size(); ++i) {
        double temp = i / ratio;
        int j1 = (int) temp;
        int j2 = j1 + 1;
        assert(j1 < source.size());
        if (j2 >= source.size()) { // if j1 is the last index, just copy
            for (int k = 0; k < retVal[i].size(); ++k) {
                retVal[i][k] = source[j1][k];
            }
        } else { // else, blend between two frames
            retVal[i][0] = source[j1][0] * (j2 - temp) + source[j2][0] * (temp - j1);
            for (int k = 1; k < retVal[i].size(); ++k) {
                retVal[i][k] = source[j1][k].SlerpWith(source[j2][k], temp - j1);
            }
        }
    }

    // convert retVal to relative
    ConvertAbsoluteToRelative(&retVal);

    return retVal;
}

void Animation::InterpolateMotions(vector<vector<Quaternion2>> *firstMotion,
                        vector<vector<Quaternion2>> *secondMotion,
                        vector<vector<Quaternion2>> *currentMotion, double t) {
                            for (int frameIdx = 0; frameIdx < firstMotion->size(); ++frameIdx) {
                                (*currentMotion)[frameIdx][0] = (*firstMotion)[frameIdx][0] * (1.0 - t) + (*secondMotion)[frameIdx][0] * t;
                                for (int jointIdx = 1; jointIdx < (*firstMotion)[frameIdx].size(); ++jointIdx) {
                                    (*currentMotion)[frameIdx][jointIdx] = (*firstMotion)[frameIdx][jointIdx].SlerpWith((*secondMotion)[frameIdx][jointIdx], t);
                                }
                            }
}

vector<vector<Quaternion2>> Animation::MakeLoop(vector<vector<Quaternion2>> &motion, int frameCount, int begin, int end)
{
    // blend the last segment and the first segment
    int lastSegStart = motion.size() - frameCount;
    int firstSegEnd = frameCount - 1;
    double t = 0;
    double step = 1.0 / frameCount;
    for (int k = 0; k < frameCount; ++k) {
        int i = lastSegStart + k;
        int j = k;
        motion[i][0] = motion[i][0] * (1.0 - t) + motion[j][0] * t;
        for (int jointIdx = 1; jointIdx < motion[i].size(); ++jointIdx) {
            motion[i][jointIdx] = motion[i][jointIdx].SlerpWith(motion[j][jointIdx], t);
        }
        t += step;
    }
    // make a new vector and return
    vector<vector<Quaternion2>> retVal;
    if (begin <= end) {
        retVal.resize(end - begin + 1);
        for (int i = 0; i < retVal.size(); ++i) {
            retVal[i] = motion[begin + i];
        }
    } else {
        retVal.resize(motion.size() - end);
        for (int i = 0; i < motion.size() - begin; ++i) {
            retVal[i] = motion[begin + i];
        }
        int e = end;
        for (int i = motion.size() - begin; i < retVal.size(); ++i) {
            retVal[i] = motion[e];
            e++;
        }
    }
    return retVal;
}

void Animation::Downsample(const vector<vector<Quaternion2>> &first, vector<vector<Quaternion2>> *second)
{
    for (int i = 0; i < first.size(); ++i) {
        double temp = i * (0.0 + second->size()) / (0.0 + first.size());
        int j1 = (int) temp;
        int j2 = j1 + 1;
        if (j2 >= second->size()) { // if j1 is the last index, just copy
            for (int k = 0; k < (*second)[i].size(); ++k) {
                (*second)[i][k] = (*second)[j1][k];
            }
        } else { // else, blend between two frames
            (*second)[i][0] = (*second)[j1][0] * (j2 - temp) + (*second)[j2][0] * (temp - j1);
            for (int k = 1; k < (*second)[i].size(); ++k) {
                // slerp
                (*second)[i][k] = (*second)[j1][k].SlerpWith((*second)[j2][k], temp - j1);
            }
        }
    }
    second->resize(first.size());
}

void Animation::RotateFrameBtoOriginal(vector<vector<VectorR3>> *jointTrajectory, vector<vector<Quaternion2>> *jointQuaternion)
{
    Quaternion2 qB = (*jointQuaternion)[0][1];
    Quaternion2 qY = qB.DecomposeRotation2(VectorR3::UnitY);
    qY.toUnit();
    for (int i = 0; i < jointTrajectory->size(); ++i) {
        Quaternion2 qB = (*jointQuaternion)[i][1];
        Quaternion2 newRotation = qY.Inverse() * qB;
        (*jointQuaternion)[i][1] = newRotation;

        VectorR3 newPos = qY.Inverse().rotate((*jointQuaternion)[i][0].ToPoint());
        (*jointQuaternion)[i][0].Set(newPos);
    }
}

Quaternion2 Animation::ConvertEulerAnglesToQuaternion(const VectorR3 &v)
{
    Quaternion2 qX;
    Quaternion2 qY;
    Quaternion2 qZ;
    qX.Set(v.x, VectorR3::UnitX);
    qY.Set(v.y, VectorR3::UnitY);
    qZ.Set(v.z, VectorR3::UnitZ);
    Quaternion2 retVal = qX * qY * qZ;
    return retVal;
}

void Animation::ConvertJointAnglesToQuaternion(const vector<vector<VectorR3>> &source, vector<vector<Quaternion2>> &target)
{
    for (int frameIdx = 0; frameIdx < source.size(); ++frameIdx) {
        target[frameIdx][0].Set(source[frameIdx][0]);
        for (int jointIdx = 1; jointIdx < source[frameIdx].size(); ++jointIdx) {
            Quaternion2 q = ConvertEulerAnglesToQuaternion(source[frameIdx][jointIdx]);
            target[frameIdx][jointIdx] = q;
        }
    }
}

void Animation::TranslateToOrigin(vector<vector<VectorR3>> *jointTrajectory)
{
    double x = (*jointTrajectory)[0][0].x;
    double z = (*jointTrajectory)[0][0].z;

    for (int frameIdx = 0; frameIdx < jointTrajectory->size(); ++frameIdx) {
        (*jointTrajectory)[frameIdx][0].x -= x;
        (*jointTrajectory)[frameIdx][0].z -= z;
    }

    for (int frameIdx = jointTrajectory->size() - 1; frameIdx > 0; --frameIdx) {
        (*jointTrajectory)[frameIdx][0].x -= (*jointTrajectory)[frameIdx - 1][0].x;
        (*jointTrajectory)[frameIdx][0].z -= (*jointTrajectory)[frameIdx - 1][0].z;
    }
}

void Animation::ConvertFromDegreeToRadian(vector<vector<VectorR3>> *jointTrajectory)
{
    // convert all rotation number to radian
    for (int i = 0; i < jointTrajectory->size(); ++i) {
        for (int j = 1; j < (*jointTrajectory)[i].size(); ++j) {
            (*jointTrajectory)[i][j] *= DegreesToRadians;
        }
    }
}

void Animation::ConvertFromRadianToDegree(vector<vector<VectorR3>> *jointTrajectory)
{
    // convert all rotation number to degree
    for (int i = 0; i < jointTrajectory->size(); ++i) {
        for (int j = 1; j < (*jointTrajectory)[i].size(); ++j) {
            (*jointTrajectory)[i][j] *= RadiansToDegrees;
        }
    }
}

/*
 * Reset the data before running each test
 */
void Animation::Reset()
{
    trajectory_.clear();
    jointTrajectory_.clear();
    learnedTargets_.clear();
    step_ = 0;
}

// trim from end
static inline string &rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

// trim from start
static inline string &ltrim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
    return s;
}

// trim from both ends
static inline string &trim(string &s) {
    return ltrim(rtrim(s));
}

/*
 * Builds a skeleton from a bvh
 */
void Animation::BuildSkeletonFromBVH(const char *fileName)
{
    const double     kNoRotation[3] = { 0.0, 0.0, 0.0 };
    const VectorR3 kRotationAxes[3] = { VectorR3::UnitX, VectorR3::UnitY, VectorR3::UnitZ };

    ifstream file(fileName);
    string line;

    getline(file, line); // HIERARCHY
    int id = -1;
    string root("ROOT");
    string offset("OFFSET");
    string channels("CHANNELS");
    string joint("JOINT");
    string endSite("End Site");

    stack<int> nodeStack;
    node_.resize(30);
    tree_.Reset();
    string name;

    while (line.compare("MOTION") != 0) {
        getline(file, line);
        line = trim(line);
        if (line.compare(0, offset.length(), offset) == 0) {
            istringstream lineStream(line);
            string temp;
            double x, y, z;
            lineStream >> temp >> x >> y >> z;

            assert(id < 30);
            node_[id] = Node(3, VectorR3(x, y, z), kRotationAxes, kNoRotation, 0.8, JOINT, name);

            if (id > 0)
                tree_.InsertChild(&node_[nodeStack.top()], &node_[id]);
            else if (id == 0)
                tree_.InsertRoot(&node_[id], false);

            nodeStack.push(id);
        } else if (line.compare(0, channels.length(), channels) == 0) {
            continue;
        } else if (line.compare(0, joint.length(), joint) == 0) {
            name = line.substr(root.length() + 1);
            ++id;
            continue;
        } else if (line.compare(0, endSite.length(), endSite) == 0) {
            name = "End";
            ++id;
            continue;
        } else if (line.compare(0, root.length(), root) == 0) {
            name = line.substr(root.length() + 1);
            ++id;
            continue;
        } else if (line[0] == '{') {
            continue;
        } else if (line[0] == '}') {
            nodeStack.pop();
            continue;
        }
    }

    file.close();
}

/*
 * Draw the 3 axes
 */
void Animation::DrawAxes(float size)
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glColor3f(1.00f, 0.00f, 0.00f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(size, 0.0f, 0.0f);
        glColor3f(0.00f, 1.00f, 0.00f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, size, 0.0f);
        glColor3f(0.00f, 0.00f, 1.00f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, size);
    glEnd();
    glLineWidth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

/*
 * Draws the skeleton each frame
 */
void Animation::draw()
{
    glPushMatrix();
    tree_.Draw();
    glPopMatrix();
    /*glPushMatrix();
    glTranslatef(kTranslateX, 0.0f, kTranslateZ);
    glScalef(kScaleX, 1.0f, 1.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(1.0 / kScale, 1.0 / kScale, 1.0 / kScale);

    tree_.Draw();
    glPopMatrix();

    DrawAxes(5.0f);

    float footWidth = 0.25f;
    float footLength = 0.5f;
    float footThickness = 0.1f;

    Point3d boxMin(-0.5f * footWidth + trajectory_[step_][0].x, trajectory_[step_][0].y, -0.5f * footLength + trajectory_[step_][0].z);
    Point3d boxMax(0.5f * footWidth + trajectory_[step_][0].x, trajectory_[step_][0].y, 0.5f * footLength + trajectory_[step_][0].z);

    glPushMatrix();
    GLUtils::drawWireFrameBox(boxMin, boxMax);
    glPopMatrix();

    Point3d boxMin2(-0.5f * footWidth + trajectory_[step_][1].x, trajectory_[step_][1].y, -0.5f * footLength + trajectory_[step_][1].z);
    Point3d boxMax2(0.5f * footWidth + trajectory_[step_][1].x, trajectory_[step_][1].y, 0.5f * footLength + trajectory_[step_][1].z);

    glPushMatrix();
    GLUtils::drawWireFrameBox(boxMin2, boxMax2);
    glPopMatrix();*/
}

// to be in the air twice longer, we need to jump four times higher
void Animation::ScaleJumpingHeight(double ratio, vector<vector<Quaternion2>> *jointQuaternion, int begin, int end)
{
    assert(0 <= begin && begin <= end && end < jointQuaternion->size());
    // calculate the number of frames it take to reach max height
    int t = 0;
    for (t = begin; t < end; ++t) {
        double height = (*jointQuaternion)[t][0].ToPoint().y;
        double nextHeight = (*jointQuaternion)[t + 1][0].ToPoint().y;
        if (height < nextHeight) { // if we see the height increases for the first time, break
            break;
        }
        ++t;
    }
    int trueBegin = t; // when the trajectory starts going up
    for (t = trueBegin + 1; t < end; ++t) {
        double height = (*jointQuaternion)[t][0].ToPoint().y;
        double nextHeight = (*jointQuaternion)[t + 1][0].ToPoint().y;
        if (height > nextHeight) { // if we see the height decreases break
            break;
        }
    }
    int half = t; // when we reach max height
    for (t = half + 1; t < end; ++t) {
        double height = (*jointQuaternion)[t][0].ToPoint().y;
        double nextHeight = (*jointQuaternion)[t + 1][0].ToPoint().y;
        if (height < nextHeight) { // if we see the height increases again, break
            break;
        }
    }
    int trueEnd = t;

    const double g = 70.00; // 981 cm/s^2
    int T = trueEnd - trueBegin; // T = time to reach max height
    double v0y = T * g / 2.0 / 60.0;
    v0y = v0y * ratio;
    double y0 = (*jointQuaternion)[trueBegin][0].ToPoint().y;
    //printf("h max: %.3f\n", y0 + v0y * v0y / g);
    for (int t = 0; t <= T; ++t) {
        double desiredHeight = y0 + v0y * t * ratio / 60.0 - 0.5 * g * t * t * ratio * ratio / 3600.0;
        VectorR3 currentPos = (*jointQuaternion)[t + trueBegin][0].ToPoint();
        double currentHeight = currentPos.y;
        // printf("%d : %.3f\n", t + trueBegin, desiredHeight);
        currentPos.y += desiredHeight - currentHeight;
        (*jointQuaternion)[t + trueBegin][0].Set(currentPos);
    }
}

/*
 * Updates the skeleton each frame
 */
bool Animation::Update(int motionType, double stepLengthRatio, double stepDurationRatio)
{
    bool retVal = false;
    double t = stepLengthRatio;
    double d = stepDurationRatio;

    static bool firstCycle = true;
    static VectorR3 fixedFootPos;
    static int fixedFoot;

    // select the types of motion
    static vector<vector<Quaternion2>> *firstMotion;
    static vector<vector<Quaternion2>> *secondMotion;
    static int previousMotionType = -1;
    static bool isRootMovedLastFrame;

    // if we finish one step
    if (step_ == 0 || step_ == currentMotion_.size() / 2) {
        // save the current motion
        oldMotion_ = currentMotion_;
        int oldHalf = oldMotion_.size() / 2;
        // select the motion type
        if (motionType == WALKING) {
            firstMotion = &jointQuaternions_[WALK_INPLACE];
            secondMotion = &jointQuaternions_[WALK_FORWARD];
        } else if (motionType == WALKING_BACKWARDS) {
            firstMotion = &jointQuaternions_[WALK_INPLACE];
            secondMotion = &jointQuaternions_[WALK_BACKWARD];
        } else if (motionType == RUNNING_FAST) {
            firstMotion = &jointQuaternions_[RUN_NORMAL];
            secondMotion = &jointQuaternions_[RUN_FAST];
        } else if (motionType == JUMPING_SHORT) {
            firstMotion = &jointQuaternions_[JUMP_SHORT_INPLACE];
            secondMotion = &jointQuaternions_[JUMP_SHORT_FORWARD];
        } else if (motionType == TIPTOEING) {
            t /= 4.0;
            firstMotion = &jointQuaternions_[TIPTOE_INPLACE];
            secondMotion = &jointQuaternions_[TIPTOE_FORWARD];
        } else if (motionType == TIPTOEING_BACKWARDS) {
            t /= 4.0;
            firstMotion = &jointQuaternions_[TIPTOE_INPLACE];
            secondMotion = &jointQuaternions_[TIPTOE_BACKWARD];
        } else if (motionType == PINO_WALKING) {
            firstMotion = &jointQuaternions_[PINO_WALK_INPLACE];
            secondMotion = &jointQuaternions_[PINO_WALK_FORWARD];
        } else if (motionType == PINO_WALKING_BACKWARDS) {
            firstMotion = &jointQuaternions_[PINO_WALK_INPLACE];
            secondMotion = &jointQuaternions_[PINO_WALK_BACKWARD];
        } else if (motionType == LIMPING) {
            firstMotion = &jointQuaternions_[LIMP_INPLACE];
            secondMotion = &jointQuaternions_[LIMP_FORWARD];
        } else if (motionType == LIMPING_BACKWARDS) {
            firstMotion = &jointQuaternions_[LIMP_INPLACE];
            secondMotion = &jointQuaternions_[LIMP_BACKWARD];
        } else if (motionType == JUMPING_LONG) {
            firstMotion = &jointQuaternions_[JUMP_SHORT_FORWARD];
            secondMotion = &jointQuaternions_[JUMP_LONG_FORWARD];
            d *= 2;
        } else if (motionType == JOGGING) {
            firstMotion = &jointQuaternions_[RUN_INPLACE];
            secondMotion = &jointQuaternions_[JOG];
        } else if (motionType == RUNNING_NORMAL) {
            firstMotion = &jointQuaternions_[JOG];
            secondMotion = &jointQuaternions_[RUN_NORMAL];
        }
		else if (motionType == CATWALK_NORMAL) {
            firstMotion = &jointQuaternions_[CATWALK_FORWARD];
            secondMotion = &jointQuaternions_[CATWALK_FORWARD];
        }
		else if (motionType == MOONWALK) {
            firstMotion = &jointQuaternions_[MOONWALK_FORWARD];
            secondMotion = &jointQuaternions_[MOONWALK_FORWARD];
        }
		else if (motionType == ONE_HOP) {
            firstMotion = &jointQuaternions_[ONE_HOPPING];
            secondMotion = &jointQuaternions_[ONE_HOPPING];
        }
		else if (motionType == TAP) {
            firstMotion = &jointQuaternions_[TAP_DANCE];
            secondMotion = &jointQuaternions_[TAP_DANCE];
        }
		else if (motionType == SPIN) {
            firstMotion = &jointQuaternions_[BALLET_SPIN];
            secondMotion = &jointQuaternions_[BALLET_SPIN];
        }
		else if (motionType == SKATING) {
            firstMotion = &jointQuaternions_[SKATE_FORWARD];
            secondMotion = &jointQuaternions_[SKATE_FORWARD];
        }
		else if (motionType == DUCKING) {
            firstMotion = &jointQuaternions_[DUCK];
            secondMotion = &jointQuaternions_[DUCK];
        }
		else if (motionType == KICKING) {
            firstMotion = &jointQuaternions_[KICK_FORWARD];
            secondMotion = &jointQuaternions_[KICK_FORWARD];
        }
		else if (motionType == MARCHING) {
            firstMotion = &jointQuaternions_[MARCH_FORWARD];
            secondMotion = &jointQuaternions_[MARCH_FORWARD];
        }
		else if (motionType == SIDE_WALKING) {
            firstMotion = &jointQuaternions_[SIDE_WALK];
            secondMotion = &jointQuaternions_[SIDE_WALK];
        }
		else if (motionType == SNEAKING) {
            firstMotion = &jointQuaternions_[SNEAK_FORWARD];
            secondMotion = &jointQuaternions_[SNEAK_FORWARD];
        }
		else if (motionType == SPLITTING) {
            firstMotion = &jointQuaternions_[SPLIT_FORWARD];
            secondMotion = &jointQuaternions_[SPLIT_FORWARD];
        }
		else if (motionType == SITTING) {
            firstMotion = &jointQuaternions_[SIT_FORWARD];
            secondMotion = &jointQuaternions_[SIT_FORWARD];
        }
		else if (motionType == SIDE_JUMPING) {
            firstMotion = &jointQuaternions_[SIDE_JUMP];
            secondMotion = &jointQuaternions_[SIDE_JUMP];
        }
		else if (motionType == BRIDGING) {
            firstMotion = &jointQuaternions_[BRIDGE];
            secondMotion = &jointQuaternions_[BRIDGE];
        }
        // interpolate the two motions
		
        currentMotion_.resize(firstMotion->size());
        for (int i = 0; i < currentMotion_.size(); ++i) {
            currentMotion_[i].resize((*firstMotion)[i].size());
        }
        InterpolateMotions(firstMotion, secondMotion, &currentMotion_, t);
        // speed up or slow down
       // currentMotion_ = SpeedUp(d, currentMotion_);
        // if this is jumping, scale the height
        /*if (motionType == JUMPING_SHORT) {
            if (step_ == 0) {
                ScaleJumpingHeight(d, &currentMotion_, 0, currentMotion_.size() / 2 - 1);
            } else {
                ScaleJumpingHeight(d, &currentMotion_, currentMotion_.size() / 2, currentMotion_.size() - 1);
            }
        }*/
        int newHalf = currentMotion_.size() / 2;
        // blend the previous motion and the current one, if not the first cycle
        if (!firstCycle) {
            tree_.Compute();
            if (step_ == 0) {
                currentMotion_ = GetTransition(0, oldHalf, oldMotion_, 0, newHalf, currentMotion_);
                // save the right foot
                fixedFoot = RIGHT_FOOT;
                fixedFootPos = node_[fixedFoot].GetGlobalPosition();
            } else if (step_ == oldHalf) {
                currentMotion_ = GetTransition(oldHalf, oldMotion_.size() - 1, oldMotion_,
                    newHalf, currentMotion_.size() - 1, currentMotion_);
                step_ = newHalf;
                // save the left foot
                fixedFoot = LEFT_FOOT;
                fixedFootPos = node_[fixedFoot].GetGlobalPosition();
            }
            isRootMovedLastFrame = true;
        }
    }

    // set the position of the root
	//modified
	//character_orientation = 30*asin(1.0)/90; //for testing
    VectorR3 newPos = currentMotion_[step_][0].ToPoint();
    VectorR3 currPos = node_[0].GetLocalPosition();
    node_[0].SetPosition(currPos.x + newPos.x*cos(character_orientation)+newPos.z*sin(character_orientation), newPos.y,
							currPos.z + newPos.z*cos(character_orientation)-newPos.x*sin(character_orientation)); //added
	//node_[0].SetPosition(currPos.x + newPos.x, newPos.y, currPos.z + newPos.z);
    //node_[0].SetPosition(0, 32.0, 0);

    // set the orientation of the joints
    node_[0].SetQuaternion(Quaternion2().getRotationQuaternion2(character_orientation, VectorR3(0.0, 1.0, 0.0))*currentMotion_[step_][1]); //added
	//node_[0].SetQuaternion(Quaternion2().getRotationQuaternion2(currentMotion_[step_][1]);
    //node_[0].SetQuaternion(Quaternion::IdentityQuaternion);
    for (int i = 1; i < 30; ++i) {
        int j;
        if ((j = node_[i].GetIndex()) != -1) {
            node_[i].SetQuaternion(currentMotion_[step_][j]);
        }
    }

	//added
	//rotate the knee if the ankle is below the ground
	tree_.Compute();
	double hAnkleLeft = node_[LEFT_ANKLE].GetGlobalPosition().y;
	double hAnkleRight = node_[RIGHT_ANKLE].GetGlobalPosition().y;
	while (hAnkleLeft < 0.0) {
		tree_.Compute();
		hAnkleLeft = node_[LEFT_ANKLE].GetGlobalPosition().y;
		Quaternion2 rot;
        //rot.Set(7.0 * -asin(hAnkleLeft / 30.0), VectorR3::UnitX);
		if (node_[LEFT_KNEE].GetGlobalPosition().z>node_[LEFT_ANKLE].GetGlobalPosition().z)
			rot.Set(0.02, VectorR3::UnitX);
		else
			rot.Set(-0.02, VectorR3::UnitX);
        Quaternion2 q = node_[LEFT_KNEE].GetQuaternion();
        q = rot * q;
        node_[LEFT_KNEE].SetQuaternion(q);
    }
    while (hAnkleRight < 0.0) {
		tree_.Compute();
		hAnkleRight = node_[RIGHT_ANKLE].GetGlobalPosition().y;
        Quaternion2 rot;
        //rot.Set(7.0 * -asin(hAnkleRight / 30.0), VectorR3::UnitX);
		if (node_[RIGHT_KNEE].GetGlobalPosition().z>node_[RIGHT_ANKLE].GetGlobalPosition().z)
			rot.Set(0.02, VectorR3::UnitX);
		else
			rot.Set(-0.02, VectorR3::UnitX);
        Quaternion2 q = node_[RIGHT_KNEE].GetQuaternion();
        q = rot * q;
        node_[RIGHT_KNEE].SetQuaternion(q);
    }

    // rotate the ankle if the tip is below the ground
    tree_.Compute();
    double dLeft = min(node_[LEFT_FOOT].GetGlobalPosition().y, node_[LEFT_TOE].GetGlobalPosition().y);
    double dRight = min(node_[RIGHT_FOOT].GetGlobalPosition().y, node_[RIGHT_TOE].GetGlobalPosition().y);
    if (dLeft < 0) {
        Quaternion2 rot;
        rot.Set(4.0 * -asin(-dLeft / 30.0), VectorR3::UnitX);
        Quaternion2 q = node_[LEFT_ANKLE].GetQuaternion();
        q = rot * q;
        node_[LEFT_ANKLE].SetQuaternion(q);
    }
    if (dRight < 0) {
        Quaternion2 rot;
        rot.Set(4.0 * -asin(-dRight / 30.0), VectorR3::UnitX);
        Quaternion2 q = node_[RIGHT_ANKLE].GetQuaternion();
        q = rot * q;
        node_[RIGHT_ANKLE].SetQuaternion(q);
    }

    // if not the first cycle, try to move the root to prevent foot sliding
    if (!firstCycle) {
        tree_.Compute();
        if (fixedFoot == LEFT_FOOT)
            assert(step_ >= currentMotion_.size() / 2);
        else if (fixedFoot == RIGHT_FOOT)
            assert(step_ < currentMotion_.size() / 2);
        VectorR3 footPos = node_[fixedFoot].GetGlobalPosition();
		//changed
        if (fabs(footPos.y) < 0.5 && isRootMovedLastFrame) { // only move the root if the foot is in contact with the ground <4.0
            VectorR3 delta = footPos - fixedFootPos;
            node_[0].AddToTranslation(-delta.x, 0.0, -delta.z);
			//printf("moved\n");
        } else {
            isRootMovedLastFrame = false;
        }
    }

    // go to next frame, or back at the beginning
    ++step_;
    if (step_ >= currentMotion_.size() / 2) {
        firstCycle = false;
    }

    if (step_ == currentMotion_.size() / 2) { // end of half cycle
        retVal = true;
    }

    if (step_ >= currentMotion_.size()) { // end of one cycle
        retVal = true;
        step_ = 0;
    }
	
    return retVal;
}

/*
 * Recomputes the Jacobian
 */
void Animation::RebuildJacobian()
{
    //delete jacobian_;
    jacobian_ = new Jacobian(&tree_, &targets_);
    jacobian_->Reset();
    jacobian_->ComputeJacobian();
}

/*
 * Initializes some data
 */
void Animation::InitStates()
{
    method_ = DLS;
    useJacobianTargets_ = false;
    step_ = 0;
    srand (time(NULL));
}

/*
 * Updates the targets and recalculates the Jacobian
 */
bool Animation::DoUpdateStep(int motionType, double stepLengthRatio, double stepDurationRatio) {
    return Update(motionType, stepLengthRatio, stepDurationRatio);
    /*if (targets_.empty())
        return;

    if (useJacobianTargets_)
        jacobian_->SetJtargetActive();
    else
        jacobian_->SetJendActive();

    jacobian_->ComputeJacobian();

    switch (method_) {
        case TRANSPOSE:
            jacobian_->CalcDeltaThetasTranspose();
            break;
        case DLS:
            jacobian_->CalcDeltaThetasDLS();
            break;
        case PSEUDOINVERSE:
            jacobian_->CalcDeltaThetasPseudoinverse();
            break;
        case SDLS:
            jacobian_->CalcDeltaThetasSDLS();
            break;
        default:
            jacobian_->ZeroDeltaThetas();
            break;
    }

    jacobian_->UpdateThetas();
    jacobian_->UpdatedSClampValue();*/
}

/*
 * Converts the training data to the new coordinate
 */
void Animation::ConvertTrainingDatatoWilliam()
{
    for (int frameIndex = 0; frameIndex < trajectory_.size(); ++frameIndex) {
        for (int i = 0; i < trajectory_[frameIndex].size(); ++i) {
            trajectory_[frameIndex][i] /= kScale;
            trajectory_[frameIndex][i].x *= kScaleX;
            double temp = trajectory_[frameIndex][i].x;
            trajectory_[frameIndex][i].x = trajectory_[frameIndex][i].z;
            trajectory_[frameIndex][i].z = -temp;
            trajectory_[frameIndex][i].x += kTranslateX;
            trajectory_[frameIndex][i].z += kTranslateZ;
        }
    }
}

/*
 * Converts the feet positions back to mine learning space
 */
void Animation::ConvertFootDataBacktoMine()
{
    for (int frameIndex = 0; frameIndex < trajectory_.size(); ++frameIndex) {
        for (int i = 0; i < trajectory_[frameIndex].size(); ++i) {
            trajectory_[frameIndex][i].z -= kTranslateZ;
            trajectory_[frameIndex][i].x -= kTranslateX;
            double temp = trajectory_[frameIndex][i].x;
            trajectory_[frameIndex][i].x = -trajectory_[frameIndex][i].z;
            trajectory_[frameIndex][i].z = temp;
            trajectory_[frameIndex][i].x /= kScaleX;
            trajectory_[frameIndex][i] *= kScale;
        }
    }
}

/*
 * Builds a human skeleton
 */
void Animation::BuildSkeleton(float size)
{
    const double     kNoRotation[3] = { 0.0, 0.0, 0.0 };
    const VectorR3 kRotationAxes[3] = { VectorR3::UnitX, VectorR3::UnitY, VectorR3::UnitZ };

	const double  x15[3] = {  15.0, 0.0,   0.0 };
	const double xm15[3] = { -15.0, 0.0,   0.0 };
	const double  z90[3] = {   0.0, 0.0,  90.0 };
	const double zm90[3] = {   0.0, 0.0, -90.0 };

	tree_.Reset();
	node_.resize(30);

	node_[0] = Node(3, VectorR3(0.0f, 0.0f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT);  // Root
	tree_.InsertRoot(&node_[0], false);
	node_[1] = Node(3, VectorR3(0.2f, 0.0f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Left femur
	tree_.InsertChild(&node_[0], &node_[1]);
	node_[2] = Node(3, VectorR3(0.0f, -0.87f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Left tibia
	tree_.InsertChild(&node_[1], &node_[2]);
	node_[3] = Node(3, VectorR3(0.0f, -0.85f, 0.0f) * size, kRotationAxes, x15,
		0.05, JOINT); // Left foot
	tree_.InsertChild(&node_[2], &node_[3]);
	node_[4] = Node(3, VectorR3(0.0f, -0.08f, 0.32f) * size, kRotationAxes, xm15,
		0.05, JOINT); // Left toe
	tree_.InsertChild(&node_[3], &node_[4]);
	node_[5] = Node(3, VectorR3(0.0f, 0.0f, 0.16f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Left toe end
	tree_.InsertChild(&node_[4], &node_[5]);
	node_[6] = Node(3, VectorR3(-0.2f, 0.0f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Right femur
	tree_.InsertChild(&node_[0], &node_[6]);
	node_[7] = Node(3, VectorR3(0.0f, -0.87f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Right tibia
	tree_.InsertChild(&node_[6], &node_[7]);
	node_[8] = Node(3, VectorR3(0.0f, -0.85f, 0.0f) * size, kRotationAxes, x15,
		0.05, JOINT); // Right foot
	tree_.InsertChild(&node_[7], &node_[8]);
	node_[9] = Node(3, VectorR3(0.0f, -0.085f, 0.32f) * size, kRotationAxes, xm15,
		0.05, JOINT); // Right toe
	tree_.InsertChild(&node_[8], &node_[9]);
	node_[10] = Node(3, VectorR3(0.0f, 0.0f, 0.16f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Right toe end
	tree_.InsertChild(&node_[9], &node_[10]);
	node_[11] = Node(3, VectorR3(0.0f, 0.002f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Lower back
	tree_.InsertChild(&node_[0], &node_[11]);
	node_[12] = Node(3, VectorR3(0.0f, 0.56f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Thorax
	tree_.InsertChild(&node_[11], &node_[12]);
	node_[13] = Node(3, VectorR3(0.0f, 0.43f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Neck
	tree_.InsertChild(&node_[12], &node_[13]);
	node_[14] = Node(3, VectorR3(0.0f, 0.38f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Head
	tree_.InsertChild(&node_[13], &node_[14]);
	node_[15] = Node(3, VectorR3(0.0f, 0.25f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Head end
	tree_.InsertChild(&node_[14], &node_[15]);
	node_[16] = Node(3, VectorR3(0.0f, 0.43f, 0.0f) * size, kRotationAxes, zm90,
		0.05, JOINT); // Left collar
	tree_.InsertChild(&node_[12], &node_[16]);
	node_[17] = Node(3, VectorR3(0.37f, 0.0f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Left humerus
	tree_.InsertChild(&node_[16], &node_[17]);
	node_[18] = Node(3, VectorR3(0.61f, 0.0f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Left elbow
	tree_.InsertChild(&node_[17], &node_[18]);
	node_[19] = Node(3, VectorR3(0.47f, 0.0f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Left wrist
	tree_.InsertChild(&node_[18], &node_[19]);
	node_[20] = Node(3, VectorR3(0.0f, 0.0f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Left thumb
	tree_.InsertChild(&node_[19], &node_[20]);
	node_[21] = Node(3, VectorR3(0.0f, 0.0f, 0.22f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Left thumb end
	tree_.InsertChild(&node_[20], &node_[21]);
	node_[22] = Node(3, VectorR3(0.22f, 0.0f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.05, JOINT); // Left wrist end
	tree_.InsertChild(&node_[19], &node_[22]);
	node_[23] = Node(3, VectorR3(0.0f, 0.43f, 0.0f) * size, kRotationAxes, z90,
		0.08, JOINT); // Right collar
	tree_.InsertChild(&node_[12], &node_[23]);
	node_[24] = Node(3, VectorR3(-0.37f, 0.0f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.08, JOINT); // Right humerus
	tree_.InsertChild(&node_[23], &node_[24]);
	node_[25] = Node(3, VectorR3(-0.61f, 0.0f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.08, JOINT); // Right elbow
	tree_.InsertChild(&node_[24], &node_[25]);
	node_[26] = Node(3, VectorR3(-0.47f, 0.0f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.08, JOINT); // Right wrist
	tree_.InsertChild(&node_[25], &node_[26]);
	node_[27] = Node(3, VectorR3(0.0f, 0.0f, 0.0f) * size, kRotationAxes, kNoRotation,
		0.08, JOINT); // Right thumb
	tree_.InsertChild(&node_[26], &node_[27]);
	node_[28] = Node(3, VectorR3(0.0f, 0.0f, 0.22f) * size, kRotationAxes, kNoRotation,
		0.08, JOINT); // Right thumb end
	tree_.InsertChild(&node_[27], &node_[28]);
	node_[29] = Node(3, VectorR3(-0.3f, 0.0, 0.0f) * size, kRotationAxes, kNoRotation,
		0.08, JOINT); // Right wrist end
	tree_.InsertChild(&node_[26], &node_[29]);
}

double Animation::GetRootHeight()
{
    tree_.Compute();
    return node_[0].GetGlobalPosition().y - (node_[3].GetGlobalPosition().y + node_[8].GetGlobalPosition().y) / 2.0;
}

void Animation::SimpleRetarget(double rootHeightTarget, double rootHeightSource, vector<vector<Quaternion2>> *jointQuaternion)
{
    double ratioY = rootHeightSource / rootHeightTarget;
    for (int i = 0; i < jointQuaternion->size(); ++i) {
        VectorR3 pos = (*jointQuaternion)[i][0].ToPoint();
        pos /= ratioY;
        (*jointQuaternion)[i][0].Set(pos);
    }
}

void Animation::ForceNoWristRotation(vector<vector<Quaternion2>> *jointQuaternion)
{
    int leftWristIndex = node_[LEFT_WRIST].GetIndex();
    int rightWristIndex = node_[RIGHT_WRIST].GetIndex();

    for (int frameIndex = 0; frameIndex < jointQuaternion->size(); ++frameIndex) {
        (*jointQuaternion)[frameIndex][leftWristIndex].Set(Quaternion2::IdentityQuaternion2);
        (*jointQuaternion)[frameIndex][rightWristIndex].Set(Quaternion2::IdentityQuaternion2);
    }
}

/*
 * Run the test
 */
void Animation::RunTest()
{

	/* ---------------- read the bridge forward ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/Bridge.bvh");
	//"./animation/data/tapDance_crop1.bvh"
    rootHeight[BRIDGE] = GetRootHeight();
    AssignIndex();
	cout<<"bridge"<<endl;
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[BRIDGE], "./animation/data/Bridge.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[BRIDGE]);
    ConvertFromDegreeToRadian(&jointTrajectories_[BRIDGE]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[BRIDGE]);
    // resize the quaternions array accordingly
    jointQuaternions_[BRIDGE].resize(jointTrajectories_[BRIDGE].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[BRIDGE].size(); ++frameIdx) {
        jointQuaternions_[BRIDGE][frameIdx].resize(jointTrajectories_[BRIDGE][frameIdx].size());
    }
	
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[BRIDGE], jointQuaternions_[BRIDGE]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[BRIDGE], &jointQuaternions_[BRIDGE]);
    // make loop
	
    jointQuaternions_[BRIDGE] = MakeLoop(jointQuaternions_[BRIDGE],
        jointQuaternions_[BRIDGE].size() / 3,
        jointQuaternions_[BRIDGE].size() * 2 / 3,
        jointQuaternions_[BRIDGE].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[BRIDGE]);
/* ---------------- read the side jumping forward ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/SideJump.bvh");
	//"./animation/data/tapDance_crop1.bvh"
    rootHeight[SIDE_JUMP] = GetRootHeight();
    AssignIndex();
	cout<<"sidejump"<<endl;
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[SIDE_JUMP], "./animation/data/SideJump.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[SIDE_JUMP]);
    ConvertFromDegreeToRadian(&jointTrajectories_[SIDE_JUMP]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[SIDE_JUMP]);
    // resize the quaternions array accordingly
    jointQuaternions_[SIDE_JUMP].resize(jointTrajectories_[SIDE_JUMP].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[SIDE_JUMP].size(); ++frameIdx) {
        jointQuaternions_[SIDE_JUMP][frameIdx].resize(jointTrajectories_[SIDE_JUMP][frameIdx].size());
    }
	
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[SIDE_JUMP], jointQuaternions_[SIDE_JUMP]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[SIDE_JUMP], &jointQuaternions_[SIDE_JUMP]);
    // make loop
	
    jointQuaternions_[SIDE_JUMP] = MakeLoop(jointQuaternions_[SIDE_JUMP],
        jointQuaternions_[SIDE_JUMP].size() / 3,
        jointQuaternions_[SIDE_JUMP].size() * 2 / 3,
        jointQuaternions_[SIDE_JUMP].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[SIDE_JUMP]);


/* ---------------- read the sit forward ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/sitstand.bvh");
	//"./animation/data/tapDance_crop1.bvh"
    rootHeight[SIT_FORWARD] = GetRootHeight();
    AssignIndex();
    // read joint trajectories and process discontinuities
	//sitstand
	cout<<"sitstand"<<endl;
    ReadBVHFile(&jointTrajectories_[SIT_FORWARD], "./animation/data/sitstand.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[SIT_FORWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[SIT_FORWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[SIT_FORWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[SIT_FORWARD].resize(jointTrajectories_[SIT_FORWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[SIT_FORWARD].size(); ++frameIdx) {
        jointQuaternions_[SIT_FORWARD][frameIdx].resize(jointTrajectories_[SIT_FORWARD][frameIdx].size());
    }
	
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[SIT_FORWARD], jointQuaternions_[SIT_FORWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[SIT_FORWARD], &jointQuaternions_[SIT_FORWARD]);
    // make loop
	
    jointQuaternions_[SIT_FORWARD] = MakeLoop(jointQuaternions_[SIT_FORWARD],
        jointQuaternions_[SIT_FORWARD].size() / 3,
        jointQuaternions_[SIT_FORWARD].size() * 2 / 3,
        jointQuaternions_[SIT_FORWARD].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[SIT_FORWARD]);


/* ---------------- read the split forward ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/split_crop.bvh");
	//"./animation/data/tapDance_crop1.bvh"
    rootHeight[SPLIT_FORWARD] = GetRootHeight();
    AssignIndex();
	cout<<"split"<<endl;
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[SPLIT_FORWARD], "./animation/data/split_crop.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[SPLIT_FORWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[SPLIT_FORWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[SPLIT_FORWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[SPLIT_FORWARD].resize(jointTrajectories_[SPLIT_FORWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[SPLIT_FORWARD].size(); ++frameIdx) {
        jointQuaternions_[SPLIT_FORWARD][frameIdx].resize(jointTrajectories_[SPLIT_FORWARD][frameIdx].size());
    }
	
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[SPLIT_FORWARD], jointQuaternions_[SPLIT_FORWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[SPLIT_FORWARD], &jointQuaternions_[SPLIT_FORWARD]);
    // make loop
	
    jointQuaternions_[SPLIT_FORWARD] = MakeLoop(jointQuaternions_[SPLIT_FORWARD],
        jointQuaternions_[SPLIT_FORWARD].size() / 3,
        jointQuaternions_[SPLIT_FORWARD].size() * 2 / 3,
        jointQuaternions_[SPLIT_FORWARD].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[SPLIT_FORWARD]);


	/* ---------------- read the sneaking forward ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/sneak_long.bvh");
	//"./animation/data/tapDance_crop1.bvh"
    rootHeight[SNEAK_FORWARD] = GetRootHeight();
    AssignIndex();
	cout<<"sneak"<<endl;
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[SNEAK_FORWARD], "./animation/data/sneak_long.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[SNEAK_FORWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[SNEAK_FORWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[SNEAK_FORWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[SNEAK_FORWARD].resize(jointTrajectories_[SNEAK_FORWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[SNEAK_FORWARD].size(); ++frameIdx) {
        jointQuaternions_[SNEAK_FORWARD][frameIdx].resize(jointTrajectories_[SNEAK_FORWARD][frameIdx].size());
    }
	
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[SNEAK_FORWARD], jointQuaternions_[SNEAK_FORWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[SNEAK_FORWARD], &jointQuaternions_[SNEAK_FORWARD]);
    // make loop
	
    jointQuaternions_[SNEAK_FORWARD] = MakeLoop(jointQuaternions_[SNEAK_FORWARD],
        jointQuaternions_[SNEAK_FORWARD].size() / 3,
        jointQuaternions_[SNEAK_FORWARD].size() * 2 / 3,
        jointQuaternions_[SNEAK_FORWARD].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[SNEAK_FORWARD]);

	/* ---------------- read the side walking forward ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/sidewalk_right.bvh");
	//"./animation/data/tapDance_crop1.bvh"
    rootHeight[SIDE_WALK] = GetRootHeight();
    AssignIndex();
	cout<<"sidewalk"<<endl;
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[SIDE_WALK], "./animation/data/sidewalk_right.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[SIDE_WALK]);
    ConvertFromDegreeToRadian(&jointTrajectories_[SIDE_WALK]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[SIDE_WALK]);
    // resize the quaternions array accordingly
    jointQuaternions_[SIDE_WALK].resize(jointTrajectories_[SIDE_WALK].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[SIDE_WALK].size(); ++frameIdx) {
        jointQuaternions_[SIDE_WALK][frameIdx].resize(jointTrajectories_[SIDE_WALK][frameIdx].size());
    }
	
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[SIDE_WALK], jointQuaternions_[SIDE_WALK]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[SIDE_WALK], &jointQuaternions_[SIDE_WALK]);
    // make loop
	
    jointQuaternions_[SIDE_WALK] = MakeLoop(jointQuaternions_[SIDE_WALK],
        jointQuaternions_[SIDE_WALK].size() / 3,
        jointQuaternions_[SIDE_WALK].size() * 2 / 3,
        jointQuaternions_[SIDE_WALK].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[SIDE_WALK]);

/* ---------------- read the marching forward ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/march_crop.bvh");
	//"./animation/data/tapDance_crop1.bvh"
    rootHeight[MARCH_FORWARD] = GetRootHeight();
    AssignIndex();
	cout<<"march"<<endl;
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[MARCH_FORWARD], "./animation/data/march_crop.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[MARCH_FORWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[MARCH_FORWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[MARCH_FORWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[MARCH_FORWARD].resize(jointTrajectories_[MARCH_FORWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[MARCH_FORWARD].size(); ++frameIdx) {
        jointQuaternions_[MARCH_FORWARD][frameIdx].resize(jointTrajectories_[MARCH_FORWARD][frameIdx].size());
    }
	
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[MARCH_FORWARD], jointQuaternions_[MARCH_FORWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[MARCH_FORWARD], &jointQuaternions_[MARCH_FORWARD]);
    // make loop
	
    jointQuaternions_[MARCH_FORWARD] = MakeLoop(jointQuaternions_[MARCH_FORWARD],
        jointQuaternions_[MARCH_FORWARD].size() / 3,
        jointQuaternions_[MARCH_FORWARD].size() * 2 / 3,
        jointQuaternions_[MARCH_FORWARD].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[MARCH_FORWARD]);

/* ---------------- read the kicking forward ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/kickboxing02.bvh");
	//"./animation/data/tapDance_crop1.bvh"
    rootHeight[KICK_FORWARD] = GetRootHeight();
    AssignIndex();
	cout<<"kick"<<endl;
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[KICK_FORWARD], "./animation/data/kickboxing02.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[KICK_FORWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[KICK_FORWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[KICK_FORWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[KICK_FORWARD].resize(jointTrajectories_[KICK_FORWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[KICK_FORWARD].size(); ++frameIdx) {
        jointQuaternions_[KICK_FORWARD][frameIdx].resize(jointTrajectories_[KICK_FORWARD][frameIdx].size());
    }
	
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[KICK_FORWARD], jointQuaternions_[KICK_FORWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[KICK_FORWARD], &jointQuaternions_[KICK_FORWARD]);
    // make loop
	
    jointQuaternions_[KICK_FORWARD] = MakeLoop(jointQuaternions_[KICK_FORWARD],
        jointQuaternions_[KICK_FORWARD].size() / 3,
        jointQuaternions_[KICK_FORWARD].size() * 2 / 3,
        jointQuaternions_[KICK_FORWARD].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[KICK_FORWARD]);

	
/* ---------------- read the ducking ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/ducking_long.bvh");
	//"./animation/data/tapDance_crop1.bvh"
    rootHeight[DUCK] = GetRootHeight();
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[DUCK], "./animation/data/ducking_long.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[DUCK]);
    ConvertFromDegreeToRadian(&jointTrajectories_[DUCK]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[DUCK]);
    // resize the quaternions array accordingly
    jointQuaternions_[DUCK].resize(jointTrajectories_[DUCK].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[DUCK].size(); ++frameIdx) {
        jointQuaternions_[DUCK][frameIdx].resize(jointTrajectories_[DUCK][frameIdx].size());
    }
	cout<<"loading ducking.bvh"<<endl;
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[DUCK], jointQuaternions_[DUCK]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[DUCK], &jointQuaternions_[DUCK]);
    // make loop
	
    jointQuaternions_[DUCK] = MakeLoop(jointQuaternions_[DUCK],
        jointQuaternions_[DUCK].size() / 3,
        jointQuaternions_[DUCK].size() * 2 / 3,
        jointQuaternions_[DUCK].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[DUCK]);

/* ---------------- read the skating forward ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/skating_crop.bvh");
	//"./animation/data/tapDance_crop1.bvh"
    rootHeight[SKATE_FORWARD] = GetRootHeight();
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[SKATE_FORWARD], "./animation/data/skating_crop.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[SKATE_FORWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[SKATE_FORWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[SKATE_FORWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[SKATE_FORWARD].resize(jointTrajectories_[SKATE_FORWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[SKATE_FORWARD].size(); ++frameIdx) {
        jointQuaternions_[SKATE_FORWARD][frameIdx].resize(jointTrajectories_[SKATE_FORWARD][frameIdx].size());
    }
	
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[SKATE_FORWARD], jointQuaternions_[SKATE_FORWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[SKATE_FORWARD], &jointQuaternions_[SKATE_FORWARD]);
    // make loop
	
    jointQuaternions_[SKATE_FORWARD] = MakeLoop(jointQuaternions_[SKATE_FORWARD],
        jointQuaternions_[SKATE_FORWARD].size() / 3,
        jointQuaternions_[SKATE_FORWARD].size() * 2 / 3,
        jointQuaternions_[SKATE_FORWARD].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[SKATE_FORWARD]);


	/* ---------------- read the ballet spin ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/TurnDance.bvh");
	//BalletSpin
	//"./animation/data/tapDance_crop1.bvh"
    rootHeight[BALLET_SPIN] = GetRootHeight();
    cout<<"in ballet spin"<<endl;
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[BALLET_SPIN], "./animation/data/TurnDance.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[BALLET_SPIN]);
    ConvertFromDegreeToRadian(&jointTrajectories_[BALLET_SPIN]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[BALLET_SPIN]);
    // resize the quaternions array accordingly
    jointQuaternions_[BALLET_SPIN].resize(jointTrajectories_[BALLET_SPIN].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[BALLET_SPIN].size(); ++frameIdx) {
        jointQuaternions_[BALLET_SPIN][frameIdx].resize(jointTrajectories_[BALLET_SPIN][frameIdx].size());
    }
	
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[BALLET_SPIN], jointQuaternions_[BALLET_SPIN]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[BALLET_SPIN], &jointQuaternions_[BALLET_SPIN]);
    // make loop
	
    jointQuaternions_[BALLET_SPIN] = MakeLoop(jointQuaternions_[BALLET_SPIN],
        jointQuaternions_[BALLET_SPIN].size() / 3,
        jointQuaternions_[BALLET_SPIN].size() * 2 / 3,
        jointQuaternions_[BALLET_SPIN].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[BALLET_SPIN]);


	/* ---------------- read the tap dance ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/tapDance_crop1.bvh");
	//"./animation/data/tapDance_crop1.bvh"
    rootHeight[TAP_DANCE] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[TAP_DANCE], "./animation/data/tapDance_crop1.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[TAP_DANCE]);
    ConvertFromDegreeToRadian(&jointTrajectories_[TAP_DANCE]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[TAP_DANCE]);
    // resize the quaternions array accordingly
    jointQuaternions_[TAP_DANCE].resize(jointTrajectories_[TAP_DANCE].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[TAP_DANCE].size(); ++frameIdx) {
        jointQuaternions_[TAP_DANCE][frameIdx].resize(jointTrajectories_[TAP_DANCE][frameIdx].size());
    }
	
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[TAP_DANCE], jointQuaternions_[TAP_DANCE]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[TAP_DANCE], &jointQuaternions_[TAP_DANCE]);
    // make loop
	
    jointQuaternions_[TAP_DANCE] = MakeLoop(jointQuaternions_[TAP_DANCE],
        jointQuaternions_[TAP_DANCE].size() / 3,
        jointQuaternions_[TAP_DANCE].size() * 2 / 3,
        jointQuaternions_[TAP_DANCE].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[TAP_DANCE]);
	

	/* ---------------- read the one legged hoppinh ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/chickenhopping03.bvh");
    rootHeight[ONE_HOPPING] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[ONE_HOPPING], "./animation/data/chickenhopping03.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[ONE_HOPPING]);
    ConvertFromDegreeToRadian(&jointTrajectories_[ONE_HOPPING]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[ONE_HOPPING]);
    // resize the quaternions array accordingly
    jointQuaternions_[ONE_HOPPING].resize(jointTrajectories_[ONE_HOPPING].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[ONE_HOPPING].size(); ++frameIdx) {
        jointQuaternions_[ONE_HOPPING][frameIdx].resize(jointTrajectories_[ONE_HOPPING][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[ONE_HOPPING], jointQuaternions_[ONE_HOPPING]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[ONE_HOPPING], &jointQuaternions_[ONE_HOPPING]);
    // make loop
    jointQuaternions_[ONE_HOPPING] = MakeLoop(jointQuaternions_[ONE_HOPPING],
        jointQuaternions_[ONE_HOPPING].size() / 3,
        jointQuaternions_[ONE_HOPPING].size() * 2 / 3,
        jointQuaternions_[ONE_HOPPING].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[ONE_HOPPING]);

	/* ---------------- read the moon walking ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/Moonwalk002.bvh");
    rootHeight[MOONWALK_FORWARD] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[MOONWALK_FORWARD], "./animation/data/Moonwalk002.bvh");

    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[MOONWALK_FORWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[MOONWALK_FORWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[MOONWALK_FORWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[MOONWALK_FORWARD].resize(jointTrajectories_[MOONWALK_FORWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[MOONWALK_FORWARD].size(); ++frameIdx) {
        jointQuaternions_[MOONWALK_FORWARD][frameIdx].resize(jointTrajectories_[MOONWALK_FORWARD][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[MOONWALK_FORWARD], jointQuaternions_[MOONWALK_FORWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[MOONWALK_FORWARD], &jointQuaternions_[MOONWALK_FORWARD]);
    // make loop
    jointQuaternions_[MOONWALK_FORWARD] = MakeLoop(jointQuaternions_[MOONWALK_FORWARD],
        jointQuaternions_[MOONWALK_FORWARD].size() / 3,
        jointQuaternions_[MOONWALK_FORWARD].size() * 2 / 3,
        jointQuaternions_[MOONWALK_FORWARD].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[MOONWALK_FORWARD]);

	/* ---------------- read the cat walking ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/Catwalk002.bvh");//Catwalk002
    rootHeight[CATWALK_FORWARD] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[CATWALK_FORWARD], "./animation/data/Catwalk002.bvh");//Catwalk002
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[CATWALK_FORWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[CATWALK_FORWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[CATWALK_FORWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[CATWALK_FORWARD].resize(jointTrajectories_[CATWALK_FORWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[CATWALK_FORWARD].size(); ++frameIdx) {
        jointQuaternions_[CATWALK_FORWARD][frameIdx].resize(jointTrajectories_[CATWALK_FORWARD][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[CATWALK_FORWARD], jointQuaternions_[CATWALK_FORWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[CATWALK_FORWARD], &jointQuaternions_[CATWALK_FORWARD]);
    // make loop
    jointQuaternions_[CATWALK_FORWARD] = MakeLoop(jointQuaternions_[CATWALK_FORWARD],
        jointQuaternions_[CATWALK_FORWARD].size() / 3,
        jointQuaternions_[CATWALK_FORWARD].size() * 2 / 3,
        jointQuaternions_[CATWALK_FORWARD].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[CATWALK_FORWARD]);


    /* ---------------- read the inplace limping ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/inplace_limping_1_5_cycle.bvh");
    rootHeight[LIMP_INPLACE] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[LIMP_INPLACE], "./animation/data/inplace_limping_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[LIMP_INPLACE]);
    ConvertFromDegreeToRadian(&jointTrajectories_[LIMP_INPLACE]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[LIMP_INPLACE]);
    // resize the quaternions array accordingly
    jointQuaternions_[LIMP_INPLACE].resize(jointTrajectories_[LIMP_INPLACE].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[LIMP_INPLACE].size(); ++frameIdx) {
        jointQuaternions_[LIMP_INPLACE][frameIdx].resize(jointTrajectories_[LIMP_INPLACE][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[LIMP_INPLACE], jointQuaternions_[LIMP_INPLACE]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[LIMP_INPLACE], &jointQuaternions_[LIMP_INPLACE]);
    // make loop
    jointQuaternions_[LIMP_INPLACE] = MakeLoop(jointQuaternions_[LIMP_INPLACE],
        jointQuaternions_[LIMP_INPLACE].size() / 3,
        jointQuaternions_[LIMP_INPLACE].size() * 2 / 3,
        jointQuaternions_[LIMP_INPLACE].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[LIMP_INPLACE]);

    /* ---------------- read the forward limping ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/forward_limping_1_5_cycle.bvh");
    rootHeight[LIMP_FORWARD] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[LIMP_FORWARD], "./animation/data/forward_limping_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[LIMP_FORWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[LIMP_FORWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[LIMP_FORWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[LIMP_FORWARD].resize(jointTrajectories_[LIMP_FORWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[LIMP_FORWARD].size(); ++frameIdx) {
        jointQuaternions_[LIMP_FORWARD][frameIdx].resize(jointTrajectories_[LIMP_FORWARD][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[LIMP_FORWARD], jointQuaternions_[LIMP_FORWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[LIMP_FORWARD], &jointQuaternions_[LIMP_FORWARD]);
    // make loop
    jointQuaternions_[LIMP_FORWARD] = MakeLoop(jointQuaternions_[LIMP_FORWARD],
        jointQuaternions_[LIMP_FORWARD].size() / 3,
        jointQuaternions_[LIMP_FORWARD].size() * 2 / 3,
        jointQuaternions_[LIMP_FORWARD].size() / 3);
    // downsample
    Downsample(jointQuaternions_[LIMP_FORWARD], &jointQuaternions_[LIMP_INPLACE]);
    ForceNoWristRotation(&jointQuaternions_[LIMP_FORWARD]);

    /* ---------------- read the backward limping ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/backward_limping_1_5_cycle.bvh");
    rootHeight[LIMP_BACKWARD] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[LIMP_BACKWARD], "./animation/data/backward_limping_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[LIMP_BACKWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[LIMP_BACKWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[LIMP_BACKWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[LIMP_BACKWARD].resize(jointTrajectories_[LIMP_BACKWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[LIMP_BACKWARD].size(); ++frameIdx) {
        jointQuaternions_[LIMP_BACKWARD][frameIdx].resize(jointTrajectories_[LIMP_BACKWARD][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[LIMP_BACKWARD], jointQuaternions_[LIMP_BACKWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[LIMP_BACKWARD], &jointQuaternions_[LIMP_BACKWARD]);
    // make loop
    jointQuaternions_[LIMP_BACKWARD] = MakeLoop(jointQuaternions_[LIMP_BACKWARD],
        jointQuaternions_[LIMP_BACKWARD].size() / 3,
        jointQuaternions_[LIMP_BACKWARD].size() * 2 / 3,
        jointQuaternions_[LIMP_BACKWARD].size() / 3);
    // downsample
    Downsample(jointQuaternions_[LIMP_FORWARD], &jointQuaternions_[LIMP_BACKWARD]);
    ForceNoWristRotation(&jointQuaternions_[LIMP_BACKWARD]);

    /* ---------------- read the inplace pino walking ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/inplace_pino_walking_1_5_cycle.bvh");
    rootHeight[PINO_WALK_INPLACE] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[PINO_WALK_INPLACE], "./animation/data/inplace_pino_walking_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[PINO_WALK_INPLACE]);
    ConvertFromDegreeToRadian(&jointTrajectories_[PINO_WALK_INPLACE]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[PINO_WALK_INPLACE]);
    // resize the quaternions array accordingly
    jointQuaternions_[PINO_WALK_INPLACE].resize(jointTrajectories_[PINO_WALK_INPLACE].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[PINO_WALK_INPLACE].size(); ++frameIdx) {
        jointQuaternions_[PINO_WALK_INPLACE][frameIdx].resize(jointTrajectories_[PINO_WALK_INPLACE][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[PINO_WALK_INPLACE], jointQuaternions_[PINO_WALK_INPLACE]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[PINO_WALK_INPLACE], &jointQuaternions_[PINO_WALK_INPLACE]);
    // make loop
    jointQuaternions_[PINO_WALK_INPLACE] = MakeLoop(jointQuaternions_[PINO_WALK_INPLACE],
        jointQuaternions_[PINO_WALK_INPLACE].size() / 3,
        jointQuaternions_[PINO_WALK_INPLACE].size() * 2 / 3,
        jointQuaternions_[PINO_WALK_INPLACE].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[PINO_WALK_INPLACE]);

    /* ---------------- read the forward pino walking ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/forward_pino_walking_1_5_cycle.bvh");
    rootHeight[PINO_WALK_FORWARD] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[PINO_WALK_FORWARD], "./animation/data/forward_pino_walking_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[PINO_WALK_FORWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[PINO_WALK_FORWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[PINO_WALK_FORWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[PINO_WALK_FORWARD].resize(jointTrajectories_[PINO_WALK_FORWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[PINO_WALK_FORWARD].size(); ++frameIdx) {
        jointQuaternions_[PINO_WALK_FORWARD][frameIdx].resize(jointTrajectories_[PINO_WALK_FORWARD][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[PINO_WALK_FORWARD], jointQuaternions_[PINO_WALK_FORWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[PINO_WALK_FORWARD], &jointQuaternions_[PINO_WALK_FORWARD]);
    // make loop
    jointQuaternions_[PINO_WALK_FORWARD] = MakeLoop(jointQuaternions_[PINO_WALK_FORWARD],
        jointQuaternions_[PINO_WALK_FORWARD].size() / 3,
        jointQuaternions_[PINO_WALK_FORWARD].size() * 2 / 3,
        jointQuaternions_[PINO_WALK_FORWARD].size() / 3);
    // downsample
    ForceNoWristRotation(&jointQuaternions_[PINO_WALK_FORWARD]);

    /* ---------------- read the backward tiptoeing ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/backward_pino_walking_1_5_cycle.bvh");
    rootHeight[PINO_WALK_BACKWARD] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[PINO_WALK_BACKWARD], "./animation/data/backward_pino_walking_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[PINO_WALK_BACKWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[PINO_WALK_BACKWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[PINO_WALK_BACKWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[PINO_WALK_BACKWARD].resize(jointTrajectories_[PINO_WALK_BACKWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[PINO_WALK_BACKWARD].size(); ++frameIdx) {
        jointQuaternions_[PINO_WALK_BACKWARD][frameIdx].resize(jointTrajectories_[PINO_WALK_BACKWARD][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[PINO_WALK_BACKWARD], jointQuaternions_[PINO_WALK_BACKWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[PINO_WALK_BACKWARD], &jointQuaternions_[PINO_WALK_BACKWARD]);
    // make loop
    jointQuaternions_[PINO_WALK_BACKWARD] = MakeLoop(jointQuaternions_[PINO_WALK_BACKWARD],
        jointQuaternions_[PINO_WALK_BACKWARD].size() / 3,
        jointQuaternions_[PINO_WALK_BACKWARD].size() * 2 / 3,
        jointQuaternions_[PINO_WALK_BACKWARD].size() / 3);
    // downsample
    ForceNoWristRotation(&jointQuaternions_[PINO_WALK_BACKWARD]);
    Downsample(jointQuaternions_[PINO_WALK_BACKWARD], &jointQuaternions_[PINO_WALK_FORWARD]);
    Downsample(jointQuaternions_[PINO_WALK_BACKWARD], &jointQuaternions_[PINO_WALK_INPLACE]);

    /* ---------------- read the inplace tiptoeing ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/inplace_tiptoeing_1_5_cycle.bvh");
    rootHeight[TIPTOE_INPLACE] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[TIPTOE_INPLACE], "./animation/data/inplace_tiptoeing_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[TIPTOE_INPLACE]);
    ConvertFromDegreeToRadian(&jointTrajectories_[TIPTOE_INPLACE]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[TIPTOE_INPLACE]);
    // resize the quaternions array accordingly
    jointQuaternions_[TIPTOE_INPLACE].resize(jointTrajectories_[TIPTOE_INPLACE].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[TIPTOE_INPLACE].size(); ++frameIdx) {
        jointQuaternions_[TIPTOE_INPLACE][frameIdx].resize(jointTrajectories_[TIPTOE_INPLACE][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[TIPTOE_INPLACE], jointQuaternions_[TIPTOE_INPLACE]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[TIPTOE_INPLACE], &jointQuaternions_[TIPTOE_INPLACE]);
    // make loop
    jointQuaternions_[TIPTOE_INPLACE] = MakeLoop(jointQuaternions_[TIPTOE_INPLACE],
        jointQuaternions_[TIPTOE_INPLACE].size() / 3,
        jointQuaternions_[TIPTOE_INPLACE].size() * 2 / 3,
        jointQuaternions_[TIPTOE_INPLACE].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[TIPTOE_INPLACE]);

    /* ---------------- read the forward tiptoeing ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/forward_tiptoeing_1_5_cycle.bvh");
    rootHeight[TIPTOE_FORWARD] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[TIPTOE_FORWARD], "./animation/data/forward_tiptoeing_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[TIPTOE_FORWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[TIPTOE_FORWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[TIPTOE_FORWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[TIPTOE_FORWARD].resize(jointTrajectories_[TIPTOE_FORWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[TIPTOE_FORWARD].size(); ++frameIdx) {
        jointQuaternions_[TIPTOE_FORWARD][frameIdx].resize(jointTrajectories_[TIPTOE_FORWARD][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[TIPTOE_FORWARD], jointQuaternions_[TIPTOE_FORWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[TIPTOE_FORWARD], &jointQuaternions_[TIPTOE_FORWARD]);
    // make loop
    jointQuaternions_[TIPTOE_FORWARD] = MakeLoop(jointQuaternions_[TIPTOE_FORWARD],
        jointQuaternions_[TIPTOE_FORWARD].size() / 3,
        jointQuaternions_[TIPTOE_FORWARD].size() * 2 / 3,
        jointQuaternions_[TIPTOE_FORWARD].size() / 3);
    // downsample
    Downsample(jointQuaternions_[TIPTOE_INPLACE], &jointQuaternions_[TIPTOE_FORWARD]);
    ForceNoWristRotation(&jointQuaternions_[TIPTOE_FORWARD]);

    /* ---------------- read the backward tiptoeing ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/backward_tiptoeing_1_5_cycle.bvh");
    rootHeight[TIPTOE_BACKWARD] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[TIPTOE_BACKWARD], "./animation/data/backward_tiptoeing_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[TIPTOE_BACKWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[TIPTOE_BACKWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[TIPTOE_BACKWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[TIPTOE_BACKWARD].resize(jointTrajectories_[TIPTOE_BACKWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[TIPTOE_BACKWARD].size(); ++frameIdx) {
        jointQuaternions_[TIPTOE_BACKWARD][frameIdx].resize(jointTrajectories_[TIPTOE_BACKWARD][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[TIPTOE_BACKWARD], jointQuaternions_[TIPTOE_BACKWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[TIPTOE_BACKWARD], &jointQuaternions_[TIPTOE_BACKWARD]);
    // make loop
    jointQuaternions_[TIPTOE_BACKWARD] = MakeLoop(jointQuaternions_[TIPTOE_BACKWARD],
        jointQuaternions_[TIPTOE_BACKWARD].size() / 3,
        jointQuaternions_[TIPTOE_BACKWARD].size() / 3,
        jointQuaternions_[TIPTOE_BACKWARD].size() - 1);
    // downsample
    Downsample(jointQuaternions_[TIPTOE_INPLACE], &jointQuaternions_[TIPTOE_BACKWARD]);
    ForceNoWristRotation(&jointQuaternions_[TIPTOE_BACKWARD]);

    /* ---------------- read the inplace jumping ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/inplace_jumping_1_5_cycle.bvh");
    rootHeight[JUMP_SHORT_INPLACE] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[JUMP_SHORT_INPLACE], "./animation/data/inplace_jumping_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[JUMP_SHORT_INPLACE]);
    ConvertFromDegreeToRadian(&jointTrajectories_[JUMP_SHORT_INPLACE]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[JUMP_SHORT_INPLACE]);
    // resize the quaternions array accordingly
    jointQuaternions_[JUMP_SHORT_INPLACE].resize(jointTrajectories_[JUMP_SHORT_INPLACE].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[JUMP_SHORT_INPLACE].size(); ++frameIdx) {
        jointQuaternions_[JUMP_SHORT_INPLACE][frameIdx].resize(jointTrajectories_[JUMP_SHORT_INPLACE][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[JUMP_SHORT_INPLACE], jointQuaternions_[JUMP_SHORT_INPLACE]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[JUMP_SHORT_INPLACE], &jointQuaternions_[JUMP_SHORT_INPLACE]);
    // make loop
    jointQuaternions_[JUMP_SHORT_INPLACE] = MakeLoop(jointQuaternions_[JUMP_SHORT_INPLACE],
        jointQuaternions_[JUMP_SHORT_INPLACE].size() / 3,
        jointQuaternions_[JUMP_SHORT_INPLACE].size() / 3,
        jointQuaternions_[JUMP_SHORT_INPLACE].size() - 1);
    // downsample
    //Downsample(jointQuaternions_[RUN_INPLACE], &jointQuaternions_[RUN_FORWARD]);

    /* ---------------- read the forward jumping ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/forward_jumping_1_5_cycle.bvh");
    rootHeight[JUMP_SHORT_FORWARD] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_FORWARD]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[JUMP_SHORT_FORWARD], "./animation/data/forward_jumping_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[JUMP_SHORT_FORWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[JUMP_SHORT_FORWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[JUMP_SHORT_FORWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[JUMP_SHORT_FORWARD].resize(jointTrajectories_[JUMP_SHORT_FORWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[JUMP_SHORT_FORWARD].size(); ++frameIdx) {
        jointQuaternions_[JUMP_SHORT_FORWARD][frameIdx].resize(jointTrajectories_[JUMP_SHORT_FORWARD][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[JUMP_SHORT_FORWARD], jointQuaternions_[JUMP_SHORT_FORWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[JUMP_SHORT_FORWARD], &jointQuaternions_[JUMP_SHORT_FORWARD]);
    // make loop
    jointQuaternions_[JUMP_SHORT_FORWARD] = MakeLoop(jointQuaternions_[JUMP_SHORT_FORWARD],
        jointQuaternions_[JUMP_SHORT_FORWARD].size() / 3,
        jointQuaternions_[JUMP_SHORT_FORWARD].size() * 2 / 3,
        jointQuaternions_[JUMP_SHORT_FORWARD].size() / 3);
    // downsample
    Downsample(jointQuaternions_[JUMP_SHORT_INPLACE], &jointQuaternions_[JUMP_SHORT_FORWARD]);

    /* ---------------- read the long forward jumping ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/forward_jumping_long_1_5_cycle.bvh");
    rootHeight[JUMP_LONG_FORWARD] = GetRootHeight();
    //printf("%.2f\n", rootHeight[JUMP_FORWARD]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[JUMP_LONG_FORWARD], "./animation/data/forward_jumping_long_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[JUMP_LONG_FORWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[JUMP_LONG_FORWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[JUMP_LONG_FORWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[JUMP_LONG_FORWARD].resize(jointTrajectories_[JUMP_LONG_FORWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[JUMP_LONG_FORWARD].size(); ++frameIdx) {
        jointQuaternions_[JUMP_LONG_FORWARD][frameIdx].resize(jointTrajectories_[JUMP_LONG_FORWARD][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[JUMP_LONG_FORWARD], jointQuaternions_[JUMP_LONG_FORWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[JUMP_LONG_FORWARD], &jointQuaternions_[JUMP_LONG_FORWARD]);
    // make loop
    jointQuaternions_[JUMP_LONG_FORWARD] = MakeLoop(jointQuaternions_[JUMP_LONG_FORWARD],
        jointQuaternions_[JUMP_LONG_FORWARD].size() / 3,
        jointQuaternions_[JUMP_LONG_FORWARD].size() * 2 / 3,
        jointQuaternions_[JUMP_LONG_FORWARD].size() / 3);
    // downsample
    Downsample(jointQuaternions_[JUMP_SHORT_INPLACE], &jointQuaternions_[JUMP_LONG_FORWARD]);
    ForceNoWristRotation(&jointQuaternions_[JUMP_LONG_FORWARD]);

    /* ---------------- read the inplace walking ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/inplace_walking_1_5_cycle.bvh");
    rootHeight[WALK_INPLACE] = GetRootHeight();
    //printf("%.2f\n", rootHeight[WALK_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[WALK_INPLACE], "./animation/data/inplace_walking_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[WALK_INPLACE]);
    ConvertFromDegreeToRadian(&jointTrajectories_[WALK_INPLACE]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[WALK_INPLACE]);
    // resize the quaternions array accordingly
    jointQuaternions_[WALK_INPLACE].resize(jointTrajectories_[WALK_INPLACE].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[WALK_INPLACE].size(); ++frameIdx) {
        jointQuaternions_[WALK_INPLACE][frameIdx].resize(jointTrajectories_[WALK_INPLACE][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[WALK_INPLACE], jointQuaternions_[WALK_INPLACE]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[WALK_INPLACE], &jointQuaternions_[WALK_INPLACE]);
    // make loop
    jointQuaternions_[WALK_INPLACE] = MakeLoop(jointQuaternions_[WALK_INPLACE],
        jointQuaternions_[WALK_INPLACE].size() / 3,
        jointQuaternions_[WALK_INPLACE].size() / 3,
        jointQuaternions_[WALK_INPLACE].size() - 1);

    /* ---------------- read the forward walking ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/forward_walking_1_5_cycle.bvh");
    rootHeight[WALK_FORWARD] = GetRootHeight();
    //printf("%.2f\n", rootHeight[WALK_FORWARD]);
    AssignIndex();
    // read joint trajectories and process discontinuities
	//"./animation/data/forward_walking_1_5_cycle.bvh"
    ReadBVHFile(&jointTrajectories_[WALK_FORWARD],"./animation/data/forward_walking_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[WALK_FORWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[WALK_FORWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[WALK_FORWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[WALK_FORWARD].resize(jointTrajectories_[WALK_FORWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[WALK_FORWARD].size(); ++frameIdx) {
        jointQuaternions_[WALK_FORWARD][frameIdx].resize(jointTrajectories_[WALK_FORWARD][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[WALK_FORWARD], jointQuaternions_[WALK_FORWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[WALK_FORWARD], &jointQuaternions_[WALK_FORWARD]);
    // make loop
    jointQuaternions_[WALK_FORWARD] = MakeLoop(jointQuaternions_[WALK_FORWARD],
        jointQuaternions_[WALK_FORWARD].size() / 3,
        jointQuaternions_[WALK_FORWARD].size() / 3,
        jointQuaternions_[WALK_FORWARD].size() - 1);
    // downsample
    Downsample(jointQuaternions_[WALK_INPLACE], &jointQuaternions_[WALK_FORWARD]);

    /* ---------------- read the backward walking ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/backward_walking_1_5_cycle.bvh");
    rootHeight[WALK_BACKWARD] = GetRootHeight();
    //printf("%.2f\n", rootHeight[WALK_BACKWARD]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[WALK_BACKWARD], "./animation/data/backward_walking_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[WALK_BACKWARD]);
    ConvertFromDegreeToRadian(&jointTrajectories_[WALK_BACKWARD]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[WALK_BACKWARD]);
    // resize the quaternions array accordingly
    jointQuaternions_[WALK_BACKWARD].resize(jointTrajectories_[WALK_BACKWARD].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[WALK_BACKWARD].size(); ++frameIdx) {
        jointQuaternions_[WALK_BACKWARD][frameIdx].resize(jointTrajectories_[WALK_BACKWARD][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[WALK_BACKWARD], jointQuaternions_[WALK_BACKWARD]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[WALK_BACKWARD], &jointQuaternions_[WALK_BACKWARD]);
    // make loop
    jointQuaternions_[WALK_BACKWARD] = MakeLoop(jointQuaternions_[WALK_BACKWARD],
        jointQuaternions_[WALK_BACKWARD].size() / 3,
        jointQuaternions_[WALK_BACKWARD].size() * 2 / 3,
        jointQuaternions_[WALK_BACKWARD].size() / 3);
    // downsample
    Downsample(jointQuaternions_[WALK_INPLACE], &jointQuaternions_[WALK_BACKWARD]);

    /* ---------------- read the inplace running ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/inplace_running_1_5_cycle.bvh");
    rootHeight[RUN_INPLACE] = GetRootHeight();
    //printf("%.2f\n", rootHeight[RUN_INPLACE]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[RUN_INPLACE], "./animation/data/inplace_running_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[RUN_INPLACE]);
    ConvertFromDegreeToRadian(&jointTrajectories_[RUN_INPLACE]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[RUN_INPLACE]);
    // resize the quaternions array accordingly
    jointQuaternions_[RUN_INPLACE].resize(jointTrajectories_[RUN_INPLACE].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[RUN_INPLACE].size(); ++frameIdx) {
        jointQuaternions_[RUN_INPLACE][frameIdx].resize(jointTrajectories_[RUN_INPLACE][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[RUN_INPLACE], jointQuaternions_[RUN_INPLACE]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[RUN_INPLACE], &jointQuaternions_[RUN_INPLACE]);
    // make loop
    jointQuaternions_[RUN_INPLACE] = MakeLoop(jointQuaternions_[RUN_INPLACE],
        jointQuaternions_[RUN_INPLACE].size() / 3,
        jointQuaternions_[RUN_INPLACE].size() / 3,
        jointQuaternions_[RUN_INPLACE].size() / 3 - 1);

    /* ---------------- read the jogging ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/jogging_1_5_cycle.bvh");
    rootHeight[JOG] = GetRootHeight();
    //printf("%.2f\n", rootHeight[RUN_FORWARD]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[JOG], "./animation/data/jogging_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[JOG]);
    ConvertFromDegreeToRadian(&jointTrajectories_[JOG]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[JOG]);
    // resize the quaternions array accordingly
    jointQuaternions_[JOG].resize(jointTrajectories_[JOG].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[JOG].size(); ++frameIdx) {
        jointQuaternions_[JOG][frameIdx].resize(jointTrajectories_[JOG][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[JOG], jointQuaternions_[JOG]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[JOG], &jointQuaternions_[JOG]);
    // make loop
    jointQuaternions_[JOG] = MakeLoop(jointQuaternions_[JOG],
        jointQuaternions_[JOG].size() / 3,
        jointQuaternions_[JOG].size() / 3,
        jointQuaternions_[JOG].size() - 1);
    ForceNoWristRotation(&jointQuaternions_[JOG]);

    /* ---------------- read the normal running ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/forward_running_normal_1_5_cycle.bvh");
    rootHeight[RUN_NORMAL] = GetRootHeight();
    //printf("%.2f\n", rootHeight[RUN_FORWARD]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[RUN_NORMAL], "./animation/data/forward_running_normal_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[RUN_NORMAL]);
    ConvertFromDegreeToRadian(&jointTrajectories_[RUN_NORMAL]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[RUN_NORMAL]);
    // resize the quaternions array accordingly
    jointQuaternions_[RUN_NORMAL].resize(jointTrajectories_[RUN_NORMAL].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[RUN_NORMAL].size(); ++frameIdx) {
        jointQuaternions_[RUN_NORMAL][frameIdx].resize(jointTrajectories_[RUN_NORMAL][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[RUN_NORMAL], jointQuaternions_[RUN_NORMAL]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[RUN_NORMAL], &jointQuaternions_[RUN_NORMAL]);
    // make loop
    jointQuaternions_[RUN_NORMAL] = MakeLoop(jointQuaternions_[RUN_NORMAL],
        jointQuaternions_[RUN_NORMAL].size() / 3,
        jointQuaternions_[RUN_NORMAL].size() * 2 / 3,
        jointQuaternions_[RUN_NORMAL].size() / 3);
    ForceNoWristRotation(&jointQuaternions_[RUN_NORMAL]);

    /* ---------------- read the long running ---------------- */

    // read skeleton and assign index to each joint
    BuildSkeletonFromBVH("./animation/data/forward_running_long_1_5_cycle.bvh");
    rootHeight[RUN_FAST] = GetRootHeight();
    //printf("%.2f\n", rootHeight[RUN_FORWARD]);
    AssignIndex();
    // read joint trajectories and process discontinuities
    ReadBVHFile(&jointTrajectories_[RUN_FAST], "./animation/data/forward_running_long_1_5_cycle.bvh");
    tree_.ComputMatrix();
    ProcessEulerAngles(&jointTrajectories_[RUN_FAST]);
    ConvertFromDegreeToRadian(&jointTrajectories_[RUN_FAST]);
    // tranlate to origin
    TranslateToOrigin(&jointTrajectories_[RUN_FAST]);
    // resize the quaternions array accordingly
    jointQuaternions_[RUN_FAST].resize(jointTrajectories_[RUN_FAST].size());
    for (int frameIdx = 0; frameIdx < jointTrajectories_[RUN_FAST].size(); ++frameIdx) {
        jointQuaternions_[RUN_FAST][frameIdx].resize(jointTrajectories_[RUN_FAST][frameIdx].size());
    }
    // convert the euler angles to quaternion
    ConvertJointAnglesToQuaternion(jointTrajectories_[RUN_FAST], jointQuaternions_[RUN_FAST]);
    // rotate forward walking frame to inplace walking frame
    RotateFrameBtoOriginal(&jointTrajectories_[RUN_FAST], &jointQuaternions_[RUN_FAST]);
    // make loop
    jointQuaternions_[RUN_FAST] = MakeLoop(jointQuaternions_[RUN_FAST],
        jointQuaternions_[RUN_FAST].size() / 3,
        jointQuaternions_[RUN_FAST].size() * 2 / 3,
        jointQuaternions_[RUN_FAST].size() / 3);
    // downsample
    Downsample(jointQuaternions_[JOG], &jointQuaternions_[RUN_FAST]);
    Downsample(jointQuaternions_[JOG], &jointQuaternions_[RUN_NORMAL]);
    Downsample(jointQuaternions_[JOG], &jointQuaternions_[RUN_INPLACE]);

    /* ---------------- do simple retargeting ---------------- */
    for (int i = 0; i < MODEL_COUNT; ++i)
        SimpleRetarget(rootHeight[RUN_FAST], rootHeight[i], &jointQuaternions_[i]);

    // turns off jacobian computation
    targets_.resize(0);

    Update(0, 1.0, 1.0);
}

/*
 * Assigns one index to each node
 * This number is used to index into the bvh format
 */
void Animation::AssignIndex()
{
    node_[0].SetIndex(0); // Root
    node_[1].SetIndex(2); // Left femur
    node_[2].SetIndex(3); // Left tibia
    node_[3].SetIndex(4); // Left foot
    node_[4].SetIndex(5); // Left toe
    node_[5].SetIndex(-1); // Left toe end
    node_[6].SetIndex(6); // Right femur
    node_[7].SetIndex(7); // Right tibia
    node_[8].SetIndex(8); // Right foot
    node_[9].SetIndex(9); // Right toe
    node_[10].SetIndex(-1); // Right toe end
    node_[11].SetIndex(10); // Lower back
    node_[12].SetIndex(11); // Thorax
    node_[13].SetIndex(12); // Neck
    node_[14].SetIndex(13); // Head
    node_[15].SetIndex(-1); // Head end
    node_[16].SetIndex(14); // Left collar
    node_[17].SetIndex(15); // Left humerus
    node_[18].SetIndex(16); // Left elbow
    node_[19].SetIndex(17); // Left wrist
    node_[20].SetIndex(18); // Left thumb
    node_[21].SetIndex(-1); // Left thumb end
    node_[22].SetIndex(19); // Left wrist end
    node_[23].SetIndex(20); // Right collar
    node_[24].SetIndex(21); // Right humerus
    node_[25].SetIndex(22); // Right elbow
    node_[26].SetIndex(23); // Right wrist
    node_[27].SetIndex(24); // Right thumb
    node_[28].SetIndex(-1); // Right thumb end
    node_[29].SetIndex(25); // Right wrist end
}

/*
 * Builds M
 */
void Animation::BuildVectorM()
{
    M_.resize(3 * k_);

    VectorR3 left(left_->pos.x, left_->pos.y, left_->pos.z);
    VectorR3 right(right_->pos.x, right_->pos.y, right_->pos.z);

    left.z -= kTranslateZ;
    left.x -= kTranslateX;
    double temp = left.x;
    left.x = -left.z;
    left.z = temp;
    left.x /= kScaleX;
    left *= kScale;

    right.z -= kTranslateZ;
    right.x -= kTranslateX;
    temp = right.x;
    right.x = -right.z;
    right.z = temp;
    right.x /= kScaleX;
    right *= kScale;

    //cout << "left " << left.x << " " << left.y << " " << left.z << endl;
    //cout << "right " << right.x << " " << right.y << " " << right.z << endl;

    M_(0) = trajectory_[step_][0].x;
    M_(1) = trajectory_[step_][0].y;
    M_(2) = trajectory_[step_][0].z;
    M_(3) = trajectory_[step_][1].x;
    M_(4) = trajectory_[step_][1].y;
    M_(5) = trajectory_[step_][1].z;

    /*M_(0) = left.x;
    M_(1) = left.y;
    M_(2) = left.z;
    M_(3) = right.x;
    M_(4) = right.y;
    M_(5) = right.z;*/

    //cout << "LEFT" << M_(0) << " " << M_(1) << " " << M_(2) << endl;
    //cout << "RIGHT" << M_(3) << " " << M_(4) << " " << M_(5) << endl;
}

/*
 * Builds N
 */
void Animation::BuildVectorN()
{
    N_ = B_ * M_;
}

/*
 * Builds X
 */
void Animation::BuildMatrixX()
{
    X_.resize(3 * k_, n_);

    for (int i = 0; i < n_; ++i) {
        for (int j = 0; j < k_; ++j) {
            X_(3 * j    , i) = trajectory_[i][j].x;
            X_(3 * j + 1, i) = trajectory_[i][j].y;
            X_(3 * j + 2, i) = trajectory_[i][j].z;
        }
    }
}

/*
 * Builds Z
 */
void Animation::BuildMatrixZ()
{
    Z_.resize(3 * m_, n_);
    for (int i = 0; i < m_; ++i) {
        for (int j = 0; j < n_; ++j) {
            Z_(3 * i    , j) = jointTrajectory_[j][i].x;
            Z_(3 * i + 1, j) = jointTrajectory_[j][i].y;
            Z_(3 * i + 2, j) = jointTrajectory_[j][i].z;
        }
    }
}

/*
 * Builds B
 */
void Animation::BuildMatrixB()
{
    B_ = Z_ * X_.transpose() * (X_ * X_.transpose()).inverse();
}

/*
 * Avoids discontinuities in angles from consecutive frames
 */
void Animation::ProcessEulerAngles(vector<vector<VectorR3>> *trajectory)
{
    for (int i = 1; i < trajectory->size(); ++i) {
        if (i == 0)
            continue;
        for (int j = 1; j < (*trajectory)[i].size(); ++j) {
            VectorR3 &currentRotation = (*trajectory)[i][j];
            VectorR3 &lastRotation = (*trajectory)[i - 1][j];
            if (fabs(currentRotation.x - lastRotation.x) > 90.0f)
                currentRotation.x = -1.0 * Sign(currentRotation.x) *
                (360.0 - fabs(currentRotation.x));
            if (fabs(currentRotation.y - lastRotation.y) > 90.0f)
                currentRotation.y = -1.0 * Sign(currentRotation.y) *
                (360.0 - fabs(currentRotation.y));
            if (fabs(currentRotation.z - lastRotation.z) > 90.0f)
                currentRotation.z = -1.0 * Sign(currentRotation.z) *
                (360.0 - fabs(currentRotation.z));
        }
    }
}

/*
 * Reads the markers' trajectories from a text file
 */
void Animation::ReadMarkers(std::vector<std::vector<VectorR3>> &trajectory, const char *fileName)
{
	ifstream file(fileName);
	string line;

	getline(file, line); // ignore the first empty line

	getline(file, line); // this line stores the markers' names
	istringstream lineStream(line);
	string markerName;
	string temp;
	lineStream >> temp; // the "Frame" string
	int markerNo = 0;
	while (lineStream >> markerName >> temp) {
		markerName = markerName.substr(markerName.find(':') + 1);
		//cout << markerName << " ";
		markers_.push_back(markerName);
	}

	while (getline(file, line)) {
		lineStream.clear();
		lineStream.str(line);

		int frameNo;
		lineStream >> frameNo; // ignore the frame's number

		vector<VectorR3> currentFrame;
		double x, y, z;
		while(lineStream >> x >> y >> z)
			currentFrame.push_back(VectorR3(x, y, z));

		trajectory.push_back(currentFrame);
	}

	file.close();
}

/*
 * Transforms the markers' coordinates to fit with the coordinate frame
   of the program
 */
void Animation::TransformMarkers(std::vector<std::vector<VectorR3>> &trajectory, VectorR3 &scale, VectorR3 &translate)
{
	// Scale the coordinates of the markers to fit with the coordinate frame
	// of the program
	// The scale factor should be different for each set of data
	for (vector<vector<VectorR3>>::iterator it = trajectory.begin();
		it < trajectory.end();
		++it) {
			for (vector<VectorR3>::iterator v = (*it).begin();
				v < (*it).end();
				++v) {
					(*v).x /= scale.x;
					(*v).y /= scale.y;
					(*v).z /= scale.z;
			}
	}

	// Translate the left ankle marker to the left ankle of the skeleton
	double x = trajectory[0][0].x - translate.x;
	double y = trajectory[0][0].y - translate.y;
	double z = trajectory[0][0].z - translate.z;
	for (vector<vector<VectorR3>>::iterator it = trajectory.begin();
		it < trajectory.end();
		++it) {
			for(vector<VectorR3>::iterator v = (*it).begin();
				v < (*it).end();
				++v) {
					(*v).x -= x;
					(*v).y -= y;
					(*v).z -= z;
			}
	}
}

/*
 * Prints the transformed markers' data to a text file for debugging
 */
void Animation::PrintMarkers(vector<vector<VectorR3>> &trajectory, char *fileName)
{
    ofstream file(fileName);
    if (file.is_open()) {
        int frameNo = 1;
        for (vector<vector<VectorR3>>::const_iterator it = trajectory.begin();
                                                      it < trajectory.end();
                                                      ++it) {
            file << setw(5) << setfill (' ') << frameNo++ << "    ";
            file << setprecision(3) << fixed;
            for (vector<VectorR3>::const_iterator v = (*it).begin();
                                                  v < (*it).end();
                                                  ++v) {
                file << setw(8) << (*v).x << " " <<
                        setw(8) << (*v).y << " " <<
                        setw(8) << (*v).z << " ; ";
            }
            file << endl;
        }
    }
} 

/*
 * Reads data from a bvh file
 */
void Animation::ReadBVHFile(vector<vector<VectorR3>> *trajectory, const char *fileName)
{
    trajectory->clear();

    ifstream file(fileName);
    string line;

    getline(file, line);
    while (line.compare("MOTION") != 0)
        getline(file, line);

	getline(file, line); // Frames: xxx
    getline(file, line); // Frame Time: xxx

    while (getline(file, line)) {
        istringstream lineStream(line);

        vector<VectorR3> currentFrame;
        double x, y, z;
        while(lineStream >> x >> y >> z)
            currentFrame.push_back(VectorR3(x, y, z));

        trajectory->push_back(currentFrame);
    }

    file.close();
}

//added by david
//update the orientation of the character
void Animation::rotate_character(double angle)
{
	character_orientation += angle;

	//keep the value of character_orientation between 0 and 2*PI
	while (character_orientation<0)
		character_orientation += 4*asin(1.0);
	while (character_orientation>4*asin(1.0))
		character_orientation -= 4*asin(1.0);
}