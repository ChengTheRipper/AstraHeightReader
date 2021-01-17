#include "..\..\inc\StreamProcessor\HeightCal.h"

using namespace std;
using namespace astra;


double HeightCal::BoneLength(const astra::Joint& j1, const astra::Joint& j2) const
{
	const Vector3f& j1_world_pt = j1.world_position();
	const Vector3f& j2_world_pt = j2.world_position();
	const Vector3f& result = j1_world_pt - j2_world_pt;
	return result.length();
}

size_t HeightCal::NumberOfTrackedJoints(const std::vector<astra::Joint> joint_list) const
{
	size_t num_of_tracked = 0;
	/*遍历关节数组，计算他们的status
	* 如果有任一关节无法被识别，返回0
	* tracked = 1, low_confidence = 0.25, untracked直接返回零
	*/
	for (const auto& joint : joint_list)
	{
		JointStatus cur_status = joint.status();
		if (cur_status == JointStatus::NotTracked)
			return 0;
		num_of_tracked += static_cast<astra_joint_status_t>(cur_status);
	}
	return num_of_tracked;
}

double HeightCal::GetBodyHeight()
{
	const double HEAD_DIVERGENCE = 0.1;

	auto head = find_if(joint_list_.begin(), joint_list_.end(),
		[](Joint& cur_joint)
		{
			if (cur_joint.type() == JointType::Head)
				return cur_joint;
		}
	);
	auto neck = find_if(joint_list_.begin(), joint_list_.end(),
		[](Joint& cur_joint)
		{
			if (cur_joint.type() == JointType::ShoulderSpine)
				return cur_joint;
		}
	);
	auto spine = find_if(joint_list_.begin(), joint_list_.end(),
		[](Joint& cur_joint)
		{
			if (cur_joint.type() == JointType::MidSpine)
				return cur_joint;
		}
	);
	auto waist = find_if(joint_list_.begin(), joint_list_.end(),
		[](Joint& cur_joint)
		{
			if (cur_joint.type() == JointType::BaseSpine)
				return cur_joint;
		}
	);
	auto hip_left = find_if(joint_list_.begin(), joint_list_.end(),
		[](Joint& cur_joint)
		{
			if (cur_joint.type() == JointType::LeftHip)
				return cur_joint;
		}
	);
	auto hip_right = find_if(joint_list_.begin(), joint_list_.end(),
		[](Joint& cur_joint)
		{
			if (cur_joint.type() == JointType::RightHip)
				return cur_joint;
		}
	);
	auto knee_left = find_if(joint_list_.begin(), joint_list_.end(),
		[](Joint& cur_joint)
		{
			if (cur_joint.type() == JointType::LeftKnee)
				return cur_joint;
		}
	);
	auto knee_right = find_if(joint_list_.begin(), joint_list_.end(),
		[](Joint& cur_joint)
		{
			if (cur_joint.type() == JointType::RightKnee)
				return cur_joint;
		}
	);
	auto foot_left = find_if(joint_list_.begin(), joint_list_.end(),
		[](Joint& cur_joint)
		{
			if (cur_joint.type() == JointType::LeftFoot)
				return cur_joint;
		}
	);
	auto foot_right = find_if(joint_list_.begin(), joint_list_.end(),
		[](Joint& cur_joint)
		{
			if (cur_joint.type() == JointType::RightFoot)
				return cur_joint;
		}
	);

	// Find which leg is tracked more accurately.
	vector<Joint> left_leg = { *hip_left, *knee_left, *foot_left };
	vector<Joint> right_leg = { *hip_right, *knee_right, *foot_right };
	int leg_left_tracked_joints = NumberOfTrackedJoints(left_leg);
	int leg_right_tracked_joints = NumberOfTrackedJoints(right_leg);

	if (leg_left_tracked_joints == 0 && leg_right_tracked_joints == 0)
		return -1.0;

	double leg_length = leg_left_tracked_joints > leg_right_tracked_joints ?
		BoneLength(*hip_left, *knee_left) + BoneLength(*hip_left, *knee_left)
		: BoneLength(*hip_right, *knee_right) + BoneLength(*hip_right, *knee_right);

	double body_length = BoneLength(*head, *neck) + BoneLength(*neck, *spine) + BoneLength(*spine, *waist);
	return body_length + leg_length + HEAD_DIVERGENCE;
}
