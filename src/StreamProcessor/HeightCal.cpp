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
	astra::Joint head, neck, shoulder_spine, spine, waist, hip_left, hip_right,
		knee_left, knee_right, foot_left, foot_right;
	for (const auto& joint : joint_list_)
	{
		JointType cur_type = joint.type();
		switch (cur_type)
		{
		case astra::JointType::Head:
			head = joint;
			if (head.status() == JointStatus::NotTracked)
				return -1.0;
			break;
		case astra::JointType::ShoulderSpine:
			shoulder_spine = joint;
			if (shoulder_spine.status() == JointStatus::NotTracked)
				return -1.0;
			break;
		case astra::JointType::LeftShoulder:
			break;
		case astra::JointType::LeftElbow:
			break;
		case astra::JointType::LeftHand:
			break;
		case astra::JointType::RightShoulder:
			break;
		case astra::JointType::RightElbow:
			break;
		case astra::JointType::RightHand:
			break;
		case astra::JointType::MidSpine:
			spine = joint;
			if (spine.status() == JointStatus::NotTracked)
				return -1.0;
			break;
		case astra::JointType::BaseSpine:
			waist = joint;
			if (spine.status() == JointStatus::NotTracked)
				return -1.0;
			break;
		case astra::JointType::LeftHip:
			hip_left = joint;
			if (hip_left.status() == JointStatus::NotTracked)
				return -1.0;
			break;
		case astra::JointType::LeftKnee:
			knee_left = joint;
			if (knee_left.status() == JointStatus::NotTracked)
				return -1.0;
			break;
		case astra::JointType::LeftFoot:
			foot_left = joint;
			if (foot_left.status() == JointStatus::NotTracked)
				return -1.0;
			break;
		case astra::JointType::RightHip:
			hip_right = joint;
			if (hip_right.status() == JointStatus::NotTracked)
				return -1.0;
			break;
		case astra::JointType::RightKnee:
			knee_right = joint;
			if (knee_right.status() == JointStatus::NotTracked)
				return -1.0;
			break;
		case astra::JointType::RightFoot:
			foot_right = joint;
			if (foot_right.status() == JointStatus::NotTracked)
				return -1.0;
			break;
		case astra::JointType::LeftWrist:
			break;
		case astra::JointType::RightWrist:
			break;
		case astra::JointType::Neck:
			neck = joint;
			if (neck.status() == JointStatus::NotTracked)
				return -1.0;
			break;
		case astra::JointType::Unknown:
			break;
		default:
			break;
		}
	}



	// Find which leg is tracked more accurately.
	vector<Joint> left_leg = { hip_left,  knee_left,  foot_left };
	vector<Joint> right_leg = { hip_right, knee_right, foot_right };
	int leg_left_tracked_joints = NumberOfTrackedJoints(left_leg);
	int leg_right_tracked_joints = NumberOfTrackedJoints(right_leg);

	if (leg_left_tracked_joints == 0 && leg_right_tracked_joints == 0)
		return -1.0;

	double leg_length = leg_left_tracked_joints > leg_right_tracked_joints ?
		BoneLength(hip_left, knee_left) + BoneLength(knee_left, foot_left)
		: BoneLength(hip_right, knee_right) + BoneLength(knee_right, foot_right);

	double body_length = 2 * BoneLength(head, neck) + BoneLength(neck, spine) + BoneLength(spine, waist) + BoneLength(waist, hip_left);
	return body_length + leg_length + HEAD_DIVERGENCE;
}
