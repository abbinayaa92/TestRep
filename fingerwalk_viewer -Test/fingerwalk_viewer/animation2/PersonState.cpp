#define _CRT_SECURE_NO_DEPRECATE
#include <vector>
#include "string.h"
#include "stdio.h"
#include "math.h"
#include "time.h"
#include "PersonState.h"
#include "Qrotation.h"
#include "Quaternion3.h"
#include "BVHFormat.h"
#include "CartesianFormat.h"
#include <GLUtils/GLUtils.h>

using namespace std;

#ifndef PI
#define PI 3.1415926535897932384626433832795028841972
#endif

PersonState::PersonState()
{
	// Added by Craig: provide initialization values here, otherwise RANDOM initialization values may be assigned by the compiler!
	initial_rotation = 0;
	current_x = 0;
	current_z = 0;
	temp_x = 0;
	temp_z = 0;
	currentPost = 0;
	tempPost = 0;
	ct = 0;
	min_frame_visit = 0;
	min_frame_taken = 0;
	frame_to_update = 0;
	temp_update = 0;
	wopt = 0;
	eps = 0;
	currentFrame = 0;
	num_of_transition = 0;
	starting_pose = 0;
	ending_pose = 0;
	starting_orientation = 0;
	ending_orientation = 0;
	epsilon = 0;

	w_orientation = 0.2;
	cost_ending = 10;
	cost_transition = 3;
	epsilon = 150;

	frameToCmp = 5;
	//cite: Motion Graphs by Kovar and Gleicher section 4.2
	/*At each step we use branch and bound to find an optimal
	graph walk of n frames. We retain the first m frames of this graph
	walk and use the final retained node as a starting point for another
	search. This process continues until a complete graph walk is generated.
	In our implementation we used values of n from 80 to 120 frames
	(2 2/3 to 4 seconds) and m from 25 to 30 frames (about one second).*/

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//By using a smaller n, we can speed the computation by up to 10 times.
//However, the concequency is the accuracy of the character in following the path will reduce
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	//We specify different n's for different types of motions.
	//For example, while the character is getting up from bed, there is no transition that happens
	//in the process of getting up until the character has completely stood up.
	//After standing up, he can do a transition to walking motion.
	//Therefore, we need larger n's for these types of motions.

	//We specify two sets of n's.
	//One set with larger n's will give a better performance in making the character follows the path.
	//However, the computation will consume more time.

	//option 1: graph walk using shorter window
	/*min_frame_visit_walking = 90;
	min_frame_visit_sitting = 120;
	min_frame_visit_lying = 150;
	min_frame_visit_ending = 90;
	min_frame_visit_door = 120;
	min_frame_visit_drawer = 120;
	min_frame_visit_ladder = 350;
	min_frame_visit_shelf = 120;/**/

	//option 2: graph walk using longer window
	min_frame_visit_walking = 150;
	min_frame_visit_sitting = 150;
	min_frame_visit_lying = 200;
	min_frame_visit_ending = 150;
	min_frame_visit_door = 150;
	min_frame_visit_drawer = 150;
	min_frame_visit_ladder = 400;
	min_frame_visit_shelf = 200;/**/

	//Note: We will use variable min_frame_visit as n in the coding.
	//We take m 30 frames (equals to 1 second)

	//add motion from database
	motion_property.resize(1);
	setBVHFormat(new BVHFormat("../../compressed_data/compressed_30fps_walking1.bvh", 0), WALK, WALK, 0, 0);
	
	resizeNeighbor();
	FILE* fmove1 = fopen("../motion_graphs.txt", "r");
	int no_of_move, from, to, count, index_from, index_to;
	double tdistance, trotation, x_translation, z_translation;
	char tempString[100];
	fscanf(fmove1, "%d", &no_of_move);
	for (int i=0; i<no_of_move; i++)
	{ 
		//printf("i = %d\n", i);
		fscanf(fmove1, "%d %s %d", &from, tempString, &to);
		fscanf(fmove1, "%d", &count);
		for (int j=0; j<count; j++)
		{
			//printf("j = %d\n", j);
			fscanf(fmove1, "%d %d", &index_from, &index_to);
			fscanf(fmove1, "%lf %lf %lf %lf", &tdistance, &trotation, &x_translation, &z_translation);
			trotation = trotation*180/(PI);
			//printf ("%d %d\n", mapMove[from], mapMove[to]);
			add_transition(from, index_from, to, index_to, trotation, x_translation, z_translation);
		}
	}
	fclose(fmove1);

	//Compute strongly connected component
	calculateStrongConnectComponent();

	//Test the motion
	//xPost.push_back(0);		zPost.push_back(0);		typePost.push_back(WALK);
	//xPost.push_back(1000);	zPost.push_back(0);		typePost.push_back(WALK);/**/
	
	//construct_motion();
}

void PersonState::set_fps(int fps, int window_size)
{
		min_frame_taken = fps;
		frameToCmp = fps/3;

		w_orientation = 0.2;
		cost_ending = 10;
		epsilon = 150*30/fps;

		min_frame_visit_walking = window_size*fps;
		min_frame_visit_sitting = window_size*fps;
		min_frame_visit_ending = window_size*fps;
	
	if (fps==15)
	{
		cost_transition = 3;

		//option 1: graph walk using shorter window
		if (window_size <=4)
		{
			min_frame_visit_lying = 80;
			min_frame_visit_door = 60;
			min_frame_visit_drawer = 60;
			min_frame_visit_ladder = 175;
			min_frame_visit_shelf = 60;/**/
		}

		//option 2: graph walk using longer window
		else
		{
			min_frame_visit_lying = 100;
			min_frame_visit_door = 75;
			min_frame_visit_drawer = 75;
			min_frame_visit_ladder = 200;
			min_frame_visit_shelf = 100;
		}
	}

	else if (fps==30)
	{
		cost_transition = 3;

		//option 1: graph walk using shorter window
		if (window_size<=4)
		{
			min_frame_visit_lying = 150;
			min_frame_visit_door = 120;
			min_frame_visit_drawer = 120;
			min_frame_visit_ladder = 350;
			min_frame_visit_shelf = 120;
		}

		//option 2: graph walk using longer window
		else
		{
			min_frame_visit_lying = 200;
			min_frame_visit_door = 150;
			min_frame_visit_drawer = 150;
			min_frame_visit_ladder = 400;
			min_frame_visit_shelf = 200;
		}
	}
}



double PersonState::cubic(double a)
{
	return a*a*a;
}

double PersonState::square(double a)
{
	return a*a;
}

/*The function updates the next desired position following the path specified
(in this case the path from the starting pose to the ending pose).
We specify the distance travelled and use this distance to update the next desired position.*/
int PersonState::update_post(double delta, double p_x, double p_z, int p_post, double t_xz[])
{
	while (p_post<(int)xPost.size()-1 && delta>get_distance(p_x, p_z, xPost[p_post+1], zPost[p_post+1]))
	{
		delta -= get_distance(p_x, p_z, xPost[p_post+1], zPost[p_post+1]);
		p_x = xPost[p_post+1];
		p_z = zPost[p_post+1];
		p_post++;
	}
	if (p_post<(int)xPost.size()-1)
	{
		t_xz[0] = p_x + delta*(xPost[p_post+1]-xPost[p_post])/get_distance(xPost[p_post], zPost[p_post], xPost[p_post+1], zPost[p_post+1]);
		t_xz[1] = p_z + delta*(zPost[p_post+1]-zPost[p_post])/get_distance(xPost[p_post], zPost[p_post], xPost[p_post+1], zPost[p_post+1]);
	}
	else
	{
		t_xz[0] = p_x;
		t_xz[1] = p_z;
	}
	//printf("%lf %lf\n", t_xz[0], t_xz[1]);
	//printf("%d\n", p_post);
	return p_post;
}

int PersonState::update_post(double x1, double z1, double x2, double z2, double delta, double p_x, double p_z, int p_post, double t_xz[])
{
	int p_post2 = update_post(delta, p_x, p_z, p_post, t_xz);
	if (get_distance(x2, z2, p_x, p_z)<get_distance(x2, z2, t_xz[0], t_xz[1]))
	{
		p_post2 = p_post;
		t_xz[0] = p_x;
		t_xz[1] = p_z;
	}
	
	return p_post2;
}

//calculate the blending factor, ranged from 0 to 1
double PersonState::compute_alpha(int p)
{
	return 2*cubic((double)(p+1)/frameToCmp)-3*square((double)(p+1)/frameToCmp)+1;
}

/*When the character has reached the end of the path, the function returns an additional cost
to encourage the character to do transition to the ending motion and face the desired orientation.*/
double PersonState::encourage_transition_to_ending_motion(int t_post, int t_motion, double t_rotation)
{
	double cost_update = 0;
	/*We modify the cost function so that at the end of the path, the character will do a transition to the ending motion
	(e.g. lying on bed, manipulating drawer etc.) and the orientation of the ending motion follows the object orientation.
	To enforce transition to the ending motion, when the character has reached the end of the path,
	we penalize/add a constant cost to the current cost if the character has not done transition to the ending motion.*/
	//if (t_post>=(int)xPost.size()-2 && motion_property[t_motion].move_type!=typePost[t_post]) 
		//cost_update += cost_ending;
	if (motion_property[t_motion].move_type!=typePost[t_post])
		cost_update += cost_transition;

	/*To enforce that the character face the desired direction, after the character has done transition to the ending motion,
	we penalize/add a cost proportional to deviation of angle between the character actual orientation and the desired orientation.
	By balancing the weight of the angle deviation and the cost of not doing the transition to the ending motion,
	we can have the character do a transition to the ending motion at the end of path with the orientation that is 
	closed enough to the desired orientation.*/
	/*The 2 magic numbers 10 and 0.2 can be tuned so that the accuracy of the final position 
	and orientation of character is balanced.*/
	/*if (ending_pose>0 && motion_property[t_motion].move_type==ending_pose)
	{
		double motion_rotation = t_rotation + motion_property[t_motion].offset_rotation - ending_orientation;
		while (motion_rotation>=360) motion_rotation -= 360;
		while (motion_rotation<0) motion_rotation += 360;
		if (motion_rotation>180) motion_rotation = 360 - motion_rotation;
		cost_update += w_orientation*motion_rotation;
	}*/
	return cost_update;
}

