//==============================================================================
/*
    Software License Agreement (BSD License)
    Copyright (c) 2019, AMBF
    (www.aimlab.wpi.edu)

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

    * Neither the name of authors nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

    \author    <http://www.aimlab.wpi.edu>
    \author    <amunawar@wpi.edu>
    \author    Adnan Munawar
    \version   $
*/
//==============================================================================

#include "ambf_comm/ObjectRosCom.h"

ObjectRosCom::ObjectRosCom(std::string a_name, std::string a_namespace, int a_freq_min, int a_freq_max): RosComBase(a_name, a_namespace, a_freq_min, a_freq_max){
    init();
}

void ObjectRosCom::init(){
    m_State.name.data = m_name;
    m_State.sim_step = 0;

    m_pub = nodePtr->advertise<ambf_msgs::ObjectState>("/" + m_ambf_namespace + "/" + m_name + "/State", 10);
    m_sub = nodePtr->subscribe("/" + m_ambf_namespace + "/" + m_name + "/Command", 10, &ObjectRosCom::sub_cb, this);

    m_thread = boost::thread(boost::bind(&ObjectRosCom::run_publishers, this));
    std::cerr << "Thread Joined: " << m_name << std::endl;
}

ObjectRosCom::~ObjectRosCom(){
    ros::shutdown();
    std::cerr << "Thread ShutDown: " << m_State.name.data << std::endl;
}

void ObjectRosCom::reset_cmd(){
    m_Cmd.enable_position_controller = false;
    m_Cmd.wrench.force.x = 0;
    m_Cmd.wrench.force.y = 0;
    m_Cmd.wrench.force.z = 0;
    m_Cmd.wrench.torque.x = 0;
    m_Cmd.wrench.torque.y = 0;
    m_Cmd.wrench.torque.z = 0;
    for(size_t idx = 0 ; idx < m_Cmd.joint_cmds.size() ; idx++){
        m_Cmd.joint_cmds[idx] = 0;
    }
    for(size_t idx = 0 ; idx < m_Cmd.position_controller_mask.size() ; idx++){
        m_Cmd.position_controller_mask[idx] = 0;
    }
}

void ObjectRosCom::sub_cb(ambf_msgs::ObjectCmdConstPtr msg){
    m_Cmd = *msg;
    m_watchDogPtr->acknowledge_wd();
}
