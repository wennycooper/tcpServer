# ROS Remote Control server 
* This program is a remote control server on ROS
* It listens on TCP port 5001, and expect to receive commands from any TCP clients
* When it received commands, it publishes messages to /turtle1/cmd_vel to control the turtlesim
* This program can be worked with another "Android Remote Control for turtlesim" project

# Installation & Run
* You must have a ROS package, for example "beginner_tutorials"
* Put the source code under your [package_directory]/src
* Modify the CMakeLists.txt

        add_executable(tcpServer src/tcpServer.cpp)
        target_link_libraries(tcpServer ${catkin_LIBRARIES})
        add_dependencies(tcpServer beginner_tutorials_generate_messages_cpp)
      
* Then build the code by:

        catkin_make

* Run the code by:

        rosrun beginner_tutorials tcpServer