/*The function will decide whether the function search_motion() is called or not, depending on the stopping criteria:
1. The character has completed the motion
2. The motion has advanced by n frames from the start of the search
*/
bool PersonState::decide_to_continue_search(int t_post, int t_motion, int t_frame, double cost_update, double t_currentx, double t_currentz,
										  int last_frame_visit, int frame_visit, bool complete_search)
{
	//printf ("%lf %d %d %d\n", cost_update, temp_motion.size(), t_frame, motion_property[t_motion].bvh->getFrameNumber()-1);
	/*Case a: If one of the two conditions is met
	1. The end of the path has been reached and the desired ending motion is walking
	2. The end of the path has been reached and the character has done transition to the ending motion and reached the end of the motion clip,
	the search ends by default, then the optimal cost and motion are updated if the current cost is better than previous costs.*/
	if (t_post==(int)xPost.size()-1 ||
			motion_property[t_motion].bvh->getFrameNumber()-1==t_frame)
	{	
		if (cost_update<wopt)
		{
			finish_construct = true;
			wopt = cost_update;
			frame_to_update = temp_motion.size();
			currentPost = t_post;
			current_x = t_currentx;
			current_z = t_currentz;
			for (int i=0; i<frame_to_update; i++)
			{
				opt_motion[i] = temp_motion[i];
			}
			
		}
	}

	/*Case b: If we have advanced by n frames from the starting of the search, the search ends and
	the optimal cost is updated if the current cost is better than previous costs*/
	else if (last_frame_visit<min_frame_visit && frame_visit>=min_frame_visit && !finish_construct) 
	{
		//printf("entered\n");
		//printf("%lf\n", wopt);
		//If the current graph walk gives a cost that is lower than a certain threshold value epsilon,
		//the graph walk is considered good and we can stop the search.
		if (cost_update<eps) 
		{
			wopt = cost_update;
			//printf("temp_update : %d\n", temp_update);
			complete_search = true;
			for (int i=0; i<temp_update; i++)
			{
				opt_motion[i] = temp_motion[i];
			}
			frame_to_update = temp_update;
			current_x = temp_x;
			current_z = temp_z;
			currentPost = tempPost;
		}
		//If the cost is more than the value epsilon:
		//we update the optimal cost with the current cost if the cost from this graph walk is better than existing optimal cost.
		else if (cost_update<wopt) 
		{
			wopt = cost_update;
			for (int i=0; i<temp_update; i++)
			{
				opt_motion[i] = temp_motion[i];
			}
			frame_to_update = temp_update;
			current_x = temp_x;
			current_z = temp_z;
			currentPost = tempPost;
		}
	}

	//Case c: if none of the stopping conditions hold, we continue the search
	else
	{
		//When we do graph walk of m frames, we store the sequence of motion to a temporary variable.
		//If eventually the graph walk of n frames that go through this branch
		//gives the optimal cost, we will update the motion list with this sequence of motion.
		//printf("tm: %d\n", temp_motion.size());
		if (last_frame_visit<min_frame_taken && frame_visit>=min_frame_taken) 
		{
			temp_update = temp_motion.size();
			temp_x = t_currentx;
			temp_z = t_currentz;
			tempPost =  t_post;
			//printf("%lf %lf\n", temp_x, temp_z);
			//printf("%lf\n", cost);
		}
		complete_search = search_motion(temp_motion.back(), t_currentx, t_currentz, t_post, cost_update, frame_visit);
	}

	return complete_search;
}


//The function search_motion() implements the method from the paper Motion Graphs to find the motion to fit the path.
//The idea is to convert the problem of generating the motion to a search problem
//and use branch and bound to increase the efficiency of the search.

//We specify a cost function that will increase for adding one frame to the graph walk.
//This cost function specify how different the actual character path P' is from the desired path P (Motion Graphs, section 5.1, equation 9].

bool PersonState::search_motion(Root_Property rp, double p_x, double p_z, int p_post, double cost, int frame_visit)
{//printf("%d\n", p_post);
	double delta;
	double x1, z1, x2, z2;
	double x_update, z_update, cost_update;
	int t_post;
	double t_xz[2];
	double t_rp[3];
	bool complete_search = false;
	//ct++;
	//printf("%d %d\n", temp_motion.size(), ct);
	
	//In this search, we consider two cases:
	//1. advance to the very next frame.
	//2. transition to a similar frame based on the motion graph constructed.

	if (cost<wopt)
	{
		//Case 1: advance by one frame from the current frame

		if (rp.frame<motion_property[rp.motion].bvh->getFrameNumber()-1 &&
			motion_property[rp.motion].strongSet[rp.frame+1]==motion_property[rp.motion].component) 
		{
			//Compute the root position of the character after we advance one frame
			x1 = motion_property[rp.motion].bvh->getNodeX(rp.frame, 0);
			z1 = motion_property[rp.motion].bvh->getNodeZ(rp.frame, 0);
			x2 = motion_property[rp.motion].bvh->getNodeX(rp.frame+1, 0);
			z2 = motion_property[rp.motion].bvh->getNodeZ(rp.frame+1, 0);
			Qrotation(rp.rotation, 0.0, 1.0, 0.0).rotatePoint(x2-x1, 0.0, z2-z1, t_rp);
			x_update = t_rp[0] + rp.x;
			z_update = t_rp[2] + rp.z;
			temp_motion.push_back(Root_Property(rp.motion, rp.frame+1, x_update, z_update, rp.rotation, -1));
			
			//Compute the cost for adding one frame

			//case a: This is a general case, when the character is walking.
			/*A simple way to determine the actual path P' is to project the root onto the floor at each frame, forming a piecewise linear curve.
			Let P(s) be the point on P whose arc-length distance from the start of P is s. The ith frame of the graph walk, wi,
			is at some arc length s(wi) from the start of P'. We define the corresponding point on P as the point at
			the same arc length, P(s(wi)). For the jth frame of the graph walk, we calculate the distance 
			between P'(s(wi)) and P(s(wi)) (equation 9).*/

			if (motion_property[rp.motion].move_type==0 || p_post>=(int)xPost.size()-2)
			{
				delta = get_distance(x1, z1, x2, z2);
				t_post = update_post(delta, p_x, p_z, p_post, t_xz);
				//t_post = update_post(rp.x, rp.z, x_update, z_update, delta, p_x, p_z, p_post, t_xz);
			}
			
			/*case b: This is a special case, when the character is at the starting motion (before the first transition happens).
			The character does some action first before he does a walking motion. Some motions can take quite a long time
			and reduce the quality of the search function if we accumulate the cost, because the current algorithm for finding
			path length does not reflect the closest points between the actual and desired position.
			This is basically a hack that we can improve.*/
			else
			{
				delta = get_distance(xPost[0], zPost[0], x_update, z_update);
				t_post = update_post(delta, xPost[0], zPost[0], 0, t_xz);
				
			}
			cost_update = cost + get_distance(x_update, z_update, t_xz[0], t_xz[1]);
			cost_update += encourage_transition_to_ending_motion(t_post, rp.motion, rp.rotation);

			//Call the function decide_to_continue_search().
			//The function will decide whether the search is continued or not.
			complete_search = decide_to_continue_search(t_post, rp.motion, rp.frame+1, cost_update, t_xz[0], t_xz[1],
										  frame_visit, frame_visit+1, complete_search);
			
			temp_motion.pop_back();
		}
		
		//Case 2: search all possible transitions from the current state
		//Most of the steps here are similar to case 1, except that for the transition.
		//We need to blend 10 concecutive  frames (equals 1/3 seconds for 30 fps  motion clip),
		//from the source to the target frames around the transition.
		for (int i=0; i<no_of_motion; i++) if (motion_property[i].move_type==motion_property[rp.motion].move_type ||
												motion_property[i].move_type==typePost[p_post])
		{
			if (complete_search) break;
			int no_of_transition = motion_property[rp.motion].neighbor[rp.frame][i].size();

			for (int j=0; j<no_of_transition; j++) if (motion_property[i].strongSet[motion_property[rp.motion].neighbor[rp.frame][i][j].index]==motion_property[i].component)
			{
				x2 = motion_property[rp.motion].bvh->getNodeX(rp.frame, 0);
				z2 = motion_property[rp.motion].bvh->getNodeZ(rp.frame, 0);
				int transition_index = motion_property[rp.motion].neighbor[rp.frame][i][j].index - frameToCmp + 1;
				cost_update = cost;
				x_update = rp.x;
				z_update = rp.z;
				t_post = p_post;
				t_xz[0] = p_x; t_xz[1] = p_z;

				/*The following loop updates the position of the character, and compute the cost of doing the transition,
				until this transition is completed.
				In case of a 30fps motion clip, the transition takes 10 frames to blend.*/
				for (int k=0; k<frameToCmp; k++) 
				{
					//The following update the position and orientation of the character.
					x1 = x2;
					z1 = z2;
					Qrotation(motion_property[rp.motion].neighbor[rp.frame][i][j].rotation, 0.0, 1.0, 0.0).rotatePoint(motion_property[i].bvh->getNodeX(transition_index+k, 0),
																														0.0, motion_property[i].bvh->getNodeZ(transition_index+k, 0), t_rp);
					t_rp[0] += motion_property[rp.motion].neighbor[rp.frame][i][j].x_translation;
					t_rp[2] += motion_property[rp.motion].neighbor[rp.frame][i][j].z_translation;
					x2 = compute_alpha(k)*motion_property[rp.motion].bvh->getNodeX(rp.frame+k, 0) + (1-compute_alpha(k))*t_rp[0];
					z2 = compute_alpha(k)*motion_property[rp.motion].bvh->getNodeZ(rp.frame+k, 0) + (1-compute_alpha(k))*t_rp[2];

					Qrotation(rp.rotation, 0.0, 1.0, 0.0).rotatePoint(x2-x1, 0.0, z2-z1, t_rp);
					//Let delta be the displacement of the character.
					//We update the desired next position by delta. 
					delta = get_distance(x1, z1, x2, z2);
					t_post = update_post(delta, t_xz[0], t_xz[1], t_post, t_xz);
					//t_post = update_post(x_update, z_update, x_update+t_rp[0], z_update+t_rp[2], delta, p_x, p_z, p_post, t_xz);

					
					x_update += t_rp[0];
					z_update += t_rp[2];

					//accumulate the cost for adding the frame
					//the cost is defined as the distance between the current character position and the desired position on the path
					cost_update += get_distance(x_update, z_update, t_xz[0], t_xz[1]);

					/*If the character is at the end of the path and has not done transition to the desired ending motion,
					we penalize for delaying the desired transition*/
					cost_update += encourage_transition_to_ending_motion(t_post, i, rp.rotation + motion_property[rp.motion].neighbor[rp.frame][i][j].rotation);

				}
				temp_motion.push_back(Root_Property(i, motion_property[rp.motion].neighbor[rp.frame][i][j].index, x_update, z_update,
													rp.rotation+motion_property[rp.motion].neighbor[rp.frame][i][j].rotation, j));
				
				complete_search = decide_to_continue_search(t_post, i, transition_index+frameToCmp-1, cost_update, t_xz[0], t_xz[1],
										  frame_visit, frame_visit+frameToCmp, complete_search);
				
				temp_motion.pop_back();
			}
		}
	}
	return complete_search;
}

