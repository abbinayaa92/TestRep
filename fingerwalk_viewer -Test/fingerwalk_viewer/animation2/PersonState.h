#ifndef PERSONSTATE_H
#define PERSONSTATE_H

#include <math.h>
#include <vector>
#include "Qrotation.h"
#include "Quaternion3.h"
#include "BVHFormat.h"
#include "CartesianFormat.h"
#include "../GLWindow.h"

using namespace std;

class Vector3d2;
struct Point3d2;

enum {
	WALK
};

struct Neighbor //store the condition for a transition
{
	int index;
	//double distance;
	double rotation;
	double x_translation;
	double z_translation;
	Neighbor(int ind, double rot, double x_t, double z_t) {
		index = ind;
		rotation = rot;
		x_translation = x_t;
		z_translation = z_t;
	}
};

enum Joint_Name
{
	Root,
	L_Femur, L_Tibia, L_Foot, L_Toe, L_Toe_End,
	R_Femur, R_Tibia, R_Foot, R_Toe, R_Toe_End,
	LowerBack, Thorax, Neck, Head, Head_End,
	L_Collar, L_Humerus, L_Elbow, L_Wrist, L_Thumb,
	L_Thumb_End, L_Wrist_End,
	R_Collar, R_Humerus, R_Elbow, R_Wrist, R_Thumb,
	R_Thumb_End, R_Wrist_End
};

struct Motion_Property //store the motions and the transitions between motions
{
	BVHFormat* bvh;
	int move_type;
	double offset_rotation;
	int offset_frame_visit;
	vector <vector <vector <Neighbor> > > neighbor;
	vector <int> index, lowLink, strongSet; //for calculating strongly connected component;
	int component;
};

struct Root_Property //store the root properties including the rotation and transtion to start this pose
{
	int motion;
	int frame;
	double x;
	double z;
	double rotation;
	int transition_index;
	Root_Property() {};
	Root_Property(int t_motion, int t_frame, double t_x, double t_z, double t_rotation, int tr)
	{
		motion = t_motion;
		frame = t_frame;
		x = t_x;
		z = t_z;
		rotation = t_rotation;
		transition_index = tr;
	}
};

class PersonState : public Drawable
{
	//declaration of motion
	vector<Motion_Property> motion_property;
	int no_of_motion;
	int no_of_type;

	//parameter to store character joint tree in vector
	vector <vector <int> > joint_child;
	vector <int> joint_parent;

	//parameter to store offset position of the joint relative to the parents
	vector <Point3d2> joint_offset;

	//motion attribute
	int frameToCmp;
	int lineNumber;
	int nodeNumber;
	NODEHEADER* rootHeader;

	//for calculating strong connected component
	vector <int> Smotion;
	vector <int> Sframe;
	int currentIndex, currentConnected;
	int graphSize, component;

	//for motion planning and transition
	vector <double> xPost;
	vector <double> zPost;
	vector <int> typePost;
	vector <Root_Property> opt_motion;
	vector <Root_Property> temp_motion;
	double initial_rotation;
	double current_x, current_z, temp_x, temp_z;
	int currentPost, tempPost;
	
	//for tracking joint index
	int ct;

	//to construct motion sequence
	int min_frame_visit, min_frame_taken;
	int min_frame_visit_walking, min_frame_visit_sitting;
	int min_frame_visit_lying, min_frame_visit_ending;
	int min_frame_visit_door, min_frame_visit_drawer;
	int min_frame_visit_ladder, min_frame_visit_shelf;
	int frame_to_update, temp_update;
	bool finish_construct;
	double wopt;
	double eps;
	double epsilon;
	double cost_ending, cost_transition, w_orientation;
	vector <vector <double> > storedX;
	vector <vector <double> > storedY;
	vector <vector <double> > storedZ;
	vector <vector <Qrotation> > motion_quaternion;
	vector <int> colorCode;
	int currentFrame;

private:
	double cubic(double a); //compute a*a
	double square(double a); //compute a*a*a
	double getAngle (double x1, double z1, double x2, double z2);
	double get_distance(double x1, double z1, double x2, double z2);
	int update_post(double delta, double p_x, double p_z, int p_post, double t_xz[]); //compute the desired location for cost calculation
	int update_post(double x1, double z1, double x2, double z2, double delta, double p_x, double p_z, int p_post, double t_xz[]);
	void update_node(double Xtemp, double Ytemp, double Ztemp, Qrotation qr, NODEELEMENT* node_element, NODEHEADER* node_header); //compute the joint location of the current frame
	//compute the joint location of the current frame, with blending to mix two different motions
	void blend_and_update_node(double Xtemp, double Ytemp, double Ztemp, Qrotation qr, NODEELEMENT* node_element1, NODEELEMENT* node_element2, NODEHEADER* node_header, double alp);
	double compute_alpha(int p); //compute the blending factor
	void strongConnect(int motion, int frame); //recursively called to compute the strongly connected component

