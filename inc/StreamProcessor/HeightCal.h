#pragma once
#include "../STD/std_includes.h"
#include <astra/Vector3f.hpp>
#include <astra/astra.hpp>


/*
* HeightCal类
* 通过读取身体关节数据进行身高计算
*/



class HeightCal
{
public:
	HeightCal(const astra::JointList& joint_list) : joint_list_(joint_list)
	{

	}

	~HeightCal()
	{
	}

	double GetBodyHeight();
private:
	//计算关节长度
	double BoneLength(const astra::Joint& j1, const astra::Joint& j2) const;
	//测量当前的关节点的质量
	size_t NumberOfTrackedJoints(const std::vector<astra::Joint> joint_list) const;
	//判断该追踪哪条腿
	size_t Leg2Choose();
	//测量人物高度
	astra::JointList joint_list_;

};