//compute the motion sequence
void PersonState::construct_motion()
{
	//FILE *stream = freopen("..\\time_statistic.txt", "w", stdout);
	//FILE* ("..\\time_statistic.txt", "w");
	/*printf ("Begin compute motion.\n");
	double start_construct_motion = clock();
	printf ("Start time: %lf\n", start_construct_motion);
	double start_iteration, end_iteration;*/
printf("pass\n");
	//typePost.resize(xPost.size());
	finish_construct = false;
	currentPost = 0;
	current_x = xPost[0];
	current_z = zPost[0];

	motion_list.clear();
	temp_motion.clear();
	opt_motion.resize(1000);
	
	//Clock time
	//ct = 1;
	//start_iteration = clock();

	eps = epsilon;
	min_frame_visit = 90;
	min_frame_taken = 30;
	starting_orientation = getAngle(xPost[0], zPost[0], xPost[1], zPost[1]);
	int sp = typePost.front();
	int init_frame;
	wopt = 1000000000000;
	for (init_frame=70; init_frame<1000; init_frame+=200)
	{
		double angle_motion = getAngle(motion_property[sp].bvh->getNodeX(init_frame, 0),
										motion_property[sp].bvh->getNodeZ(init_frame, 0),
										motion_property[sp].bvh->getNodeX(init_frame+20, 0),
										motion_property[sp].bvh->getNodeZ(init_frame+20, 0));
		double initial_rotation = starting_orientation - angle_motion;
		Root_Property rp(sp, init_frame, xPost[0], zPost[0], initial_rotation, -1);
		temp_motion.push_back(rp);
		bool complete_motion = search_motion(rp, xPost[0], zPost[0], 0, 0, 0);
		//motion_list.push_back(rp);
		temp_motion.pop_back();
		if (complete_motion) break;
		
		
	}

	//printf("%d\n", frame_to_update);
	//frame_to_update = 5;
	for (int i=0; i<frame_to_update; i++)
	{
		motion_list.push_back(opt_motion[i]);
	}
	
	//printf("%d\n", motion_list.size());
	//do a recursive optimized search for the next motion
	printf("motion_list_size = %d\n", motion_list.size());
	while (currentPost<(int)xPost.size()-1  &&
			motion_list.back().frame<motion_property[motion_list.back().motion].bvh->getFrameNumber()-1)
	{
		//ct++;
		//start_iteration = clock();

		wopt = 1000000000000;
		search_motion(motion_list.back(), current_x, current_z, currentPost, 0, 0);
		
		//update the motion from the buffer to the motion list
		for (int i=0; i<frame_to_update; i++)
		{
			motion_list.push_back(opt_motion[i]);
			//printf("%lf %lf\n", opt_motion[i].x, opt_motion[i].z);
		}
		//printf("%lf\n", wopt);
		//printf("%d %lf %lf\n", currentPost, current_x, current_z);

		//end_iteration = clock();
		//printf ("Time for iteration %d: %lf ms\n", ct, end_iteration - start_iteration);
	}/**/

	/*double end_construct_motion = clock();
	printf ("End time: %lf\n", end_construct_motion);
	printf("Time taken for computing motion: %lf ms\n", end_construct_motion - start_construct_motion);
	printf("Average time for each iteration = %lf ms\n", (end_construct_motion - start_construct_motion)/(ct-1));
	printf("End compute motion.\n");*/
	//stream = freopen("CON", "w", stdout);

	//printf("test motion: %d", motion_list.size());
	//printf("called\n");

	int motion_list_size = motion_list.size();
	
	//initialize the variables to store the joint positions at each frame
	storedX.clear(); storedY.clear(); storedZ.clear();
	storedX.resize(nodeNumber); storedY.resize(nodeNumber); storedZ.resize(nodeNumber);

	//initialize the variables to store the rotation state of the joints at each frame
	motion_quaternion.clear();
	motion_quaternion.resize(nodeNumber);

	//initialize the varialbes to store the modified joint positions and quaternions
	modified_joint_position.clear();
	modified_joint_position.resize(nodeNumber);
	modified_joint_quaternion.clear();
	modified_joint_quaternion.resize(nodeNumber);

	//compute the joint positions for each frame, use blending if they have transition
	for (int i=0; i<motion_list_size; i++)
	{
		if (i==0 || (motion_list[i].frame==motion_list[i-1].frame+1 && motion_list[i].motion==motion_list[i-1].motion)) //condition without blending
		{
			ct = 0;
			storedX[0].push_back(motion_list[i].x);
			storedY[0].push_back(motion_property[motion_list[i].motion].bvh->getNodeY(motion_list[i].frame, 0));
			storedZ[0].push_back(motion_list[i].z);

			ROOTELEMENT* root_element = motion_property[motion_list[i].motion].bvh->getRootElement();
			int ind = motion_list[i].frame;
			Qrotation qr = Qrotation(root_element[ind].nodeElement.Zrotation, 0.0, 0.0, 1.0);
			qr.rotate(root_element[ind].nodeElement.Yrotation, 0.0, 1.0, 0.0);
			qr.rotate(root_element[ind].nodeElement.Xrotation, 1.0, 0.0, 0.0);
			//qr.rotate(qr.getAngle(), qr.getXaxis(), qr.getYaxis(), qr.getZaxis());
			qr.rotate(motion_list[i].rotation, 0.0, 1.0, 0.0);
			motion_quaternion[ct].push_back(qr);
			ct++;
			for (int j=0; j<rootHeader->numberOfChild; j++)
			{
				update_node(storedX[0].back(), storedY[0].back(), storedZ[0].back(), qr, root_element[ind].nodeElement.child[j], rootHeader->child[j]);
			}
		}
		else //condition with blending
		{
			double x1, z1;
			double t_rp[3];
			double x2 = motion_property[motion_list[i-1].motion].bvh->getNodeX(motion_list[i-1].frame, 0);
			double z2 = motion_property[motion_list[i-1].motion].bvh->getNodeZ(motion_list[i-1].frame, 0);
			double x_update = motion_list[i-1].x;
			double z_update = motion_list[i-1].z;
			double y2;
			for (int j=0; j<frameToCmp; j++)
			{
				ct = 0;
				x1 = x2;
				z1 = z2;
				Qrotation(motion_list[i].rotation-motion_list[i-1].rotation, 0.0, 1.0, 0.0).rotatePoint(motion_property[motion_list[i].motion].bvh->getNodeX(motion_list[i].frame-frameToCmp+1+j, 0),
																										0.0, motion_property[motion_list[i].motion].bvh->getNodeZ(motion_list[i].frame-frameToCmp+1+j, 0), t_rp);
				t_rp[0] += motion_property[motion_list[i-1].motion].neighbor[motion_list[i-1].frame][motion_list[i].motion][motion_list[i].transition_index].x_translation;
				t_rp[2] += motion_property[motion_list[i-1].motion].neighbor[motion_list[i-1].frame][motion_list[i].motion][motion_list[i].transition_index].z_translation;
				x2 = compute_alpha(j)*motion_property[motion_list[i-1].motion].bvh->getNodeX(motion_list[i-1].frame+j, 0) + (1-compute_alpha(j))*t_rp[0];
				z2 = compute_alpha(j)*motion_property[motion_list[i-1].motion].bvh->getNodeZ(motion_list[i-1].frame+j, 0) + (1-compute_alpha(j))*t_rp[2];
				y2 = compute_alpha(j)*motion_property[motion_list[i-1].motion].bvh->getNodeY(motion_list[i-1].frame+j, 0) + (1-compute_alpha(j))*motion_property[motion_list[i].motion].bvh->getNodeY(motion_list[i].frame-frameToCmp+1+j, 0);
				Qrotation(motion_list[i-1].rotation, 0.0, 1.0, 0.0).rotatePoint(x2-x1, 0.0, z2-z1, t_rp);
				x_update += t_rp[0];
				z_update += t_rp[2];
				storedX[0].push_back(x_update);
				storedZ[0].push_back(z_update);
				storedY[0].push_back(y2);

				ROOTELEMENT* root_element1 = motion_property[motion_list[i-1].motion].bvh->getRootElement();
				ROOTELEMENT* root_element2 = motion_property[motion_list[i].motion].bvh->getRootElement();
				
				double alp = compute_alpha(j);

				//compute the quaternion of state before transition
				Qrotation qr = Qrotation(root_element1[motion_list[i-1].frame+j+1].nodeElement.Zrotation, 0.0, 0.0, 1.0);
				//printf("%lf %lf %lf %lf\n", qtemp1.getAngle(), qtemp1.getXaxis(), qtemp1.getYaxis(), qtemp1.getZaxis());
				qr.rotate(root_element1[motion_list[i-1].frame+j+1].nodeElement.Yrotation, 0.0, 1.0, 0.0);
				//printf("%lf %lf %lf %lf\n", qtemp1.getAngle(), qtemp1.getXaxis(), qtemp1.getYaxis(), qtemp1.getZaxis());
				qr.rotate(root_element1[motion_list[i-1].frame+j+1].nodeElement.Xrotation, 1.0, 0.0, 0.0);
				if (qr.getAngle()>180) qr = Qrotation(360-qr.getAngle(), -qr.getXaxis(), -qr.getYaxis(), -qr.getZaxis()); 
				//else if (qr.getAngle()<-180) qr = Qrotation(360+qr.getAngle(), -qr.getXaxis(), -qr.getYaxis(), -qr.getZaxis());
				//printf("1 : %lf %lf %lf %lf\n", qr.getAngle(), qr.getXaxis(), qr.getYaxis(), qr.getZaxis());
				//while (motion_list[i-1].rotation>360) motion_list[i-1].rotation-=360;
				//while (motion_list[i-1].rotation<0) motion_list[i-1].rotation += 360;
				//qr.rotate(motion_list[i-1].rotation, 0.0, 1.0, 0.0);
				//if (qr.getYaxis()<0) qr = Qrotation(-qr.getAngle(), -qr.getXaxis(), -qr.getYaxis(), -qr.getZaxis());

				//compute the quaternion of the state after transition
				Qrotation qr2 = Qrotation(root_element2[motion_list[i].frame-frameToCmp+j+1].nodeElement.Zrotation, 0.0, 0.0, 1.0);
				qr2.rotate(root_element2[motion_list[i].frame-frameToCmp+j+1].nodeElement.Yrotation, 0.0, 1.0, 0.0);
				qr2.rotate(root_element2[motion_list[i].frame-frameToCmp+j+1].nodeElement.Xrotation, 1.0, 0.0, 0.0);
				//while (motion_list[i].rotation>360) motion_list[i].rotation -=360;
				//while (motion_list[i].rotation<0) motion_list[i].rotation += 360;
				double delta_angle = motion_list[i].rotation - motion_list[i-1].rotation;
				//while (delta_angle>360) delta_angle -= 360;
				//while (delta_angle<360) delta_angle += 360;
				qr2.rotate(delta_angle, 0.0, 1.0, 0.0);
				//if (qr2.getYaxis()<0) qr2 = Qrotation(-qr2.getAngle(), -qr2.getXaxis(), -qr2.getYaxis(), -qr2.getZaxis());
				if (qr2.getAngle()>180) qr2 = Qrotation(360-qr2.getAngle(), -qr2.getXaxis(), -qr2.getYaxis(), -qr2.getZaxis());
				//else if (qr2.getAngle()<-180) qr2 = Qrotation(360+qr2.getAngle(), -qr2.getXaxis(), -qr2.getYaxis(), -qr2.getZaxis());
				if (qr.getXaxis()*qr2.getXaxis()+qr.getYaxis()*qr2.getYaxis()+qr.getZaxis()*qr2.getZaxis()<0) qr2 = Qrotation(360-qr2.getAngle(), -qr2.getXaxis(), -qr2.getYaxis(), -qr2.getZaxis());
				if (abs(qr.getAngle()-qr2.getAngle())>180) qr2 = Qrotation(qr2.getAngle()-360, qr2.getXaxis(), qr2.getYaxis(), qr2.getZaxis());
				//printf("2 : %lf %lf %lf %lf\n", qr2.getAngle(), qr2.getXaxis(), qr2.getYaxis(), qr2.getZaxis());
				//printf("%lf %lf %lf %lf\n", qtemp1.getAngle(), qtemp1.getXaxis(), qtemp1.getYaxis(), qtemp1.getZaxis());
				
				//blending the quaternion, using slerp function
				qr = qr.slerp(qr2, 1-alp);
				qr.rotate(motion_list[i-1].rotation, 0.0, 1.0, 0.0);
				
				//store the rotation state of the root position
				motion_quaternion[ct].push_back(qr);
				ct++;
				//qtemp1.rotate(orientation, 0.0, 1.0, 0.0);
				//printf("%lf %lf %lf %lf\n", qtemp1.getAngle(), qtemp1.getXaxis(), qtemp1.getYaxis(), qtemp1.getZaxis());

				//call the blending function
				for (int k=0; k<rootHeader->numberOfChild; k++)
				{
					blend_and_update_node(storedX[0].back(), storedY[0].back(), storedZ[0].back(), qr, root_element1[motion_list[i-1].frame+j].nodeElement.child[k], root_element2[motion_list[i].frame-frameToCmp+j+1].nodeElement.child[k], rootHeader->child[k], alp);
				}
			}/**/
		}
	}
	printf("Size: %d %d\n", motion_list.size(), storedX[0].size());
	//printf("Transition: %d\n", (storedX[0].size()-motion_list.size())/29);
	num_of_transition = (storedX[0].size() - motion_list.size())/(frameToCmp-1);
	currentFrame = 0;

	//copy the actual joint positions and quaternions to the variables that will contain
	//the modified versions of the joint positions and quaternions
	for (int i=0; i<nodeNumber; i++)
	{
		for (int j=0; j<(int)storedX[i].size(); j++)
		{
			modified_joint_position[i].push_back(Point3d2(storedX[i][j], storedY[i][j], storedZ[i][j]));
		}
		for (int j=0; j<(int)motion_quaternion[i].size(); j++)
		{
			modified_joint_quaternion[i].push_back(motion_quaternion[i][j]);
		}
	}

	//Examples of moving the joint position to the target position by linear interpolation 
	//Move the left hand
	//Point3d2 ptemp_rhand = modified_joint_position[L_Wrist].back();
	//move_joint_with_linear_interpolation(Point3d2(ptemp_rhand.x+5, ptemp_rhand.y+8, ptemp_rhand.z-5), storedX[0].size()-1, 30, L_Wrist);
	//Move the right foot
	//Point3d2 ptemp_lfoot = modified_joint_position[L_Foot].back();
	//move_joint_with_linear_interpolation(Point3d2(ptemp_lfoot.x+4, ptemp_lfoot.y+10, ptemp_lfoot.z+5), storedX[0].size()-1, 30, R_Foot);

	//Example of moving the grand parent to the target position
	//Point3d2 p_temp = modified_joint_position[L_Femur].back();
	//move_grandparent_with_linear_interpolation(Point3d2(p_temp.x+5, p_temp.y-10, p_temp.z-5), storedX[0].size()-1, 10, L_Foot);
}