	//not used here
	void calcAlpha(int p);
	void createTransition();
	void blend(double Xtemp, double Ytemp, double Ztemp, Qrotation qr, NODEELEMENT* node1, NODEELEMENT* node2, NODEHEADER* nodeHeader);
	

public:
	int num_of_transition;
	int starting_pose;
	int ending_pose;
	double starting_orientation;
	double ending_orientation;
	vector <Root_Property> motion_list;

	//to store the modified joint point positions and quaternions after we apply inverse kinematic
	vector <vector <Point3d2> > modified_joint_position;
	vector <vector <Qrotation> > modified_joint_quaternion;

	//store the joint connection
	LINESETCF* lineCF;

	PersonState();
	PersonState(double angle, double xPos, double yPos, double zPos);
	int update_post(double p_x, double p_z, int p_post, double t_xz[]);
	void set_fps(int fps, int window_size);
	double encourage_transition_to_ending_motion(int t_post, int t_motion, double rotation);
	bool decide_to_continue_search(int t_post, int t_motion, int t_frame, double cost_update, double t_currentx, double t_currentz,
										  int last_frame_visit, int frame_visit, bool finish_construct);
	bool search_motion(Root_Property rp, double p_x, double p_z, int p_post, double cost, int frame_visit); //this function is recursively called to compute the optimized next frame
	void construct_motion(); //to compute sequence of motion


	void addBVHFormat(BVHFormat* input_bvh, int type, double rotation, int offset_frame_visit);
	void setBVHFormat(BVHFormat* input_bvh, int ind, int type, double rotation, int offset_frame_visit);
	void resizeNeighbor();
	void add_transition(int motion1, int index1, int motion2, int index2, double rotation, double x_translation, double z_translation);
	void calculateStrongConnectComponent();
	
	//reset varaiables
	void clearPath();
	void clearConstruction();

	//draw
	void draw();
	void gotoNextFrame();
	void setCurrentFrame(int fn);
	int getCurrentFrame();
	int getTotalFrame();
	void initializeMove();
	void addPost(double xp, double zp, int tp);
	int getMoveNumber();
	int getNodeNumber();
	int getLineNumber();

	//retrieve the motion
	double getStoredNodeX(int ind);
	double getStoredNodeY(int ind);
	double getStoredNodeZ(int ind);
	double getStoredLineStartX(int ind);
	double getStoredLineStartY(int ind);
	double getStoredLineStartZ(int ind);
	double getStoredLineEndX(int ind);
	double getStoredLineEndY(int ind);
	double getStoredLineEndZ(int ind);
	double getStoredNodeX(int fn, int ind);
	double getStoredNodeY(int fn, int ind);
	double getStoredNodeZ(int fn, int ind);
	double getStoredLineStartX(int fn, int ind);
	double getStoredLineStartY(int fn, int ind);
	double getStoredLineStartZ(int fn, int ind);
	double getStoredLineEndX(int fn, int ind);
	double getStoredLineEndY(int fn, int ind);
	double getStoredLineEndZ(int fn, int ind);

	//retrieve the post position
	int get_post_size();
	double get_post_x(int ind);
	double get_post_z(int ind);

	//not used here
	void setPose(int a, int b, int c, int d, int e);
	void setOrientation(double a, double b);
	void setFrom(int a, int b, int ind);
	void setTo(int a, int b, int ind);
	int getColorCode();
	//void goToNextFrame();

