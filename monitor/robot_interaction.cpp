/******************************************************************************
 *                                                                            *
 * Copyright (C) 2019 Fondazione Istituto Italiano di Tecnologia (IIT)   *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file robot_interaction.cpp
 * @authors: Michele Colledanchise <michele.colldanchise@iit.it>
 */


#include "robot_interaction.h"
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;

RobotInteraction::RobotInteraction()
{

}

bool RobotInteraction::configure()
{

    // initializes yarp network
    yarp::os::Network yarp;
    if (!yarp::os::Network::checkNetwork(5.0))
    {
        yError() << " YARP server not available! ";
        return false;
    }


    // opens the YARP port used to read messages from the robot

    bool is_open = robot_input_messages_port_.open("/GUI/robotMessages:i");

    if (!is_open)
    {
        yError() << "[GUI] cannot open port named /GUI/robotMessages:i";
        return false;
    }


    // opens the YARP port used to send messages from the robot

    is_open = robot_output_messages_port_.open("/GUI/robotMessages:o");

    if (!is_open)
    {
        yError() << "[GUI] cannot open port named /GUI/robotMessages:o";
        return false;
    }


    // opens the YARP RPC port used to reset the blackboard

    blackboard_client_port_ = Port();
    std::string blackboard_client_port_name = "/GUI/blackboard/rpc:o";
    std::string blackboard_server_port_name = "/blackboard/rpc:i";


    blackboard_client_port_.open(blackboard_client_port_name);
    bool is_connected = yarp.connect(blackboard_client_port_name, blackboard_server_port_name);

    if (!is_connected)
    {
        yError() << "[GUI] cannot connect to " << blackboard_server_port_name;
        return false;
    }

    // opens the YARP RPC port used to close the door


    world_interface_client_port_ = Port();
    std::string world_client_port_name = "/GUI/world/rpc:o";
    std::string world_server_port_name = "/world_input_port";


    world_interface_client_port_.open(world_client_port_name);
    is_connected = yarp.connect(world_client_port_name, world_server_port_name);

    if (!is_connected)
    {
        yError() << "[GUI] cannot connect to " << world_server_port_name;
        return false;
    }

    return true;
}


bool RobotInteraction::sendMessage(std::string message)
{
    Bottle message_btl;
    message_btl.addString(message);
    robot_output_messages_port_.write(message_btl);
    return true;
}

bool RobotInteraction::readMessage(std::string &message)
{
    Bottle* message_btl = robot_input_messages_port_.read(false);
    if (message_btl!=NULL)
    {
        message = message_btl->get(0).asString();
        return true;
    }
    return  false;
}


bool RobotInteraction::resetBlackboard()
{

    Bottle cmd, reply;

    cmd.addString("initialize");
    blackboard_client_port_.write(cmd, reply);
    return true;
}




bool RobotInteraction::closeDoor()
{

    Bottle cmd, reply;
    cmd.addString("setPose");
    cmd.addString("Door");
    cmd.addDouble(9.204);
    cmd.addDouble(0.221);
    cmd.addDouble(0.719);
    cmd.addDouble(0.0);
    cmd.addDouble(0.0);
    cmd.addDouble(1.330);

    world_interface_client_port_.write(cmd, reply);
    return true;
}


bool RobotInteraction::openDoor()
{

    Bottle cmd, reply;
    cmd.addString("setPose");
    cmd.addString("Door");
    cmd.addDouble(7.821);
    cmd.addDouble(0.560);
    cmd.addDouble(0.719);
    cmd.addDouble(0.0);
    cmd.addDouble(0.0);
    cmd.addDouble(1.330);

    world_interface_client_port_.write(cmd, reply);
    return true;
}