//recursively compute the joint position (without blending)
void PersonState::update_node(double Xtemp, double Ytemp, double Ztemp, Qrotation qr, NODEELEMENT* node_element, NODEHEADER* node_header)
{ 
	double tempOffset[3];
	qr.rotatePoint(node_header->offsetX, node_header->offsetY, node_header->offsetZ, tempOffset);
	Xtemp += tempOffset[0];
	Ytemp += tempOffset[1];
	Ztemp += tempOffset[2];
	storedX[ct].push_back(Xtemp);
	storedY[ct].push_back(Ytemp);
	storedZ[ct].push_back(Ztemp);

	
	if (strcmp(node_header->name, "endSite")!=0)
	{
		Qrotation qtemp = Qrotation(node_element->Zrotation, 0.0, 0.0, 1.0);
		qtemp.rotate(node_element->Yrotation, 0.0, 1.0, 0.0);
		qtemp.rotate(node_element->Xrotation, 1.0, 0.0, 0.0);
		qtemp.rotate(qr.getAngle(), qr.getXaxis(), qr.getYaxis(), qr.getZaxis());
		qr = qtemp;
		motion_quaternion[ct].push_back(qr);
	}
	ct++;
	for (int i=0; i<node_header->numberOfChild; i++)
	{
		update_node(Xtemp, Ytemp, Ztemp, qr, node_element->child[i], node_header->child[i]);
	}
}

void PersonState::blend_and_update_node(double Xtemp, double Ytemp, double Ztemp, Qrotation qr, NODEELEMENT* node_element1, NODEELEMENT* node_element2, NODEHEADER* node_header, double alp)
{ //recursively compute the joint position (with blending)
	double tempOffset[3];
	qr.rotatePoint(node_header->offsetX, node_header->offsetY, node_header->offsetZ, tempOffset);
	Xtemp += tempOffset[0];
	Ytemp += tempOffset[1];
	Ztemp += tempOffset[2];
	storedX[ct].push_back(Xtemp);
	storedY[ct].push_back(Ytemp);
	storedZ[ct].push_back(Ztemp);
	
	if (strcmp(node_header->name, "endSite")!=0)
	{
		Qrotation qr1 = Qrotation(node_element1->Zrotation, 0.0, 0.0, 1.0);
		qr1.rotate(node_element1->Yrotation, 0.0, 1.0, 0.0);
		qr1.rotate(node_element1->Xrotation, 1.0, 0.0, 0.0);
		//printf("1 : %lf %lf %lf %lf\n", qr1.getAngle(), qr1.getXaxis(), qr1.getYaxis(), qr1.getZaxis());
		Qrotation qr2 = Qrotation(node_element2->Zrotation, 0.0, 0.0, 1.0);
		qr2.rotate(node_element2->Yrotation, 0.0, 1.0, 0.0);
		qr2.rotate(node_element2->Xrotation, 1.0, 0.0, 0.0);
		//printf("2: %lf %lf %lf %lf\n", qr2.getAngle(), qr2.getXaxis(), qr2.getYaxis(), qr2.getZaxis());
		qr1 = qr1.slerp(qr2, 1-alp);
		qr1.rotate(qr.getAngle(), qr.getXaxis(), qr.getYaxis(), qr.getZaxis());
		qr = qr1;
		motion_quaternion[ct].push_back(qr);
	}
	ct++;
	for (int i=0; i<node_header->numberOfChild; i++)
	{
		blend_and_update_node(Xtemp, Ytemp, Ztemp, qr, node_element1->child[i], node_element2->child[i], node_header->child[i], alp);
	}
}