	//convert the tree structure of the character joint into vector
	void express_joint_tree_to_vector(NODEHEADER* nh, int parent_index);

	//modify the joint position and rotation quaternion of the current joint and all its children
	void update_after_move_joint(int frameNumber, Qrotation& qrot, int jn_pt, int jn);

	//modify the joint/end effector to reach certain point
	/*We will update the joint positions and joint quaternions of
	its parent and all its parent's descendant*/
	void move_joint(int frameNumber, Point3d2 pt, Joint_Name jn);

	//move the joint position by a vector delta
	void move_joint_by_delta(int frameNumber, double dx, double dy, double dz, Joint_Name jn);

	//move joint to target position slowly with linear interpolation
	//specify the target position, target frame, frame number to reach the target, and the joint name
	void move_joint_with_linear_interpolation(Point3d2 pt, int target_frame, int n_frame, Joint_Name jn);

	//move joint grandparent to target position slowly with linear interpolation
	//specify the target position, target frame, frame number to reach the target, and the joint name
	void move_grandparent_with_linear_interpolation(Point3d2 pt, int target_frame, int n_frame, Joint_Name jn);

	//modify the root so that it moves to the target location while fixing the position of one of the joints
	void move_grandparent(int frameNumber, Point3d2 pt, Joint_Name jn);

	//modify the position of the joints and the childs except the joint specified
	void move_joint_position(int frameNumber, Vector3d2 delta, int jn, int joint_except);

	//modify the root position by changing the joint angles of the joints
	//that connect the root and the feet
	void move_root(int frameNumber, double dx, double dy, double dz);

	//move the femur and tibia
	void move_tibia_and_femur(int frameNumber, Vector3d2 delta, int jn, Point3d2& p_tibia_new, Point3d2& p_femur_new, Qrotation& qrot_tibia, Qrotation& qrot_femur);
	
	//return the distance between two Point3d2 objects
	double get_distance(Point3d2 p1, Point3d2 p2);

	//check if the foot is fixed on the floor
	bool is_fixed_on_floor(int frameNumber, Joint_Name jn);

	//calculate two link inverse kinematic
	/*Note:
		pt		-	target position
		p1		-	parent joint position
		p2		-	child joint position
		p3		-	end effector old position
		qrot1	-	Quaternion rotation of the parent joint (with respect to the global coordinate)
		qrot2	-	Quaternion rotation of the child joint (with respect to the global coordinate)
	*/
	Point3d2 compute_twoLinkIK(Point3d2 pt, Point3d2 p1, Point3d2 p2, Point3d2 p3, Qrotation& qrot1, Qrotation& qrot2);
};

//class to store points in 3d cartesian coordinate
struct Point3d2
{
	double x, y, z;

	Point3d2(void)
	{ 
		x = 0; y = 0; z = 0;
	}

	Point3d2(double t_x, double t_y, double t_z)
	{
		x = t_x; y = t_y; z = t_z;
	}
};

class Vector3d2
{
public:
	double x, y, z;

	Vector3d2(void)
	{
		x = 0; y = 0; z = 0;
	}

	Vector3d2(double t_x, double t_y, double t_z)
	{
		x = t_x; y = t_y; z = t_z;
	}

	Vector3d2(Point3d2 p1, Point3d2 p2)
	{
		x = p2.x-p1.x; y = p2.y - p1.y; z = p2.z - p1.z;
	}

	Vector3d2 operator*(double scalar)
	{
		return Vector3d2(x*scalar, y*scalar, z*scalar);
	}

	double length()
	{
		return sqrt(x*x + y*y + z*z);
	}

	Vector3d2 toUnit()
	{
		double len = this->length();
		return Vector3d2(x/len, y/len, z/len);
	}
	
	Vector3d2 crossProductWith(Vector3d2 vec)
	{
		return Vector3d2(y*vec.z-z*vec.y, z*vec.x-x*vec.z, x*vec.y-y*vec.x);
	}

	double dotProductWith(Vector3d2 vec)
	{
		return (x*vec.x + y*vec.y + z*vec.z);
	}
};

#endif