void PersonState::addBVHFormat(BVHFormat* input_bvh, int type, double rotation, int offset_frame_visit) //add input motion
{/*
	bvh.push_back(bvht);
	if (bvh.size()==1)
	{
		nodeNumber = bvht->getNodeNumber();
		lineNumber = bvht->getLineNumber();
		Xposition = new double[nodeNumber];
		Yposition = new double[nodeNumber];
		Zposition = new double[nodeNumber];
		rootHeader = bvht->getRootHeader();
		lineCF = bvht->getLineSet();
	}*/
	Motion_Property mp;
	int ind = motion_property.size();
	motion_property.push_back(mp);
	motion_property[ind].bvh = input_bvh;
	motion_property[ind].move_type = type;
	motion_property[ind].offset_rotation = rotation;
	motion_property[ind].offset_frame_visit = offset_frame_visit;
	motion_property[ind].neighbor.resize(input_bvh->getFrameNumber());
	motion_property[ind].index.resize(input_bvh->getFrameNumber());
	motion_property[ind].lowLink.resize(input_bvh->getFrameNumber());
	motion_property[ind].strongSet.resize(input_bvh->getFrameNumber());
	motion_property[ind].component = 0;
	nodeNumber = input_bvh->getNodeNumber();
	lineNumber = input_bvh->getNodeNumber()-1;
	rootHeader = input_bvh->getRootHeader();
	lineCF = input_bvh->getLineSet();
}

void PersonState::setBVHFormat(BVHFormat* input_bvh, int ind, int type, double rotation, int offset_frame_visit)
{
	motion_property[ind].bvh = input_bvh;
	motion_property[ind].move_type = type;
	motion_property[ind].offset_rotation = rotation;
	motion_property[ind].offset_frame_visit = offset_frame_visit;
	motion_property[ind].neighbor.resize(input_bvh->getFrameNumber());
	motion_property[ind].index.resize(input_bvh->getFrameNumber());
	motion_property[ind].lowLink.resize(input_bvh->getFrameNumber());
	motion_property[ind].strongSet.resize(input_bvh->getFrameNumber());
	motion_property[ind].component = 0;
	nodeNumber = input_bvh->getNodeNumber();
	lineNumber = input_bvh->getNodeNumber()-1;
	rootHeader = input_bvh->getRootHeader();
	lineCF = input_bvh->getLineSet();
}

void PersonState::resizeNeighbor() //initialization the size of motion_property
{
	no_of_motion = motion_property.size();
	no_of_type = 1; //need to modify
	for (int i=0; i<no_of_motion; i++)
	{
		int frame_size = motion_property[i].bvh->getFrameNumber();
		for (int j=0; j<frame_size; j++)
		{
			motion_property[i].neighbor[j].resize(no_of_motion);
		}
	}

	//initialize the size of joint_child
	joint_child.clear();
	joint_child.resize(nodeNumber);

	//initialize the size of joint_parent
	joint_parent.clear();
	joint_parent.resize(nodeNumber);

	//initialize the size of offset position of joints
	joint_offset.clear();
	joint_offset.resize(nodeNumber);

	//call the function to epress the joint offset relative positions and joint childs in vectors
	ct = 0;
	express_joint_tree_to_vector(rootHeader, -1);
}

void PersonState::add_transition(int motion1, int index1, int motion2, int index2, double rotation, double x_translation, double z_translation)
{

	motion_property[motion1].neighbor[index1][motion2].push_back(
		Neighbor(index2, rotation, x_translation, z_translation));
	//printf("%d\n", motion_property[motion1].neighbor[index1][motion2].size());
}

void PersonState::strongConnect(int motion, int frame) //recursively compute the srongly connected component for the graph
{
	motion_property[motion].index[frame] = currentIndex;
	motion_property[motion].lowLink[frame] = currentIndex;
	currentIndex++;
	Smotion.push_back(motion);
	Sframe.push_back(frame);

	//strong connect next frame
	if (frame<motion_property[motion].bvh->getFrameNumber()-1)
	{
		if (motion_property[motion].index[frame+1]==-1)
		{
			strongConnect(motion, frame+1);
			if (motion_property[motion].lowLink[frame]>
				motion_property[motion].lowLink[frame+1])
			{
				motion_property[motion].lowLink[frame] = motion_property[motion].lowLink[frame+1];
			}
		}
		else
		{
			int Ssize = Smotion.size();
			for (int i=0; i<Ssize; i++)
			{
				if (Smotion[i]==motion && Sframe[i]==frame+1)
				{
					if (motion_property[motion].lowLink[frame]>
						motion_property[motion].lowLink[frame+1])
					{
						motion_property[motion].lowLink[frame] =
							motion_property[motion].lowLink[frame+1];
					}
					break;
				}
			}
		}
	}
	
	//strong connect adjacent node
	for (int i=0; i<no_of_motion; i++) if (motion_property[motion].move_type==motion_property[i].move_type)
	{
		int no_of_transition = motion_property[motion].neighbor[frame][i].size();
		for (int j=0; j<no_of_transition; j++)
		{
			int transition_index = motion_property[motion].neighbor[frame][i][j].index;
			if (motion_property[i].index[transition_index]==-1)
			{
				strongConnect(i, transition_index);
				if (motion_property[motion].lowLink[frame]>
					motion_property[i].lowLink[transition_index])
				{
					motion_property[motion].lowLink[frame] = motion_property[i].lowLink[transition_index];
				}
			}
			else
			{
				int Ssize = Smotion.size();
				for (int k=0; k<Ssize; k++)
				{
					if (Smotion[k]==i && Sframe[k]==transition_index)
					{
						if (motion_property[motion].lowLink[frame]>
							motion_property[i].lowLink[transition_index])
						{
							motion_property[motion].lowLink[frame] =
								motion_property[i].lowLink[transition_index];
						}
						break;
					}
				}
			}
		}
	}
	
	//update set
	if (motion_property[motion].lowLink[frame]==motion_property[motion].index[frame])
	{
		int popmotion, popframe;
		int ct = 0;
		do
		{
			popmotion = Smotion.back();
			popframe = Sframe.back();
			motion_property[popmotion].strongSet[popframe] = currentConnected;
			Smotion.pop_back();
			Sframe.pop_back();
			ct++;
		}
		while (popmotion!=motion || popframe!=frame);
		//printf ("ct : %d %d\n", ct, currentConnected);
		if (graphSize<ct)
		{
			graphSize = ct;
			component = currentConnected;
		}
		currentConnected++;
	}
}

void PersonState::calculateStrongConnectComponent() //compute the strongly connected component
{
	//Initialization value of index and strongSet of motion_property
	//printf("%d\n", no_of_motion);
	for (int i=0; i<no_of_motion; i++)
	{
		int no_of_frame = motion_property[i].bvh->getFrameNumber();
		for (int j=0; j<no_of_frame; j++)
		{
			motion_property[i].index[j] = -1;
			//if(motion_property[i].move_type==0)
				motion_property[i].strongSet[j] = -1;
		}
	}

	for (int i=0; i<no_of_motion; i++)
	{
		Smotion.clear();
		Sframe.clear();
		currentIndex = 0;
		currentConnected = 0;
		graphSize = 0;
		component = 0;
		for (int j=0; j<no_of_motion; j++) if (motion_property[j].move_type==i)
		{
			int no_of_frame = motion_property[j].bvh->getFrameNumber();
			for (int k=0; k<no_of_frame; k++)
			{
				if (motion_property[j].index[k]==-1)
				{
					strongConnect(j, k);
				}
			}/**/

			//FILE* fp = fopen("..\\check.txt", "w");
			/*for (int i=0; i<maxTrans; i++)
			{
				for (int j=0; j<maxTrans; j++)
				{
					int frameNo = frameFrom[i][j].size();
					fprintf (fp, "%d to %d\n", i, j);
					for (int k=0; k<frameNo; k++)
					{
						if (strongSet[i][frameFrom[i][j][k]]==150)
							fprintf(fp, "%d %d\n", frameFrom[i][j][k], frameTo[i][j][k]);
					}
				}
			}*/
			//fclose(fp);
		}
		printf("strong connected type %d: %d %d\n", i, component, graphSize);
		for (int j=0; j<no_of_motion; j++) if (motion_property[j].move_type==i)
		{
			motion_property[j].component = component;
		}
	}
}

void PersonState::clearPath()
{
	xPost.clear();
	zPost.clear();
	typePost.clear();
}

void PersonState::clearConstruction()
{
	storedX.clear();
	storedY.clear();
	storedZ.clear();
	colorCode.clear();
	num_of_transition = 0;
	motion_list.clear();
}

void PersonState::setCurrentFrame(int fn)
{
	currentFrame = fn;
}

int PersonState::getCurrentFrame()
{
	return currentFrame;
}

int PersonState::getTotalFrame()
{
	return storedX[0].size();
}

int PersonState::getColorCode()
{
	return colorCode[currentFrame];
}

void PersonState::draw()
{
	
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	//printf("%d\n", storedX[0].size());
	for (int i=0; i<lineNumber; i++)
	{
		glVertex3f(storedZ[lineCF->start[i]][currentFrame],
					storedY[lineCF->start[i]][currentFrame],
					storedX[lineCF->start[i]][currentFrame]);
		glVertex3f(storedZ[lineCF->end[i]][currentFrame],
					storedY[lineCF->end[i]][currentFrame],
					storedX[lineCF->end[i]][currentFrame]);
	}
	glEnd();
	printf("current frame: %d %d\n", currentFrame, storedX[0].size());
	gotoNextFrame();
}

void PersonState::gotoNextFrame()
{
	if (currentFrame<(signed)storedX[0].size()-1) currentFrame++;
}

double PersonState::getStoredNodeX(int ind)
{
	return storedX[ind][currentFrame];
}

double PersonState::getStoredNodeY(int ind)
{
	return storedY[ind][currentFrame];
}

double PersonState::getStoredNodeZ(int ind)
{
	return storedZ[ind][currentFrame];
}

double PersonState::getStoredLineStartX(int ind)
{
	return storedX[lineCF->start[ind]][currentFrame];
}

double PersonState::getStoredLineStartY(int ind)
{
	return storedY[lineCF->start[ind]][currentFrame];
}

double PersonState::getStoredLineStartZ(int ind)
{
	return storedZ[lineCF->start[ind]][currentFrame];
}

double PersonState::getStoredLineEndX(int ind)
{
	return storedX[lineCF->end[ind]][currentFrame];
}

double PersonState::getStoredLineEndY(int ind)
{
	return storedY[lineCF->end[ind]][currentFrame];
}

double PersonState::getStoredLineEndZ(int ind)
{
	return storedZ[lineCF->end[ind]][currentFrame];
}

double PersonState::getStoredNodeX(int fn, int ind)
{
	return storedX[ind][fn];
}

double PersonState::getStoredNodeY(int fn, int ind)
{
	return storedY[ind][fn];
}

double PersonState::getStoredNodeZ(int fn, int ind)
{
	return storedZ[ind][fn];
}

double PersonState::getStoredLineStartX(int fn, int ind)
{
	return storedX[lineCF->start[ind]][fn];
}

double PersonState::getStoredLineStartY(int fn, int ind)
{
	return storedY[lineCF->start[ind]][fn];
}

double PersonState::getStoredLineStartZ(int fn, int ind)
{
	return storedZ[lineCF->start[ind]][fn];
}

double PersonState::getStoredLineEndX(int fn, int ind)
{
	return storedX[lineCF->end[ind]][fn];
}

double PersonState::getStoredLineEndY(int fn, int ind)
{
	return storedY[lineCF->end[ind]][fn];
}

double PersonState::getStoredLineEndZ(int fn, int ind)
{
	return storedZ[lineCF->end[ind]][fn];
}

double PersonState::getAngle(double x1, double z1, double x2, double z2)
{
	double x = x2 - x1;
	double z = z2 - z1;
	double orientation = -180/PI*asin(z/sqrt(z*z+x*x));
	if (x<0)
	{
		if (orientation<0) orientation = -180 - orientation;
		else orientation = 180 - orientation;
	}
	return orientation;
}

double PersonState::get_distance(double x1, double z1, double x2, double z2)
{
	return sqrt((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1));
}

void PersonState::addPost(double xp, double zp, int tp)
{
	xPost.push_back(xp);
	zPost.push_back(zp);
	typePost.push_back(tp);
}

int PersonState::getNodeNumber()
{
	return nodeNumber;
}

int PersonState::getLineNumber()
{
	return lineNumber;
}

int PersonState::get_post_size()
{
	return xPost.size();
}

double PersonState::get_post_x(int ind)
{
	return xPost[ind];
}

double PersonState::get_post_z(int ind)
{
	return zPost[ind];
}

Point3d2 operator+(Point3d2 pt, Vector3d2 vec)
{
		return Point3d2(pt.x+vec.x, pt.y+vec.y, pt.z+vec.z);
}

//convert the tree structure of the joints to a vector
void PersonState::express_joint_tree_to_vector(NODEHEADER* nh, int parent_index)
{
	//store the current index to update child from this index
	int current_index = ct;

	//store the relative offset position of the joints
	joint_offset[ct] = Point3d2(nh->offsetX, nh->offsetY, nh->offsetZ);
	

	//store the parent index of the current joint
	joint_parent[ct] = parent_index;
	
	//global variable to track the index being explored
	ct++;

	//call express_joint_tree_to_vector recursively to explore the childs of the current joint
	for (int i=0; i<nh->numberOfChild; i++)
	{
		joint_child[current_index].push_back(ct);
		express_joint_tree_to_vector(nh->child[i], current_index);
	}
}

//modify the position and orientation of the joint as well as its parent and grand parent
//the descendant position and orientation of its grand parent's descendants will be modified accordingly
void PersonState::move_joint(int frameNumber, Point3d2 pt, Joint_Name jn)
{
	//check if the joint is not the root
	if (joint_parent[jn]!=-1)
	{
		//store the parent's index
		int jn_p = joint_parent[jn];

		//check if the parent is not the root
		if (joint_parent[jn_p]!=-1)
		{
			//store the grandparent's index
			int jn_gp = joint_parent[jn_p];

			//convert the position of grand parent to Point3d2
			Point3d2 p1(modified_joint_position[jn_gp][frameNumber].x,
						modified_joint_position[jn_gp][frameNumber].y,
						modified_joint_position[jn_gp][frameNumber].z);

			//convert the position of the parent to Point3d2
			Point3d2 p2(modified_joint_position[jn_p][frameNumber].x,
						modified_joint_position[jn_p][frameNumber].y,
						modified_joint_position[jn_p][frameNumber].z);

			//convert the position of the joint to Point3d2
			Point3d2 p3(modified_joint_position[jn][frameNumber].x,
						modified_joint_position[jn][frameNumber].y,
						modified_joint_position[jn][frameNumber].z);

			//store the change in rotation of the grandparent and parent into qrot1 and qrot2 respectively
			Qrotation qrot1, qrot2;
			//do two link inverse kinematic to update quaternion rotation of the grand parent
			Point3d2 p2_new = compute_twoLinkIK(pt, p1, p2, p3, qrot1, qrot2);
			//for debugging
			//printf("%lf %lf\n", qrot1.getAngle(), qrot2.getAngle());

			//update the quaternion rotation and joint position of all the descendants of the target joint's grand parent except the parent
			modified_joint_quaternion[jn_gp][frameNumber].rotate(qrot1.getAngle(), qrot1.getXaxis(), qrot1.getYaxis(), qrot1.getZaxis());
			for (int i=0; i<(int)joint_child[jn_gp].size(); i++)
			{
				if (joint_child[jn_gp][i]!=jn_p)
				{
					update_after_move_joint(frameNumber, qrot1, jn_gp, joint_child[jn_gp][i]);
				}
			}

			//update the new position of the parent
			modified_joint_position[jn_p][frameNumber] = Point3d2(p2_new.x, p2_new.y, p2_new.z);

			//update the quaternion rotation and joint position of the descendant of the target joint's parent
			modified_joint_quaternion[jn_p][frameNumber].rotate(qrot2.getAngle(), qrot2.getXaxis(), qrot2.getYaxis(), qrot2.getZaxis());
			for (int i=0; i<(int)joint_child[jn_p].size(); i++)
			{
				update_after_move_joint(frameNumber, qrot2, jn_p, joint_child[jn_p][i]);
			}
			//for debugging
			/*printf("%lf %lf %lf\n", modified_joint_position[jn][frameNumber].x,
									modified_joint_position[jn][frameNumber].y,
									modified_joint_position[jn][frameNumber].z);/**/
		}
	}
}

//move the joint position by delta from the original position
void PersonState::move_joint_by_delta(int frameNumber, double dx, double dy, double dz, Joint_Name jn)
{
	Point3d2 pt = modified_joint_position[jn][frameNumber] + Vector3d2(dx, dy, dz);
	move_joint(frameNumber, pt, jn);
}

//move the joint by linearly interpolating the starting and target position of the joint
void PersonState::move_joint_with_linear_interpolation(Point3d2 pt, int target_frame, int n_frame, Joint_Name jn)
{
	//total number of frame for the whole motion
	int total_frame =modified_joint_position[0].size();

	//check if the target frame is in the range of motion frames
	if(target_frame>=0 && target_frame<total_frame)
	{
		//frame number to start the interpolation
		int start_frame = target_frame - n_frame + 1;
		//check if the start_frame has non negative value
		if (start_frame<0)
		{
			n_frame = target_frame + 1;
			start_frame = 0;
		}
		
		//initial position
		Point3d2 p0(modified_joint_position[jn][start_frame].x,
					modified_joint_position[jn][start_frame].y,
					modified_joint_position[jn][start_frame].z);

		//move the joint
		for (int i=0; i<n_frame; i++)
		{
			move_joint(start_frame+i, Point3d2(p0.x+(pt.x-p0.x)*(i+1)/n_frame, p0.y+(pt.y-p0.y)*(i+1)/n_frame,
														p0.z+(pt.z-p0.z)*(i+1)/n_frame), jn);
		}
	}
}

//modify the joint position and rotation quaternion of the current joint and all its children
/*Note:
	frameNumber		-	the current frame number
	qrot			-	the change to quaternion rotation
	jn_p			-	the joint name/order of the parent
	jn				-	the current joint name/order
*/
void PersonState::update_after_move_joint(int frameNumber, Qrotation& qrot, int jn_p, int jn)
{
	//update the quaternion rotation of the current joint
	//first check if this joint has child, means that the joint is not the end effector
	if (joint_child[jn].size()>0)
	{
		modified_joint_quaternion[jn][frameNumber].rotate(qrot.getAngle(), qrot.getXaxis(), qrot.getYaxis(), qrot.getZaxis());
	}

	//update the position of the current joint
	double offset_temp[3];
	modified_joint_quaternion[jn_p][frameNumber].rotatePoint(joint_offset[jn].x, joint_offset[jn].y,
															joint_offset[jn].z, offset_temp);
	modified_joint_position[jn][frameNumber] = Point3d2(modified_joint_position[jn_p][frameNumber].x + offset_temp[0],
														modified_joint_position[jn_p][frameNumber].y + offset_temp[1],
														modified_joint_position[jn_p][frameNumber].z + offset_temp[2]);

	for (int i=0; i<(int)joint_child[jn].size(); i++)
	{
		update_after_move_joint(frameNumber, qrot, jn, joint_child[jn][i]);
	}
}

//move the joint by linearly interpolating the starting and target position of the joint
void PersonState::move_grandparent_with_linear_interpolation(Point3d2 pt, int target_frame, int n_frame, Joint_Name jn)
{
	//total number of frame for the whole motion
	int total_frame = modified_joint_position[0].size();

	//check if the target frame is in the range of motion frames
	if(target_frame>=0 && target_frame<total_frame)
	{
		//check if the joint has parent
		if(joint_parent[jn]>-1)
		{
			int jn_p = joint_parent[jn];

			//check if the joint has grand parent
			if (joint_parent[jn_p]>-1)
			{
				int jn_gp = joint_parent[jn_p];

				//for debugging
				printf("%lf %lf %lf\n", modified_joint_position[jn_gp][target_frame].x,
									modified_joint_position[jn_gp][target_frame].y,
									modified_joint_position[jn_gp][target_frame].z);

				//frame number to start the interpolation
				int start_frame = target_frame - n_frame + 1;
				//check if the start_frame has non negative value
				if (start_frame<0)
				{
					n_frame = target_frame + 1;
					start_frame = 0;
				}
				
				//initial position
				Point3d2 p0(modified_joint_position[jn_gp][start_frame].x,
							modified_joint_position[jn_gp][start_frame].y,
							modified_joint_position[jn_gp][start_frame].z);

				//move the joint
				for (int i=0; i<n_frame; i++)
				{
					move_grandparent(start_frame+i, Point3d2(p0.x+(pt.x-p0.x)*(i+1)/n_frame, p0.y+(pt.y-p0.y)*(i+1)/n_frame,
																p0.z+(pt.z-p0.z)*(i+1)/n_frame), jn);
				}

				printf("%lf %lf %lf\n", modified_joint_position[jn_gp][target_frame].x,
									modified_joint_position[jn_gp][target_frame].y,
									modified_joint_position[jn_gp][target_frame].z);
			}
		}
	}
}

//modify the root to move to the target position
void PersonState::move_grandparent(int frameNumber, Point3d2 pt, Joint_Name jn)
{
	//check if the joint is not the root
	if (joint_parent[jn]!=-1)
	{
		//store the parent's index
		int jn_p = joint_parent[jn];

		//check if the parent is not the root
		if (joint_parent[jn_p]!=-1)
		{
			//store the grandparent's index
			int jn_gp = joint_parent[jn_p];

			//convert the position of the grandparent to Point3d2
			Point3d2 p1(modified_joint_position[jn_gp][frameNumber].x,
						modified_joint_position[jn_gp][frameNumber].y,
						modified_joint_position[jn_gp][frameNumber].z);

			//convert the position of the parent to Point3d2
			Point3d2 p2(modified_joint_position[jn_p][frameNumber].x,
						modified_joint_position[jn_p][frameNumber].y,
						modified_joint_position[jn_p][frameNumber].z);

			//convert the position of joint to Point3d2
			Point3d2 p3(modified_joint_position[jn][frameNumber].x,
						modified_joint_position[jn][frameNumber].y,
						modified_joint_position[jn][frameNumber].z);
			
			//store the change in rotation of the grandparent and parent into qrot1 and qrot2 respectively
			Qrotation qrot1, qrot2;
			//do two link inverse kinematic to update quaternion rotation of the grand parent
			Point3d2 p2_new = compute_twoLinkIK(pt, p3, p2, p1, qrot2, qrot1);
			//for debugging
			//printf("%lf %lf\n", qrot1.getAngle(), qrot2.getAngle());

			//update the new position of the parent
			modified_joint_position[jn_p][frameNumber] = Point3d2(p2_new.x, p2_new.y, p2_new.z);

			//update the quaternion rotation and joint position of the descendant of the target joint's parent except the jont
			modified_joint_quaternion[jn_p][frameNumber].rotate(qrot2.getAngle(), qrot2.getXaxis(), qrot2.getYaxis(), qrot2.getZaxis());
			for (int i=0; i<(int)joint_child[jn_p].size(); i++)
			{
				if (joint_child[jn_p][i]!=jn)
				{
					update_after_move_joint(frameNumber, Qrotation(qrot2.getAngle(), qrot2.getXaxis(), qrot2.getYaxis(), qrot2.getZaxis()), jn_p, joint_child[jn_p][i]);
				}
			}

			//update the quaternion rotation and joint position of all the descendants of the target joint's grand parent except the parent
			modified_joint_quaternion[jn_gp][frameNumber].rotate(qrot1.getAngle(), qrot1.getXaxis(), qrot1.getYaxis(), qrot1.getZaxis());

			//update the position of the grandparent
			double p1_temp[3];
			modified_joint_quaternion[jn_gp][frameNumber].rotatePoint(joint_offset[jn_p].x, joint_offset[jn_p].y, joint_offset[jn_p].z, p1_temp);
			modified_joint_position[jn_gp][frameNumber] = Point3d2(modified_joint_position[jn_p][frameNumber].x - p1_temp[0],
														modified_joint_position[jn_p][frameNumber].y - p1_temp[1],
														modified_joint_position[jn_p][frameNumber].z - p1_temp[2]);
			
			//update the position and quaternion of the grandparent's descendants except the parent
			for (int i=0; i<(int)joint_child[jn_gp].size(); i++)
			{
				if (joint_child[jn_gp][i]!=jn_p)
				{
					update_after_move_joint(frameNumber, qrot1, jn_gp, joint_child[jn_gp][i]);
				}
			}
			
			//update the position of all other joints
			if (joint_parent[jn_gp]!=-1)
			{
				Vector3d2 delta(p1, modified_joint_position[jn_gp][frameNumber]);
				move_joint_position(frameNumber, delta, 0, jn_gp);
			}
			
		}
	}
}

//move the position of joint specified and its child except one exception joint
void PersonState::move_joint_position(int frameNumber, Vector3d2 delta, int jn, int joint_except)
{
	//modify the position of the target joint
	modified_joint_position[jn][frameNumber] = Point3d2(modified_joint_position[jn][frameNumber].x + delta.x,
														modified_joint_position[jn][frameNumber].y + delta.y,
														modified_joint_position[jn][frameNumber].z + delta.z);

	//modify the position of its children
	for (int i=0; i<(int)joint_child[jn].size(); i++)
	{
		if (joint_child[jn][i]!=joint_except)
		{
			move_joint_position(frameNumber, delta, joint_child[jn][i], joint_except);
		}
	}
}

//modify the root position by changing the joint angles of the joints
//that connect the root and the feet
void PersonState::move_root(int frameNumber, double dx, double dy, double dz)
{
	//check and store the state whether the feet are fixed on the floor
	bool left_foot_fixed = is_fixed_on_floor(frameNumber, L_Foot);
	bool right_foot_fixed= is_fixed_on_floor(frameNumber, R_Foot);

	//for debugging
	printf("Left foot: %d\n",left_foot_fixed);
	printf("Right foot: %d\n", right_foot_fixed);

	//compute the displacement in Vector3d2
	Vector3d2 delta(dx, dy, dz);

	//declare the variables to store the states of the left leg
	Point3d2 p_ltibia_new, p_lfemur_new;
	Qrotation qrot_lfemur, qrot_ltibia;

	//declare the variables to store the states of the right leg
	Point3d2 p_rtibia_new, p_rfemur_new;
	Qrotation qrot_rfemur, qrot_rtibia;

	//the following will compute the displacement that can be achieved by both left and right legs
	//check if the left foot is fixed on the ground
	if (left_foot_fixed)
	{
		//compute the left tibia and femur new position and quaternion
		move_tibia_and_femur(frameNumber, delta, L_Foot, p_ltibia_new, p_lfemur_new, qrot_ltibia, qrot_lfemur);

		//check if the left femur can be moved toward the target position
		if (dx*(p_lfemur_new.x - modified_joint_position[L_Femur][frameNumber].x)>=-1e-7 &&
			dy*(p_lfemur_new.y - modified_joint_position[L_Femur][frameNumber].y)>=-1e-7 &&
			dz*(p_lfemur_new.z - modified_joint_position[L_Femur][frameNumber].z)>=-1e-7 &&
			abs(dx)+1e-10>abs(p_lfemur_new.x - modified_joint_position[L_Femur][frameNumber].x) &&
			abs(dy)+1e-10>abs(p_lfemur_new.y - modified_joint_position[L_Femur][frameNumber].y) &&
			abs(dz)+1e-10>abs(p_lfemur_new.z - modified_joint_position[L_Femur][frameNumber].z))
		{
			//replace the target displacement by the left femur displacement
			dx = p_lfemur_new.x - modified_joint_position[L_Femur][frameNumber].x;
			dy = p_lfemur_new.y - modified_joint_position[L_Femur][frameNumber].y;
			dz = p_lfemur_new.z - modified_joint_position[L_Femur][frameNumber].z;
		}
		//if we can't move the left femur towards the target position, we set the displacement to 0
		else
		{
			dx = 0;
			dy = 0;
			dz = 0;
		}
	}
	
	//check if the right foot is fixed on the ground
	if (right_foot_fixed)
	{
		//compute the right tibia and femur new position and quaternion
		move_tibia_and_femur(frameNumber, delta, R_Foot, p_rtibia_new, p_rfemur_new, qrot_rtibia, qrot_rfemur);

		//check if the right femur can be moved toward the target position
		if (dx*(p_rfemur_new.x-modified_joint_position[R_Femur][frameNumber].x)>=-1e-7 &&
			dy*(p_rfemur_new.y-modified_joint_position[R_Femur][frameNumber].y)>=-1e-7 &&
			dz*(p_rfemur_new.z-modified_joint_position[R_Femur][frameNumber].z)>=-1e-7 &&
			abs(dx)+1e-10>abs(p_rfemur_new.x-modified_joint_position[R_Femur][frameNumber].x) &&
			abs(dy)+1e-10>abs(p_rfemur_new.y-modified_joint_position[R_Femur][frameNumber].y) &&
			abs(dz)+1e-10>abs(p_rfemur_new.z-modified_joint_position[R_Femur][frameNumber].z))
		{
			//replace the target displacement by the right femur displacement
			dx = p_rfemur_new.x - modified_joint_position[R_Femur][frameNumber].x;
			dy = p_rfemur_new.y - modified_joint_position[R_Femur][frameNumber].y;
			dz = p_rfemur_new.z - modified_joint_position[R_Femur][frameNumber].z;
			
		}
		//if we can't move the right femur towards the target position, we set the displacement to 0
		else
		{
			dx = 0;
			dy = 0;
			dz = 0;	
		}
	}

	//convert the new displacement to Vector3d2
	delta = Vector3d2 (dx, dy, dz);

	//for debugging
	printf("%lf %lf %lf\n", dx, dy, dz);

	//if the total target displacement is less than certain threshold (0.5) we set the displacement to 0
	if (delta.length()<=1e-5)
		delta = Vector3d2(0.0, 0.0, 0.0);
	
	//After we compute the achievable displacement by both left and right leg,
	//we compute the new positions and states of the left and right legs

	//check if the left foot is fixed on the ground
	if (left_foot_fixed)
	{
		//compute the displacement and quaternion of the left tibia and femur
		move_tibia_and_femur(frameNumber, delta, L_Foot, p_ltibia_new, p_lfemur_new, qrot_ltibia, qrot_lfemur);

		//update the position and quaternion of the left tibia and femur
		modified_joint_position[L_Tibia][frameNumber] = p_ltibia_new;
		modified_joint_position[L_Femur][frameNumber] = p_lfemur_new;
		modified_joint_quaternion[L_Tibia][frameNumber] = qrot_ltibia;
		modified_joint_quaternion[L_Femur][frameNumber] = qrot_lfemur;
	}

	//check if the right foot is fixed on the ground
	if (right_foot_fixed)
	{
		//compute the displacement and quaternion of the right tibia and femur
		move_tibia_and_femur(frameNumber, delta, R_Foot, p_rtibia_new, p_rfemur_new, qrot_rtibia, qrot_rfemur);

		//update the position and quaternion of the right tibia and femur
		modified_joint_position[R_Tibia][frameNumber] = p_rtibia_new;
		modified_joint_position[R_Femur][frameNumber] = p_rfemur_new;
		modified_joint_quaternion[R_Tibia][frameNumber] = qrot_rtibia;
		modified_joint_quaternion[R_Femur][frameNumber] = qrot_rfemur;
	}
	
	//check if one of the feet is fixed on the ground
	if (left_foot_fixed || right_foot_fixed)
	{
		//update the position of the root
		modified_joint_position[Root][frameNumber] = modified_joint_position[Root][frameNumber] + delta;

		//update the position of lower back and its children
		move_joint_position(frameNumber, delta, LowerBack, -1);

		//check if the left foot is not fixed
		if (!left_foot_fixed)
		{
			//move the left leg following the root
			move_joint_position(frameNumber, delta, L_Femur, -1);
		}

		//check if the right foot is not fixed
		if (!right_foot_fixed)
		{
			//move the right leg following the root
			move_joint_position(frameNumber, delta, R_Femur, -1);
		}
	}
}

void PersonState::move_tibia_and_femur(int frameNumber, Vector3d2 delta, int jn, Point3d2& p_tibia_new, Point3d2& p_femur_new, Qrotation& qrot_tibia, Qrotation& qrot_femur)
{
	//get the position of the foot of the target frame
	Point3d2 p_foot = modified_joint_position[jn][frameNumber];

	//get the position of the tibia of the target frame
	int jn_p = joint_parent[jn];
	Point3d2 p_tibia = modified_joint_position[jn_p][frameNumber];

	//get the position of the femur of the target frame
	int jn_gp = joint_parent[jn_p];
	Point3d2 p_femur = modified_joint_position[jn_gp][frameNumber];

	//compute the target position of the right femur
	Point3d2 pt_femur = p_femur + delta;

	//store the change in rotation of the grandparent and parent into qrot1 and qrot2 respectively
	Qrotation qrot1, qrot2;
	//do two link inverse kinematic to update quaternion rotation of the grand parent
	p_tibia_new = compute_twoLinkIK(pt_femur, p_foot, p_tibia, p_femur, qrot2, qrot1);

	//update the quaternion rotation of the tibia
	qrot_tibia = modified_joint_quaternion[jn_p][frameNumber];
	qrot_tibia.rotate(qrot2.getAngle(), qrot2.getXaxis(), qrot2.getYaxis(), qrot2.getZaxis());

	//update the quaternion rotation of the femur
	qrot_femur = modified_joint_quaternion[jn_gp][frameNumber];
	qrot_femur.rotate(qrot1.getAngle(), qrot1.getXaxis(), qrot1.getYaxis(), qrot1.getZaxis());

	//update the position of the femur
	double p_temp[3];
	qrot_femur.rotatePoint(joint_offset[R_Tibia].x, joint_offset[R_Tibia].y, joint_offset[R_Tibia].z, p_temp);
	p_femur_new = Point3d2(p_tibia_new.x - p_temp[0],
							p_tibia_new.y - p_temp[1],
							p_tibia_new.z - p_temp[2]);
}
	

//check if the foot is fixed on the floor
bool PersonState::is_fixed_on_floor(int frameNumber, Joint_Name jn)
{
	//compute the nearest frame for joint speed calculation
	int next_frame;
	if (frameNumber>0)
		next_frame = frameNumber -1;
	else
		next_frame = frameNumber + 1;

	//for debugging
	printf("Y: %lf\n", storedY[jn][frameNumber]);

	//for debugging
	printf("dis: %lf\n", get_distance(Point3d2(storedX[jn][frameNumber], storedY[jn][frameNumber], storedZ[jn][frameNumber]),
					Point3d2(storedX[jn][next_frame], storedY[jn][next_frame], storedZ[jn][next_frame])));

	//check if the feet is on the surface of the floor
	if (storedY[jn][frameNumber]<4.5)
	{
		//check if the speed of the point on the foot is below certain threshold value
		//we can assume that the point does not move in this case
		//from the two conditions, we can conclude that the foot is fixed on the floor
		if (get_distance(Point3d2(storedX[jn][frameNumber], storedY[jn][frameNumber], storedZ[jn][frameNumber]),
						Point3d2(storedX[jn][next_frame], storedY[jn][next_frame], storedZ[jn][next_frame]))<1.0)
		{
			return true;
		}
		return false;
	}
	return false;
}

//computet the distance between two points in 3d Cartesian Coordinate
double PersonState::get_distance(Point3d2 p1, Point3d2 p2)
{
	return sqrt((p2.x-p1.x)*(p2.x-p1.x) +
				(p2.y-p1.y)*(p2.y-p1.y) +
				(p2.z-p1.z)*(p2.z-p1.z));
}

//compute two link inverse kinematic
//all calculations are done in global coordinate
Point3d2 PersonState::compute_twoLinkIK(Point3d2 pt, Point3d2 p1, Point3d2 p2, Point3d2 p3,
									Qrotation& qrot1, Qrotation& qrot2)
{
	Vector3d2 delta(p3, pt);
	//check if the displacement is still in the computational limit of double
	//for smaller value of the limit, the computation may fail to resolve the number
	if (delta.length()>1e-5)
	{
		//compute the normal vector of p1, p2, and p3
		Vector3d2 n = Vector3d2(p2, p1).crossProductWith(Vector3d2(p2, p3)).toUnit();

		//modify n so that it's perpendicular to the vector (p1, pt), and still as close as possible to n
		Vector3d2 line = Vector3d2(p1, pt);
		Vector3d2 n_new = (line.crossProductWith(n.crossProductWith(line))*(1)).toUnit();

		//now compute the new position of the child joint
		/*The solution for this comes from computation of the intersection of two circles of radii r1 and r2,
		located at p1 and p2 respectively. There are, of course, two solutions to this problem.
		The calling application can differentiate between these by passing in n or -n for the plane normal.
		This is the distance between p1 and p2. If it is > r1+r2, then we have no solutions.
		To be nice about it, we will set r to r1+r2 - the behaviour will be to reach as much as possible,
		even though you don't hit the target*/
		double r = Vector3d2(p1, pt).length();
		double r1 = Vector3d2(p1, p2).length();
		double r2 = Vector3d2(p2, p3).length();
		if (r > (r1+r2) * 0.993)
			r = (r1 + r2) * 0.993;
		//this is the length of the vector starting at p1 and going to the midpoint between p1 and pt
		double a = (r1 * r1 - r2 * r2 + r * r) / (2 * r);
		double tmp = r1*r1 - a*a;
		if (tmp < 0)
			tmp = 0;
		//and this is the distance from the midpoint of p1-p2 to the intersection point
		double h = sqrt(tmp);
		//now we need to get the two directions needed to reconstruct the intersection point
		Vector3d2 d1 = Vector3d2(p1, pt).toUnit();
		Vector3d2 d2 = d1.crossProductWith(n_new).toUnit();
		//and now get the intersection point
		Point3d2 p2_new = p1 + d1 * a + d2 * (-h);

		//compute the new quaternion of the parent joint
		Vector3d2 v1 = Vector3d2(p1, p2).toUnit();
		Vector3d2 v1_new = Vector3d2(p1, p2_new).toUnit();
		double angle1 = acos(v1.dotProductWith(v1_new));
		Vector3d2 axis1 = v1.crossProductWith(v1_new).toUnit();
		qrot1 = Qrotation(angle1*180/(PI), axis1.x, axis1.y, axis1.z);

		//compute the new quaternion of the child joint
		Vector3d2 v2 = Vector3d2(p2, p3).toUnit();
		Vector3d2 v2_new = Vector3d2(p2_new, pt).toUnit();

		//for debugging
		//printf("%lf %lf %lf\n", v2.x, v2.y, v2.z);
		//printf("%lf %lf %lf\n", v2_new.x, v2_new.y, v2_new.z);
		//printf("%lf\n", v2.toUnit().dotProductWith(v2_new.toUnit()));
		double angle2 = acos(v2.dotProductWith(v2_new));
		//for debugging
		//printf("%lf\n", angle2);
		Vector3d2 axis2 = v2.crossProductWith(v2_new).toUnit();
		qrot2 = Qrotation(angle2*180/(PI), axis2.x, axis2.y, axis2.z);

		return p2_new;
	}
	//if the displacement is below the threshold, set the variables to default values
	else
	{
		qrot1 = Qrotation(0.0, 1.0, 0.0, 0.0);
		qrot2 = Qrotation(0.0, 1.0, 0.0, 0.0);
		return p2;
	}
}
